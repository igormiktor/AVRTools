/*
    USART2.cpp - Functions and classes to use USART2 on AVR systems for
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
#error "USART2 doesn't exist on ATmega328p (Arduino Uno); you can only use this on ATmega2560 (Arduino Mega)."
#endif


#include "USART2.h"

#include <stdint.h>

#include <avr/io.h>
#include <util/atomic.h>

#include "RingBuffer.h"


#ifndef USART2_RX_BUFFER_SIZE
#define USART2_RX_BUFFER_SIZE   32
#endif

#ifndef USART2_TX_BUFFER_SIZE
#define USART2_TX_BUFFER_SIZE   64
#endif


#if USART2_RX_BUFFER_SIZE > 255
#error "USART2_RX_BUFFER_SIZE exceeds size of a uint8_t"
#endif

#if USART2_TX_BUFFER_SIZE > 255
#error "USART2_TX_BUFFER_SIZE exceeds size of a uint8_t"
#endif




namespace
{

    unsigned char rxStorage[ USART2_RX_BUFFER_SIZE ];
    RingBuffer rxBuffer( rxStorage, USART2_RX_BUFFER_SIZE );

    unsigned char txStorage[ USART2_TX_BUFFER_SIZE ];
    RingBuffer txBuffer( txStorage, USART2_TX_BUFFER_SIZE );

};





#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

ISR( USART2_RX_vect )
{
    // If no parity error, put it in the rx buffer
    // Eitherway, we need to read UDR register to clear the interrupt
    if ( !( UCSR2A & (1<<UPE2) ) )
    {
        unsigned char c = UDR2;
        rxBuffer.push( c );
    }
    else
    {
        unsigned char c = UDR2;
    }
}

#pragma GCC diagnostic pop


ISR( USART2_UDRE_vect )
{
    if ( txBuffer.isNotEmpty() )
    {
        // Send the next byte
        UDR2 = txBuffer.pull();
    }
    else
    {
        // Nothing more to transmit so disable UDRE interrupts
        UCSR2B &= ~( 1 << UDRIE2 );
    }
}





void USART2::start( unsigned long baudRate, UsartSerialConfiguration config )
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
        UCSR2A &= ~( (1<<U2X2) | (1<<MPCM2) );
        UCSR2B &= ~( (1<<RXCIE2) | (1<<TXCIE2) | (1<<UDRIE2) | (1<<RXEN2) | (1<<TXEN2)
                        | (1<< UCSZ22) | (1<<TXB82) );
        UCSR2C = kSerial_8N1;

        // Set baud rate
        UBRR2H = baudSetting >> 8;
        UBRR2L = baudSetting;
        if ( use2x )
        {
            UCSR2A |= ( 1 << U2X2 );
        }
        else
        {
            UCSR2A &= ~( 1 << U2X2 );
        }

        // Turn on TX and RX
        UCSR2B |= ( 1 << RXEN2 ) | ( 1 << TXEN2 );

        // Set data bits, stop bits, and parity
        // UCSR2C = static_cast<unsigned char>( config );
        UCSR2C |= (1<<UCSZ20) | (1<<UCSZ21);

        // Configure interrupts
        UCSR2B |= ( 1 << RXCIE2 ) | ( 1 << UDRIE2 );
    }
}



void USART2::stop()
{
    flush();

    // Turn off TX, RX, and interrupts
    UCSR2B &= ~( (1<<RXCIE2) | (1<<TXCIE2) | (1<<UDRIE2) | (1<<RXEN2) | (1<<TXEN2) );

    // Clear the receive buffer
    rxBuffer.clear();
}



void USART2::flush()
{
  // UDRE interrupt keeps transmitting until transmit buffer is empty.
  // Just wait for the bit that tells us transmission done and nothing else to send (UDR empty).
  while ( txBuffer.isNotEmpty() && !( UCSR2A & (1<<TXC2) ) )
      ;

  // Clear TXCO by writing a 1 (not a typo)
  UCSR2A |= ( 1 << TXC2 );
}



int USART2::peek()
{
    return rxBuffer.peek();
}



int USART2::read()
{
    return rxBuffer.pull();
}



size_t USART2::write( char c )
{
    // If buffer is full, wait...
    while ( txBuffer.isFull() )
        ;
    txBuffer.push( static_cast<unsigned char>( c ) );

    // Set UDRE interrupt
    UCSR2B |= ( 1 << UDRIE2 );

    // Clear TXC flag by writing a 1 (*not* a typo)
    UCSR2A |= ( 1 << TXC2 );

    return 1;
}



size_t USART2::write( const char* c )
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
            UCSR2B |= ( 1 << UDRIE2 );
        }

        // Clear TXC flag by writing a 1 (*not* a typo); suffices to do this at the end
        UCSR2A |= ( 1 << TXC2 );
    }

    return cnt;
}




size_t USART2::write( const char* c, size_t n )
{
    return write( reinterpret_cast< const uint8_t*>( c ) , n );
}




size_t USART2::write( const uint8_t* c, size_t n )
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
            UCSR2B |= ( 1 << UDRIE2 );
        }

        // Clear TXC flag by writing a 1 (*not* a typo); suffices to do this at the end
        UCSR2A |= ( 1 << TXC2 );
    }

    return cnt;
}




bool USART2::available()
{
    return !rxBuffer.isEmpty();
}







size_t Serial2::write( char c )
{
    return USART2::write( c );
}

size_t Serial2::write( const char* str )
{
    return USART2::write( str );
}

size_t Serial2::write( const char* buffer, size_t size )
{
    return USART2::write( buffer, size );
}

size_t Serial2::write( const uint8_t* buffer, size_t size )
{
    return USART2::write( buffer, size );
}


void Serial2::flush()
{
    USART2::flush();
}

int Serial2::read()
{
    return USART2::read();
}

int Serial2::peek()
{
    return USART2::peek();
}


bool Serial2::available()
{
    return USART2::available();
}



