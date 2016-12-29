/*
    USART0.cpp - Functions and classes to use USART0 on AVR systems for
    serial I/O (includes buffering).
    For AVR ATMega328p (Arduino Uno) and ATMega2560 (Arduino Mega).
    This is part of the AVRTools library.
    Copyright (c) 2014 Igor Mikolic-Torreira.  All right reserved.
    Functions readlong() and readFloat() adapted from Arduino code that
    is Copyright (c) 2005-2006 David A. Mellis and licensed under LGPL.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "USART0.h"

#include <stdint.h>

#include <avr/io.h>
#include <util/atomic.h>

#include "RingBuffer.h"


#ifndef USART0_RX_BUFFER_SIZE
#define USART0_RX_BUFFER_SIZE   32
#endif

#ifndef USART0_TX_BUFFER_SIZE
#define USART0_TX_BUFFER_SIZE   64
#endif


#if USART0_RX_BUFFER_SIZE > 255
#error "USART0_RX_BUFFER_SIZE exceeds size of a uint8_t"
#endif

#if USART0_TX_BUFFER_SIZE > 255
#error "USART0_TX_BUFFER_SIZE exceeds size of a uint8_t"
#endif




namespace
{

    unsigned char rxStorage[ USART0_RX_BUFFER_SIZE ];
    RingBuffer rxBuffer( rxStorage, USART0_RX_BUFFER_SIZE );

    unsigned char txStorage[ USART0_TX_BUFFER_SIZE ];
    RingBuffer txBuffer( txStorage, USART0_TX_BUFFER_SIZE );

};




#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

#if defined(__AVR_ATmega2560__)
ISR( USART0_RX_vect )
#else
ISR( USART_RX_vect )
#endif
{
    // If no parity error, put it in the rx buffer
    // Eitherway, we need to read UDR register to clear the interrupt
    if ( !( UCSR0A & (1<<UPE0) ) )
    {
        unsigned char c = UDR0;
        rxBuffer.push( c );
    }
    else
    {
        unsigned char c = UDR0;
    }
}

#pragma GCC diagnostic pop




#if defined(__AVR_ATmega2560__)
ISR( USART0_UDRE_vect )
#else
ISR( USART_UDRE_vect )
#endif
{
    if ( txBuffer.isNotEmpty() )
    {
        // Send the next byte
        UDR0 = txBuffer.pull();
    }
    else
    {
        // Nothing more to transmit so disable UDRE interrupts
        UCSR0B &= ~( 1 << UDRIE0 );
    }
}





void USART0::start( unsigned long baudRate, UsartSerialConfiguration config )
{
    bool use2x = true;
    uint16_t baudSetting = (F_CPU + baudRate * 4L) / ( 8L * baudRate ) - 1;
    if ( baudSetting > 4095 || baudRate == 57600)
    {
        use2x = false;
        baudSetting = (F_CPU + baudRate * 8L) / (baudRate * 16L) - 1;
    }

    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        // Asynchronous mode, with everything else off
        UCSR0A &= ~( (1<<U2X0) | (1<<MPCM0) );
        UCSR0B &= ~( (1<<RXCIE0) | (1<<TXCIE0) | (1<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0)
                        | (1<< UCSZ02) | (1<<TXB80) );
        UCSR0C = kSerial_8N1;

        // Set baud rate
        UBRR0H = baudSetting >> 8;
        UBRR0L = baudSetting;
        if ( use2x )
        {
            UCSR0A |= ( 1 << U2X0 );
        }
        else
        {
            UCSR0A &= ~( 1 << U2X0 );
        }

        // Turn on TX and RX
        UCSR0B |= ( 1 << RXEN0 ) | ( 1 << TXEN0 );

        // Set data bits, stop bits, and parity
        // UCSR0C = static_cast<unsigned char>( config );
        UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);

        // Configure interrupts
        UCSR0B |= ( 1 << RXCIE0 ) | ( 1 << UDRIE0 );
    }
}



void USART0::stop()
{
    flush();

    // Turn off TX, RX, and interrupts
    UCSR0B &= ~( (1<<RXCIE0) | (1<<TXCIE0) | (1<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0) );

    // Clear the receive buffer
    rxBuffer.clear();
}



void USART0::flush()
{
  // UDRE interrupt keeps transmitting until transmit buffer is empty.
  // Just wait for the bit that tells us transmission done and nothing else to send (UDR empty).
  while ( txBuffer.isNotEmpty() && !( UCSR0A & (1<<TXC0) ) )
      ;

  // Clear TXCO by writing a 1 (not a typo)
  UCSR0A |= ( 1 << TXC0 );
}



int USART0::peek()
{
    return rxBuffer.peek();
}



int USART0::read()
{
    return rxBuffer.pull();
}



size_t USART0::write( char c )
{
    // If buffer is full, wait...
    while ( txBuffer.isFull() )
        ;
    txBuffer.push( static_cast<unsigned char>( c ) );

    // Set UDRE interrupt
    UCSR0B |= ( 1 << UDRIE0 );

    // Clear TXC flag by writing a 1 (*not* a typo)
    UCSR0A |= ( 1 << TXC0 );

    return 1;
}



size_t USART0::write( const char* c )
{
    unsigned short cnt = 0;
    if ( c )
    {
        while ( *c )
        {
            // If buffer is full, wait...
            while ( txBuffer.isFull() )
                ;

            txBuffer.push( static_cast<unsigned char>( *c++ ) );
            ++cnt;

            // Set UDRE interrupt (each time in case interrupt fires and clears in between)
            UCSR0B |= ( 1 << UDRIE0 );
        }

        // Clear TXC flag by writing a 1 (*not* a typo); suffices to do this at the end
        UCSR0A |= ( 1 << TXC0 );
    }

    return cnt;
}




size_t USART0::write( const char* c, size_t n )
{
    return write( reinterpret_cast< const uint8_t*>( c ) , n );
}




size_t USART0::write( const uint8_t* c, size_t n )
{
    size_t cnt = 0;
    if ( c )
    {
        while ( n-- )
        {
            // If buffer is full, wait...
            while ( txBuffer.isFull() )
                ;

            txBuffer.push( *c++ );
            ++cnt;

            // Set UDRE interrupt (each time in case interrupt fires and clears in between)
            UCSR0B |= ( 1 << UDRIE0 );
        }

        // Clear TXC flag by writing a 1 (*not* a typo); suffices to do this at the end
        UCSR0A |= ( 1 << TXC0 );
    }

    return cnt;
}




bool USART0::available()
{
    return !rxBuffer.isEmpty();
}







size_t Serial0::write( char c )
{
    return USART0::write( c );
}

size_t Serial0::write( const char* str )
{
    return USART0::write( str );
}

size_t Serial0::write( const char* buffer, size_t size )
{
    return USART0::write( buffer, size );
}

size_t Serial0::write( const uint8_t* buffer, size_t size )
{
    return USART0::write( buffer, size );
}


void Serial0::flush()
{
    USART0::flush();
}

int Serial0::read()
{
    return USART0::read();
}

int Serial0::peek()
{
    return USART0::peek();
}


bool Serial0::available()
{
    return USART0::available();
}



