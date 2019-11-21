#include "ser.h"
#include "main_fm.h"

uint8_t _written=0;

volatile uint8_t _rx_buffer_head=0;
volatile uint8_t _rx_buffer_tail=0;
volatile uint8_t _tx_buffer_head=0;
volatile uint8_t _tx_buffer_tail=0;

uint8_t _rx_buffer[SERIAL_RX_BUFFER_SIZE];
uint8_t _tx_buffer[SERIAL_TX_BUFFER_SIZE];

ISR(USART0_RX_vect)
{
    ser_rx_complete_irq();
}

ISR(USART0_UDRE_vect)
{
    ser_tx_udr_empty_irq();
}

void ser_tx_udr_empty_irq(void)
{
    unsigned char c = _tx_buffer[_tx_buffer_tail];
    _tx_buffer_tail = (_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;

    UDR0 = c;

    UCSR0A |= _BV(TXC0);

    if (_tx_buffer_head == _tx_buffer_tail) {
        UCSR0B &= ~(_BV(UDRIE0));
    }
}

void ser_rx_complete_irq(void)
{
    if (bit_is_clear(UCSR0A, UPE0)) {
        unsigned char c = UDR0;
        uint8_t i = (unsigned int)(_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;

        if (i != _rx_buffer_tail) {
            _rx_buffer[_rx_buffer_head] = c;
            _rx_buffer_head = i;
        }
    } else {
        UDR0;
    }
}

void ser_begin(uint32_t baud)
{
    UCSR0B = 0; //disconnect bootloader
    DDRA |= _BV(PINA1);//pin1 of portD as OUTPUT
    DDRA &= ~(_BV(PINA2));//pin0 of portD as INPUT

    UCSR0A = 0 | _BV(U2X0); // Use double speed

    ser_changebaud(baud);

    _written = 0;

    UCSR0C = SERIAL_8N1;
    UCSR0B |= _BV(RXEN0);
    UCSR0B |= _BV(TXEN0);
    UCSR0B |= _BV(RXCIE0);
    UCSR0B &= ~(_BV(UDRIE0));
}

uint8_t ser_read(void)
{
    // if the head isn't ahead of the tail, we don't have any characters
    if (_rx_buffer_head == _rx_buffer_tail) {
        return 0;
    } else {
        uint8_t c = _rx_buffer[_rx_buffer_tail];
        _rx_buffer_tail = (int8_t)(_rx_buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
        return c;
    }
}

int8_t ser_print(uint8_t *c)
{
    uint8_t count=0;
    while (c[count]!=0) {
        ser_write(c[count]);
        count++;
    }
    return count;
}

int8_t ser_write(uint8_t c)
{
    _written = 1;
    // If the buffer and the data register is empty, just write the byte
    // to the data register and be done. This shortcut helps
    // significantly improve the effective datarate at high (>
    // 500kbit/s) bitrates, where interrupt overhead becomes a slowdown.
    if (_tx_buffer_head == _tx_buffer_tail && bit_is_set(UCSR0A, UDRE0)) {
        UDR0 = c;
        UCSR0A |= _BV(TXC0);
        return 1;
    }
    uint8_t i = (_tx_buffer_head + 1) % SERIAL_TX_BUFFER_SIZE;

    // If the output buffer is full, there's nothing for it other than to 
    // wait for the interrupt handler to empty it a bit
    while (i == _tx_buffer_tail) {
        if (bit_is_clear(SREG, SREG_I)) {
            // Interrupts are disabled, so we'll have to poll the data
            // register empty flag ourselves. If it is set, pretend an
            // interrupt has happened and call the handler to free up
            // space for us.
            if(bit_is_set(UCSR0A, UDRE0)) {
                ser_tx_udr_empty_irq();
            }
        } else {
            // nop, the interrupt handler will free up space for us
        }
    }

    _tx_buffer[_tx_buffer_head] = c;
    _tx_buffer_head = i;

    UCSR0B |= _BV(UDRIE0);

    return 1;
}

uint8_t ser_available(void)
{
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) % SERIAL_RX_BUFFER_SIZE;
}

void ser_changebaud(uint32_t baud)
{
    uint32_t baud_setting = (F_CPU / 8 / baud - 1);
    UBRR0H = baud_setting >> 8;
    UBRR0L = baud_setting;
}

void ser_flush()
{
    if(!_written) {
        return;
    }
    while(bit_is_set(UCSR0B, UDRIE0) || bit_is_clear(UCSR0A, TXC0)) {
        if (bit_is_set(SREG, SREG_I) && bit_is_set(UCSR0B, UDRIE0)) {
            if (bit_is_set(UCSR0A, UDRE0)) {
                ser_tx_udr_empty_irq();
            }
        }
    }
}
