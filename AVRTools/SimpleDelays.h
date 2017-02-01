/*
    SimpleDelays.h - Simple delay functions.
    This is part of the AVRTools library.
    Copyright (c) 2015 Igor Mikolic-Torreira.  All right reserved.

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
 * \brief This file provides simple delay functions that do not involve
 * timers or interrupts.  These functions simply execute a series of
 * nested loops with known and precise timing.
 *
 * For precision, these functions are all implemented directly in assembler.
 *
 * \note These functions are implemented for (and automatically adjust to) either
 * an 8 MHz, 12 MHz, or a 16 MHz clock cycle.
 *
 */


#ifndef SimpleDelays_h
#define SimpleDelays_h


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif






/*!
 * \brief Delay a given number of quarter microseconds.  Due to function call
 * overhead, at 16 MHz the smallest possible delay is just under 6 quarter microseconds (~1.5 microseconds).
 * Delays of 7 quarter microseconds or greater are reasonably accurate.  At 8 MHz the
 * smallest possible delay is just under 12 quarter microseconds (~3 microseconds).  Delays of
 * 13 quarter microseconds or greater are reasonably accurate
 *
 * At 16 MHz delays of less than 7 quarter microseconds produce a delay of just under
 * 6 quarter microseconds (~1.5 microseconds). At 8 MHz delays of less than 12 quarter
 * microseconds produce a delay of just under 12 quarter microseconds (~3 microseconds).
 *
 * The maximum delay is 65535 quarter microseconds (equal to 16,383.75 microseconds, or
 * about 16.4 milliseconds).
 *
 * \arg \c nbrOfQuartersOfMicroSeconds the number of quarter microseconds to delay.
 * For 16 MHz clocks, arguments less than 7 quarter microseconds for 16 MHz clocks
 * all produce delays of just under 6 quarter microseconds.  For 8 MHz clocks,
 * arguments less than 13 quarter microseconds all produce delays of about
 * 12 quarter microseconds.
 *
 * \note This delay function is only accurate if interrupts are disabled.  If interrupts
 * are enabled, the delays will be at least as long as requested, but may actually be
 * longer.  If accurate delays are desired, disable interrupts before calling this
 * function (remember to enable interrupts afterwards).
 *
 * \note This function only works for CPU clocks running at either 8 MHz, 12 MHz, or 16 MHz.
 *
 * For precision, this function is implemented directly in assembler.
 */

void delayQuartersOfMicroSeconds( uint16_t nbrOfQuartersOfMicroSeconds );





/*!
 * \brief Delay a given number of milliseconds.  Despite function call
 * overhead, this function is accurate within a few hundreds of
 * microseconds.
 *
 * \arg \c nbrOfMilliSeconds the number of milliseconds to delay.  The maximum
 * delay is 256 milliseconds (pass 0 for a delay of 256 milliseconds).
 *
 * \note This delay function is only accurate if interrupts are disabled.  If interrupts
 * are enabled, the delays will be at least as long as requested, but may actually be
 * longer.  If accurate delays are desired, disable interrupts before calling this
 * function (remember to enable interrupts afterwards).
 *
 * \note This function only works for CPU clocks running at either 8 MHz, 12 MHz, or 16 MHz.
 *
 * For precision, this function is implemented directly in assembler.
 */

void delayWholeMilliSeconds( uint8_t nbrOfMilliSeconds );





/*!
 * \brief Delay a given number of tenths of a seconds.  Despite function call
 * overhead, this function is accurate within a few hundreds of microseconds.
 *
 * \arg \c nbrOfTenthsOfSeconds the number of tenths of seconds to delay.  The maximum
 * delay is 256 tenths of a second or 25.6 seconds (pass 0 for a delay of 256 tenths
 * of a second).
 *
 * \note This delay function is only accurate if interrupts are disabled.  If interrupts
 * are enabled, the delays will be at least as long as requested, but may actually be
 * longer.  If accurate delays are desired, disable interrupts before calling this
 * function (remember to enable interrupts afterwards).
 *
 * \note This function only works for CPU clocks running at either 8 MHz, 12 MHz, or 16 MHz.
 *
 * For precision, this function is implemented directly in assembler.
 */

void delayTenthsOfSeconds( uint8_t nbrOfTenthsOfSeconds );





#ifdef __cplusplus
}
#endif


#endif
