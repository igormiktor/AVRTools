/*
    USARTMinimal.cpp - Minimal, light-weight functions to use the
    USARTs available on AVR processors (no buffering).
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





#include "USARTMinimal.h"

#include <stdint.h>

#include <avr/io.h>





void initUSART0( unsigned long baudRate )
{
    uint16_t baudSetting = F_CPU / ( baudRate * 16UL ) - 1;

    UBRR0H = baudSetting >> 8;                      // shift the register right by 8 bits
    UBRR0L = baudSetting;                           // set baud rate
    UCSR0B |= ( 1 << TXEN0 ) | ( 1 << RXEN0 );      // enable receiver and transmitter
    UCSR0C |= ( 1 << UCSZ00 ) | ( 1 << UCSZ01 );    // 8 bit data format
}


void transmitUSART0( unsigned char data )
{
    while ( !( UCSR0A & (1<<UDRE0) ) )              // wait while register is free
        ;
    UDR0 = data;                                    // load data in the register
}


void transmitUSART0( unsigned char* data )
{
    if ( data )
    {
        while ( *data )
        {
            while ( !( UCSR0A & (1<<UDRE0) ) )              // wait while register is free
                ;
            UDR0 = *data++;                                 // load data in the register
        }
    }
}


unsigned char receiveUSART0()
{
    while ( !UCSR0A & (1<<RXC0) )                     // wait while data is being received
        ;
    return UDR0;                                     // return 8-bit data
}


void releaseUSART0()
{
    UCSR0B = 0;
}




#if defined(__AVR_ATmega2560__)




void initUSART1( unsigned long baudRate )
{
    uint16_t baudSetting = F_CPU / ( baudRate * 16UL ) - 1;

    UBRR1H = baudSetting >> 8;                      // shift the register right by 8 bits
    UBRR1L = baudSetting;                           // set baud rate
    UCSR1B |= ( 1 << TXEN1 ) | ( 1 << RXEN1 );      // enable receiver and transmitter
    UCSR1C |= ( 1 << UCSZ10 ) | ( 1 << UCSZ11 );    // 8 bit data format
}


void transmitUSART1( unsigned char data )
{
    while ( !( UCSR1A & (1<<UDRE1) ) )              // wait while register is free
        ;
    UDR1 = data;                                    // load data in the register
}


void transmitUSART1( unsigned char* data )
{
    if ( data )
    {
        while ( *data )
        {
            while ( !( UCSR1A & (1<<UDRE1) ) )              // wait while register is free
                ;
            UDR1 = *data++;                                 // load data in the register
        }
    }
}


unsigned char receiveUSART1()
{
    while ( !UCSR1A & (1<<RXC1) )                     // wait while data is being received
        ;
    return UDR1;                                     // return 8-bit data
}


void releaseUSART1()
{
    UCSR1B = 0;
}




void initUSART2( unsigned long baudRate )
{
    uint16_t baudSetting = F_CPU / ( baudRate * 16UL ) - 1;

    UBRR2H = baudSetting >> 8;                      // shift the register right by 8 bits
    UBRR2L = baudSetting;                           // set baud rate
    UCSR2B |= ( 1 << TXEN2 ) | ( 1 << RXEN2 );      // enable receiver and transmitter
    UCSR2C |= ( 1 << UCSZ20 ) | ( 1 << UCSZ21 );    // 8 bit data format
}


void transmitUSART2( unsigned char data )
{
    while ( !( UCSR2A & (1<<UDRE2) ) )              // wait while register is free
        ;
    UDR2 = data;                                    // load data in the register
}


void transmitUSART2( unsigned char* data )
{
    if ( data )
    {
        while ( *data )
        {
            while ( !( UCSR2A & (1<<UDRE2) ) )              // wait while register is free
                ;
            UDR2 = *data++;                                 // load data in the register
        }
    }
}


unsigned char receiveUSART2()
{
    while ( !UCSR2A & (1<<RXC2) )                     // wait while data is being received
        ;
    return UDR2;                                     // return 8-bit data
}


void releaseUSART2()
{
    UCSR2B = 0;
}




void initUSART3( unsigned long baudRate )
{
    uint16_t baudSetting = F_CPU / ( baudRate * 16UL ) - 1;

    UBRR3H = baudSetting >> 8;                      // shift the register right by 8 bits
    UBRR3L = baudSetting;                           // set baud rate
    UCSR3B |= ( 1 << TXEN3 ) | ( 1 << RXEN3 );      // enable receiver and transmitter
    UCSR3C |= ( 1 << UCSZ30 ) | ( 1 << UCSZ31 );    // 8 bit data format
}


void transmitUSART3( unsigned char data )
{
    while ( !( UCSR3A & (1<<UDRE3) ) )              // wait while register is free
        ;
    UDR3 = data;                                    // load data in the register
}


void transmitUSART3( unsigned char* data )
{
    if ( data )
    {
        while ( *data )
        {
            while ( !( UCSR3A & (1<<UDRE3) ) )              // wait while register is free
                ;
            UDR3 = *data++;                                 // load data in the register
        }
    }
}


unsigned char receiveUSART3()
{
    while ( !UCSR3A & (1<<RXC3) )                     // wait while data is being received
        ;
    return UDR3;                                     // return 8-bit data
}


void releaseUSART3()
{
    UCSR3B = 0;
}


#endif
