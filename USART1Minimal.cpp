/*
    USART1Minimal.cpp - Minimal, light-weight functions to use
    USART1 available on ATMega2560 (Arduino Mega) processors (no buffering).
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





#include "USART1Minimal.h"

#include <stdint.h>

#include <avr/io.h>





#if defined(__AVR_ATmega2560__)




void initUSART1( unsigned long baudRate )
{
    uint16_t baudSetting = ((F_CPU + baudRate * 8L) / (baudRate * 16L) - 1);

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



#endif
