/*
    Analog2Digital.cpp - A library for analog-to-digital conversions.
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




#include "Analog2Digital.h"

#include <avr/io.h>
#include <util/delay.h>



enum A2DPrescalar
{
    kA2dPrescaleDiv2 =      0x00,    // 0x01,0x00    -> clk/2    = 8 MHz, 6 MHz, 4 Mhz
    kA2dPrescaleDiv4 =      0x02,    // 0x02         -> clk/4    = 4 MHz, 3 MHz, 2 MHz
    kA2dPrescaleDiv8 =      0x03,    // 0x03         -> clk/8    = 2 MHz, 1.5 MHz, 1 MHz
    kA2dPrescaleDiv16 =     0x04,    // 0x04         -> clk/16   = 1 MHz, 750 KHz, 500 KHz
    kA2dPrescaleDiv32 =     0x05,    // 0x05         -> clk/32   = 500 KHz, 375 KHz, 250 KHz
    kA2dPrescaleDiv64 =     0x06,    // 0x06         -> clk/64   = 250 KHz, 187.5 KHz, 125 KHz
    kA2dPrescaleDiv128 =    0x07     // 0x07         -> clk/128  = 125 KHz, 93.75 KHz, 62.5 KHz
};


namespace
{
    int8_t  sCurrentChannel;
};



void initA2D( uint8_t ref )
{
    // Set default prescaler, and zero the rest of ADCSRA

    // Desired range is 50-200 KHz
#if F_CPU == 16000000

    // 16 MHz / 128 = 125 KHz
    ADCSRA = kA2dPrescaleDiv128;

#elif F_CPU == 12000000

    // 12 MHz / 128 = 93.75 KHz
    ADCSRA = kA2dPrescaleDiv128;

#elif F_CPU == 8000000

    // 8 MHz / 64 = 125 KHz
    ADCSRA = kA2dPrescaleDiv64;

#else

#error "initA2D() is only implemented for CPU speeds of 8 MHz, 12 MHz, or 16 MHz."

#endif

    // Zero ADCSRB
    ADCSRB = 0;

    // Set default reference and zero the rest:
    //  - Default reference is AVCC (safe default)
    //  - ADLAR cleared (right adjusted output)
    //  - MUX cleared (default channel ADC0)
    ADMUX = ( ref << 6 );

    // Enable (turn on) ADC
    ADCSRA |= (1 << ADEN);

    sCurrentChannel = 0;
}


void turnOffA2D()
{
    // Clear (turn off) ADC
    ADCSRA &= ~(1 << ADEN);
}



void setA2DVoltageReference( A2DVoltageReference ref )
{
    ADMUX = ( ADMUX & ~0xc0 ) | ( ref << 6 );

    // Need to let ADC system restablize
    _delay_us( 125 );
}


int readA2D( int8_t channel )
{
    // Differentiate between ATmega328p and ATmega2560
#if defined(__AVR_ATmega2560__)

    if ( channel < 0 || channel > 15 )
    {
        // Not a valid ADC channel
        return 0;
    }

    if ( sCurrentChannel != channel )
    {
        // Set MUX5 if channel > 7 (i.e, bit 3 set); otherwise clear it
        ADCSRB = ( ADCSRB & ~(1<< MUX5) ) | ( ( channel & (1 << 3) ) ? ( 1 << MUX5 ) : 0  );
    }

#else  // ATmega328p

    if ( channel < 0 || channel > 7 )
    {
        // Not a valid ADC channel
        return 0;
    }

#endif

    if ( sCurrentChannel != channel )
    {
        // Set MUX2-0
        ADMUX = ( ADMUX & ~0x1f ) | ( channel & 0x07 );

        // Need to let ADC system restablize
        _delay_us( 125 );

        sCurrentChannel = channel;
    }

    // Start A2D conversion
    ADCSRA |= ( 1 << ADSC );

    // ADSC is cleared when the conversion finishes
    while ( ADCSRA & ( 1 << ADSC ) )
        ;

    // NOTE: must read ADCL before ADCH
    return  ADCL | ( static_cast<uint16_t>(ADCH) << 8 );
}
