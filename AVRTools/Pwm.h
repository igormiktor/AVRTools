/*
    Pwm.h - Macros and Functions for accessing the PMW capabilities of AVRs.
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
 * \brief This file provides functions that access the PWM capability of the ATmega328 and ATmega2560
 * microcontrollers.
 *
 * To use these functions, include Pwm.h in your source code and link against Pwm.cpp.
 *
 * Before you use the writePinPwm() function, you must first initialize the appropriate timers using
 * the appropriate initPwmTimerN() function.
 *
 * The association between PWN pins and timers is as follows:
 *
 * For Arduino Uno (ATmega328)
 * Arduino Uno pin | ATmega328 pin | Timer
 * :------------: | :-----------: | :-------:
 *    3           |      PD3      |   timer2
 *    5           |      PD5      |   timer0
 *    6           |      PD6      |   timer0
 *    9           |      PB1      |   timer1
 *    10          |      PB2      |   timer1
 *    11          |      PB3      |   timer2
 *
 *
 * For Arduino Mega (ATmega2560)
 * Arduino Mega pin | ATmega2560 pin | Timer
 * :------------: | :-----------: | :-------:
 *    2           |      PE4      |   timer3
 *    3           |      PE5      |   timer3
 *    4           |      PG5      |   timer0
 *    5           |      PE3      |   timer3
 *    6           |      PH3      |   timer4
 *    7           |      PH4      |   timer4
 *    8           |      PH5      |   timer4
 *    9           |      PH6      |   timer2
 *    10          |      PB4      |   timer2
 *    11          |      PB5      |   timer1
 *    12          |      PB6      |   timer1
 *    13          |      PB7      |   timer0
 *    44          |      PL5      |   timer5
 *    45          |      PL4      |   timer5
 *    46          |      PL3      |   timer5
 *
 * \note Timer0 is also used by the system clock.  \e Do \e not \e initialize \e or \e clear \e timer0
 * if you are also using the system clock function from SystemClock.h.  If you are using
 * the system clock function, you can use timer0-based PWM functions \e without having
 * to call initPwmTimer0().
 *
 */




#ifndef Pwm_h
#define Pwm_h


#include <stdint.h>

#include <util/atomic.h>

#include "GpioPinMacros.h"



#define _writeGpioPinPwm( ddr, port, pin, nbr, chl, ocr, com, tccr, value )                 \
                                        do                                                  \
                                        {                                                   \
                                            if ( value <= 0 )                               \
                                            {                                               \
                                                tccr &= ~(1<<com);                          \
                                                port &= ~(1<<nbr);                          \
                                            }                                               \
                                            else if ( value >= 255 )                        \
                                            {                                               \
                                                tccr &= ~(1<<com);                          \
                                                port |= (1<<nbr);                           \
                                            }                                               \
                                            else                                            \
                                            {                                               \
                                                tccr |= (1<<com);                           \
                                                ATOMIC_BLOCK( ATOMIC_RESTORESTATE )         \
                                                {                                           \
                                                    ocr = value;                            \
                                                }                                           \
                                            }                                               \
                                        }                                                   \
                                        while ( 0 )



/*!
 * \brief Write a PWM value to a pin.
 *
 * This sets the duty cycle for the PWM on the pin.  Completely off is represented by 0;
 * completely on is represented by 1.
 *
 * Before calling this function, you must initialize the appropriate timer
 * by calling initPwmTimerN(), where N = 1, 2, 3, 4, or 5 is the timer corresponding
 * to that particular pin.
 *
 * \arg \c pinName a pin name macro generated by GpioPinPwm().
 *
 * \arg \c value a value between 0 and 255.
 *
 * \warning Timer0 is also used by the system clock.  \e Do \e not \e initialize \e or \e clear \e timer0
 * if you are also using the system clock function from SystemClock.h.  If you are using
 * the system clock function, you can use timer0-based PWM functions \e without having
 * to call initPwmTimer0().
 *
 * \note You can temporarily turn off PWM by writing a 0 to the pin with writePinPwm( pin, 0 ).
 * In particular, this is how to turn off PWM to pins associated with timer0 when timer0 is also
 * being used by the system clock.
 *
 * \note This macro ensures operations on 16-bit timers are atomic (at the cost of a small amount of
 * overhead in the case of 8-bt timers).
 *
 * \hideinitializer
 */

#define writeGpioPinPwm( pinName, value )       _writeGpioPinPwm( pinName, value )




/*!
 * \brief Write a PWM value to a pin.
 *
 * This sets the duty cycle for the PWM on the pin.  Completely off is represented by 0;
 * completely on is represented by 1.
 *
 * Before calling this function, you must initialize the appropriate timer
 * by calling initPwmTimerN(), where N = 1, 2, 3, 4, or 5 is the timer corresponding
 * to that particular pin.
 *
 * \arg \c pinVar a pin variable that has PWM capabilities (i.e., initialized with makeGpioVarFromGpioPinPwm()).
 *
 * \arg \c value a value between 0 and 255.
 *
 * \warning Timer0 is also used by the system clock.  \e Do \e not \e initialize \e or \e clear \e timer0
 * if you are also using the system clock function from SystemClock.h.  If you are using
 * the system clock function, you can use timer0-based PWM functions \e without having
 * to call initPwmTimer0().
 *
 * \note You can temporarily turn off PWM by writing a 0 to the pin with writePinPwm( pin, 0 ).
 * In particular, this is how to turn off PWM to pins associated with timer0 when timer0 is also
 * being used by the system clock.
 *
 * \note This function ensures operations on 16-bit timers are atomic (at the cost of a small amount of
 * overhead in the case of 8-bt timers).
 *
 */

inline void writeGpioPinPwmV( const GpioPinVariable& pinVar, uint8_t value )
{
    if ( value == 0 )
    {
        *(pinVar.tccr()) &= ~( 1 << pinVar.com() );
        *(pinVar.port()) &= ~( 1 << pinVar.bitNbr() );

    }
    else if ( value == 255 )
    {
        *(pinVar.tccr()) &= ~( 1 << pinVar.com() );
        *(pinVar.port()) |= ( 1 << pinVar.bitNbr() );
    }
    else
    {
        *(pinVar.tccr()) |= ( 1 << pinVar.com() );
        // Provide atomicity for 16-bit timers (not needed for 8-bit timers, but be safe)
        ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
        {
            *(pinVar.ocr()) = value;
        }
    }
}



/*!
 * \brief Initialize timer0 for PWM.
 *
 * This function sets timer0 for phase-correct PWM mode.  You must call
 * this function or initSystemClock() before calling writePinPwm() on a
 * PWM pin associated with timer0.
 *
 * The PWM pins supported by timer0 are:
 * - Arduino Uno (ATmega328):   pin 5 (PD5), pin 6 (PD6)
 * - Arduino Mega (ATmega2560):  pin 4 (PG5), pin 13 (PB7)
 *
 * \note Timer0 is also used by the system clock.  \e Do \e not \e initialize \e timer0
 * if you are also using the system clock function from SystemClock.h.
 *
 * The function initSystemClock() puts timer0 in fast PWM mode.  While this
 * is different than the phase-correct PWM mode preferred for PWM usage, fast PWM
 * mode still allows PWM operations on the associated pins.  However, the duty cycles
 * may be slightly off, and calling writePinPwm( pin, 0 ) may not completely turn off output
 * on the pins associated with timer0.
 *
 * Only call initPwmTimer0() if you did \e not call initSystemClock() (i.e., you are \e not using the
 * system clock) and you wish to use PWM on the pins associate with timer0.
 *
 * \note To turn off PWM on pins associated with timer0 while also using the system clock, write a
 * zero to the pin by calling writePinPwm( pinName, 0 ).
 */

void initPwmTimer0();


/*!
 * \brief Initialize timer1 for PWM.
 *
 * This function sets timer1 for phase-correct PWM mode.  You must call
 * this function before calling writePinPwm() on a PWM pin associated
 * with timer1.
 *
 * The PWM pins supported by timer1 are:
 * - Arduino Uno (ATmega328):   pin 9 (PB1), pin 10 (PB2)
 * - Arduino Mega (ATmega2560):  pin 11 (PB5), pin 12 (PB6)
 */

void initPwmTimer1();


/*!
 * \brief Initialize timer2 for PWM.
 *
 * This function sets timer2 for phase-correct PWM mode.  You must call
 * this function before calling writePinPwm() on a PWM pin associated
 * with timer2.
 *
 * The PWM pins supported by timer2 are:
 * - Arduino Uno (ATmega328):   pin 3 (PD3), pin 11 (PB3)
 * - Arduino Mega (ATmega2560):  pin 9 (PH6), pin 10 (PB4)
 */

void initPwmTimer2();


/*!
 * \brief Clear timer0.
 *
 * This function clears timer0.
 *
 * \note Timer0 is also used by the system clock.  \e Do \e not \e clear \e timer0
 * if you are also using the system clock function from SystemClock.h.
 *
 * Only call this function if you called initPwmTimer0() instead of initSystemClock().
 *
 * \note To turn off PWM on pins associated with timer0 while also using the system clock, write a
 * zero to the pin by calling writePinPwm( pinName, 0 ).
 */

void clearTimer0();




/*!
 * \brief Clear timer1.
 *
 * This function clears timer1, turning off the PWM functionality.
 */

void clearTimer1();


/*!
 * \brief Clear timer2.
 *
 * This function clears timer2, turning off the PWM functionality.
 */

void clearTimer2();


#if defined(__AVR_ATmega2560__)


/*!
 * \brief Initialize timer3 for PWM.
 *
 * This function sets timer3 for phase-correct PWM mode.  You must call
 * this function before calling writePinPwm() on a PWM pin associated
 * with timer3.
 *
 * The PWM pins supported by timer3 are:
 * - Arduino Mega (ATmega2560):  pin 2 (PE4), pin 3 (PE5)
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

void initPwmTimer3();


/*!
 * \brief Initialize timer4 for PWM.
 *
 * This function sets timer4 for phase-correct PWM mode.  You must call
 * this function before calling writePinPwm() on a PWM pin associated
 * with timer4.
 *
 * The PWM pins supported by timer4 are:
 * - Arduino Mega (ATmega2560):  pin 6 (PH3), pin 7 (PH4), pin 8 (PH5)
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

void initPwmTimer4();



/*!
 * \brief Initialize timer5 for PWM.
 *
 * This function sets timer5 for phase-correct PWM mode.  You must call
 * this function before calling writePinPwm() on a PWM pin associated
 * with timer5.
 *
 * The PWM pins supported by timer5 are:
 * - Arduino Mega (ATmega2560):  pin 44 (PL5), pin 45 (PL4), pin 46 (PL3)
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

void initPwmTimer5();




/*!
 * \brief Clear timer3.
 *
 * This function clears timer3, turning off the PWM functionality.
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

void clearTimer3();


/*!
 * \brief Clear timer4.
 *
 * This function clears timer4, turning off the PWM functionality.
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

void clearTimer4();


/*!
 * \brief Clear timer5.
 *
 * This function clears timer5, turning off the PWM functionality.
 *
 * \note This function is only available on Arduino Mega (ATmega2560).
 */

void clearTimer5();

#endif

#endif

