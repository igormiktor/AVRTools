/*
    SPI.h - an interface to the SPI subsystem of the
    AVR ATMega328p (Arduino Uno) and ATMega2560 (Arduino Mega).
    This is part of the AVRTools library.
    Copyright (c) 2015 Igor Mikolic-Torreira.  All right reserved.
    Various portions of this code adapted from Arduino SPI code that
    is Copyright (c) 2010 by Cristian Maglie, Copyright (c) 2014 by Paul Stoffregen,
    Copyright (c) 2014 by Matthijs Kooijman, and Copyright (c) 2014 by Andrew J. Kroll
    and licensed under the terms of either the GNU General Public License version 2
    or the GNU Lesser General Public License version 2.1.

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
 * \brief This file provides an interface to %SPI subsystem available on the AVR ATMega328p (Arduino Uno)
 * and ATMega2560 (Arduino Mega) microcontrollers.
 */



#ifndef SPI_h
#define SPI_h

#include <stdint.h>
#include <stddef.h>

#include <avr/io.h>


/*!
 * \brief This namespace bundles an interface to the %SPI hardware subsystem on the AVR ATMega328p (Arduino Uno)
 * and ATMega2560 (Arduino Mega) microcontrollers.  It provides logical cohesion for functions implement the
 * Master portion of the %SPI protocol and prevents namespace collisions.
 *
 * These interfaces are synchronous, based on polling the flag in the %SPI status register to determine
 * transmission is complete and refill the transmit register with data.
 * While it is possible to create an interrupt driven, asynchronous
 * interface to the %SPI subsystem, %SPI-based communications are so fast that interrupt-based implementations are
 * slower than polling by nearly a factor of 2.  This is based on actual testing data which you can review
 * [here] (https://www.tablix.org/~avian/blog/archives/2012/06/spi_interrupts_versus_polling/). What happens is that
 * %SPI can work at half the CPU frequency, which means the CPU can only execute about 16 instructions per
 * byte sent.  When the CPU is calling interrupts that often, the overhead of calling the interrupt function dominates,
 * and is greater than the overhead of a simple polling loop.
 *
 * The AVRTools implementation is based in part on the Arduino Library %SPI module.  In particular, the
 * SPISettings class from the Arduino library is very cleverly and efficiently coded and has been adopted here.
 * The lessons learned by the Arduino library %SPI authors in correctly initializing the %SPI subsystem have
 * also been incorporated into this implementation.  However, the packaging of the interface is somewhat different
 * the AVRTools implementation takes a different approach to deconflicting %SPI usage between the main thread of
 * code execution and interrupt code.
 *
 * The fundamental problem is this:  if %SPI is used in both the main code and in interrupt code, then it is
 * important to ensure that the %SPI "transactions" not be interleaved, that only one %SPI "transaction" happen
 * at a time.  More specifically, you have to ensure that interrupt code using %SPI does not interrupt an
 * on-going %SPI transaction in the main thread.  The Arduino library achieves this by requiring library
 * users to register any interrupts that use %SPI and then requirng users to formally define (via function calls)
 * the beginning and end of an %SPI "transaction".  The AVRTools library instead
 * provides tools (via the InterruptUtils module) to temporarily suppress selected interrupts while the main thread
 * is executing an %SPI transaction.  This approach allows more fine-tuned control of interrupt suppression,
 * automatically restores interrupts when the %SPI transaction is complete (no risk of a missing
 * "end-of-transaction"), and does it with less overhead and memory footprint than the Arduino %SPI library.  The
 * following code snippet illustrates how to protect main thread %SPI usage from conflicts with %SPI usage by
 * two external interrupt handlers:
 *
 * ~~~{.cpp}
 * uint8_t send( uint8_t data )
 * {
 *   // SPI is used by the interrupt functions that respond to external interrupts 0 and 1,
 *   // so to prevent clashes, we suppress these two external interrupts for
 *   // the duration of this function
 *   Interrupts::ExternalOff interruptsOff( kExternalInterrupt0 | kExternalInterrupt1 );
 *
 *   // Configure SPI
 *   SPI::configure( SPISettings( 4000000, SPI::kLsbFirst, SPI::kSpiMode2 ) );
 *
 *   // Set the remote slave SS pin low to initiate a transmission
 *   setGpioPinLow( pConnectedToSlaveSSpin );
 *
 *   // Transmit
 *   uint8_t retVal = SPI::transmit( data );
 *
 *   // Set the remote slave SS pin high to terminate the transmission
 *   setGpioPinLow( pConnectedToSlaveSSpin );
 *
 *   // Interrupts automatically reset when this function exits
 *   return retVal;
 * }
 * ~~~
 *
 *
 * \note This module implements %SPI master mode only.
 */

namespace SPI
{

    /*!
     * \brief An enumeration that defines the byte order for multibyte %SPI transmissions.
     */
    enum ByteOrder
    {
        kLsbFirst    = 0,   //!< Least significant byte first   \hideinitializer
        kMsbFirst    = 1    //!< Most significant byte first    \hideinitializer
    };


    /*!
     * \brief An enumeration that defines the modes available for %SPI transmissions.
     *
     * There are four modes controlling whether data is shifted in and out on the
     * rising or falling edge of the data clock signal (called the phase, CPHA), and
     * whether the clock is idle when high or low (called the polarity, CPOL).  The four
     * modes are simply the possible combinations of phase and polarity.
     */
    enum SpiMode
    {
        kSpiMode0   = 0x00,   //!< Phase falling, idle low (CPHA = 0, CPOL = 0)     \hideinitializer
        kSpiMode1   = 0x04,   //!< Phase rising, idle low (CPHA = 1, CPOL = 0)      \hideinitializer
        kSpiMode2   = 0x08,   //!< Phase falling, idle high (CPHA = 0, CPOL = 1)    \hideinitializer
        kSpiMode3   = 0x0C    //!< Phase rising, idle high (CPHA = 1, CPOL = 1)     \hideinitializer
    };


    /*!
    * \brief A class that binds settings for configuring %SPI transmissions.
    *
    * The SPISettings object is used to configure the %SPI hardware. The three parameters are combined into
    * a single SPISettings object, which is passed to SPI::configure().  You need to configure the %SPI
    * subsystem in this way before transmitting any data.  The configuration remains in effect until explicitly
    * changed by another call to SPI::configure() or the %SPI subsystem is disabled by a call to
    * SPI::disable().
    *
    * This class is taken almost verbatim from the Arduino library SPISettings class created by
    * Matthijs Kooijman and licensed under terms of either the GNU General Public License version 2
    * or the GNU Lesser General Public License version 2.1.
    *
    * The implementation makes clever use of GCC intrinsic functions to do essentially all the heavy lifting at
    * compile time whenever the %SPI parameters are compile-time constants, producing very small and efficient
    * code in this case.  My modifications reformat the code to the AVRTools library conventions and adapt
    * the interface to align with the AVRTools %SPI implementation.
    */

    class SPISettings
    {
    public:

        /*!
         * \brief The constructor builds an SPISettings object out of three parameters
         * describing the maximum transmission speed, the data order (most or least
         * significant bit first), and the data mode (phase and polarity).  Note that bit
         * order extends to byte order when passing multibyte integers.
         *
         * The code is designed to be exceptionally efficient and small if all three
         * parameters are compile-time constants.
         *
         * \arg \c maxSpeed the maximum speed of transmission, in herz (Hz). For a %SPI
         * chip rated up to 16 MHz, use 16000000.
         * \arg \c bitOrder whether least significant or most significant bit is first.
         * Pass either kMsbFirst or kLsbFirst.
         * \arg \c dataMode sets the data mode (phase and polarity) for %SPI communications.
         * Pass one of kSpiMode0, kSpiMode1, kSpiMode2, or kSpiMode3.
         */

        SPISettings( uint32_t maxSpeed, uint8_t bitOrder, uint8_t dataMode )
        {
            if ( __builtin_constant_p( maxSpeed ) )
            {
                initAlwaysInline( maxSpeed, bitOrder, dataMode );
            }
            else
            {
                initMightInline( maxSpeed, bitOrder, dataMode );
            }
        }



        /*!
         * \brief The constructor builds an SPISettings object with default settings
         * corresponding to a maximum transmission speed of 8 MHz, most significant
         * bit first, and kSpiMode0.
         */

        SPISettings()
        {
            initAlwaysInline( 8000000, kMsbFirst, kSpiMode0 );
        }



        /*!
         * \brief Return the appropriate configure value for the SPCR register.
         *
         * \returns a value to load in the SPCR register to configure the %SPI hardware.
         */

        uint8_t getSpcr() const
        {
            return mSpcr;
        }


        /*!
         * \brief Return the appropriate configure value for the SPSR register.
         *
         * \returns a value to load in the SPSR register to configure the %SPI hardware.
         */

        uint8_t getSpsr() const
        {
            return mSpsr;
        }





    private:


        void initMightInline( uint32_t maxSpeed, uint8_t bitOrder, uint8_t dataMode )
        {
            initAlwaysInline( maxSpeed, bitOrder, dataMode );
        }


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

        void initAlwaysInline( uint32_t maxSpeed, uint8_t bitOrder, uint8_t dataMode )  __attribute__((__always_inline__))
        {
            /*
            * The following are internal constants
            */
            const uint8_t kSpiClockDiv4     = 0x00;
            const uint8_t kSpiClockDiv16    = 0x01;
            const uint8_t kSpiClockDiv64    = 0x02;
            const uint8_t kSpiClockDiv128   = 0x03;
            const uint8_t kSpiClockDiv2     = 0x04;
            const uint8_t kSpiClockDiv8     = 0x05;
            const uint8_t kSpiClockDiv32    = 0x06;

            const uint8_t kSpiModeMask      = 0x0C;   // CPOL = bit 3, CPHA = bit 2 on SPCR
            const uint8_t kSpiClockMask     = 0x03;   // SPR1 = bit 1, SPR0 = bit 0 on SPCR
            const uint8_t kSpi2xClockMask   = 0x01;   // SPI2X = bit 0 on SPSR


            // Clock settings are defined as follows. Note that this shows SPI2X
            // inverted, so the bits form increasing numbers. Also note that
            // fosc/64 appears twice
            // SPR1 SPR0 ~SPI2X Freq
            //   0    0     0   fosc/2
            //   0    0     1   fosc/4
            //   0    1     0   fosc/8
            //   0    1     1   fosc/16
            //   1    0     0   fosc/32
            //   1    0     1   fosc/64
            //   1    1     0   fosc/64
            //   1    1     1   fosc/128

            // We find the fastest clock that is less than or equal to the
            // given clock rate. The clock divider that results in clock_setting
            // is 2 ^^ (clock_div + 1). If nothing is slow enough, we'll use the
            // slowest (128 == 2 ^^ 7, so clock_div = 6).
            uint8_t clockDiv;

            // When the clock is known at compile time, use this if-then-else
            // cascade, which the compiler knows how to completely optimize
            // away. When clock is not known, use a loop instead, which generates
            // shorter code.
            if ( __builtin_constant_p( maxSpeed ) )
            {
                if ( maxSpeed >= F_CPU / 2 )
                {
                    clockDiv = 0;
                }
                else if ( maxSpeed >= F_CPU / 4 )
                {
                    clockDiv = 1;
                }
                else if ( maxSpeed >= F_CPU / 8 )
                {
                    clockDiv = 2;
                }
                else if ( maxSpeed >= F_CPU / 16 )
                {
                    clockDiv = 3;
                }
                else if ( maxSpeed >= F_CPU / 32 )
                {
                    clockDiv = 4;
                }
                else if ( maxSpeed >= F_CPU / 64 )
                {
                    clockDiv = 5;
                }
                else
                {
                    clockDiv = 6;
                }
            }
            else
            {
                uint32_t clockSetting = F_CPU / 2;
                clockDiv = 0;
                while ( clockDiv < 6 && maxSpeed < clockSetting )
                {
                    clockSetting /= 2;
                    clockDiv++;
                }
            }

            // Compensate for the duplicate fosc/64
            if ( clockDiv == 6 )
            {
                clockDiv = 7;
            }

            // Invert the SPI2X bit
            clockDiv ^= 0x1;

            // Pack into the SPISettings class
            mSpcr = _BV(SPE)
                    | _BV(MSTR)
                    | ( (bitOrder == kLsbFirst) ? _BV(DORD) : 0 )
                    | ( dataMode & kSpiModeMask )
                    | ( (clockDiv >> 1) & kSpiClockMask );

            mSpsr = clockDiv & kSpi2xClockMask;
        }

#pragma GCC diagnostic pop


        uint8_t mSpcr;
        uint8_t mSpsr;
    };




    /*!
     * \brief Enable the %SPI subsystem for transmission.
     *
     * This call enables the %SPI hardware and configures the MOSI, MISO, CLK, and SS pins, making them
     * unavailable for other uses.  It also sets a default configuration of the %SPI subsystem to
     * a maximum transmission speed of 8 MHz, most significant bit first, and kSpiMode0.
     *
     * \note Even though %SPI is configured in master-mode, the configuration of the SS pin is affected.
     * The SS pin is set to output to prevent inadvertent automatic triggering of slave-mode by the %SPI
     * hardware (this happens if a low signal is received on the SS pin).  Although the SS pin must be in
     * output mode, it can still be used as  a general purpose output port (it doesn't affect %SPI
     * operations as long as it remains in output mode).
     */

    void enable();



    /*!
     * \brief Disable the %SPI subsystem, precluding further transmissions.
     *
     * This call disables the %SPI hardware, releasing the MOSI, MISO, CLK, and SS pins for other uses.
     *
     * \note No further %SPI transmissions should be made after calling this function, unless you re-enable
     * the %SPI subsystem by again calling enable().
     */

    void disable();


    /*!
     * \brief Set the configuration of %SPI subsystem to match the needs of the system you are going to communicate
     * with.
     *
     * You should always configure the %SPI subsystem \e before transmitting any data.  The configuration settings
     * remain in place until a subsequent call to this function or until you disable %SPI.
     *
     * \note If you are using %SPI both from interrupts and from the main thread of execution, you must protect
     * %SPI onfigurations and transmissions from interleaving.  To do this, disable interrupts in the main
     * thread by using the appropriate objects from InterruptUtils.  %Interrupts should be disabled starting before
     * setting the configuration until the end of the corresponding data transmission.  For example:
     *
     * ~~~{.cpp}
     * uint8_t send( uint8_t data )
     * {
     *   // SPI is used by the interrupt functions that respond to external interrupts 0 and 1,
     *   // so to prevent clashes, we suppress these two external interrupts for
     *   // the duration of this function
     *   Interrupts::ExternalOff interruptsOff( kExternalInterrupt0 | kExternalInterrupt1 );
     *
     *   // Configure SPI
     *   SPI::configure( SPISettings( 4000000, SPI::kLsbFirst, SPI::kSpiMode2 ) );
     *
     *   // Set the remote slave SS pin low to initiate a transmission
     *   setGpioPinLow( pConnectedToSlaveSSpin );
     *
     *   // Transmit
     *   uint8_t retVal = SPI::transmit( data );
     *
     *   // Set the remote slave SS pin high to terminate the transmission
     *   setGpioPinLow( pConnectedToSlaveSSpin );
     *
     *   // Interrupts automatically reset when this function exits
     *   return retVal;
     * }
     * ~~~
     *
     */

    inline void configure( SPISettings settings )
    {
        SPCR = settings.getSpcr();
        SPSR = settings.getSpsr();
    }


    /*!
     * \brief Transmit a single byte using the %SPI subsystem.
     *
     * \arg \c data the byte to be transmitted.
     *
     * \returns the byte received from the %SPI subsystem.
     */

    inline uint8_t transmit( uint8_t data )
    {
        SPDR = data;
        /*
        * The following NOP introduces a small delay that can prevent the wait
        * loop from iterating when running at the maximum speed. This gives
        * about 10% more speed, even if it seems counter-intuitive. At lower
        * speeds it is unnoticed.
        */
        asm volatile( "nop" );
        while ( !( SPSR & _BV(SPIF) ) )
            ; // wait
        return SPDR;
    }


    /*!
     * \brief Transmit a word-sized integer (two bytes) using the %SPI subsystem.  The order in which
     * the bytes are sent is determined by the bit order configuration that has been set.
     *
     * \arg \c data the word-sized integer (two bytes) to be transmitted.
     *
     * \returns the word-sized integer (two bytes) received from the %SPI subsystem, with byte order
     * determined by the bit order configuration that has been set.
     */

    inline uint16_t transmit16( uint16_t data )
    {
        union
        {
            uint16_t val;
            struct
            {
                uint8_t lsb;
                uint8_t msb;

            };
        } in, out;

        in.val = data;

        if ( SPCR & _BV(DORD) )
        {
            SPDR = in.lsb;
            asm volatile( "nop" );              // See transmit( uint8_t ) function
            while ( !( SPSR & _BV(SPIF) ) )
                ;
            out.lsb = SPDR;

            SPDR = in.msb;
            asm volatile( "nop" );
            while ( !( SPSR & _BV(SPIF) ) )
                ;
            out.msb = SPDR;
        }
        else
        {
            SPDR = in.msb;
            asm volatile( "nop" );                // See transmit( uint8_t ) function
            while ( !( SPSR & _BV(SPIF) ) )
                ;
            out.msb = SPDR;
            SPDR = in.lsb;
            asm volatile( "nop" );
            while ( !( SPSR & _BV(SPIF) ) )
                ;
            out.lsb = SPDR;
        }

        return out.val;
    }


    /*!
     * \brief Transmit a long-word-sized integer (four bytes) using the %SPI subsystem.  The order
     * in which the bytes are sent is determined by the bit order configuration that has been set.
     *
     * \arg \c data the long-word-sized integer (four bytes) to be transmitted.
     *
     * \returns the long-word-sized integer(four bytes) received from the %SPI subsystem, with byte
     * order determined by the bit order configuration that has been set.
     */

    uint32_t transmit32( uint32_t data );



    /*!
     * \brief Transmit an array of bytes using the %SPI subsystem.  The bytes are transmitted
     * in array order.
     *
     * \arg \c buffer the array of bytes to transmit.  Incoming bytes are also stored here, replacing
     * the outgoing data, byte-for-byte.
     *
     * \returns nothing, but the received stream of bytes is loaded into the buffer, replacing
     * the data originally in the buffer.
     */

    inline void transmit( uint8_t* buffer, size_t count )
    {
        if ( count )
        {
            uint8_t* p = buffer;
            SPDR = *p;

            while ( --count > 0 )
            {
                uint8_t out = *(p + 1);
                while ( !( SPSR & _BV(SPIF) ) )
                    ;
                uint8_t in = SPDR;
                SPDR = out;
                *p++ = in;
            }

            while ( !(SPSR & _BV(SPIF) ) )
                ;
            *p = SPDR;
        }
    }


}   // End namespace

#endif

