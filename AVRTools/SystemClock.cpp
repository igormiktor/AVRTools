/*
    SystemClock.cpp - Functions to initialize and use a system clock
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



#include "SystemClock.h"

#include <inttypes.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>



namespace
{
    // These variables are private to this module

    // The prescaler is set so that timer0 ticks every 64 clock cycles, and the
    // the overflow handler is called every 256 ticks.
    const unsigned long kMicrosecondsPerOverflowTIMER0 = ( clockCyclesToMicroseconds( 64 * 256 ) );

    // The whole number of milliseconds per timer0 overflow
    const unsigned long kMillisInc = ( kMicrosecondsPerOverflowTIMER0 / 1000 );

    // The fractional number of milliseconds per timer0 overflow. Shift right
    // by three to fit these numbers into a byte. (for 16 MHz this doesn't lose precision.)
    const uint8_t kFractInc =  ( ( kMicrosecondsPerOverflowTIMER0 % 1000 ) >> 3 );
    const uint8_t kFractMax =  ( 1000 >> 3 );

    // Variables to keep track of time
    volatile unsigned long      timer0_overflow_count = 0;
    volatile unsigned long      timer0_millis = 0;
    uint8_t                     timer0_fract = 0;

};




ISR( TIMER0_OVF_vect )
{
    // Copy these to local variables so they can be stored in registers
    // (volatile variables must be read from memory on every access)
    unsigned long m = timer0_millis;
    uint8_t f = timer0_fract;

    m += kMillisInc;
    f += kFractInc;
    if ( f >= kFractMax )
    {
        f -= kFractMax;
        ++m;
    }

    timer0_fract = f;
    timer0_millis = m;
    timer0_overflow_count++;
}



unsigned long millis()
{
    // Disable interrupts while we read timer0_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer0_millis)
    unsigned long m;

    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        m = timer0_millis;
    }

    return m;
}




unsigned long micros()
{
    // Disable interrupts to avoid reading inconsistent values
    unsigned long m;
    uint8_t t;

    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        m = timer0_overflow_count;
        t = TCNT0;

        if ( ( TIFR0 & _BV(TOV0) ) && ( t < 255 ) )
        {
            m++;
        }
    }

    return ( (m << 8) + t ) * ( 64 / clockCyclesPerMicrosecond() );
}




void delayMilliseconds( unsigned long ms )
{
    uint16_t start = static_cast<uint16_t>( micros() );

    while ( ms > 0 )
    {
        if ( ( static_cast<uint16_t>( micros() ) - start ) >= 1000 )
        {
            ms--;
            start += 1000;
        }
    }
}




/* Delay for the given number of microseconds.  Assumes a 8 or 16 MHz clock. */
void delayMicroseconds( unsigned int us )
{
    // calling avrlib's delay_us() function with low values (e.g. 1 or
    // 2 microseconds) gives delays longer than desired.
    //delay_us(us);

    // for the 16 MHz clock on most Arduino boards

    // for a one-microsecond delay, simply return.  the overhead
    // of the function call yields a delay of approximately 1 1/8 us.
    if ( --us == 0 )
    {
        return;
    }

    // the following loop takes a quarter of a microsecond (4 cycles)
    // per iteration, so execute it four times for each microsecond of
    // delay requested.
    us <<= 2;

    // account for the time taken in the preceeding commands.
    us -= 2;

    // busy wait
    __asm__ __volatile__
    (
        "1: sbiw %0,1" "\n\t" // 2 cycles
        "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
    );
}




void initSystemClock()
{
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        // Use Timer0 for the system clock, but configure it so it also supports
        // fast hardware pwm (using phase-correct PWM would mean that Timer0
        // overflowed half as often)

        TCCR0A = 0;     // Clear all settings
        TCCR0B = 0;     // Clear all settings
        TIMSK0 = 0;     // Disable all interrupts
        TCNT0  = 0;     // initialize counter value to 0

        TCCR0A |= (1 << WGM01) | (1 << WGM00);

        // set timer 0 prescale factor to 64
        TCCR0B |= (1 << CS01) | (1 << CS00);

        // enable timer 0 overflow interrupt
        TIMSK0 |= (1 << TOIE0);
    }
}

