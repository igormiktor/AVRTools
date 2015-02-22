/*
    SystemClock.cpp - Functions to initialize and use a system clock
    on AVR chips that is compatible with Arduino.
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


/*!
 * \file
 *
 * \brief Include this file to use the functions that initialize the microcontroller to a known, basic state.
 *
 * To use these functions, include InitSystem.h in your source code and link against InitSystem.cpp.
 *
 */




#ifndef InitSystem_h
#define InitSystem_h


/*!
 * \brief This function initializes the microcontroller by clearing any bootloader settings,
 * clearing all timers, and turning on interrupts.
 *
 * This function is generally called at the very beginning of \c main().
 *
 */

void initSystem();


#endif
