/*
    InterruptUtils.h - Utilities for managing interrupts for
    AVR ATMega328p (Arduino Uno) and ATMega2560 (Arduino Mega).
    This is part of the AVRTools library.
    Copyright (c) 2015 Igor Mikolic-Torreira.  All right reserved.
    Functions printNumber() and printFloat() adapted from Arduino code that
    is Copyright (c) 2008 David A. Mellis and licensed under LGPL.

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


/*!
 * \file
 *
 * \brief This file provides utilities for temporarily disabling (suppressing)
 * interrupts of various kinds in a block of code.  It uses the C++ RAII paradigm
 * to ensure interrupt state is restored automatically when the block of code is
 * exited.  While all interrupts can be suppressed, tools are provided that allow
 * more selective control of which interrupts are suppressed.
 */



#ifndef InterruptUtils_h
#define InterruptUtils_h

#include <stdint.h>
#include <stddef.h>

#include <avr/io.h>
#include <util/atomic.h>





/*!
 * \brief This namespace bundles various utility classes designed to suppress selected
 * interrupts using the RAII idiom.
 */

namespace Interrupts
{

    /*!
     * \brief This class defines an object that disables all interrupts during its lifetime.
     * Interrupt state is restored by the object's destructor when the object goes out of
     * scope.
     */

    class AllOff
    {
    public:

        /*!
         * \brief Suppress all interrupts when the object is instantiated.
         */

        AllOff()
        {
            mSreg = SREG;
            cli();
        }


        /*!
         * \brief Re-enable interrupts, restoring the interrupt state as it was when the
         * object was instantiated
         */

        ~AllOff()
        {
            // Turn on global interrupt, only if it was already on.  Leave other bits alone.
            if ( mSreg & static_cast<uint8_t>(1 << SREG_I) )
            {
                sei();
            }
        }


    private:

        uint8_t mSreg;
    };







#if defined(__AVR_ATmega328P__)
#define kExternalInterruptMask         0x03
#elif defined(__AVR_ATmega2560__)
#define kExternalInterruptMask         0xFF
#else
#error "Undefined AVR processor type"
#endif


    /*!
     * \brief This enum lists the external interrupts that can be suppressed (disabled).  To
     * pass more than one external interrupt, simply "or" them.
     *
     * \hideinitializer
     */

    enum ExternalInterrupts
    {
        kExternalInterrupt0     = ( 1 << INT0 ),    //!< External interrupt 0  \hideinitializer
        kExternalInterrupt1     = ( 1 << INT1 ),    //!< External interrupt 1  \hideinitializer

#if defined(__AVR_ATmega2560__)
        kExternalInterrupt2     = ( 1 << INT2 ),    //!< External interrupt 2 (ATmega2560 only)  \hideinitializer
        kExternalInterrupt3     = ( 1 << INT3 ),    //!< External interrupt 3 (ATmega2560 only)  \hideinitializer
        kExternalInterrupt4     = ( 1 << INT4 ),    //!< External interrupt 4 (ATmega2560 only)  \hideinitializer
        kExternalInterrupt5     = ( 1 << INT5 ),    //!< External interrupt 5 (ATmega2560 only)  \hideinitializer
        kExternalInterrupt6     = ( 1 << INT6 ),    //!< External interrupt 6 (ATmega2560 only)  \hideinitializer
        kExternalInterrupt7     = ( 1 << INT7 ),    //!< External interrupt 7 (ATmega2560 only)  \hideinitializer
#endif

        kExternalInterruptAll   = kExternalInterruptMask    //!< All external interrupts  \hideinitializer
    };



    /*!
     * \brief This class defines an object that disables selected external interrupts during its lifetime.
     * The selected external interrupts are restored by the object's destructor when it goes out of scope.
     */

    class ExternalOff
    {
    public:

        /*!
         * \brief Suppress some or all of the external interrupts when the object is instantiated.
         *
         * \arg \c whichOnesToTurnOff is a bit mask, indicating the external interrupts to disable.
         * The mask bits correspond to the bits in the External Interrupt Mask Register (EIMSK).  If
         * the argument is omitted, all external interrupts will be disabled.
         *
         */

        ExternalOff( uint8_t whichOnesToTurnOff = kExternalInterruptMask )
        : mExternalInterruptsToSuppress( whichOnesToTurnOff & kExternalInterruptMask )
        {
            // Disable the selected interrupts
            EIMSK &= ~(mExternalInterruptsToSuppress);
        }


        /*!
         * \brief Re-enable the selected external interrupts.
         *
         */

        ~ExternalOff()
        {
            // Enable the selected interrupts
            EIMSK |= mExternalInterruptsToSuppress;
        }


    private:

        uint8_t mExternalInterruptsToSuppress;
    };







#if defined(__AVR_ATmega328P__)
#define kPinChangeInterruptMask         0x07
#elif defined(__AVR_ATmega2560__)
#define kPinChangeInterruptMask         0x07
#else
#error "Undefined AVR processor type"
#endif


    /*!
     * \brief This enum lists the pin change interrupts that can be suppressed (disabled).  To
     * pass more than one pin change interrupt, simply "or" them.
     *
     * \hideinitializer
     */

    enum PinChangeInterrupts
    {
        kPinChangeInterrupt0     = ( 1 << PCINT0 ),           //!< Pin change interrupt 0  \hideinitializer
        kPinChangeInterrupt1     = ( 1 << PCINT1 ),           //!< Pin change interrupt 1  \hideinitializer
        kPinChangeInterrupt2     = ( 1 << PCINT2 ),           //!< Pin change interrupt 2  \hideinitializer
        kPinChangeInterruptAll   = kPinChangeInterruptMask  //!< All pin change interrupts  \hideinitializer
    };



    /*!
     * \brief This class defines an object that disables selected pin change interrupts during its lifetime.
     * The selected pin change interrupts are restored by the object's destructor when it goes out of scope.
     */

    class PinChangeOff
    {
    public:

        /*!
         * \brief Suppress some or all of the pin change interrupts when the object is instantiated.
         *
         * \arg \c whichOnesToTurnOff is a bit mask, indicating the pin change interrupts to disable.
         * The mask bits correspond to the bits in the Pin Change Interrupt Control Register (PCICR).
         * If the argument is omitted, all pin change interrupts will be disabled.
         *
         */
        PinChangeOff( uint8_t whichOnesToTurnOff = kPinChangeInterruptMask )
        : mPinChangeInterruptsToSuppress( whichOnesToTurnOff & kPinChangeInterruptMask )
        {
            // Disable the selected interrupts
            PCICR &= ~(mPinChangeInterruptsToSuppress);
        }


        /*!
         * \brief Re-enable the selected pin change interrupts.
         *
         */
        ~PinChangeOff()
        {
            // Enable the selected interrupts
            PCICR |= mPinChangeInterruptsToSuppress;
        }


    private:

        uint8_t mPinChangeInterruptsToSuppress;
    };



};  // End namespace


#endif

