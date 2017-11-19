AVRTools:  A Library for the AVR ATmega328 and ATmega2560 Microcontrollers       {#mainpage}
==========================================================================

# Overview #                                                                    {#AvrOverview}

This library provides an Arduino-like simple-to-use interface to the AVR ATmega328 and ATmega2560 microcontrollers
without the bloat and slowness of the official Arduino libraries.

AVRTools is an attempt to provide the convenience of the Ardiuno library interface while embracing the fundametal C/C++
philosopy of "you don't pay for what you don't use" and "assume the programmer knows what he or she is doing."

Like the Arduino libraries, AVRTools allows you to refer to pins on an Arduino via simple names such as `pPin07` for
digital pin 7 or `pPinA03` for analog pin 3.  However, unlike the Arduino libraries, these names are pure macros so that
`setGpioPinHigh( pPin12 )` always translates directly into `PORTB |= (1<<PORTB4)` on an Arduino Uno.  Similar macros are
available for conveniently naming any pin on an ATmega328 or ATmega2560, providing easy and efficient access to all
the functionality available on that pin (digital I/O, analog-to-digital conversion, PWM, etc).  In combination with
these pin name macros, AVRTools provides
functions to access the major subsystems and functionality of the ATmega328 and ATmega2560 microcontrollers.

On the other hand, because "you don't pay for what you don't use," when using AVRTools nothing is initialized or
configured unless
you explicitly do it.  If you need analog inputs, then you must explicitly initialize the analog-to-digital subsystem before
reading any analog pins.  If you need an Arduino-style system clock (for functions like `delay()` or `millis()`), then
you must explicitly start a system clock.  AVRTools provides functions to do any necessary initialization, but the
programmer must explicitly call these function to perform the initialization.

Similarly, because AVRTools "assumes the programmer knows what he or she is doing," it doesn't conduct a lot of checks
to ensure you don't do something stupid.  For example, when you set the output value of a digital pin using the Arduino
library
function `digitalWrite()`, it checks if that pin is currently configured for PWM and if it is, it automatically
turns off PWM-mode
before writing to the pin. The equivalent of `digitalWrite()` in the AVRTools library, `writeGpioPinDigital()` doesn't do that:
it assumes that if
the programmer previously used the pin in PWM mode that he or she remembered to turn off PWM mode before using the
pin digitally.  Assuming the programmer knows what he or she is doing allows the functions in AVRTools to be much faster
than their Arduino library counterparts.  For example, a call to the Arduino function `digitalWrite()` takes about 70 cycles;
a call to the equivalent AVRTools function `writeGpioPinDigital()` takes
2 cycles (it's actually a macro in AVRTools that the compiler translates to a single, 2-cycle assembler instruction).

# Audience #                                                          {#AvrAudience}

If you are an Arduino programmer, you may want to try AVRTools if:
- You are comfortable programming the Arduino Uno and Mega directly using the the avr-gcc toolset.
- You are frustrated by the slowness of even simple functions in the official Arduino libraries.
- Your code doesn't fit into the available memory because the official Arduino libraries are so big.

If you are an ATmega328 or ATmega2560 microcontroller programmer, you may want to try AVRTools if:
- You are secretly jealous of how easy and convenient it is to use the Arduino libraries.
- You wish you could bind together DDRs, PORTs, and PINs so you didn't have to write code like:

~~~C
#define MY_PIN_DDR      DDRB
#define MY_PIN_PORT     PORTB
#define MY_PIN_PIN      PINB
#define MY_PIN_NBR      7

/* Put MY_PIN in output mode and set it high */
MY_PIN_DDR |= (1<<MY_PIN_NBR)
MY_PIN_PORT |= (1<<MY_PIN_NBR)
~~~

- You wish you could use a function-like syntax to switch input/output mode, read a pin, or set a pin high or low but still have the compiler generate simple
`in` and `out` type of instructions.

If you fit into either category, then you should read further.

# AVRTools is not... #       {#AvrToolsIsNot}

AVRTools is not a general purpose AVR programming library.  I use the Arduino Uno and the Arduino Mega in my projects, and
I wrote AVRTools to support these specific needs.  There is conditional code throughout the implementation that is tailored to the
ATmega328 and ATmega2560 microcontrollers.  Additional conditional code could be added to create corresponding implementations for
other AVR processors in the AT-family, but I haven't done it.  Furthermore, the code is written for (and works with) microcontrollers
running at either 8 MHz, 12 MHz, or 16 MHz (the code automatically adapts to these three clock speeds).  I have not tested any other clock speeds,
and some of the delay functions are specifically coded for 8 MHz, 12 MHz, and 16 MHz and will not work (as written) at other clock speeds.

Finally, the AVRTools interface is designed to meet my needs and coding style.  That means the interfaces are designed in ways
which may not reflect your usage.  A particular example of this is the I2C module, which is designed to support the I2C
idioms I use in my projects and is significantly different from the I2C interface offered by the Arduino libraries.

AVRTools is a C++ library.  People may say that it is crazy to use C++ to
program a microcontroller because C++ adds bloat and overhead, because behind
your back the C++ compiler adds lots of code to make unnecessary copies, manage
heap objects, handle exceptions, etc.  C++, much like C, is a language that
rewards programmers who know what they are doing and punishes those who don't.
One can use C++ because it is a "better C" and use C++ features without
incurring performance penalties or code bloat.  For example, AVRTools uses
namespaces to compartmentalize functionality into logical units and avoid name
clashes; AVRTools also uses classes in a few cases where objects provide the
most natural and convenient implementation of a capability (for example, certain
advanced output classes such as `USART0` or `I2cLcd`; note that AVRTools also
provides a minimalistic USART interface using functions instead of classes,
because different needs call for different tools).


# Quick Tour of AVRTools #       {#QuickTour}

This section provides an overview of how AVRTools works, starting with the foundational elements and then summarizing the
modules that provide interfaces into the major hardware subsystems of the ATmega328 and ATmega2560 microcontrollers.

## Foundational Elements and Concepts ##  {#Foundations}

The foundation of the AVRTools library consists of a collection of macros that
enable you to refer to "pins" on the chips using a single name that can be used
to switch input/output mode, read, or write a pin.  This single name provides
access, as appropriate, to the DDRx, PORTx, PINx registers and also the specific
pin number.  For pins that support analog-to-digital conversion, the single name
also provides access the analog channel associated with the pin.  For pins that
support PWM, the single name also provides access to the control and compare
registers and bits needed to configure and control the PWM functionality of that
pin.

This is all done via preprocessor macros, both for the single pin name mechanism
and for the "functions" that make use of that single pin name. This means that
access to any pin-related functionality is as fast as possible, designed
specifically so that the `avr-gcc` compiler will emit simple 1- or 2-cycle `in`, `out`,
`sbi`, `cbi`, `sbic`, or `sbis` instructions for such operations whenever possible.  However, the
complex internal representation of the macros means that the pin names are
strictly constant and can only be passed to the specialized macro-functions
designed to manipulate them. Although they may look and feel like simple
constants, pin names cannot be assigned to variables, or passed to ordinary
C/C++ functions (however, see the [GPIO Pin Variables section] (@ref AdvancedGpioVars)
in the [Advanced Features] (@ref AdvancedFeatures) section for
a way to create and use variables for the GPIO pins). The AVRTools library does
include macro-functions to extract any of the components related to a pin name
so that users can access and manipulate the individual components as needed.

## What you need to know about pin name macros ##  {#WhatNeedKnow}

To access the pin names of the Arduino Uno or Mega, you only need to include the file "ArduinoPins.h".  It will automatically detect
whether you are compiling for Uno or Mega and it will correspondingly define the macros `pPinNN` (NN = 00 to 13 for Arduino Uno,
NN = 00 to 53 for Mega) for digital ports and macros `pPinAnn` (nn = 00 to 07 for Uno, nn = 00 to 15 for Mega) for the
analog ports.  These correspond directly to the labelled pins on the Arduino boards.  You can use these pin names to define
your own macros:

~~~C
#define THE_RED_LED             pPin12      // Red LED on Arduino pin 12
#define THE_GRN_LED             pPin11      // Green LED on Arduino pin 11
#define POTENTIOMETER           pPinA03     // Potentiometer on Arduino pin A3
~~~

While you cannot assign these to pin names to variables or pass them to ordinary functions, AVRTools provides a large collection of
macro-functions to operate on the pin names.  These include:

- `setGpioPinModeOutput( pin )`         Enable the corresponding DDRn bit
- `setGpioPinModeInput( pin )`          Clear the corresponding DDRn bit
- `setGpioPinModeInputPullup( pin )`    Clear the corresponding DDRn and PORTn bits
- `isGpioPinModeOutput( pin )`          Is the corresponding DDRn bit set?
- `isGpioPinModeInput( pin )`           Is the corresponding DDRn bit clear?
- `readGpioPinDigital( pin )`           Is the corresponding PINn bit is set? (returns zero or non-zero)
- `writeGpioPinDigital( pin, value )`   Write a 0 or 1 to the corresponding PORTn bit
- `setGpioPinHigh( pin )`               Set the corresponding PORTn bit
- `setGpioPinLow( pin )`                Clear the corresponding PORTn bit
- `readGpioPinAnalog( pin )`            Read an analog value from the corresponding ADC channel
- `writeGpioPinPwm( pin, value )`       Set the corresponding PWM output level for that pin

Most of these macros are automatically defined when you include "ArduinoPins.h", although to define the last two you need to include
"Analog2Digital.h" and "Pwm.h" (respectively).  These macros allow you to write code such as:

~~~C
    // Assuming everything has been initialized properly before this point

    setGpioPinModeOutput( THE_RED_LED );
    setGpioPinLow( THE_RED_LED );

    setGpioPinModeOutput( THE_GRN_LED );
    setGpioPinLow( THE_GRN_LED );

    if ( readGpioPinAnalog( POTENTIOMETER ) < 100 )
    {
        setGpioPinHigh( THE_RED_LED );
    }
    else
    {
        setGpioPinHigh( THE_GRN_LED );
    }
~~~

If you are working directly with an AVR ATmega328 or ATmega2560, you can define pin macros yourself by including "GpioPinMacros.h" (this file is automatically included for you when you include
"ArduinoPins.h" if you are working on Arduinos) and using one of three pin naming macros:

- `GpioPin( letter, number )`      An ordinary pin located on bank `letter` and bit `number`; for example the macro `GpioPin( B, 5 )` corresponds to pin PB5.
- `GpioPinAnalog( letter, number, channel )`    An ADC capable pin on bank `letter` and bit `number` with ADC `channel`, e.g., `GpioPinAnalog( C, 5, 5 )` for Atmega328 pin PC5/ADC5.
- `GpioPinPwm( letter, number, timer, channel )` A PWM capable pin on bank `letter` and bit `number` with `timer` and `channel` used to select the appropriate OCRn[A/B], TCCRnA registers, and COMn[A/B]1 bits needed to configure the PWM settings, e.g., GpioPinPwm( B, 2, 1, B ) for ATmega328 pin PB2/OC1B.

So for example, pin 11 on the Arduino Uno, which corresponds to ATmega328 pin B3 which is PWM capable using OC2A, would be defined as follows:

~~~C
#define pPin11  GpioPinPwm( B, 3, 2, A )
~~~

## The core modules ##        {#CodeModules}

In addition to the macro-based pin naming and access system discussed above, there are seven additional elements that make up the
core of AVRTools and provide access to basic functional elements of the ATmega328
and ATmega2560 microcontrollers.  Together, these provide an Arduino-like interface to the microcontroller features.
Five of the seven modules directly interface to microcontroller capabilities:

- [System initialization module] (@ref SysInit)
- [System clock module] (@ref SysClock)
- [Analog-to-Digital module] (@ref A2dMod)
- [PWM module] (@ref PwmMod)
- [Minimal USART modules] (@ref MinUsart)

Two of the seven modules supplement the C++ implementation provided by the `avr-gcc` toolset:

- [ABI module] (@ref AbiMod) (support for the C++ ABI not included in the `avr-gcc` distribution)
- [New module] (@ref NewMod) (implementation for `operator new` and `operator delete`)

Brief descriptions of these modules follow.

### System initialization module ###  {#SysInit}

This module provides a single function that puts the microcontroller in a clean, known state.  To use it include the
header file `InitSystem.h` and link against `InitSystem.cpp`.  These files provides a single function:

~~~C
void initSystem();
~~~

The `initSystem()` function clears any bootloader settings, clears all timers, and turns on interrupts.  This should be the first function your code calls at start up.

### System clock module ###   {#SysClock}

This module provides a system clock functionality similar to that in the Arduino library.  To employ this functionality include
the header file `SystemClock.h` and link against `SystemClock.cpp`.  Some of key functions provided by this module include:

~~~C
void initSystemClock();
unsigned long millis();
void delayMilliseconds( unsigned long ms );
~~~

Note that unlike the Arduino libary, you must explicitly initialize the clock functionality by calling `initSystemClock()`.
This module also provides additional functions providing a richer interface to the system clock.

### Analog-to-Digital module ###      {#A2dMod}

This module provides access to the analog read capabilities of the ATmega328 and ATmega2560.  To employ this functionality
include the header file `Analog2Digital.h` and link against `Analog2Digital.cpp`.  The principle functions provided by
this module include:

~~~C
void initA2D();
void turnOffA2D();
readGpioPinAnalog( pinName );   /* implemented as a macro */
~~~

You must initialize the analog-to-digital subsystem by calling `initA2D()`
before attempting to read any analog pins.

### PWM module ###            {#PwmMod}

This module provides access to the PWM features available on certain ATmega328 and ATmega2560 pins.  To employ this
functionality include the header file `Pwm.h` and link against `Pwm.cpp`.  The principle functions provided by this
module include (among others):

~~~C
void initPwmTimer1();
void initPwmTimer2();
void clearTimer1();
void clearTimer2();
writeGpioPinPwm( pinName, value );  /* implemented as a macro */
~~~

Depending on which pins you wish to employ in PWM mode, you should initialize the appropriate timers by calling the appropriate
`initPwmTimerN()` function (where `N` is the appropriate timer number) before writing to the pin in PWM mode.
This module also includes additional functions
to access the extended PWM capabilities of the ATmega2560.  The philosophical difference between
the standard Arduino library and AVRTools is evident in this module:  none of these function try to deduce which timers
need to be turned on for any given pin, because that would require adding extra code and look-up tables.
Instead AVRTools assumes the programmer will check the appropriate references to determine which timers correspond to
the pins they want to use in PWM mode, and will use that knowledge to initialize the appropriate timers.  For convenience,
tables of PWM-capable pins and corresponding timers are included in the `Pwm.h` documentation.

### Minimal USART modules ###     {#MinUsart}

These modules provide basic functionality for reading and writing from the USARTs available on the ATmega328
and ATmega2560.  To employ the %USART0 functionality, you must include the header file `USART0Minimal.h` and link against the
file `USART0Minimal.cpp`.
The principle functions for accessing the USARTs are:

~~~C
void initUSART0( unsigned long baudRate );
void transmitUSART0( unsigned char data );
void transmitUSART0( const char* data );
unsigned char receiveUSART0();
void releaseUSART0();
~~~

To make use of the %USART0 capability, first call `initUSART0()` to initialize the USART.  Then you can use
`transmitUSART0()` and `receiveUSART0()` functions to communicate on %USART0.  When you are done with %USART0 and
want to use pins 0 and 1 for other purposes, call `releaseUSART0()`.  Similar functions are provided to access the
other three USARTs available on the ATmega2560; simply include `USARTnMinimal.h` and link against the
file `USARTnMinimal.cpp`, where `n = 1, 2,` or `3`.  If you want more advanced serial capabilities, checkout the class
Serial0 in `USART0.h`.

### ABI module ###        {#AbiMod}

You only need this module if building your code produces link errors regarding missing symbols with strange names
like `__cxa_XXX` (where `XXX` is some unusual string).  In that case, simply link your code against `abi.cpp`.  These are
symbols related to the way the avr-gcc C++ compiler implements abstract virtual functions.

### New module ###           {#NewMod}

This module implements `operator new` and `operator delete`.  You only need this if you use `new` and `delete` to
manage objects on the heap.  Link against `new.cpp` to make use of these operators.  AVRTools itself does not make
any use of heap objects or operators `new` or `delete`.


# Sample start up code using AVRTools #       {#SampleStartCode}

You can use AVRTools to create an environment that is very similar to the standard Arduino environment.  The
following sample code illustrates how to do this.  The sample code reads a potentiometer and sets both a
digital pin and a PWM pin based on the
value of the potentiometer.

~~~C
#include "AVRTools/ArduinoPins.h"
#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"
#include "AVRTools/Analog2Digital.h"
#include "AVRTools/Pwm.h"

#define pPot            pPinA01
#define pPwmLed         pPin11
#define pLed            pPin04

init main()
{
    initSystem();
    initSystemClock();
    initPwmTimer2();
    initA2D();

    setGpioPinModeOutput( pLed );
    setGpioPinModeOutput( pPwmLed );
    setGpioPinModeInput( pPot );

    while ( 1 )
    {
        int i = readGpioPinAnalog( pPot ) / 4;

        writeGpioPinPwm( pPwmLed, i );

        if ( i > 127 )
        {
            setGpioPinHigh( pLed );
        }
        else
        {
            setGpioPinLow( pLed );
        }

        delayMilliseconds( 100 );
    }
}
~~~

# Advanced modules #                    {#AdvancedModIntro}

AVRTools also includes modules that provide access to more complex microcontroller capabilities and provide advanced services.
These include modules for I2C communication (both master and slave mode), a module for %SPI communications,
a module for more advanced serial input and output (including conversion of various numerical types and strings),
a module for temporarily surpressing selected interrupts, a module for driving an LCD display via I2C, a module for
reporting memory utilization, a module for very precise delays, and a module for manipulating GPIO pins as actual
variables.  Information on these modules can be found in the [Advanced Features] (@ref AdvancedFeatures) sections of
the documentation.

# Documentation #

Detailed documentation is provided by this PDF document located in the repository, or
[online in HTML form] (http://igormiktor.github.io/AVRTools/).


# Questions #                           {#QuestionsIntro}

If you have questions, please check out the [FAQ] (@ref AvrFaq).

