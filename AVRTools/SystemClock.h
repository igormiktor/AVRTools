/*
    SystemClock.h - Functions to initialize and use a system clock
    on AVR chips that is compatible with Arduino.
    For AVR ATMega328p (Arduino Uno) and ATMega2560 (Arduino Mega).
    This is part of the AVRTools library.
    Copyright (c) 2014 Igor Mikolic-Torreira.  All right reserved.
    Functions delayMicroseconds() and delayMilliseconds() adapted from Arduino code that
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
 * \brief Include this file to use the functions that instantiate and access a system clock
 * that counts elapsed milliseconds.
 *
 * To use these functions, include SystemClock.h in your source code and link against SystemClock.cpp.
 *
 * \note Linking against SystemClock.cpp installs a interrupt function on timer0.  This interrupt
 * routine is installed regardless of whether the system clock is actually initialized or not.
 * If you have other uses for timer0, do not use SystemClock functions and do not link against SystemClock.cpp.
 *
 */



#ifndef SystemClock_h
#define SystemClock_h




#define clockCyclesPerMicrosecond()         ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds( a )      ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles( a )      ( (a) * clockCyclesPerMicrosecond() )


/*!
 * \brief This function initializes a system clock that tracks elapsed milliseconds.
 *
 * The system clock uses timer0, so you cannot use timer0 for other functions if you
 * use the system clock functionality.
 *
 * \note Linking against SystemClock.cpp installs a interrupt function on timer0.  This interrupt
 * routine is installed regardless of whether the system clock is actually initialized or not.
 * If you have other uses for timer0, do not use SystemClock functions and do not link against SystemClock.cpp.
 *
 */

void initSystemClock();


/*!
 * \brief Delay a certain number of microseconds.
 *
 * \arg \c us the number of microseconds to delay.
 *
 * \note This function only works for CPU clocks running at either 8 MHz, 12 MHz, or 16 MHz.
 *
 */

void delayMicroseconds( unsigned int us );


/*!
 * \brief Delay a certain number of milliseconds.
 *
 * \arg \c m the number of milliseconds to delay.
 *
 */

void delayMilliseconds( unsigned long ms );


/*!
 * \brief Delay a certain number of milliseconds.
 *
 * This inline function is a synonym for delayMilliseconds(); it is provided for
 * compatibility with the standard Arduino library.
 *
 * \arg \c m the number of milliseconds to delay.
 *
 */

inline void delay( unsigned long ms )
{ delayMilliseconds( ms ); }


/*!
 * \brief Return the number of elasped microseconds since the system clock was turned on.
 *
 * The microsecond count will overflow back to zero in approximately 70 minutes.
 *
 * \returns the number of elapsed microseconds.
 *
 */

unsigned long micros();


/*!
 * \brief Return the number of elasped milliseconds since the system clock was turned on.
 *
 * \returns the number of elapsed milliseconds.
 *
 */

unsigned long millis();



#endif
