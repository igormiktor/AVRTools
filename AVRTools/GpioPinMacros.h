/*
    GpioPinMacros.h - Macros for naming and manipulating Arduino pins.
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
 *  \file
 *
 *  \brief This file contains the primary macros for naming and
 *  manipulating GPIO pin names.
 *
 *  Normally you do not include this file directly.  Instead include either ArduinoPins.h, which
 *  will automatically include this file.
 *
 */



#ifndef GpioPinMacros_h
#define GpioPinMacros_h


#include <avr/io.h>



/*! \brief Constants for digital values representing LOW and HIGH
 *
 */

enum
{
    kDigitalLow = 0,        //!< Value representing digital LOW
    kDigitalHigh = 1        //!< Value representing digital HIGH
};





/*

    These macros are implementation details for the port naming macros and are not intended
    for end-users.  These are required to make the macros work due to the
    reparsing and resubstitution rules of the C/C++ preprocessor.

*/


#define _GpioPin( ddr, port, pin, nbr )     \
                                        ddr, port, pin, nbr, -1, 0, -1, 0

#define _GpioPinAnalog( ddr, port, pin, nbr, adc )  \
                                        ddr, port, pin, nbr, adc, 0, -1, 0

#define _GpioPinPwm( ddr, port, pin, nbr, ocr, com, tccr )       \
                                     ddr, port, pin, nbr, -1, ocr, com, tccr

#define _isGpioPinModeOutput( ddr, port, pin, nbr, adc, ocr, com, tccr )        ( ddr & (1<<nbr) )

#define _isGpioPinModeInput( ddr, port, pin, nbr, adc, ocr, com, tccr )         (!( ddr & (1<<nbr) ))

#define _setGpioPinModeOutput( ddr, port, pin, nbr, adc, ocr, com, tccr )       ddr |= (1<<nbr)

#define _setGpioPinModeInput( ddr, port, pin, nbr, adc, ocr, com, tccr )        ddr &= ~(1<<nbr), port &= ~(1<<nbr)

#define _setGpioPinModeInputPullup( ddr, port, pin, nbr, adc, ocr, com, tccr )  ddr &= ~(1<<nbr), port |= (1<<nbr)

#define _readGpioPinDigital( ddr, port, pin, nbr, adc, ocr, com, tccr )         ( pin & (1<<nbr) )

#define _writeGpioPinDigital( ddr, port, pin, nbr, adc, ocr, com, tccr, value )     \
                                        do { if (value) port |= (1<<nbr); else port &= ~(1<<nbr); } while ( 0 )

#define _setGpioPinHigh( ddr, port, pin, nbr, adc, ocr, com, tccr )             port |= (1<<nbr)

#define _setGpioPinLow( ddr, port, pin, nbr, adc, ocr, com, tccr )              port &= ~(1<<nbr)

#define _getGpioDDR( ddr, port, pin, nbr, adc, ocr, com, tccr )                 ddr

#define _getGpioPORT( ddr, port, pin, nbr, adc, ocr, com, tccr )                port

#define _getGpioPIN( ddr, port, pin, nbr, adc, ocr, com, tccr )                 pin

#define _getGpioMASK( ddr, port, pin, nbr, adc, ocr, com, tccr )                (1<<nbr)

#define _getGpioADC( ddr, port, pin, nbr, adc, ocr, com, tccr )                 adc

#define _getGpioOCR( ddr, port, pin, nbr, adc, ocr, com, tccr )                 ocr

#define _getGpioCOM( ddr, port, pin, nbr, adc, ocr, com, tccr )                 com

#define _getGpioTCCR( ddr, port, pin, nbr, adc, ocr, com, tccr )                tccr





/*

    These macros are for end-users to name GPIO pins and manipulate GPIO pin name macros.

*/



/*!
 * \brief Primary macro-function for defining a GPIO pin name
 *
 * \arg \c portLtr an uppercase letter identifying the port (e.g., A, B, C, ...) the GPIO pin belongs to.
 * \arg \c pinNbr a number between 0 and 7 identifying the bit on that port that corresponds to the GPIO pin.
 *
 * \hideinitializer
 */

#define GpioPin( portLtr, pinNbr )          \
                                        _GpioPin( DDR##portLtr, PORT##portLtr, PIN##portLtr, pinNbr )



/*!
 * \brief Secondary macro-function for defining a GPIO pin name for GPIO pins that support analog conversion.
 *
 * \arg \c portLtr an uppercase letter identifying the port (e.g., A, B, C, ...) the GPIO pin belongs to.
 * \arg \c pinNbr a number between 0 and 7 identifying the bit on that port that corresponds to the GPIO pin.
 * \arg \c adcNbr a number representing the ADC converter channel corresponding to this GPIO pin (0-7 for ArduinoUno; 0-15 for ArduinoMega)
 *
 * \hideinitializer
 */

#define GpioPinAnalog( portLtr, pinNbr, adcNbr )    \
                                        _GpioPinAnalog( DDR##portLtr, PORT##portLtr, PIN##portLtr, pinNbr, adcNbr )


/*!
 * \brief Secondary macro-function for defining a GPIO pin name for GPIO pins that support PWM output.
 *
 * \arg \c portLtr an uppercase letter identifying the port (e.g., A, B, C, ...) the GPIO pin belongs to.
 * \arg \c pinNbr a number between 0 and 7 identifying the bit on that port that corresponds to the GPIO pin.
 * \arg \c timer a number representing the timer number associated with the PWM function on this GPIO pin.
 * \arg \c chan a letter (A, B, or C) representing the channel on the timer associated with the PWM function on this GPIO pin.
 *
 * \hideinitializer
 */

#define GpioPinPwm( portLtr, pinNbr, timer, chan )   \
    _GpioPinPwm( DDR##portLtr, PORT##portLtr, PIN##portLtr, pinNbr, OCR##timer##chan, COM##timer##chan##1, TCCR##timer##A )



/*!
 * \brief Test if the mode of the GPIO pin is output (i.e., the corresponding DDRn bit is set).
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \hideinitializer
 */

#define isGpioPinModeOutput( pinName )                                         _isGpioPinModeOutput( pinName )



/*!
 * \brief Test if the mode of the GPIO pin is input (i.e., the corresponding DDRn is clear).
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \hideinitializer
 */

#define isGpioPinModeInput( pinName )                                          _isGpioPinModeInput( pinName )



/*!
 * \brief Set the mode of the GPIO pin to output (i.e., set the corresponding DDRn bit).
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \hideinitializer
 */

#define setGpioPinModeOutput( pinName )                                         _setGpioPinModeOutput( pinName )



/*!
 * \brief Set the mode of the GPIO pin to input (i.e., clear the corresponding DDRn and PORTn bits).
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \hideinitializer
 */

#define setGpioPinModeInput( pinName )                                          _setGpioPinModeInput( pinName )



/*!
 * \brief Set the mode of the GPIO pin to input with pullup (i.e., clear the corresponding DDRn bit and set the PORTn bit).
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \hideinitializer
 */

#define setGpioPinModeInputPullup( pinName )                                    _setGpioPinModeInputPullup( pinName )



/*!
 * \brief Read the value of the GPIO pin (i.e., return the value of correspoinding the PINn bit).
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \returns 0 (false) or a non-zero (true) value
 *
 * \hideinitializer
 */

#define readGpioPinDigital( pinName )                                           _readGpioPinDigital( pinName )


/*!
 * \brief Write a value the GPIO pin (i.e., set or clear the correspoinding the PORTn bit).
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 * \arg \c val the value to be written: 0 means to clear the GPIO pin; any other value means to set it.
 *
 * \hideinitializer
 */

#define writeGpioPinDigital( pinName, val )                                     _writeGpioPinDigital( pinName, val )



/*!
 * \brief Write a 1 to the GPIO pin (i.e., set the correspoinding the PORTn bit).
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \hideinitializer
 */

#define setGpioPinHigh( pinName )                                               _setGpioPinHigh( pinName )



/*!
 * \brief Write a 0 the GPIO pin (i.e., clear the correspoinding the PORTn bit).
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \hideinitializer
 */

#define setGpioPinLow( pinName )                                                _setGpioPinLow( pinName )



/*!
 * \brief Get the DDRn corresponding to this GPIO pin.
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \returns a DDRn register name (e.g., DDRB)
 *
 * \hideinitializer
 */

#define getGpioDDR( pinName )                                                   _getGpioDDR( pinName )



/*!
 * \brief Get the PORTn corresponding to this GPIO pin.
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \returns a PORTn register name (e.g., PORTB)
 *
 * \hideinitializer
 */

#define getGpioPORT( pinName )                                                  _getGpioPORT( pinName )



/*!
 * \brief Get the bit number corresponding to this GPIO pin.
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \returns a number between 0 and 7
 *
 * \hideinitializer
 */

#define getGpioPIN( pinName )                                                   _getGpioPIN( pinName )



/*!
 * \brief Get the bit mask corresponding to this GPIO pin.
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \returns a byte-sized bitmask
 *
 * \hideinitializer
 */

#define getGpioMASK( pinName )                                                  _getGpioMASK( pinName )



/*!
 * \brief Get the ADC channel corresponding to this GPIO pin, assuming it is an ADC capable GPIO pin.
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \returns a number between 0-7 (for ATmega328) or between 0-15 (for ATmega2560).
 *
 * \hideinitializer
 */

#define getGpioADC( pinName )                                                   _getGpioADC( pinName )



/*!
 * \brief Get the OCR register corresponding to this GPIO pin, assuming it is a PWM capable GPIO pin.
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \returns a OCRn[A/B] register name (e.g., OCR2B)
 *
 * \hideinitializer
 */

#define getGpioOCR( pinName )                                                   _getGpioOCR( pinName )



/*!
 * \brief Get the COM bit name corresponding to this GPIO pin, assuming it is a PWM capable GPIO pin.
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \returns a COMn[A/B]1 bit name (e.g., COM2B1)
 *
 * \hideinitializer
 */

#define getGpioCOM( pinName )                                                   _getGpioCOM( pinName )



/*!
 * \brief Get the TCCR register corresponding to this GPIO pin, assuming it is a PWM capable GPIO pin.
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \returns a TCCTn[A/B] register name (e.g., TCCR2B)
 *
 * \hideinitializer
 */

#define getGpioTCCR( pinName )                                                  _getGpioTCCR( pinName )




/******************************************/

/*
 * Support for GPIO pin variables
 *
 */


typedef volatile uint8_t* Gpio8Ptr;
typedef volatile uint16_t* Gpio16Ptr;

/*!
 * \brief This class defines a type that can encode a GPIO pin as a variable.  Read the section on
 * [GPIO Pin Variables] (@ref AdvancedGpioVars) to understand how to use this class.
 *
 * There are also three macros that you need to create vaiables of type GpioPinVariable:
 * makeGpioVarFromGpioPin(), makeGpioVarFromGpioPinAnalog(), and makeGpioVarFromGpioPinPwm().
 * These are used like this:
 *
 * ~~~C
 *     GpioPinVariable  pinA( makeGpioVarFromGpioPin( pPin10 ) );
 *     GpioPinVariable  pinB( makeGpioVarFromGpioPinAnalog( pPinA01 ) );
 *     GpioPinVariable  pinC = makeGpioVarFromGpioPinPwm( pPin03 );
 *
 *     GpioPinVariable pinArray[3];
 *     pinArray[0] = pinA;
 *     pinArray[1] = pinB;
 *     pinArray[2] = makeGpioVarFromGpioPin( pPin07 );
 * ~~~
 *
 * Once you've done this, these variables can be assign and passed to functions as needed.  To use these GPIO pin variables,
 * there are special function analogs of the GPIO pin manipulation macros.  These have the same names as the GPIO pin manipulation macros,
 * except with a "V" appended.
 */

class GpioPinVariable
{
public:

    GpioPinVariable()
    : mDdr( 0 ), mPort( 0 ), mPin( 0 ), mOcr( 0 ), mTccr( 0 ), mCom( 0xFF ),
    mNbr( 0xFF ), mAdc( 0xFF )
    {}

    GpioPinVariable( Gpio8Ptr ddr, Gpio8Ptr port, Gpio8Ptr pin, int8_t nbr )
    : mDdr( ddr ), mPort( port ), mPin( pin ), mOcr( 0 ), mTccr( 0 ), mCom( 0xFF ),
    mNbr( static_cast<uint8_t>(nbr) ), mAdc( 0xFF )
    {}

    GpioPinVariable( Gpio8Ptr ddr, Gpio8Ptr port, Gpio8Ptr pin, int8_t nbr, int8_t adc )
    : mDdr( ddr ), mPort( port ), mPin( pin ), mOcr( 0 ), mTccr( 0 ), mCom( 0xFF ),
    mNbr( static_cast<uint8_t>(nbr) ), mAdc( static_cast<uint8_t>(adc) )
    {}

    GpioPinVariable( Gpio8Ptr ddr, Gpio8Ptr port, Gpio8Ptr pin, int8_t nbr, Gpio16Ptr ocr, Gpio8Ptr tccr, int8_t com )
    : mDdr( ddr ), mPort( port ), mPin( pin ), mOcr( ocr ), mTccr( tccr ), mCom( com ),
    mNbr( static_cast<uint8_t>(nbr) ), mAdc( 0xFF )
    {}

    /*! \brief Return a pointer to the DDR register. */
    Gpio8Ptr ddr() const
    { return mDdr; }

    /*! \brief Return a pointer to the PORT register. */
    Gpio8Ptr port() const
    { return mPort; }

    /*! \brief Return a pointer to the PIN register. */
    Gpio8Ptr pin() const
    { return mPin; }

    /*! \brief Return a pointer to the OCR register (PWM related). */
    Gpio16Ptr ocr() const
    { return mOcr; }

    /*! \brief Return a pointer to the TCCR register (PWM related). */
    Gpio8Ptr tccr() const
    { return mTccr; }

    /*! \brief Return the bit number of this GPIO pin within the DDR, PORT, and PIN registers. */
    uint8_t bitNbr() const
    { return mNbr; }

    /*! \brief Return the bit number needed for manipulating TCCR register (PWM related). */
    uint8_t com() const
    { return mCom; }

    /*! \brief Return the ADC channel number (analog-to-digital related). */
    uint8_t adcNbr() const
    { return mAdc; }


private:

    Gpio8Ptr        mDdr;
    Gpio8Ptr        mPort;
    Gpio8Ptr        mPin;
    Gpio16Ptr       mOcr;
    Gpio8Ptr        mTccr;
    uint8_t         mCom;
    uint8_t         mNbr;
    uint8_t         mAdc;
};





#define _makeGpioVarFromGpioPin( ddr, port, pin, nbr, adc, ocr, com, tccr )         \
                                                    GpioPinVariable( &(ddr), &(port), &(pin), nbr )

#define _makeGpioVarFromGpioPinAnalog( ddr, port, pin, nbr, adc, ocr, com, tccr )   \
                                                    GpioPinVariable( &(ddr), &(port), &(pin), nbr, adc )

#define _makeGpioVarFromGpioPinPwm( ddr, port, pin, nbr, adc, ocr, com, tccr )      \
                                                    GpioPinVariable( &(ddr), &(port), &(pin), nbr, ocr, tccr, com )



/*!
 * \brief Create a GPIO pin variable of type GpioPinVariable from a GPIO pin macro.
 *
 * \arg \c pinName a GPIO pin name macro generated by either GpioPin(), GpioPinAnalog(), or GpioPinPwm().
 *
 * \returns a GpioPinVariable.
 *
 * \hideinitializer
 */

#define makeGpioVarFromGpioPin( pinName )               _makeGpioVarFromGpioPin( pinName )


/*!
 * \brief Create a GPIO pin variable of type GpioPinVariable that can be used for analog-to-digital reading from a GPIO pin macro.
 *
 * \arg \c pinName a GPIO pin name macro generated by GpioPinAnalog().
 *
 * \returns a GpioPinVariable that can be used for analog-to-digital reading.
 *
 * \hideinitializer
 */

#define makeGpioVarFromGpioPinAnalog( pinName )         _makeGpioVarFromGpioPinAnalog( pinName )


/*!
 * \brief Create a GPIO pin variable of type GpioPinVariable that can be used for PWM from a GPIO pin macro.
 *
 * \arg \c pinName a GPIO pin name macro generated by GpioPinPwm().
 *
 * \returns a GpioPinVariable that can be used for PWM.
 *
 * \hideinitializer
 */

#define makeGpioVarFromGpioPinPwm( pinName )            _makeGpioVarFromGpioPinPwm( pinName )





/*!
 * \brief Test if the mode of the GPIO pin is output (i.e., the corresponding DDRn bit is set).
 *
 * \arg \c pinVar a GPIO pin variable of type GpioPinVariable.
 */

inline bool isGpioPinModeOutputV( const GpioPinVariable& pinVar )
{
    return *(pinVar.ddr()) & ( 1 << pinVar.bitNbr() );
}


/*!
 * \brief Test if the mode of the GPIO pin is input (i.e., the corresponding DDRn is clear).
 *
 * \arg \c pinVar a GPIO pin variable of type GpioPinVariable.
 */

inline bool isGpioPinModeInputV( const GpioPinVariable& pinVar )
{
    return  !( *(pinVar.ddr()) & ( 1 << pinVar.bitNbr() ) );
}





/*!
 * \brief Set the mode of the GPIO pin to output (i.e., set the corresponding DDRn bit).
 *
 * \arg \c pinVar a GPIO pin variable of type GpioPinVariable.
 *
 * \hideinitializer
 */

inline void setGpioPinModeOutputV( const GpioPinVariable& pinVar )
{
    *(pinVar.ddr()) |= (1 << pinVar.bitNbr() );
}




/*!
 * \brief Set the mode of the GPIO pin to input (i.e., clear the corresponding DDRn and PORTn bits).
 *
 * \arg \c pinVar a GPIO pin name variable of type GpioPinVariable.
 *
 * \hideinitializer
 */

inline void setGpioPinModeInputV( const GpioPinVariable& pinVar )
{
    *(pinVar.ddr()) &= ~( 1 << pinVar.bitNbr() );
    *(pinVar.port()) &= ~( 1 << pinVar.bitNbr() );
}



/*!
 * \brief Set the mode of the GPIO pin to input with pullup (i.e., clear the corresponding DDRn bit and set the PORTn bit).
 *
 * \arg \c pinVar a GPIO pin variable of type GpioPinVariable.
 *
 * \hideinitializer
 */

inline void setGpioPinModeInputPullupV( const GpioPinVariable& pinVar )
{
    *(pinVar.ddr()) &= ~( 1 << pinVar.bitNbr() );
    *(pinVar.port()) |= ( 1 << pinVar.bitNbr() );
}



/*!
 * \brief Read the value of the GPIO pin (i.e., return the value of correspoinding the PINn bit).
 *
 * \arg \c pinVar a GPIO pin variable of type GpioPinVariable.
 *
 * \returns 0 (false) or 1 (true)
 *
 * \hideinitializer
 */

inline bool readGpioPinDigitalV( const GpioPinVariable& pinVar )
{
    return *(pinVar.pin()) & ( 1 << pinVar.bitNbr() );
}




/*!
 * \brief Write a value the GPIO pin (i.e., set or clear the correspoinding the PORTn bit).
 *
 * \arg \c pinVar  a GPIO pin variable of type GpioPinVariable.
 * \arg \c val the value to be written: 0 means to clear the GPIO pin; any other value means to set it.
 *
 * \hideinitializer
 */

inline void writeGpioPinDigitalV( const GpioPinVariable& pinVar, bool value )
{
    if  ( value )
    {
        *(pinVar.port()) |= ( 1 << pinVar.bitNbr() );
    }
    else
    {
        *(pinVar.port()) &= ~( 1 << pinVar.bitNbr() );
    }
}



/*!
 * \brief Write a 1 to the GPIO pin (i.e., set the correspoinding the PORTn bit).
 *
 * \arg \c pinVar a GPIO pin variable of type GpioPinVariable.
 *
 * \hideinitializer
 */

inline void setGpioPinHighV( const GpioPinVariable& pinVar )
{
    *(pinVar.port()) |= ( 1 << pinVar.bitNbr() );
}




/*!
 * \brief Write a 0 to the GPIO pin (i.e., clear the correspoinding the PORTn bit).
 *
 * \arg \c pinVar aa GPIO pin variable of type GpioPinVariable.
 *
 * \hideinitializer
 */

inline void setGpioPinLowV( const GpioPinVariable& pinVar )
{
    *(pinVar.port()) &= ~( 1 << pinVar.bitNbr() );
}



#endif
