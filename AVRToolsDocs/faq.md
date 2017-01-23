FAQ                   {#faq}
=================


# Frequently Asked Questions #              {#AvrFaq}

- [Can AVRTools be installed as an Arduino IDE Library?] (@ref FaqIdeLib)
- [Why can't I assign pins like pPin01 to a variable?] (@ref FaqPins)
- [Why isn't the SPI module asynchronous?] (@ref FaqSpiAsynch)
- [Why does the SPI module only implement master mode?] (@ref FaqSpiMaster)
- [Why is there a setGpioPinHigh() macro and a _setGpioPinHigh() macro?] (@ref FaqWhyUnder)
- [_setGpioPinHigh() is defined with 8 arguments, but called with 1: how can that work?] (@ref FaqHowWork)
- [Why is there a setGpioPinHigh() macro and a setGpioPinHighV() function?] (@ref FaqWhyV)



# Can AVRTools be installed as an Arduino IDE Library? #      {#FaqIdeLib}

No, AVRTools is designed to replace the Arduino Library.  It is designed for use directly
with the `avr-gcc` compiler (the
same compiler used by the Arduino IDE).


# Why can't I assign pins like pPin01 to a variable? #      {#FaqPins}

Because pin names like `pPin01` are actually complex macros that expand to a
comma separated list of other macros.  The macro pin names can only be understood
and used by the function macros specifically designed to use them. This is
explained in greater detail in
[What you need to know about pin name macros] (@ref WhatNeedKnow).

If you really need GPIO pin variables, there is a way to do it.
See the section on [GPIO pin variables] (@ref AdvancedGpioVars). Note in particular
that GPIO pin variables come with high costs, both in speed and memory requirements.



# Why isn't the SPI module asynchronous? #                                      {#FaqSpiAsynch}

The SPI module is implemented synchronously using polling loops because actual testing
has shown this to be nearly twice as fast as implementing the functionality
asynchronously using interrupts.  Tomaž Šolc has done the research and posted the
results on his
[blog] (https://www.tablix.org/~avian/blog/archives/2012/06/spi_interrupts_versus_polling/).
Check it out (and check out the other articles, his blog is pretty interesting).




# Why does the SPI module only implement master mode? #                     {#FaqSpiMaster}

Easy answer: I have never needed anything other than %SPI master mode.  In every case I
use %SPI, the AVR microcontroller is the master talking to some external sensor or device
that is the slave.  While AVR's %SPI hardware supports slave mode, I don't think it
is common.  If you want to use %SPI to communicate across two AVR microcontrollers,
obviously one of them would have to be in slave mode.  But in that situation, I'd
probably have them communicate via a serial connection.

If you need a slave mode %SPI interface, let me know.  It's pretty straightforward to
code.  I may well get around to doing it one day in any case, just for completeness.


# Why is there a setGpioPinHigh() macro and a _setGpioPinHigh() macro? #          {#FaqWhyUnder}

Getting maximum efficiency from the GPIO pin name macros while making them
easy to use requires a series of recursive macro expansions.  To make this work,
it is essential to force rescanning of macro expansions, and using nested macro
function calls is a practical way to force macro rescanning.  So all of the
GPIO pin related macro functions call a helper macro function that has the
same name except for a prepended underscore.

The helper macro functions are an internal implementation detail, and that is
why they are not formally documented.



# _setGpioPinHigh() is defined with 8 arguments, but called with 1: how can that work? #      {#FaqHowWork}

Someone has been reading header files.  It works because of the magic of the
C/C++ preprocessor rescanning rules.  The rescanning rules are described in
6.10.3.4 of the [ISO Standard for C]
(http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1124.pdf) (the same rules
apply to C++). It requires lawyer-like abilities to completely comprehend the full
implications of this short paragraph.  However, the gist of it is that if
you have the following three macros:

~~~C
#define BAR(X,Y) (X+Y)
#define FOO(X) BAR(X)
#define A B,C
~~~

And if you then call `FOO(A)` in your code, the preprocessor executes the following steps:
- first `FOO(A)` is expanded to `BAR(A)`
- next `BAR(A)` is expanded to `BAR(B,C)`
- then finally `BAR(B,C)` is expanded to `(B+C)`.

This preprocessor rescanning logic is what powers all of the pin macro magic, not just
_setGpioPinHigh().




# Why is there a setGpioPinHigh() macro and a setGpioPinHighV() function? #        {#FaqWhyV}

All of the GPIO pin related "functions" come in two versions.  The versions
that do not end in a "V" are actually macros and work with the GPIO pin
name macros (e.g, `pPin01`).  The versions that end with a "V" are true
functions and work with GPIO variables.  See
[GPIO pin variables] (@ref AdvancedGpioVars).


