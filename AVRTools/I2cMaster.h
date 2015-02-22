/*
    I2cMaster.h - An I2C master library
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
 * I2C protocol communications.  Include this file if you want your application will operate in Master mode
 * as defined in the I2C protocol.
 *
 * To use these functions, include I2cMaster.h and link against I2cMaster.cpp.
 *
 * These interfaces are buffered for both input and output and operate using interrupts associated
 * with the TWI hardware.  This means the asynchronous transmit functions return immediately after queuing data
 * in the output buffer for transmission and the transmission happens asynchronously, using
 * dedicated TWI hardware. Similarly, data is received asynchronously and placed into the input buffer.
 *
 * The transmit buffer is a ring buffer.  If you try to queue more data than the transmit
 * buffer can hold, the write functions will block until there is room in the buffer (as a result of
 * data being transmitted).  Receive buffers are provided by the callers of these functions.  Note that
 * due to the nature of the I2C protocol, Master I2C "read" operations must still write a command instructing
 * the destination device to send data for the Master to read, and thus "read" operations still utilize the
 * transmit buffer.
 *
 * The size of the transmit buffer can be set at compile time via macro constants (the receive buffers are
 * provided the corresponding functions are called).  The default
 * size of the transmit buffer assumes the maximum transmit message length is 24 bytes and allows 3
 * out-going messages to be queued.  You can change these defaults by defining the macros
 * \c I2C_MASTER_MAX_TX_MSG_LEN to specify the maximum transmit message length and \c I2C_MASTER_MAX_TX_MSG_NBR
 * to specify the maximum number of transmit messages to hold in the buffer.  You need to make these define
 * these macros prior to including the file I2cMaster.h, each time it is included.  So you should define these
 * using a compiler option (e.g., \c -DI2C_MASTER_MAX_TX_MSG_LEN=32 \c -DI2C_MASTER_MAX_TX_MSG_NBR=5) to ensure they
 * are consistently defined throughout your project.
 *
 * This interface assumes your application will operator in I2C Master mode as defined in the I2C protocol.  If you
 * wish your application to operate in I2C Slave mode, then instead include I2cSlave.h and link against I2cSlave.cpp.
 *
 * \note Only one of I2cMaster.cpp and I2cSlave.cpp can be linking into your application.  These two
 * files install different, incompatible versions of the TWI interrupt function. AVRTools does not support
 * building an application that functions both as a Master and as a Slave under the I2C protocol.  This limitation
 * allows the corresponding TWI interrupt functions to be significantly leaner and faster.
 *
 */





#ifndef I2cMaster_h
#define I2cMaster_h


#ifdef I2cSlave_h
#error "You cannot use both I2cMaster and I2cSlave in the same application"
#endif


#include <stdint.h>
#include <stdlib.h>

#include <util/atomic.h>


#if defined( DEBUG_I2cMasterBuffer ) || defined( DEBUG_I2cMasterDiary )
#include "USART0.h"
#endif





#ifndef I2C_MASTER_MAX_TX_MSG_LEN
#define I2C_MASTER_MAX_TX_MSG_LEN       24
#endif

#ifndef I2C_MASTER_MAX_TX_MSG_NBR
#define I2C_MASTER_MAX_TX_MSG_NBR       3
#endif

#if I2C_MASTER_MAX_TX_MSG_LEN > 255
#error "I2C_MASTER_MAX_TX_MSG_LEN exceeds size of a uint8_t"
#endif

#if I2C_MASTER_MAX_TX_MSG_NBR > 255
#error "I2C_MASTER_MAX_TX_MSG_NBR exceeds size of a uint8_t"
#endif





/*!
 * \brief This namespace bundles the I2C-protocol-based interface to the TWI hardware.  It provides logical cohesion
 * for functions implement the Master portions of the I2C protocol and prevents namespace collisions.
 *
 * These interfaces are buffered for both input and output and operate using interrupts associated
 * with the TWI hardware.  This means the asynchronous transmit functions return immediately after queuing data
 * in the output buffer for transmission and the transmission happens asynchronously, using
 * dedicated TWI hardware. Similarly, data is received asynchronously and placed into the input buffer.
 *
 * These functions are designed around the normal operating modes of the I2C protocol.  From a Master device point of view,
 * I2C communications consist of sending a designated device a message to do something, and then either:
 *
 * - doing nothing because no further action required on the Master's part (e.g., telling the designated device to shutdown)
 * - transmitting additional data needed by the designated device (e.g., telling the designated device to store some data)
 * - receiving data from the designated device (e.g., telling the designated device to report the current temperature or to
 * read back some data from its memory)
 *
 * For very simple devices, the receipt of the message itself can suffice to tell it to do something.  More commonly,
 * the instruction to the designated device consists of a single byte that passes a "register address" on the device.  It is
 * call a register address because it often corresponds directly to a memory register on the device.  But
 * it is best to think of it as an instruction code to the designated device (e.g., 0x01 = report the temperature;
 * 0x02 = set the units to either F or C (depending on additional data sent by the Master); 0x03 = report the humidity; etc.)
 *
 * The functions defined by this module conform directly to the above I2C paradigm.  The functions come in both synchronous and
 * asynchronous versions.  The synchronous versions simply call the asynchronous versions and block internally until the
 * asynchronous operations are complete.
 *
 * Note also that even "read" operations always begin (from the Master's point of view) with a "send" to the designated device
 * the Master wants to read data from.  For this reason all operations (both read and write) utilize the transmit buffer.
 */

namespace I2cMaster
{

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
        kI2cCompletedOk                    = 0x00,          //!< I2C communications completed on this message with no error.
        kI2cError                          = 0x01,          //!< I2C communications had an error on this message.
        kI2cNotStarted                     = 0x02,          //!< I2C communications not started on this message.
        kI2cInProgress                     = 0x04           //!< I2C communications on this message still in progress.
    };


    /*!
    * \brief This enum lists I2C errors codes that may occur when you try to write a message.
    */
    enum I2cSendErrorCodes
    {
        kI2cNoError                 = 0,                    //!< No error
        kI2cErrTxBufferFull         = 1,                    //!< The transmit buffer is full (try again later)
        kI2cErrMsgTooLong           = 2,                    //!< The message is too long for the transmit buffer
        kI2cErrNullStatusPtr        = 3,                    //!< The pointer to the status variable is null (need to provide a valid pointer)
        kI2cErrWriteWithoutData     = 4,                    //!< No data provided to send
        kI2cErrReadWithoutStorage   = 5                     //!< Performing a write+read, but no buffer provided to store the "read" data
    };


    /*!
    * \brief This enum lists the options for controlling the built-in  pullups in the TWI hardware.
    *
    * \hideinitializer
    */
    enum I2cPullups
    {
        kPullupsOff                 = 0,                    //!< Disable the built-in TWI hardware pullups   \hideinitializer
        kPullupsOn                  = 1                     //!< Enable the built-in TWI hardware pullups    \hideinitializer
    };





    /*!
     * \brief Configures the TWI hardware for I2C communications in Master mode.  You must call this function before conducting any
     * I2C communications using the functions in this module.
     *
     * This function enables the TWI related interrupts and enables the built-in hardware pullups.
     *
     * \arg \c speed the speed mode for the I2C protocol.  The options are slow (100 KHz) or fast (400 KHz); the
     * default is fast (kI2cBusFast).
     */
    void start( uint8_t speed = kI2cBusFast );


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




    // Asynchronous functions


    /*!
     * \brief Transmit a single register address (a one-byte message) asynchronously. This function queues the
     * message and returns immediately.  Eventual status of the transmitted message can be monitored via the
     * designated status variable (passed as a pointer to this function).
     *
     * If the transmit buffer is full, this function will block until room is available in the buffer.
     *
     * \arg \c address the I2C address of the destination device for this message
     * \arg \c registerAddress in device-centric terms, the register address on the destination device; think of it as
     * a one-byte instruction to the destination device telling it to do something (e.g., turn off or on).
     * \arg \c status a pointer to a byte-size location in which the commincations status of this message will be
     * reported (volatile because the value will be updates asynchronously after the function returns by the TWI
     * hardware); values correspond to I2cStatusCodes.
     *
     * \returns error codes corresponding to I2cSendErrorCodes (0 means no error)
     */
    uint8_t writeAsync( uint8_t address, uint8_t registerAddress, volatile uint8_t* status );


    /*!
     * \brief Transmit a single register address and corresponding single byte of data asynchronously. This function queues the
     * message and returns immediately.  Eventual status of the transmitted message can be monitored via the
     * designated status variable (passed as a pointer to this function).
     *
     * If the transmit buffer is full, this function will block until room is available in the buffer.
     *
     * \arg \c address the I2C address of the destination device for this message
     * \arg \c registerAddress in device-centric terms, the register address on the destination device; think of it as
     * a one-byte instruction to the destination device telling it to do something (e.g., set the volume level).
     * \arg \c data a single byte of data serving as a parameter to the register address (e.g., the volume level to set).
     * \arg \c status a pointer to a byte-size location in which the commincations status of this message will be
     * reported (volatile because the value will be updates asynchronously after the function returns by the TWI
     * hardware); values correspond to I2cStatusCodes.
     *
     * \returns error codes corresponding to I2cSendErrorCodes (0 means no error)
     */
    uint8_t writeAsync( uint8_t address, uint8_t registerAddress, uint8_t data, volatile uint8_t* status );


    /*!
     * \brief Transmit a single register address and corresponding null-terminated string of data asynchronously.
     * This function queues the message and returns immediately.  Eventual status of the transmitted message can
     * be monitored via the designated status variable (passed as a pointer to this function).
     *
     * If the transmit buffer is full, this function will block until room is available in the buffer.
     *
     * \arg \c address the I2C address of the destination device for this message
     * \arg \c registerAddress in device-centric terms, the register address on the destination device; think of it as
     * a one-byte instruction to the destination device telling it to do something (e.g., an address in a memory device).
     * \arg \c data a null-terminated string of data serving as a parameter to the register address (e.g., a string to store
     * sequentially starting at the registerAddress).
     * \arg \c status a pointer to a byte-size location in which the commincations status of this message will be
     * reported (volatile because the value will be updates asynchronously after the function returns by the TWI
     * hardware); values correspond to I2cStatusCodes.
     *
     * \returns error codes corresponding to I2cSendErrorCodes (0 means no error)
     */
    uint8_t writeAsync( uint8_t address, uint8_t registerAddress, const char* data, volatile uint8_t* status );


    /*!
     * \brief Transmit a single register address and corresponding buffer of data asynchronously.
     * This function queues the message and returns immediately.  Eventual status of the transmitted message
     * can be monitored via the designated status variable (passed as a pointer to this function).
     *
     * If the transmit buffer is full, this function will block until room is available in the buffer.
     *
     * \arg \c address the I2C address of the destination device for this message
     * \arg \c registerAddress in device-centric terms, the register address on the destination device; think of it as
     * a one-byte instruction to the destination device telling it to do something (e.g., an address in a memory device).
     * \arg \c data a buffer of data serving as a parameter to the register address (e.g., the data to store
     * sequentially starting at the registerAddress).
     * \arg \c numberBytes the number of bytes from the buffer to transmit.
     * \arg \c status a pointer to a byte-size location in which the commincations status of this message will be
     * reported (volatile because the value will be updates asynchronously after the function returns by the TWI
     * hardware); values correspond to I2cStatusCodes.
     *
     * \returns error codes corresponding to I2cSendErrorCodes (0 means no error)
     */
    uint8_t writeAsync( uint8_t address, uint8_t registerAddress, uint8_t* data, uint8_t numberBytes,
                            volatile uint8_t* status );



    /*!
     * \brief Request to read data from a device and receive that data asynchronously.
     * This function queues the message and returns immediately.  Eventual status of the transmitted message
     * can be monitored via the designated status variable (passed as a pointer to this function).  When the status
     * variable reports kI2cCompletedOk, the requested data can be read from the receive buffer.
     *
     * If the transmit buffer is full, this function will block until room is available in the buffer.
     *
     * \arg \c address the I2C address of the destination device you want to read from.
     * \arg \c numberBytes the number of bytes you expect to read.
     * \arg \c destination a pointer to a buffer in which the received data will be stored; the buffer should be
     * at least \c numberBytes large.
     * \arg \c bytesRead a pointer to a byte-sized countered in which the TWI hardware will asynchronously keep track
     * of how many bytes have been received.
     * \arg \c status a pointer to a byte-size location in which the commincations status of this message will be
     * reported (volatile because the value will be updates asynchronously after the function returns by the TWI
     * hardware) values correspond to I2cStatusCodes.
     *
     * \returns error codes corresponding to I2cSendErrorCodes (0 means no error)
     */
    uint8_t readAsync( uint8_t address, uint8_t numberBytes, volatile uint8_t* destination,
                        volatile uint8_t* bytesRead, volatile uint8_t* status );


    /*!
     * \brief Request to read data from a specific register on a device and receive that data asynchronously.
     * This function queues the message and returns immediately.  Eventual status of the transmitted message
     * can be monitored via the designated status variable (passed as a pointer to this function).  When the status
     * variable reports kI2cCompletedOk, the requested data can be read from the receive buffer.
     *
     * If the transmit buffer is full, this function will block until room is available in the buffer.
     *
     * \arg \c address the I2C address of the destination device you want to read from.
     * \arg \c registerAddress in device-centric terms, the register address on the destination device; think of it as
     * a one-byte instruction to the destination device telling it what you want to read (e.g., temperature or the
     * starting address of a block of memory).
     * \arg \c numberBytes the number of bytes you expect to read.
     * \arg \c destination a pointer to a buffer in which the received data will be stored; the buffer should be
     * at least \c numberBytes large.
     * \arg \c bytesRead a pointer to a byte-sized countered in which the TWI hardware will asynchronously keep track
     * of how many bytes have been received.
     * \arg \c status a pointer to a byte-size location in which the commincations status of this message will be
     * reported (volatile because the value will be updates asynchronously after the function returns by the TWI
     * hardware); values correspond to I2cStatusCodes.
     *
     * \returns error codes corresponding to I2cSendErrorCodes (0 means no error)
     */
    uint8_t readAsync( uint8_t address, uint8_t registerAddress, uint8_t numberBytes,
                        volatile uint8_t* destination, volatile uint8_t* bytesRead,
                        volatile uint8_t* status );



    // Synchronous


    /*!
     * \brief Transmit a single register address (a one-byte message) synchronously. This function blocks until
     * the communications exchange is complete or encounters an error. Error codes are returned (0 means no error).
     *
     * \arg \c address the I2C address of the destination device for this message.
     * \arg \c registerAddress in device-centric terms, the register address on the destination device; think of it as
     * a one-byte instruction to the destination device telling it to do something (e.g., turn off or on).
     *
     * \returns an error code which if positive corresponds to I2cSendErrorCodes, or if negative the absolute value
     * corresponds to I2cStatusCodes (0 means no error).
     */
    int writeSync( uint8_t address, uint8_t registerAddress );


    /*!
     * \brief Transmit a single register address and corresponding single byte of data synchronously. This function blocks until
     * the communications exchange is complete or encounters an error. Error codes are returned (0 means no error).
     *
     * \arg \c address the I2C address of the destination device for this message.
     * \arg \c registerAddress in device-centric terms, the register address on the destination device; think of it as
     * a one-byte instruction to the destination device telling it to do something (e.g., set the volume level).
     * \arg \c data a single byte of data serving as a parameter to the register address (e.g., the volume level to set).
     *
     * \returns an error code which if positive corresponds to I2cSendErrorCodes, or if negative the absolute value
     * corresponds to I2cStatusCodes (0 means no error).
     */
    int writeSync( uint8_t address, uint8_t registerAddress, uint8_t data );


    /*!
     * \brief Transmit a single register address and corresponding null-terminated string of data synchronously.
     * This function blocks until
     * the communications exchange is complete or encounters an error. Error codes are returned (0 means no error).
     *
     * \arg \c address the I2C address of the destination device for this message
     * \arg \c registerAddress in device-centric terms, the register address on the destination device; think of it as
     * a one-byte instruction to the destination device telling it to do something (e.g., an address in a memory device).
     * \arg \c data a null-terminated string of data serving as a parameter to the register address (e.g., a string to store
     * sequentially starting at the registerAddress).
     *
     * \returns an error code which if positive corresponds to I2cSendErrorCodes, or if negative the absolute value
     * corresponds to I2cStatusCodes (0 means no error).
     */
    int writeSync( uint8_t address, uint8_t registerAddress, const char* data );


    /*!
     * \brief Transmit a single register address and corresponding buffer of data synchronously.
     * This function blocks until
     * the communications exchange is complete or encounters an error. Error codes are returned (0 means no error).
     *
     *
     * \arg \c address the I2C address of the destination device for this message
     * \arg \c registerAddress in device-centric terms, the register address on the destination device; think of it as
     * a one-byte instruction to the destination device telling it to do something (e.g., an address in a memory device).
     * \arg \c data a buffer of data serving as a parameter to the register address (e.g., the data to store
     * sequentially starting at the registerAddress).
     * \arg \c numberBytes the number of bytes from the buffer to transmit.
     *
     * \returns an error code which if positive corresponds to I2cSendErrorCodes, or if negative the absolute value
     * corresponds to I2cStatusCodes (0 means no error).
     */
    int writeSync( uint8_t address, uint8_t registerAddress, uint8_t* data, uint8_t numberBytes );



    /*!
     * \brief Request to read data from a device and receive that data synchronously.
     * This function blocks until
     * the communications exchange is complete or encounters an error. Error codes are returned (0 means no error).
     *
     * \arg \c address the I2C address of the destination device you want to read from.
     * \arg \c numberBytes the number of bytes you expect to read.
     * \arg \c destination a pointer to a buffer in which the received data will be stored; the buffer should be
     * at least \c numberBytes large.
     *
     * \returns an error code which if positive corresponds to I2cSendErrorCodes, or if negative the absolute value
     * corresponds to I2cStatusCodes (0 means no error).
     */
    int readSync( uint8_t address, uint8_t numberBytes, uint8_t* destination );


    /*!
     * \brief Request to read data from a specific register on a device and receive that data synchronously.
     * This function blocks until
     * the communications exchange is complete or encounters an error. Error codes are returned (0 means no error).
     *
     * \arg \c address the I2C address of the destination device you want to read from.
     * \arg \c registerAddress in device-centric terms, the register address on the destination device; think of it as
     * a one-byte instruction to the destination device telling it what you want to read (e.g., temperature or the
     * starting address of a block of memory).
     * \arg \c numberBytes the number of bytes you expect to read.
     * \arg \c destination a pointer to a buffer in which the received data will be stored; the buffer should be
     * at least \c numberBytes large.
     *
     * \returns an error code which if positive corresponds to I2cSendErrorCodes, or if negative the absolute value
     * corresponds to I2cStatusCodes (0 means no error).
     */
    int readSync( uint8_t address, uint8_t registerAddress, uint8_t numberBytes, uint8_t* destination );


#if defined( DEBUG_I2cMasterBuffer ) || defined( DEBUG_I2cMasterDiary )
    void setDebugSout( Serial0* s );
#endif

#ifdef DEBUG_I2cMasterBuffer
    void dumpBufferContents();
#endif

#ifdef DEBUG_I2cMasterDiary
    void clearDebugI2cDiary();
    void dumpDebugI2cDiary();
#endif


};


#endif
