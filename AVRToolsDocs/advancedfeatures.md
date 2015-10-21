Advanced Features                   {#AdvancedFeatures}
=================


The AVRTools library includes either more advanced features:

- [Advanced serial (USART) module] (@ref AdvancedSerial)
- [I2C modules] (@ref AdvancedI2c)
- [I2C-based LCD module] (@ref AdvancedLcd)
- [Interrupt utilities module] (@ref InterruptUtils)
- [SPI module] (@ref SPI)
- [Memory utilities module] (@ref MemUtil)
- [Simple delays module] (@ref SimDelay)
- [GPIO pin variables] (@ref AdvancedGpioVars)

These features provide functionality that is different from that provided by the Arduino
libraries, either in the design of their interface or in the underlying implementation, or both.
While the core modules of the AVRTools library are basically independent and can be used
individually, these advanced features depend in various ways upon the core modules and, sometimes,
each other.  These dependencies are highlighted in the corresponding sections.

# Advanced serial (USART) module #       {#AdvancedSerial}

The advanced USART module provides two different high-level interfaces to
%USART0 hardware available on the Arduino Uno (ATmega328) and the Arduino Mega
(ATmega2560). These provide serial input and output that is flexible, buffered,
and asynchronous by exploiting the interrupts that are associated with the %USART0
hardware. This means the transmit functions return immediately after queuing
data in the output buffer for transmission, and the actual transmission happens
asynchronously while your code continues to execute. Similarly, data is received
asynchronously and placed into the input buffer for your code to read at its
convenience.

If you try to queue more data than the transmit buffer can hold, the write
functions will block until there is room in the buffer (as a result of data
being transmitted).  The receive buffer, however, will overwrite if it gets
full.  You must clear the receive buffer by reading it regularly when receiving
significant amounts of data.  The sizes of the transmit and receive buffers can
be set at compile time via macro constants.

Two interfaces are provided.  The first is provided in namespace USART0 and is a
functional interface that makes use of the buffering and asynchronous transmit
and receive capabilities of the microcontrollers. However, USART0 is limited to
transmitting and receiving byte and character streams.  Think of USART0 as a
buffered version of the receiveUSART0() and transmitUSART0() functions provided
by the [Minimal USART modules] (@ref MinUsart).

The second interface is Serial0.  Serial0 is the most advanced and capable
interface to the %USART0 hardware.  Serial0 provides a object-oriented interface
that includes the ability to read and write numbers of various types and in
various formats, all asynchronously. Serial0 is implemented using USART0, so you
may mix the use of USART0 and Serial0 interfaces in your code (although it is
not recommended).

To use these the advanced serial capabilities, include the file USART0.h in your source
code and link against the file USART0.cpp.

\note The advanced serial module is incompatible with the minimal interface to
%USART0.  If you link against the file USART0.cpp (even if you don't actually use Serial0
or USART0), do \e not call initUSART0() or releaseUSART0(); there is no point in
any case because the receiveUSART0()
and transmitUSART0() functions won't work.  You may, however, use
the minimal interface to access USART1, USART2, and/or USART3 while simultaneously
using Serial0 and USART0 to access %USART0.


Use of the timeout feature in Serial0
or USART0 requires linking against SystemClock.cpp and
calling initSystemClock() from your start-up code.

If you are coding for the ATmega2560, you can also use USART1, USART2, and
USART3 and/or Serial1, Serial2, and Serial3 to access the %USART1, %USART2, and
%USART3 hardware available on the ATmega2560.  These work just like USART0 and
Serial0.  Again, if you link against USART1.cpp, USART2.cpp, or USART3.cpp then
you cannot use the corresponding minimal interfaces for %USART1, %USART2, and
%USART3, and calling initUSART1() or releaseUSART1() (or their corresponding
equivalents for the other USARTs) will put that USART in an inoperable
configuration.



# I2C modules #                        {#AdvancedI2c}

These two modules provide two different interfaces to the two-wire serial interface (TWI) hardware
of the Arduino Uno (ATmega328) and Arduino Mega (ATmega2560), providing a high-level interface to
I2C protocol communications. There are two different modules corresponding to whether your application
will function as a [Master] (@ref AdvI2cMaster) (as per the I2C protocol), or as a
[Slave] (@ref AdvI2cSlave).

\note AVRTools does not support applications that function both as I2C Masters and I2C Slaves.  The two
I2C modules provided by AVRTools are incompatible and cannot be mixed.

Both modules offer interfaces that are buffered for both input and output which operate using interrupts associated
with the TWI hardware.  This means the asynchronous transmit functions return immediately after queuing data
in the output buffer for transmission and the transmission happens asynchronously, using
dedicated TWI hardware. Similarly, data is received asynchronously and placed into the input buffer.




## I2C Master module ##              {#AdvI2cMaster}

The [I2C Master module] (@ref I2cMaster) provides I2C-protocol-based interface
to the TWI hardware that implements the Master portions of the I2C protocol. The
interfaces are buffered for both input and output and operate using interrupts
associated with the TWI hardware.  This means the asynchronous transmit
functions return immediately after queuing data in the output buffer for
transmission and the transmission happens asynchronously, using dedicated TWI
hardware. Similarly, data is received asynchronously and placed into the input
buffer.

The interface offered by the [I2C Master module] (@ref I2cMaster) is designed
around the normal operating modes of the I2C protocol. From a Master device
point of view, I2C communications consist of sending a designated device a
message to do something, and then either:

- doing nothing because no further action required on the Master's part
(e.g., telling the designated device to shutdown)
- transmitting additional data needed by the designated device (e.g., you told
the designated device to store some data, next you need to send the data)
- receiving data from the designated device (e.g., telling the designated
device to report the current temperature or to read back some data from its memory)

For very simple devices, the receipt of the message itself can suffice to tell
it to do something.  More commonly, the instruction to the designated device
consists of a single byte that passes a "register address" on the device.  It is
called a register address because it often corresponds directly to a memory
register on the device.  But it is best to think of it as an instruction code to
the designated device (e.g., 0x01 = report the temperature; 0x02 = set the units
to either degrees F or degrees C (depending on additional data sent by the Master); 0x03 =
report the humidity; etc.)

The interface offered by the [I2C Master module] (@ref I2cMaster) conforms
directly to the above I2C paradigm.  For convenience, the interface  functions
come in both synchronous and asynchronous versions.  The synchronous versions
simply call the asynchronous versions and block internally until the
asynchronous operations are complete.

\note The [I2C Master module] (@ref I2cMaster) is incompatible with the
[I2C Slave module] (@ref I2cSlave): you must
use and link against only one of the two modules.


## I2C Slave module ##          {#AdvI2cSlave}

The [I2C Slave module] (@ref I2cSlave) provides I2C-protocol-based interface to
the TWI hardware that implements the Slave portions of the I2C protocol. The
interfaces are buffered for both input and output and operate using interrupts
associated with the TWI hardware.  This means the functions return immediately
after queuing data for transmission and the transmission happens asynchronously,
using the dedicated TWI hardware. Similarly, data is received asynchronously and
placed into a buffer.

The interface offered by the [I2C Slave module] (@ref I2cSlave) is designed
designed around the normal operating modes of the I2C protocol.  From a Slave
device point of view, I2C communications consist of receiving a message from the
Master telling it to do something, and in response:

- Processing the message and taking whatever action is appropriate.
- If that action includes returning data to the Master, queuing the data for transmission.

The interface offered by the [I2C Slave module] (@ref I2cSlave) conforms
directly to the above I2C paradigm.



# I2C-based LCD module #               {#AdvancedLcd}

The [I2C-based LCD module] (@ref I2cLcd) provides a high-level interface to an
LCD offering an I2C interface. The most common variant of this is HD44780U
controlled LCD driven by an MCP23017 that offers an I2C interface (such LCDs are
available from Adafruit and SparkFun).   This module allows you to write to the
LCD much as it if were a serial device and includes the ability to write numbers
of various types in various formats. It also lets you detect button presses on
the 5-button keypad generally assocaited with such devices.

\note The [I2C-based LCD module] (@ref I2cLcd) requires the [I2C Master module] (@ref I2cMaster).



# Interrupt utilities module #               {#InterruptUtils}

It is often necessary to suppress interrupts to avoid conflicts between
the main thread of code execution and code that runs in interrupts.  While
it is easy to suppress all interrupts using the AVR `cli()` function,
often a more selective approach is desireable.  And when interrupts
are suppressed, it is also easy to forget to re-enable them.

The Interrupts module addresses these problems by providing simple utility C++
classes whose constructors disable certain kinds of interrupts and corresponding
destructors re-enable them.  A block of code can suppress interrupts by simply
declaring an object of one of these classes; interrupts will be automatically
restored when the block of code is exited for any reason.  For example, if you
want to suppress two of the pin change interrupts in a certain block of code,
you would do this:

~~~{.cpp}
#include "AVRTools/InterruptUtils.h"

//
// ... snip ...
//

void dontLetPinChangeInterruptsHappenHere( uint8_t data )
{
    // Pin change interrupts 1 and 2 conflict with this function, so suppress these
    // two pin change interrupts for the duration of this function
    Interrupts::PinChangeOff interruptsOff( kPinChangeInterrupt1 | kPinChangeInterrupt2 );

    // Here is some code that would conflict with the interrupt routines
    // assigned to pin change interrupt 1 and 2...
    // ... snip ...

    // Pin change interrupts 1 and 2 are automatically restored when this
    // function exits
}
~~~

One common application for this is when using %SPI transmissions in both main thread code
and interrupt routines.  See the documentation for the [SPI module] (@ref SPI) for an
example.



# SPI module #               {#SPI}

The SPI module provides a high-level interface to the %SPI hardware subsystem present
on the AVR ATMega328p (Arduino Uno) and ATMega2560 (Arduino Mega) microcontrollers. This
module provides functions to initialize the %SPI hardware, configure it appropriately for
your needs, and transmit (and receive) data.  While the %SPI hardware supports asynchronous
transmission via an interrupt functions (analogous to the %I2C hardware), AVRTools does not
implement asynchronous %SPI transmission, instead implementing sychronous transmission that
polls the appropriate %SPI status register to determine when transmission of a byte has
completed.  The reason for this is that testing of polling and interrupt implementations by
[Tomaž Šolc] (https://www.tablix.org/~avian/blog/archives/2012/06/spi_interrupts_versus_polling/)
has shown that polling implementations are faster than interrupt-based implementations by nearly
a factor of 2.  This is because %SPI can work at half the CPU frequency; at this speed, the
CPU can only execute about 16 instructions per byte sent via %SPI.  When the CPU is calling
interrupts so often, the overhead of calling the interrupt function dominates, and is greater
than the overhead of a simple polling loop.

The SPI module only implements %SPI operation in master mode.  Slave mode %SPI operation is
not supported at this time.  In master mode, you may use any free pin as the Slave Select (SS)
for the remote device.

The SPI module contains functions to enable and disable the SPI hardware.  Note that
when enabled the %SPI hardware takes control of the MOSI, MISO, and CLK pins. It also
sets the local SS pin to output mode to prevent inadvertent automatic triggering of
slave-mode by the %SPI hardware.  This happens if a low signal is received on the SS pin.
The SS pin can still be used as a general purpose output port, because it doesn't affect
%SPI operations as long as it remains in output mode.

To use the SPI module, call the SPI::enable() function as part of your initialization.
Then when you are aready to transmit, configure the hardware appropriately using
SPI::configure(), write the receiving device's slave select pin LOW, call SPI::transmit() (or any one
of the related transmit functions) any number of times to transfer data, and finally write the
receiving device's SS pin HIGH to indicate that transmission has ended.

One potential complication may occur if you use %SPI to transmit data from inside an interrupt
routine and also use %SPI in the main execution thread.  In this situation, you have to make sure that
a %SPI transmission in the main thread is not interrupted by a %SPI transmission from an interrupt
routine.  You can do this very easily by disabling the appropriate interrupt (or interrupts) during the
period of time that an %SPI transmission occurs in the main thread.  The classes in InterruptUtils
allow you to selectively disable interrupts.

The following example code illustrates how to do this.  Assume that %SPI is used by interrupt routines
associated with pin change interrupts 0 and 1, and with external interrupt 1.  Your main thread code
would then look like this:


~~~{.cpp}

// ... snip ...

#include "AVRTools/SPI.h"
#include "AVRTools/InterruptUtils.h"

// ... snip ...

void initializeEverything()
{
    // ... snip ...

    // Initialize the SPI subsystem
    SPI::enable();
}

// ... snip ...

uint8_t sendData( uint8_t data )
{
  // SPI is used by the interrupt functions that respond to pin change interrupts 0 and 1,
  // and external interrupt 1.  To prevent clashes, we suppress these three interrupts
  // for the duration of this function
  Interrupts::PinChangeOff pinChangeOff( kPinChangeInterrupt0 | kPinChangeInterrupt1 );
  Interrupts::ExternalOff externalOff( kExternalInterrupt1 );

  // Configure SPI
  SPI::configure( SPISettings( 4000000, SPI::kLsbFirst, SPI::kSpiMode2 ) );

  // Set the remote slave SS pin low to initiate a transmission
  setGpioPinLow( pConnectedToSlaveSS );

  // Transmit (and receive)
  uint8_t retVal = SPI::transmit( data );

  // Set the remote slave SS pin high to terminate the transmission
  setGpioPinHigh( pConnectedToSlaveSSpin );

  // Interrupts automatically reset when this function exits
  return retVal;
}
~~~




# Memory utilities module #       {#MemUtil}

The [Memory Utilities module] (@ref MemUtils) provides functions that report the available
memory in SRAM.  These help you gauge in real-time whether your application is approaching
memory exhaustion.



# Simple delays module #       {#SimDelay}

The [Simple Delays module] (@ref SimpleDelays.h) provides simple delay functions that do not
involve timers or interrupts.  These functions simply execute a series of
nested loops with known and precise timing.

These functions are all implemented directly in assembler to guarantee cycle counts.  However,
if interrupts are enabled, then the delays will be at least as long as requested, but may actually be
longer.



# GPIO pin variables #                {#AdvancedGpioVars}

There is sometimes a desire to assign GPIO pins to variables.  Unfortunately,
the pin name macros defined for you when you include ArduinoPins.h or that you
define yourself using GpioPin(), GpioPinAnalog(), or GpioPinPwm() cannot be
assigned to variables or used for anything other than passing them to the
specialize macro functions designed to handle them.  This is normally not a big
limitation: the use of GPIO pins is generally encapsulated in functions or classes that
function much like software drivers, hiding the pins from the rest of the application.
Treating the pins as macro constants usually works well in such
situations.  However, there do sometimes arise situations in which it would be
convenient to be able to assign GPIO pins to variables and manipulate GPIO pins via those
variables.

AVRTools provides a way to convert GPIO pins macros into variables and provides
corresponding functions for manipulating those variables.  However, this convenience
comes at a very significant cost for two reasons.

The first reason is that functions that manipulate AVR I/O registers via variables
are inherently slower than those that manipulate them as constants.  When using
the GPIO pin macros, most operations map directly to `in` and `out` AVR
assembler instructions. However, due to the limitations of these instructions,
when using variables to pass the pins, the compiler must use slower `ld` and
`st` instruction to access the I/O registers (for more on this issue, see the section in the
[AVR-GCC FAQ] (http://www.nongnu.org/avr-libc/user-manual/FAQ.html#faq_port_pass)).  In addition,
when using variables and function calls the bit-shifts needed to generate suitable masks have
to be generated at run-time (often using loops) instead of at compile-time.

The second reason is that the variables that store GPIO pins are rather large.
On the AVR hardware architecture, manipulating a GPIO pin requires knowing three different I/O registers (DDRn,
PORTn, and PINn) and a bit number.  Access an analog pin requires a
corresponding analog-to-digital channel number.  Manipulating a PWM pin requires knowing
two additional registers (OCRn[A/B] and TCCRnA) and another bit number
(COMn[A/B]1).  So a general-purpose variable representing a GPIO pin has to store all of these
registers, bit numbers, and channel numbers.  It is possible to create smaller
GPIO pin variables by encoding information and using look-up tables. The costs
are still there, and it is simply a choice of where to pay them.  In AVRTools,
the choice is to implement "heavy" variables and avoid look-up tables and
encoding schemes.


In AVRTools, GPIO pin variables have type GpioPinVariable, which is a class defined in GpioPinMacros.h
(recall that this file is automatically included by ArduinoPins.h). There are also three macros that you can use to initialize
GPIO pin variables of type GpioPinVariable.  The three are: makeGpioVarFromGpioPin(), makeGpioVarFromGpioPinAnalog(), and
makeGpioVarFromGpioPinPwm().
These are used like this:

~~~C
    GpioPinVariable  pinA( makeGpioVarFromGpioPin( pPin10 ) );
    GpioPinVariable  pinB( makeGpioVarFromGpioPinAnalog( pPinA01 ) );
    GpioPinVariable  pinC = makeGpioVarFromGpioPinPwm( pPin03 );

    GpioPinVariable pinArray[3];
    pinArray[0] = pinA;
    pinArray[1] = pinB;
    pinArray[2] = makeGpioVarFromGpioPin( pPin07 );
~~~

Which macro you choose depends upon what functionality of the GPIO pin you plan to access: you are free to use makeGpioVarFromGpioPin()
with an analog pin macro (e.g., pPinA01) if you just plan to use the resulting variable digitally, but if you plan to use the analog capabilities of the GPIO pin, you must use makeGpioVarFromGpioPinAnalog() to initialize the variable.  Similarly for PWM
functionality.

Once you've created GPIO pin variables using the above macros, these variables can be assign and passed to functions as needed.  To use these GPIO pin variables, there are special function analogs of the pin manipulation macros.  These have the same names as the pin manipulation macros, except with a "V" appended:

Macro Version | Function Version  | Purpose
:------------ | :---------------  | :------
setGpioPinModeOutput( pinMacro ) | setGpioPinModeOutputV( const GpioPinVariable& pinVar ) | Enable the corresponding DDRn bit
setGpioPinModeInput( pinMacro ) | setGpioPinModeInputV( const GpioPinVariable& pinVar ) | Clear the corresponding DDRn bit
setGpioPinModeInputPullup( pinMacro ) | setGpioPinModeInputPullupV( const GpioPinVariable& pinVar )  | Clear the corresponding DDRn and PORTn bits
readGpioPinDigital( pinMacro ) | readGpioPinDigitalV( const GpioPinVariable& pinVar ) | Return the corresponding PINn bit
writeGpioPinDigital( pinMacro, value ) | writeGpioPinDigitalV( const GpioPinVariable& pinVar, bool value ) | Write a 0 or 1 to the corresponding PORTn bit
setGpioPinHigh( pinMacro )  | setGpioPinHighV( const GpioPinVariable& pinVar ) | Set the corresponding PORTn bit
setGpioPinLow( pinMacro ) | setGpioPinLowV( const GpioPinVariable& pinVar ) | Clear the corresponding PORTn bit
readGpioPinAnalog( pinMacro ) | readGpioPinAnalogV( const GpioPinVariable& pinVar ) | Read an analog value from the corresponding ADC channel
writeGpioPinPwm( pinMacro, value ) | writeGpioPinPwmV( const GpioPinVariable& pinVar, uint8_t value ) | Set the corresonding PWM output level for that pin

\note GPIO pin variables can only be passed to the function versions; GPIO pin variables cannot be passed to the macro versions.
Similarly, GPIO pin macros cannot be passed to the function versions.

To illustrate how GPIO pin variables can be used, here are two versions of a trivial program, the first using the macros, and
the second using variables.


## Example using GPIO pin macros ##                {#AdvancedGpioVarsExampleMacros}

Compiled for an Arduino Uno, the following program is 1,978 bytes.

~~~C
#include "AVRTools/ArduinoPins.h"
#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"

#define pRed            pPin10
#define pYellow         pPin07
#define pGreen          pPin04

int main()
{
    initSystem();
    initSystemClock();

    setGpioPinModeOutput( pGreen );
    setGpioPinModeOutput( pYellow );
    setGpioPinModeOutput( pRed );

    setGpioPinHigh( pGreen );
    setGpioPinHigh( pYellow );
    setGpioPinHigh( pRed );

    delayMilliseconds( 2000 );

    setGpioPinLow( pGreen );
    setGpioPinLow( pYellow );
    setGpioPinLow( pRed );

    while ( 1 )
    {
        delayMilliseconds( 1000 );

        setGpioPinLow( pRed );
        setGpioPinHigh( pGreen );

        delayMilliseconds( 1000 );

        setGpioPinLow( pGreen );
        setGpioPinHigh( pYellow );

        delayMilliseconds( 1000 );

        setGpioPinLow( pYellow );
        setGpioPinHigh( pRed );
    }
}
~~~



## Example using GPIO pin variables ##                {#AdvancedGpioVarsExampleVars}

Compiled for an Arduino Uno, the following program is 2,456 bytes (478 bytes larger than the macro version) and uses an
additional 45 bytes of SRAM compared to the macro version.

~~~C
#include "AVRTools/ArduinoPins.h"
#include "AVRTools/InitSystem.h"
#include "AVRTools/SystemClock.h"

#define pRed            pPin10
#define pYellow         pPin07
#define pGreen          pPin04

int main()
{
    initSystem();
    initSystemClock();

    GpioPinVariable pins[3];
    pins[0] = makeGpioVarFromGpioPin( pRed );
    pins[1] = makeGpioVarFromGpioPin( pYellow );
    pins[2] = makeGpioVarFromGpioPin( pGreen );

    for ( int i = 0; i < 3; i++ )
    {
        setGpioPinModeOutputV( pins[i] );
        setGpioPinHighV( pins[i] );
    }

    delayMilliseconds( 2000 );

    for ( int i = 0; i < 3; i++ )
    {
        setGpioPinLowV( pins[i] );
    }

    int i = 0;
    while ( 1 )
    {
        delayMilliseconds( 1000 );

        setGpioPinLowV( pins[i++] );
        i %= 3;
        setGpioPinHighV( pins[i] );
    }
}
~~~






