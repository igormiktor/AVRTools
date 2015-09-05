/*
    ArduinoMegaPins.h - Macros naming the pins on the Arduino Mega.
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
 * The standard Arduino Uno digital pins will be defined as pPin00 through pPin53.
 *
 * The standard Arduino Uno analog pins will be defined as pPinA00 through pPinA15.
 *
 * Additionally, the I2C SDA and SCL pins are also defined as pSDA and pSCL (these
 * are synonyms for pPin20 and pPin21, respectively).
 *
 */


#ifndef ArduinoMegaPins_h
#define ArduinoMegaPins_h

#ifndef ArduinoPinsDefined
#define ArduinoPinsDefined
#else
#error "Only include one Arduino model pin definition file; more than one appears to be included"
#endif



#include "GpioPinMacros.h"


#define pPinA00             GpioPinAnalog( F, 0, 0 )        // PF0, ADC0
#define pPinA01             GpioPinAnalog( F, 1, 1 )        // PF1, ADC1
#define pPinA02             GpioPinAnalog( F, 2, 2 )        // PF2, ADC2
#define pPinA03             GpioPinAnalog( F, 3, 3 )        // PF3, ADC3
#define pPinA04             GpioPinAnalog( F, 4, 4 )        // PF4, ADC4, TCK
#define pPinA05             GpioPinAnalog( F, 5, 5 )        // PF5, ADC5, TMS
#define pPinA06             GpioPinAnalog( F, 6, 6 )        // PF5, ADC6, TDO
#define pPinA07             GpioPinAnalog( F, 7, 7 )        // PF5, ADC7, TDI

#define pPinA08             GpioPinAnalog( K, 0, 8 )        // PF0, ADC8, PCINT16
#define pPinA09             GpioPinAnalog( K, 1, 9 )        // PF1, ADC9, PCINT17
#define pPinA10             GpioPinAnalog( K, 2, 10 )       // PF2, ADC10, PCINT18
#define pPinA11             GpioPinAnalog( K, 3, 11 )       // PF3, ADC11, PCINT19
#define pPinA12             GpioPinAnalog( K, 4, 12 )       // PF4, ADC12, PCINT20
#define pPinA13             GpioPinAnalog( K, 5, 13 )       // PF5, ADC13, PCINT21
#define pPinA14             GpioPinAnalog( K, 6, 14 )       // PF5, ADC14, PCINT22
#define pPinA15             GpioPinAnalog( K, 7, 15 )       // PF5, ADC15, PCINT23

#define pPin00              GpioPin( E, 0 )                 // PE0, RXD0, PCINT8
#define pPin01              GpioPin( E, 1 )                 // PE1, TXD0, PCINT3
#define pPin02              GpioPinPwm( E, 4, 3, B )        // PE4, INT4, OC3B
#define pPin03              GpioPinPwm( E, 5, 3, C )        // PE5, INT5, OC3C
#define pPin04              GpioPinPwm( G, 5, 0, B )        // PG5, OC0B
#define pPin05              GpioPinPwm( E, 3, 3, A )        // PE3, AIN1, OC3A

#define pPin06              GpioPinPwm( H, 3, 4, A )        // PH3, OC4A, PCINT8
#define pPin07              GpioPinPwm( H, 4, 4, B )        // PH4, OC4B
#define pPin08              GpioPinPwm( H, 5, 4, C )        // PH5, OC4C
#define pPin09              GpioPinPwm( H, 6, 2, B )        // PH6, OC2B
#define pPin10              GpioPinPwm( B, 4, 2, A )        // PB4, OC2A, PCINT4

#define pPin11              GpioPinPwm( B, 5, 1, A )        // PB5, OC1A, PCINT5
#define pPin12              GpioPinPwm( B, 6, 1, B )        // PB6, OC1B, PCINT6
#define pPin13              GpioPinPwm( B, 7, 0, A )        // PB7, OC0A, PCINT7
#define pPin14              GpioPin( J, 1 )                 // PJ1, TXD3, PCINT10
#define pPin15              GpioPin( J, 0 )                 // PJ0, RXD3, PCINT9

#define pPin16              GpioPin( H, 1 )                 // PH1, TXD2
#define pPin17              GpioPin( H, 0 )                 // PH0, RXD2
#define pPin18              GpioPin( D, 3 )                 // PD3, INT3, TXD1
#define pPin19              GpioPin( D, 2 )                 // PD2, INT2, RXD1
#define pPin20              GpioPin( D, 1 )                 // PD1, INT1, SDA

#define pPin21              GpioPin( D, 0 )                 // PD0, INT0, SCL
#define pPin22              GpioPin( A, 0 )                 // PA0, AD0
#define pPin23              GpioPin( A, 1 )                 // PA1, AD1
#define pPin24              GpioPin( A, 2 )                 // PA2, AD2
#define pPin25              GpioPin( A, 3 )                 // PA3, AD3

#define pPin26              GpioPin( A, 4 )                 // PA4, AD4
#define pPin27              GpioPin( A, 5 )                 // PA5, AD5
#define pPin28              GpioPin( A, 6 )                 // PA6, AD6
#define pPin29              GpioPin( A, 7 )                 // PA7, AD7
#define pPin30              GpioPin( C, 7 )                 // PC7, A15

#define pPin31              GpioPin( C, 6 )                 // PC6, A14
#define pPin32              GpioPin( C, 5 )                 // PC5, A13
#define pPin33              GpioPin( C, 4 )                 // PC4, A12
#define pPin34              GpioPin( C, 3 )                 // PC3, A11
#define pPin35              GpioPin( C, 2 )                 // PC2, A10

#define pPin36              GpioPin( C, 1 )                 // PC1, A9
#define pPin37              GpioPin( C, 0 )                 // PC0, A8
#define pPin38              GpioPin( D, 7 )                 // PD7, T0
#define pPin39              GpioPin( G, 2 )                 // PG2, ALE
#define pPin40              GpioPin( G, 1 )                 // PG1, RD

#define pPin41              GpioPin( G, 0 )                 // PG0, WR
#define pPin42              GpioPin( L, 7 )                 // PL7
#define pPin43              GpioPin( L, 6 )                 // PL6
#define pPin44              GpioPinPwm( L, 5, 5, C )        // PL5, OC5C
#define pPin45              GpioPinPwm( L, 4, 5, B )        // PL4, OC5B

#define pPin46              GpioPinPwm( L, 3, 5, A )        // PL3, OC5A
#define pPin47              GpioPin( L, 2 )                 // PL2, T5
#define pPin48              GpioPin( L, 1 )                 // PL1, ICP5
#define pPin49              GpioPin( L, 0 )                 // PL0, ICP4
#define pPin50              GpioPin( B, 3 )                 // PB3, MISO, PCINT3

#define pPin51              GpioPin( B, 2 )                 // PB2, MOSI, PCINT2
#define pPin52              GpioPin( B, 1 )                 // PB1, SCK, PCINT1
#define pPin53              GpioPin( B, 0 )                 // PB0, SS, PCINT0

#define pSDA                pPin20                          // I2C SDA
#define pSCL                pPin21                          // I2C SCL

#define pSS                 pPin53                          // SPI SS
#define pMOSI               pPin51                          // SPI MOSI
#define pMISO               pPin50                          // SPI MISO
#define pSCK                pPin52                          // SPI SCK


#endif
