/*
    USART2Minimal.h - Minimal, light-weight functions to use
    USART2 available on ATMega2560 (Arduino Mega) processors (no buffering).
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



/*!
 * \file
 *
 * \brief This file provides functions that provide a minimalist interface to USART2
 * available on the Arduino Mega (ATmega2560).
 *
 * These functions are minimalist in the following sense:
 * - They only send sigle bytes or zero-terminated character strings.
 * - They only receive single characters.
 * - They do not use the USART-related interrupts.
 * - They determine when the USART is ready to send by polling the relevant register bit.
 * - They determine when the USART has received data by polling the relevant register bit.
 *
 * To use these functions, include USART2Minimal.h in your source code and link against USART2Minimal.cpp.
 *
 * For a more advanced USART2 interface, consider using either the USART2 or Serial2 interfaces.
 * Both of these are available by including USART2.h instead of USART2Minimal.h.
 *
 */



#ifndef USART2Minimal_h
#define USART2Minimal_h



#if defined(__AVR_ATmega2560__)



/*!
 * \brief Initialize USART2 for serial receive and transmit.
 *
 * USART2 is tied to pins 16 (TX) and 17 (RX) on Arduino Mega (ATmega2560 pins PH1, PH0).
 *
 * Communications are configured for 8 data bits, no parity, and 1 stop bit.
 *
 * \arg \c baudRate the baud rate for the communications, usually one of the following
 * values: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200
 * (although other values below can be specified).
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

void initUSART2( unsigned long baudRate );


/*!
 * \brief Transmit a single byte on USART2.
 *
 * You must first initialize USART2 by calling initUSART2().
 *
 * This function blocks until the USART becomes available and the byte can be transmitted.
 *
 * \arg \c data the byte to be transmitted.
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

void transmitUSART2( unsigned char data );


/*!
 * \brief Transmit a null-terminated string on USART2.
 *
 * You must first initialize USART2 by calling initUSART2().
 *
 * This function blocks until the USART becomes available and all the bytes can be transmitted.
 *
 * \arg \c data the null-terminated string to be transmitted.
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

void transmitUSART2( const char* data );


/*!
 * \brief Receive a byte on USART2.
 *
 * You must first initialize USART2 by calling initUSART2().
 *
 * This function blocks until the USART receives a byte.
 *
 * \returns the byte received.
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

unsigned char receiveUSART2();


/*!
 * \brief Release USART2, making pins 0 and 1 again available for non-USART use.
 *
 * After calling this function, you cannot read or write to the USART unless
 * you first call initUSART2().
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

void releaseUSART2();



#endif



#endif
