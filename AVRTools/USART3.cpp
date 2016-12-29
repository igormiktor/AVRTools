/*
    USART3.cpp - Functions and classes to use USART3 on AVR systems for
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
#error "USART3 doesn't exist on ATmega328p (Arduino Uno); you can only use this on ATmega2560 (Arduino Mega)."
#endif


#include "USART3.h"

#include <stdint.h>

#include <avr/io.h>
#include <util/atomic.h>

#include "RingBuffer.h"


#ifndef USART3_RX_BUFFER_SIZE
#define USART3_RX_BUFFER_SIZE   32
#endif

#ifndef USART3_TX_BUFFER_SIZE
#define USART3_TX_BUFFER_SIZE   64
#endif


#if USART3_RX_BUFFER_SIZE > 255
#error "USART3_RX_BUFFER_SIZE exceeds size of a uint8_t"
#endif

#if USART3_TX_BUFFER_SIZE > 255
#error "USART3_TX_BUFFER_SIZE exceeds size of a uint8_t"
#endif




namespace
{

    unsigned char rxStorage[ USART3_RX_BUFFER_SIZE ];
    RingBuffer rxBuffer( rxStorage, USART3_RX_BUFFER_SIZE );

    unsigned char txStorage[ USART3_TX_BUFFER_SIZE ];
    RingBuffer txBuffer( txStorage, USART3_TX_BUFFER_SIZE );

};





#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

ISR( USART3_RX_vect )
{
    // If no parity error, put it in the rx buffer
    // Eitherway, we need to read UDR register to clear the interrupt
    if ( !( UCSR3A & (1<<UPE3) ) )
    {
        unsigned char c = UDR3;
        rxBuffer.push( c );
    }
    else
    {
        unsigned char c = UDR3;
    }
}

#pragma GCC diagnostic pop



ISR( USART3_UDRE_vect )
{
    if ( txBuffer.isNotEmpty() )
    {
        // Send the next byte
        UDR3 = txBuffer.pull();
    }
    else
    {
        // Nothing more to transmit so disable UDRE interrupts
        UCSR3B &= ~( 1 << UDRIE3 );
    }
}





void USART3::start( unsigned long baudRate, UsartSerialConfiguration config )
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
        UCSR3A &= ~( (1<<U2X3) | (1<<MPCM3) );
        UCSR3B &= ~( (1<<RXCIE3) | (1<<TXCIE3) | (1<<UDRIE3) | (1<<RXEN3) | (1<<TXEN3)
                        | (1<< UCSZ32) | (1<<TXB83) );
        UCSR3C = kSerial_8N1;

        // Set baud rate
        UBRR3H = baudSetting >> 8;
        UBRR3L = baudSetting;
        if ( use2x )
        {
            UCSR3A |= ( 1 << U2X3 );
        }
        else
        {
            UCSR3A &= ~( 1 << U2X3 );
        }

        // Turn on TX and RX
        UCSR3B |= ( 1 << RXEN3 ) | ( 1 << TXEN3 );

        // Set data bits, stop bits, and parity
        // UCSR3C = static_cast<unsigned char>( config );
        UCSR3C |= (1<<UCSZ30) | (1<<UCSZ31);

        // Configure interrupts
        UCSR3B |= ( 1 << RXCIE3 ) | ( 1 << UDRIE3 );
    }
}



void USART3::stop()
{
    flush();

    // Turn off TX, RX, and interrupts
    UCSR3B &= ~( (1<<RXCIE3) | (1<<TXCIE3) | (1<<UDRIE3) | (1<<RXEN3) | (1<<TXEN3) );

    // Clear the receive buffer
    rxBuffer.clear();
}



void USART3::flush()
{
  // UDRE interrupt keeps transmitting until transmit buffer is empty.
  // Just wait for the bit that tells us transmission done and nothing else to send (UDR empty).
  while ( txBuffer.isNotEmpty() && !( UCSR3A & (1<<TXC3) ) )
      ;

  // Clear TXCO by writing a 1 (not a typo)
  UCSR3A |= ( 1 << TXC3 );
}



int USART3::peek()
{
    return rxBuffer.peek();
}



int USART3::read()
{
    return rxBuffer.pull();
}



size_t USART3::write( char c )
{
    // If buffer is full, wait...
    while ( txBuffer.isFull() )
        ;
    txBuffer.push( static_cast<unsigned char>( c ) );

    // Set UDRE interrupt
    UCSR3B |= ( 1 << UDRIE3 );

    // Clear TXC flag by writing a 1 (*not* a typo)
    UCSR3A |= ( 1 << TXC3 );

    return 1;
}



size_t USART3::write( const char* c )
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
            UCSR3B |= ( 1 << UDRIE3 );
        }

        // Clear TXC flag by writing a 1 (*not* a typo); suffices to do this at the end
        UCSR3A |= ( 1 << TXC3 );
    }

    return cnt;
}




size_t USART3::write( const char* c, size_t n )
{
    return write( reinterpret_cast< const uint8_t*>( c ) , n );
}




size_t USART3::write( const uint8_t* c, size_t n )
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
            UCSR3B |= ( 1 << UDRIE3 );
        }

        // Clear TXC flag by writing a 1 (*not* a typo); suffices to do this at the end
        UCSR3A |= ( 1 << TXC3 );
    }

    return cnt;
}




bool USART3::available()
{
    return !rxBuffer.isEmpty();
}







size_t Serial3::write( char c )
{
    return USART3::write( c );
}

size_t Serial3::write( const char* str )
{
    return USART3::write( str );
}

size_t Serial3::write( const char* buffer, size_t size )
{
    return USART3::write( buffer, size );
}

size_t Serial3::write( const uint8_t* buffer, size_t size )
{
    return USART3::write( buffer, size );
}


void Serial3::flush()
{
    USART3::flush();
}

int Serial3::read()
{
    return USART3::read();
}

int Serial3::peek()
{
    return USART3::peek();
}


bool Serial3::available()
{
    return USART3::available();
}



