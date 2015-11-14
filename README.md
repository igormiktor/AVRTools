# AVRTools: A Library for the AVR ATmega328 and ATmega2560 Microcontrollers #

## Overview ##

This library provides an Arduino-like simple-to-use interface to the AVR ATmega328 and ATmega2560 microcontrollers
without the bloat and slowness of the official Arduino libraries.

AVRTools is an attempt to provide the convenience of the Ardiuno library interface while embracing the fundametal C/C++
philosopy of "you don't pay for what you don't use" and "assume the programmer knows what he or she is doing"

Like the Arduino libraries, AVRTools allows you to refer to pins on an Arduino via simple names such as `pPin07` for
digital pin 7 or `pPinA03` for analog pin 3.  Unlike the Arduino libraries, these names are pure macros so that
`setGpioPinHigh( pPin12 )` always translates directly into `PORTB |= (1<<4)` on an Arduino Uno.  Similar macros are
available for conveniently naming any pin on an ATmega328 or ATmega2560 and provide easy and efficient access to all
the functionality available on that pin (digital I/O, analog-to-digital conversion, PWM, etc).  AVRTools provides
functions to access the primary functionality of the ATmega328 and ATmega2560 microcontrollers.

On the otherhand, because "you don't pay for what you don't use", when using AVRTools nothing is initialized or configured unless
you explicitly do it.  If you need analog inputs, then you must explicitly initialize the analog-to-digital subsystem before
reading any analog pins.  If you need an Arduino-style system clock (for functions like `delay()` or `millis()`), then
you must explicitly start a system clock.  AVRTools provides functions to do any necessary initialization, but the
programmer must explicitly call these function to perform the initialization.

Similarly, because AVRTools "assumes the programmer knows what he or she is doing," it doesn't conduct a lot of checks
to ensure you don't do something stupid.  For example when you set the output value of a digital pin using the Arduino library
function `digitalWrite()`, it checks if that pin is currently configured for PWM and if it is, it automatically turns off PWM-mode
before writing to the pin. The equivalent AVRTools function, `writeGpioPinDigital()` doesn't do that:  it assumes that if
the programmer previously used the pin in PWM mode that he or she remembered to turn off PWM mode before using the
pin digitally.  Assuming the programmer knows what he or she is doing allows the functions in AVRTools to be much faster
than their Arduino library counterparts.  For example, a call to the Arduino function `digitalWrite()` takes about 70 cycles;
a call to the equivalent AVRTools function `writeGpioPinDigital()` takes
1 cycle (it's actually a macro in AVRTools).

## Audience ##

If you are an Arduino programmer, you may want to try out AVRTools if:
- You are comfortable programming the Arduino Uno and Mega directly using the the avr-gcc toolset.
- You are frustrated by the slowness of even simple functions in the official Arduino libraries.
- Your code doesn't fit into the available memory because the official Arduino libraries are so big.

If you are an ATmega328 or ATmega2560 microcontroller programmer, you may want to try out AVRTools if:
- You are secretly jealous of how easy and convenient it is to use the Arduino libraries.
- You wish you could bind together DDRs, PORTs, and PINs so you didn't have to write code like:

```C
#define MY_PIN_DDR      DDRB
#define MY_PIN_PORT     PORTB
#define MY_PIN_PIN      PINB
#define MY_PIN_NBR      7

/* Put MY_PIN in output mode and set it high */
MY_PIN_DDR |= (1<<MY_PIN_NBR)
MY_PIN_PORT |= (1<<MY_PIN_NBR)
```

- You wish you could use a function-like syntax to switch input/output mode, read a pin, or set a pin high or low but still have the compiler generate single-cycle `in` and `out` instructions.


## What it is like to use AVRTools ##

Using AVRTools, you can write code like this:

~~~C
#include "AVRTools/ArduinoPins.h"
#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"

#define pRedLed            pPin10
#define pYellowLed         pPin07
#define pGreenLed          pPin04

int main()
{
    initSystem();
    initSystemClock();

    setGpioPinModeOutput( pGreenLed );
    setGpioPinModeOutput( pYellowLed );
    setGpioPinModeOutput( pRedLed );

    setGpioPinHigh( pGreenLed );
    setGpioPinHigh( pYellowLed );
    setGpioPinHigh( pRedLed );

    delayMilliseconds( 2000 );

    setGpioPinLow( pGreenLed );
    setGpioPinLow( pYellowLed );
    setGpioPinLow( pRedLed );

    while ( 1 )
    {
        delayMilliseconds( 1000 );

        setGpioPinLow( pRedLed );
        setGpioPinHigh( pGreenLed );

        delayMilliseconds( 1000 );

        setGpioPinLow( pGreenLed );
        setGpioPinHigh( pYellowLed );

        delayMilliseconds( 1000 );

        setGpioPinLow( pYellowLed );
        setGpioPinHigh( pRedLed );
    }
}
~~~

It is as simple to code and read as Arduino code.  Yet the setGpioPinXXX calls
translate directly into single-cycle `in`, `out`, `sbi`, `cbi`, `sbic`, or
`sbis` instructions.

If you find this appealing, download AVRTools and try it out.  You can learn
more by reviewing the documentation found in the file AVRToolsDocumentation.pdf
that is part of the AVRTools package or in the equivalent
[online documentation] (http://igormiktor.github.io/AVRTools/).



