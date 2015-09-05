/*
    ArduinoUnoPins.h - Macros naming the pins on the Arduino Uno.
    For AVR ATMega328p (Arduino Uno) and ATMega2560 (Arduino Mega).
    This is part of the AVRTools library.
    Copyright (c) 2014 Igor Mikolic-Torreira.  All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/*!
 * \file
 *
 * \brief This file defines the standard Arduino Uno pin name macros.  It may be
 * included directly by user code, although more commonly user code includes the
 * file ArduinoPins.h, which in turn includes this file (when compiling for Arduino
 * Uno targets).
 *
 * The standard Arduino Uno digital pins will be defined as pPin00 through pPin13.
 *
 * The standard Arduino Uno analog pins will be defined as pPinA00 through pPinA07.
 *
 * Additionally, the I2C SDA and SCL pins are also defined as pSDA and pSCL (these
 * are synonyms for pPinA04 and pPinA05, respectively).
 *
 */



#ifndef ArduinoUnoPins_h
#define ArduinoUnoPins_h

#ifndef ArduinoPinsDefined
#define ArduinoPinsDefined
#else
#error "Only include one Arduino model pin definition file; more than one appears to be included"
#endif



#include "GpioPinMacros.h"


#define pPinA00             GpioPinAnalog( C, 0, 0 )        // PC0, ADC0, PCINT8
#define pPinA01             GpioPinAnalog( C, 1, 1 )        // PC1, ADC1, PCINT9
#define pPinA02             GpioPinAnalog( C, 2, 2 )        // PC2, ADC2, PCINT10
#define pPinA03             GpioPinAnalog( C, 3, 3 )        // PC3, ADC3, PCINT11
#define pPinA04             GpioPinAnalog( C, 4, 4 )        // PC4, ADC4, SDA, PCINT12
#define pPinA05             GpioPinAnalog( C, 5, 5 )        // PC5, ADC5, SCL, PCINT13

#define pPin00              GpioPin( D, 0 )                 // PD0, RXD, PCINT16
#define pPin01              GpioPin( D, 1 )                 // PD1, TXD, PCINT17
#define pPin02              GpioPin( D, 2 )                 // PD2, INT0, PCINT18
#define pPin03              GpioPinPwm( D, 3, 2, B )        // PD3, INT1, OC2B, PCINT19
#define pPin04              GpioPin( D, 4 )                 // PD4, T0, XCK, PCINT20
#define pPin05              GpioPinPwm( D, 5, 0, B )        // PD5, T1, OC0B, PCINT21
#define pPin06              GpioPinPwm( D, 6, 0, A )        // PD6, AIN0, OC0A, PCINT22
#define pPin07              GpioPin( D, 7 )                 // PD7, AIN1, PCINT23
#define pPin08              GpioPin( B, 0 )                 // PB0, ICP1, CLKO, PCINT0
#define pPin09              GpioPinPwm( B, 1, 1, A )        // PB1, OC1A, PCINT1
#define pPin10              GpioPinPwm( B, 2, 1, B )        // PB2, SS, OC1B, PCINT2
#define pPin11              GpioPinPwm( B, 3, 2, A )        // PB3, MOSI, OC2A, PCINT3
#define pPin12              GpioPin( B, 4 )                 // PB4, MISO, PCINT4
#define pPin13              GpioPin( B, 5 )                 // PB5, SCK, PCINT5

#define pSDA                pPinA04                         // I2C SDA
#define pSCL                pPinA05                         // I2C SCL

#define pSS                 pPin10                          // SPI SS
#define pMOSI               pPin11                          // SPI MOSI
#define pMISO               pPin12                          // SPI MISO
#define pSCK                pPin13                          // SPI SCK


#endif
