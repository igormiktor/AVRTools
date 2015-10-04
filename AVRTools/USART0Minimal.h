/*
    USART0Minimal.h - Minimal, light-weight functions to use
    USART0 available on AVR ATMega328p (Arduino Uno) and
    ATMega2560 (Arduino Mega) processors (no buffering).
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



/*!
 * \file
 *
 * \brief This file provides functions that provide a minimalist interface to USART0
 * available on the Arduino Uno (ATmega328) and Arduino Mega (ATmega2560).
 *
 * These functions are minimalist in the following sense:
 * - They only send sigle bytes or zero-terminated character strings.
 * - They only receive single characters.
 * - They do not use the USART-related interrupts.
 * - They determine when the USART is ready to send by polling the relevant register bit.
 * - They determine when the USART has received data by polling the relevant register bit.
 *
 * To use these functions, include USART0Minimal.h in your source code and link against USART0Minimal.cpp.
 *
 * For a more advanced USART0 interface, consider using either the USART0 or Serial0 interfaces.
 * Both of these are available by including USART0.h instead of USART0Minimal.h.
 *
 */



#ifndef USART0Minimal_h
#define USART0Minimal_h



/*!
 * \brief Initialize %USART0 for serial receive and transmit.
 *
 * %USART0 is tied to pins 0 (RX) and 1 (TX) on both Arduino Uno (ATmega328 pins PD0, PD1)
 * and Arduino Mega (ATmega2560 pins PE0, PE1).
 *
 * Communications are configured for 8 data bits, no parity, and 1 stop bit.
 *
 * \arg \c baudRate the baud rate for the communications, usually one of the following
 * values: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200
 * (although other values below can be specified).
 */

void initUSART0( unsigned long baudRate );


/*!
 * \brief Transmit a single byte on %USART0.
 *
 * You must first initialize %USART0 by calling initUSART0().
 *
 * This function blocks until the USART becomes available and the byte can be transmitted.
 *
 * \arg \c data the byte to be transmitted.
 */

void transmitUSART0( unsigned char data );


/*!
 * \brief Transmit a null-terminated string on %USART0.
 *
 * You must first initialize %USART0 by calling initUSART0().
 *
 * This function blocks until the USART becomes available and all the bytes can be transmitted.
 *
 * \arg \c data the null-terminated string to be transmitted.
 */

void transmitUSART0( const char* data );


/*!
 * \brief Receive a byte on %USART0.
 *
 * You must first initialize %USART0 by calling initUSART0().
 *
 * This function blocks until the USART receives a byte.
 *
 * \returns the byte received.
 */

unsigned char receiveUSART0();


/*!
 * \brief Release %USART0, making pins 0 and 1 again available for non-USART use.
 *
 * After calling this function, you cannot read or write to the USART unless
 * you first call initUSART0().
 */

void releaseUSART0();




#endif
