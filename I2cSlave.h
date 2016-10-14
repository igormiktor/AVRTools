/*
    I2cSlave.h - An I2C slave library
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
 * \file
 *
 * \brief This file provides functions that interface to the TWI (two-wire serial interface) hardware
 * of the Arduino Uno (ATmega328) and Arduino Mega (ATmega2560), providing a high-level interface to
 * I2C protocol communications.  Include this file if you want your application will operate in Slave mode
 * as defined in the I2C protocol.
 *
 * To use these functions, include I2cSlave.h and link against I2cSlave.cpp.
 *
 * These interfaces are buffered for recieving and sending data and operate using interrupts associated
 * with the TWI hardware.  This means data from the Master is received asynchronously and when reception
 * is complete, a user-supplied function is called.  That function has the option of placing data in a
 * buffer to be transmitted asynchronously back to the Master.
 *
 * The Slave buffer is a simple array.  The size of the Slave buffer can be set at compile time
 * via the macro constant \c I2C_SLAVE_BUFFER_SIZE.  The default size of the Slave buffer is 32 bytes.
 * You can change the default by defining the macro \c I2C_SLAVE_BUFFER_SIZE prior to including the
 * file I2cSlave.h, each time it is included.  So you should define it using a compiler option
 * (e.g., \c -DI2C_SLAVE_BUFFER_SIZE=64) to ensure it is consistently defined throughout your project.
 *
 * This interface assumes your application will operator in I2C Slave mode as defined in the I2C protocol.
 * If you wish your application to operate in I2C Master mode, then instead include I2cMaster.h and link
 * against I2cMaster.cpp.
 *
 * \note Only one of I2cMaster.cpp and I2cSlave.cpp can be linking into your application.  These two
 * files install different, incompatible versions of the TWI interrupt function. AVRTools does not support
 * building an application that functions both as a Master and as a Slave under the I2C protocol.  This limitation
 * allows the corresponding TWI interrupt functions to be significantly leaner and faster.
 *
 */



#ifndef I2cSlave_h
#define I2cSlave_h


#ifdef I2cMaster_h
#error "You cannot use both I2cMaster and I2cSlave in the same application"
#endif


#include <stdint.h>

#ifdef DEBUG_I2cSlaveDiary
#include "USART0.h"
#endif



#ifndef I2C_SLAVE_BUFFER_SIZE
#define I2C_SLAVE_BUFFER_SIZE       32
#endif

#if I2C_SLAVE_BUFFER_SIZE > 255
#error "I2C_SLAVE_BUFFER_SIZE exceeds size of a uint8_t"
#endif





/*!
 * \brief This namespace bundles the I2C-protocol-based interface to the TWI hardware.  It provides logical cohesion
 * for functions implement the Slave portions of the I2C protocol and prevents namespace collisions.
 *
 * These interfaces are buffered for both input and output and operate using interrupts associated
 * with the TWI hardware.  This means the functions return immediately after queuing data
 * for transmission and the transmission happens asynchronously, using the dedicated TWI hardware.
 *
 * These functions are designed around the normal operating modes of the I2C protocol.  From a Slave device point of view,
 * I2C communications consist of receiving a message from the Master telling it to do something, and in response:
 *
 * - Processing the message and taking whatever action is appropriate.
 * - If that action includes returning data to the Master, queuing that data for transmission.
 *
 * The functions defined by this module conform directly to the above I2C paradigm. The key function is processI2cMessage()
 * and must be defined by the user.  This function is called whenever the Slave
 * receives a message and is also used to pass back any data that should be transmitted back to the Master.
 */

namespace I2cSlave
{


    /*!
     * \brief This function must be defined by the user.  It is called by the TWI interrupt function installed as
     * part of I2cSlave.cpp whenever it receives a message from the Master.  The user should implement this function
     * to respond to the data in the buffer, taking actions and as appropriate returning data to the buffer
     * (for asynchronous transmission to the Master).
     *
     * The user should implement this function to do the following:
     *  - review the incoming data from the Master
     *  - take appropriate actions in response to that data
     *  - if data must be returned to the Master, write the data into the buffer and return the number of bytes
     * you placed in the buffer
     *  - if no data must be returned to the Master, return 0
     *
     * \note This function is called at interrupt time, so the implementation must be kept short.  If any significant
     * work must be done as a result of the message received from the Master, this function should simply set a flag
     * that can be detected by the main execution thread and have it do the heavy lifting.
     *
     * \arg \c buffer is both an input and output parameter.  On entrance to the function, it
     * contains the message received from the Master; on return from the function should contain
     * data (if any) that should be sent back to the Master.
     * \arg \c len is only an input parameter.  It is the number of received bytes in the
     * input buffer.
     *
     * \returns the number of bytes placed in the \c buffer to be sent back to the Master; 0 if no data is to
     * be returned to the Master.
     */

    uint8_t processI2cMessage( uint8_t* buffer, uint8_t len );




    /*!
    * \brief This enum lists I2C bus speed configurations.
    *
    * \hideinitializer
    */
    enum I2cBusSpeed
    {
        kI2cBusSlow                 = 0,                    //!< I2C slow (standard) mode: 100 KHz  \hideinitializer
        kI2cBusFast                 = 1                     //!< I2C fast mode: 400 KHz  \hideinitializer
    };



    /*!
    * \brief This enum lists I2C status codes reported by the various transmit functions.
    */
    enum I2cStatusCodes
    {
/*
        kI2cCompletedOk                    = 0x00,
        kI2cNotStarted                     = 0x01,
        kI2cInProgress                     = 0x02,
        kI2cError                          = 0x04,
        kI2cBusError                       = 0x07
*/
        kI2cCompletedOk                    = 0x00,          //!< I2C communications completed with no error.
        kI2cError                          = 0x01,          //!< I2C communications encountered an error.
        kI2cTxPartial                      = 0x02,          //!< I2C Master terminated transmission before all data were sent.
        kI2cRxOverflow                     = 0x04,          //!< Recieved a message larger than can be held in the receive buffer.
        kI2cInProgress                     = 0x06           //!< I2C communications on this message still in progress.
    };



    /*!
    * \brief This enum lists the options for controlling the built-in pullups in the TWI hardware.
    *
    * \hideinitializer
    */
    enum I2cPullups
    {
        kPullupsOff                 = 0,                    //!< Disable the built-in TWI hardware pullups   \hideinitializer
        kPullupsOn                  = 1                     //!< Enable the built-in TWI hardware pullups    \hideinitializer
    };




    /*!
     * \brief Configures the TWI hardware for I2C communications in Slave mode.  You must call this function before conducting any
     * I2C communications using the functions in this module.
     *
     * This function enables the TWI related interrupts and enables the built-in hardware pullups.
     *
     * \arg \c ownAddress is the I2C address for this slave.
     * \arg \c speed the speed mode for the I2C protocol.  The options are slow (100 KHz) or fast (400 KHz); the
     * default is fast (kI2cBusFast).
     * \arg \c answerGeneralCall pass true for the Slave to answer I2C general calls; false for the Slave to ignore
     * I2C general calls and only answer calls to his specific address.  The defaults is to not answer general calls.
     * and defaults to not answering I2C general calls.
     */
    void start( uint8_t ownAddress, uint8_t speed = kI2cBusFast, bool answerGeneralCall = false );


    /*!
     * \brief Terminates the I2C communications using the TWI hardware, and disables the TWI interrupts.
     *
     * After calling this function, you need to call start() again if you want to resume I2C communications.
     */
    void stop();


    /*!
     * \brief Sets the state of the internal pullups that are part of the TWI hardware.
     *
     * start() automatically enables the internal pullups.  You only need to call this function
     * if you want to turn them off, or if you want to alter their state.
     *
     * \arg \c set the desired state of the built-in internal pullup.  Defaults to enable (kPullupsOn).
     */
    void pullups( uint8_t set = kPullupsOn );


    /*!
     * \brief Reports whether the TWI hardware is busy communicating (either transmitting or
     * receiving).
     *
     * \returns true if the TWI hardware is busy communicating; false if the TWI hardware is idle.
     */
    bool busy();


#ifdef DEBUG_I2cSlaveDiary

    void setDebugSout( Serial0* s );

    void clearDebugI2cDiary();
    void dumpDebugI2cDiary();

#endif

};


#endif

