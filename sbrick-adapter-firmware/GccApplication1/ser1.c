#include "ser1.h"
#include "main_fm.h"

uint8_t _written1=0;

volatile uint8_t _rx_buffer_head1=0;
volatile uint8_t _rx_buffer_tail1=0;
volatile uint8_t _tx_buffer_head1=0;
volatile uint8_t _tx_buffer_tail1=0;

uint8_t _rx_buffer1[SERIAL_RX_BUFFER_SIZE1];
uint8_t _tx_buffer1[SERIAL_TX_BUFFER_SIZE1];

ISR(USART1_RX_vect)
{
    ser_rx_complete_irq1();
}

ISR(USART1_UDRE_vect)
{
    ser_tx_udr_empty_irq1(); 
}

void ser_tx_udr_empty_irq1(void)
{
    unsigned char c = _tx_buffer1[_tx_buffer_tail1];
    _tx_buffer_tail1 = (_tx_buffer_tail1 + 1) % SERIAL_TX_BUFFER_SIZE1;

    UDR1 = c;

    UCSR1A |= _BV(TXC1);

    if (_tx_buffer_head1 == _tx_buffer_tail1) {
        UCSR1B &= ~(_BV(UDRIE1));
    }
}

void ser_rx_complete_irq1(void)
{
    if (bit_is_clear(UCSR1A, UPE1)) {
        unsigned char c = UDR1;
        uint8_t i = (unsigned int)(_rx_buffer_head1 + 1) % SERIAL_RX_BUFFER_SIZE1;

        if (i != _rx_buffer_tail1) {
            _rx_buffer1[_rx_buffer_head1] = c;
            _rx_buffer_head1 = i;
        }
    } else {
        UDR1;
    };
}

void ser_begin1(uint32_t baud)
{
    UCSR1B = 0; //disconnect bootloader
    DDRA |= _BV(PINA5);//pin1 of portD as OUTPUT
    DDRA &= ~(_BV(PINA4));//pin0 of portD as INPUT

    UCSR1A = 0 | _BV(U2X0);

    ser_changebaud1(baud);

    _written1 = 0;

    UCSR1C = SERIAL_8N1;
    UCSR1B |= _BV(RXEN1);
    UCSR1B |= _BV(TXEN1);
    UCSR1B |= _BV(RXCIE1);
    UCSR1B &= ~(_BV(UDRIE1));
}

uint8_t ser_read1(void)
{
    // if the head isn't ahead of the tail, we don't have any characters
    if (_rx_buffer_head1 == _rx_buffer_tail1) {
        return 0;
    } else {
        uint8_t c = _rx_buffer1[_rx_buffer_tail1];
        _rx_buffer_tail1 = (int8_t)(_rx_buffer_tail1 + 1) % SERIAL_RX_BUFFER_SIZE1;
        return c;
    }
}

int8_t ser_write1(uint8_t c)
{
    _written1 = 1;
    // If the buffer and the data register is empty, just write the byte
    // to the data register and be done. This shortcut helps
    // significantly improve the effective datarate at high (>
    // 500kbit/s) bitrates, where interrupt overhead becomes a slowdown.
    if (_tx_buffer_head1 == _tx_buffer_tail1 && bit_is_set(UCSR1A, UDRE1)) {
        UDR1 = c;
        UCSR1A |= _BV(TXC1);
    return 1;
    }
    uint8_t i = (_tx_buffer_head1 + 1) % SERIAL_TX_BUFFER_SIZE1;

    // If the output buffer is full, there's nothing for it other than to 
    // wait for the interrupt handler to empty it a bit
    while (i == _tx_buffer_tail1) {
        if (bit_is_clear(SREG, SREG_I)) {
            // Interrupts are disabled, so we'll have to poll the data
            // register empty flag ourselves. If it is set, pretend an
            // interrupt has happened and call the handler to free up
            // space for us.
            if(bit_is_set(UCSR1A, UDRE1)) {
                ser_tx_udr_empty_irq1();
            }
        } else {
            // nop, the interrupt handler will free up space for us
        }
    }

    _tx_buffer1[_tx_buffer_head1] = c;
    _tx_buffer_head1 = i;

    UCSR1B |= _BV(UDRIE1);

    return 1;
}

int ser1_putchar(char c, FILE *stream)
{
    ser_write1((uint8_t)c);
    return 0;
}

int8_t ser_available1(void)
{
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE1 + _rx_buffer_head1 - _rx_buffer_tail1)) % SERIAL_RX_BUFFER_SIZE1;
}

void ser_changebaud1(uint32_t baud1)
{
    uint32_t baud_setting = (F_CPU  / (8 * baud1) - 1) ;
    UBRR1H = baud_setting >> 8;
    UBRR1L = baud_setting;
}

void ser_flush1()
{
    if(!_written1) {
        return;
    }
    while(bit_is_set(UCSR1B, UDRIE1) || bit_is_clear(UCSR1A, TXC1)) {
        if (bit_is_set(SREG, SREG_I) && bit_is_set(UCSR1B, UDRIE1)) {
            if (bit_is_set(UCSR1A, UDRE1)) {
                ser_tx_udr_empty_irq1();
            }
        }
    }
}
