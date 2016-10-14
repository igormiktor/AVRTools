/*
    Pwm.h - Functions for accessing the PWM capabilities of AVRs.
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



#include "Pwm.h"


#include <inttypes.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>



void clearTimer0()
{
    // Clear Timer0
    TCCR0A = 0;     // Clear all settings
    TCCR0B = 0;     // Clear all settings
    TIMSK0 = 0;     // Disable all interrupts
}


void initPwmTimer0()
{
    clearTimer0();

    // Set Timer1 prescale factor to 64
    TCCR0B |= ( 1 << CS01 ) | ( 1 << CS00 );

    // Put Timer1 in 8-bit phase correct pwm mode
    TCCR0A |= ( 1 << WGM00 );
}





void clearTimer1()
{
    // Clear Timer1
    TCCR1A = 0;     // Clear all settings
    TCCR1B = 0;     // Clear all settings
    TIMSK1 = 0;     // Disable all interrupts
}


void initPwmTimer1()
{
    clearTimer1();

    // Set Timer1 prescale factor to 64
    TCCR1B |= ( 1 << CS11 ) | ( 1 << CS10 );

    // Put Timer1 in 8-bit phase correct pwm mode
    TCCR1A |= ( 1 << WGM10 );
}





void clearTimer2()
{
    // Clear Timer2
    TCCR2A = 0;     // Clear all settings
    TCCR2B = 0;     // Clear all settings
    TIMSK2 = 0;     // Disable all interrupts
}


void initPwmTimer2()
{
    clearTimer2();

    // Set Timer2 prescale factor to 64
    TCCR2B |= ( 1 << CS22 );

    // Put Timer2 in 8-bit phase correct pwm mode
    TCCR2A |= ( 1 << WGM20 );
}





#if defined(__AVR_ATmega2560__)

void clearTimer3()
{
    // Clear Timer3
    TCCR3A = 0;     // Clear all settings
    TCCR3B = 0;     // Clear all settings
    TIMSK3 = 0;     // Disable all interrupts
}


void initPwmTimer3()
{
    clearTimer3();

    // Set Timer3 prescale factor to 64
    TCCR3B |= ( 1 << CS31 ) | ( 1 << CS30 );

    // Put Timer3 in 8-bit phase correct pwm mode
    TCCR3A |= ( 1 << WGM30 );
}







void clearTimer4()
{
    // Clear Timer4
    TCCR4A = 0;     // Clear all settings
    TCCR4B = 0;     // Clear all settings
    TIMSK4 = 0;     // Disable all interrupts
}


void initPwmTimer4()
{
    clearTimer4();

    // Set Timer4 prescale factor to 64
    TCCR4B |= ( 1 << CS41 ) | ( 1 << CS40 );

    // Put Timer4 in 8-bit phase correct pwm mode
    TCCR4A |= ( 1 << WGM40 );
}






void clearTimer5()
{
    // Clear Timer4
    TCCR5A = 0;     // Clear all settings
    TCCR5B = 0;     // Clear all settings
    TIMSK5 = 0;     // Disable all interrupts
}


void initPwmTimer5()
{
    clearTimer5();

    // Set Timer5 prescale factor to 64
    TCCR5B |= ( 1 << CS51 ) | ( 1 << CS50 );

    // Put Timer5 in 8-bit phase correct pwm mode
    TCCR5A |= ( 1 << WGM50 );
}


#endif


