/*
    USART1.cpp - Functions and classes to use USART1 on AVR systems for
    serial I/O (includes buffering).
    For AVR ATmega2560 (Arduino Mega).
    This is part of the AVRTools library.
    Copyright (c) 2015 Igor Mikolic-Torreira.  All right reserved.
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


#if !defined(__AVR_ATmega2560__)
#error "USART1 doesn't exist on ATmega328p (Arduino Uno); you can only use this on ATmega2560 (Arduino Mega)."
#endif


#include "USART1.h"

#include <stdint.h>

#include <avr/io.h>
#include <util/atomic.h>

#include "RingBuffer.h"


#ifndef USART1_RX_BUFFER_SIZE
#define USART1_RX_BUFFER_SIZE   32
#endif

#ifndef USART1_TX_BUFFER_SIZE
#define USART1_TX_BUFFER_SIZE   64
#endif


#if USART1_RX_BUFFER_SIZE > 255
#error "USART1_RX_BUFFER_SIZE exceeds size of a uint8_t"
#endif

#if USART1_TX_BUFFER_SIZE > 255
#error "USART1_TX_BUFFER_SIZE exceeds size of a uint8_t"
#endif




namespace
{

    unsigned char rxStorage[ USART1_RX_BUFFER_SIZE ];
    RingBuffer rxBuffer( rxStorage, USART1_RX_BUFFER_SIZE );

    unsigned char txStorage[ USART1_TX_BUFFER_SIZE ];
    RingBuffer txBuffer( txStorage, USART1_TX_BUFFER_SIZE );

};




#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

ISR( USART1_RX_vect )
{
    // If no parity error, put it in the rx buffer
    // Eitherway, we need to read UDR register to clear the interrupt
    if ( !( UCSR1A & (1<<UPE1) ) )
    {
        unsigned char c = UDR1;
        rxBuffer.push( c );
    }
    else
    {
        unsigned char c = UDR1;
    }
}

#pragma GCC diagnostic pop



ISR( USART1_UDRE_vect )
{
    if ( txBuffer.isNotEmpty() )
    {
        // Send the next byte
        UDR1 = txBuffer.pull();
    }
    else
    {
        // Nothing more to transmit so disable UDRE interrupts
        UCSR1B &= ~( 1 << UDRIE1 );
    }
}





void USART1::start( unsigned long baudRate, UsartSerialConfiguration config )
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
        UCSR1A &= ~( (1<<U2X1) | (1<<MPCM1) );
        UCSR1B &= ~( (1<<RXCIE1) | (1<<TXCIE1) | (1<<UDRIE1) | (1<<RXEN1) | (1<<TXEN1)
                        | (1<< UCSZ12) | (1<<TXB81) );
        UCSR1C = kSerial_8N1;

        // Set baud rate
        UBRR1H = baudSetting >> 8;
        UBRR1L = baudSetting;
        if ( use2x )
        {
            UCSR1A |= ( 1 << U2X1 );
        }
        else
        {
            UCSR1A &= ~( 1 << U2X1 );
        }

        // Turn on TX and RX
        UCSR1B |= ( 1 << RXEN1 ) | ( 1 << TXEN1 );

        // Set data bits, stop bits, and parity
        // UCSR1C = static_cast<unsigned char>( config );
        UCSR1C |= (1<<UCSZ10) | (1<<UCSZ11);

        // Configure interrupts
        UCSR1B |= ( 1 << RXCIE1 ) | ( 1 << UDRIE1 );
    }
}



void USART1::stop()
{
    flush();

    // Turn off TX, RX, and interrupts
    UCSR1B &= ~( (1<<RXCIE1) | (1<<TXCIE1) | (1<<UDRIE1) | (1<<RXEN1) | (1<<TXEN1) );

    // Clear the receive buffer
    rxBuffer.clear();
}



void USART1::flush()
{
  // UDRE interrupt keeps transmitting until transmit buffer is empty.
  // Just wait for the bit that tells us transmission done and nothing else to send (UDR empty).
  while ( txBuffer.isNotEmpty() && !( UCSR1A & (1<<TXC1) ) )
      ;

  // Clear TXCO by writing a 1 (not a typo)
  UCSR1A |= ( 1 << TXC1 );
}



int USART1::peek()
{
    return rxBuffer.peek();
}



int USART1::read()
{
    return rxBuffer.pull();
}



size_t USART1::write( char c )
{
    // If buffer is full, wait...
    while ( txBuffer.isFull() )
        ;
    txBuffer.push( static_cast<unsigned char>( c ) );

    // Set UDRE interrupt
    UCSR1B |= ( 1 << UDRIE1 );

    // Clear TXC flag by writing a 1 (*not* a typo)
    UCSR1A |= ( 1 << TXC1 );

    return 1;
}



size_t USART1::write( const char* c )
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
            UCSR1B |= ( 1 << UDRIE1 );
        }

        // Clear TXC flag by writing a 1 (*not* a typo); suffices to do this at the end
        UCSR1A |= ( 1 << TXC1 );
    }

    return cnt;
}




size_t USART1::write( const char* c, size_t n )
{
    return write( reinterpret_cast< const uint8_t*>( c ) , n );
}




size_t USART1::write( const uint8_t* c, size_t n )
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
            UCSR1B |= ( 1 << UDRIE1 );
        }

        // Clear TXC flag by writing a 1 (*not* a typo); suffices to do this at the end
        UCSR1A |= ( 1 << TXC1);
    }

    return cnt;
}




bool USART1::available()
{
    return !rxBuffer.isEmpty();
}







size_t Serial1::write( char c )
{
    return USART1::write( c );
}

size_t Serial1::write( const char* str )
{
    return USART1::write( str );
}

size_t Serial1::write( const char* buffer, size_t size )
{
    return USART1::write( buffer, size );
}

size_t Serial1::write( const uint8_t* buffer, size_t size )
{
    return USART1::write( buffer, size );
}


void Serial1::flush()
{
    USART1::flush();
}

int Serial1::read()
{
    return USART1::read();
}

int Serial1::peek()
{
    return USART1::peek();
}


bool Serial1::available()
{
    return USART1::available();
}



