/*
    I2cSlave.cpp - An I2C slave library
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



#include "I2cSlave.h"

#include <stdint.h>
#include <string.h>

#include <util/atomic.h>
#include <util/twi.h>
#include <util/delay.h>


#include "ArduinoPins.h"






#ifdef DEBUG_I2cSlaveDiary

#ifndef DEBUG_I2cSlaveDiaryBufferSize
#define DEBUG_I2cSlaveDiaryBufferSize         32
#endif

#if DEBUG_I2cSlaveDiaryBufferSize > 255
#error "DEBUG_I2cSlaveDiaryBufferSize exceeds size of a uint8_t"
#endif


#include "USART0.h"


namespace
{

    enum
    {
        kStartXmitData              = 1,
        kStartXmitNoData            = 2,
        kContinueXmitData           = 3,
        kContinueXmitNoData         = 4,
        kGotNackOkay                = 8,
        kGotNackEarly               = 9,
        kStartGcallRcv              = 11,
        kStartRcv                   = 12,
        kDataRcv                    = 13,
        kDataRcvLastByte            = 14,
        kStopWhileInProgress        = 15,
        kStopOtherwise              = 16,
        kErrorStandby               = 90
    };


    uint8_t     debugContent[ DEBUG_I2cSlaveDiaryBufferSize ];
    uint8_t     debugControl[ DEBUG_I2cSlaveDiaryBufferSize ];
    uint8_t     debugStatus[ DEBUG_I2cSlaveDiaryBufferSize ];
    uint8_t     debugPtr;

    void DebugDiaryEntry( uint8_t content, uint8_t control, uint8_t status );

    Serial0*    debugSout;

};

#endif





namespace
{

    const uint8_t kI2cBufferSize = I2C_SLAVE_BUFFER_SIZE;

    uint8_t gI2cBuffer[ kI2cBufferSize ];

    uint8_t gI2cBufferIndex;
    uint8_t gI2cMsgSize;

    uint8_t gI2cStatus;
    bool gI2cBusy;

};






void I2cSlave::start( uint8_t ownAddress, uint8_t speed, bool answerGeneralCall )
{
    // Initialize our internal flag
    gI2cStatus = kI2cCompletedOk;
    gI2cBusy = false;
    gI2cBufferIndex = 0;
    gI2cMsgSize = 0;

    // Activate internal pull-ups
    pullups( kPullupsOn );

    // Set own TWI slave address depending on whether to accept TWI General Calls
    if ( answerGeneralCall )
    {
        TWAR = ( ownAddress << 1 ) | 0x01;
    }
    else
    {
        TWAR = ownAddress << 1;
    }

    // Default content = SDA released.
    TWDR = 0xFF;

    // Set bus speed
    if ( speed == kI2cBusSlow )
    {
        // Slow = 100 KHz
        TWBR = ( (F_CPU / 100000) - 16 ) / 2;
    }
    else
    {
        // Fast = 400 KHz
        TWBR = ( (F_CPU / 400000) - 16 ) / 2;
    }

    // Set TWI prescaler (clear it)
    TWSR &= ~( ( 1 << TWPS0 ) | ( 1 << TWPS1 ) );

    // Enable TWI module
    TWCR = (1<<TWEN)        // TWI Interface enabled
            | (1<<TWIE)     // Enable Interupt
            | (1<<TWINT)    // Clear the interrupt flag
            | (1<<TWEA)     // Prepare to ACK next time the Slave is addressed
            | (0<<TWSTA)
            | (0<<TWSTO)
            | (0<<TWWC);
}



void I2cSlave::stop()
{
    TWCR = 0;
}



void I2cSlave::pullups( uint8_t set )
{
    if ( set )
    {
        getGpioPORT( pSDA ) |= getGpioMASK( pSDA );
        getGpioPORT( pSCL ) |= getGpioMASK( pSCL );
    }
    else
    {
        getGpioPORT( pSDA ) &= ~getGpioMASK( pSDA );
        getGpioPORT( pSCL ) &= ~getGpioMASK( pSCL );
    }
}



bool I2cSlave::busy()
{
    return ( gI2cBusy );
}







#define transmitByte()          TWCR =    (1<<TWEN)     /* TWI Interface enabled */         \
                                        | (1<<TWIE)     /* Enable Interupt */               \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (1<<TWEA)     /* Send ACK */                      \
                                        | (0<<TWSTA)                                        \
                                        | (0<<TWSTO)                                        \
                                        | (0<<TWWC)


#define standby()               TWCR =    (1<<TWEN)     /* TWI Interface enabled */         \
                                        | (1<<TWIE)     /* Enable Interupt */               \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (1<<TWEA)     /* Send ACK */                      \
                                        | (0<<TWSTA)                                        \
                                        | (0<<TWSTO)                                        \
                                        | (0<<TWWC)


#define getNextByteWithACK()    TWCR =    (1<<TWEN)     /* TWI Interface enabled */         \
                                        | (1<<TWIE)     /* Enable Interupt */               \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (1<<TWEA)     /* Send ACK */                      \
                                        | (0<<TWSTA)                                        \
                                        | (0<<TWSTO)                                        \
                                        | (0<<TWWC)


#define getNextByteWithNACK()   TWCR =    (1<<TWEN)     /* TWI Interface enabled */         \
                                        | (1<<TWIE)     /* Enable Interupt */               \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (0<<TWEA)     /* Send NACK */                     \
                                        | (0<<TWSTA)                                        \
                                        | (0<<TWSTO)                                        \
                                        | (0<<TWWC)

#define pauseI2c()              TWCR =    (0<<TWEN)     /* TWI Interface disabled */        \
                                        | (0<<TWIE)     /* Disable Interupt */              \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (0<<TWEA)     /* Send NACK */                     \
                                        | (0<<TWSTA)                                        \
                                        | (0<<TWSTO)                                        \
                                        | (0<<TWWC)

#define enableI2c()             TWCR =    (1<<TWEN)     /* TWI Interface enabled */         \
                                        | (1<<TWIE)     /* Enable Interupt */               \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (1<<TWEA)     /* Send ACK */                      \
                                        | (0<<TWSTA)                                        \
                                        | (0<<TWSTO)                                        \
                                        | (0<<TWWC)




ISR( TWI_vect )
{
    switch ( TW_STATUS )
    {
        case TW_ST_SLA_ACK:             // Own SLA+R has been received; ACK has been returned
        case TW_ST_ARB_LOST_SLA_ACK:    // Arbitration lost in SLA+R/W as Master; own SLA+R received; ACK returned
            // Set buffer pointer to start
            gI2cBufferIndex = 0;
            gI2cStatus = I2cSlave::kI2cInProgress;
            gI2cBusy = true;
            if ( gI2cBufferIndex < gI2cMsgSize )
            {
                TWDR = gI2cBuffer[ gI2cBufferIndex++ ];
#ifdef DEBUG_I2cSlaveDiary
                DebugDiaryEntry( gI2cBuffer[ gI2cBufferIndex - 1 ], kStartXmitData, TW_STATUS );
#endif
            }
            else
            {
                // No more data, but master is still asking for data
                // Note that using the "send last byte (EA clear)" method doesn't work very well
                // Better to just send dummy data
                TWDR = 0xFF;
#ifdef DEBUG_I2cSlaveDiary
                DebugDiaryEntry( 0xFF, kStartXmitNoData, TW_STATUS );
#endif
            }
            transmitByte();
            break;

        case TW_ST_DATA_ACK:            // Data byte in TWDR has been transmitted; ACK has been received
            gI2cStatus = I2cSlave::kI2cInProgress;
            gI2cBusy = true;
            if ( gI2cBufferIndex < gI2cMsgSize )
            {
                TWDR = gI2cBuffer[ gI2cBufferIndex++ ];
#ifdef DEBUG_I2cSlaveDiary
                DebugDiaryEntry( gI2cBuffer[ gI2cBufferIndex - 1 ], kContinueXmitData, TW_STATUS );
#endif
            }
            else
            {
                // No more data, but master is still asking for data
                // Note that using the "send last byte (EA clear)" method doesn't work very well
                // Better to just send dummy data
                TWDR = 0xFF;
#ifdef DEBUG_I2cSlaveDiary
                DebugDiaryEntry( 0xFF, kContinueXmitNoData, TW_STATUS );
#endif
            }
            transmitByte();
            break;

        case TW_ST_DATA_NACK:          // Data byte in TWDR has been transmitted; NACK has been received.
            // This is end of the transmission.
            if ( gI2cBufferIndex == gI2cMsgSize )
            {
                // All expected data transceived
                gI2cStatus = I2cSlave::kI2cCompletedOk;
#ifdef DEBUG_I2cSlaveDiary
                DebugDiaryEntry( 0, kGotNackOkay, TW_STATUS );
#endif
            }
            else
            {
                // Master has sent a NACK before all data where sent
                gI2cStatus = I2cSlave::kI2cTxPartial;
#ifdef DEBUG_I2cSlaveDiary
                DebugDiaryEntry( 0, kGotNackEarly, TW_STATUS );
#endif
            }
            gI2cBusy = false;   // Transmit is finished, we are not busy anymore
            standby();
            break;

        case TW_SR_GCALL_ACK:           // General call address has been received; ACK has been returned
        case TW_SR_ARB_LOST_GCALL_ACK:  // Arbitration lost; General call address received; ACK returned
            gI2cBufferIndex = 0;
            gI2cBusy = true;
            gI2cStatus = I2cSlave::kI2cInProgress;
#ifdef DEBUG_I2cSlaveDiary
            DebugDiaryEntry( 0, kStartGcallRcv, TW_STATUS );
#endif
            standby();
            break;

        case TW_SR_SLA_ACK:             // Own SLA+W has been received ACK has been returned
        case TW_SR_ARB_LOST_SLA_ACK:    // Arbitration lost; own SLA+W received; ACK returned
            gI2cBufferIndex = 0;
            gI2cBusy = true;
            gI2cStatus = I2cSlave::kI2cInProgress;
#ifdef DEBUG_I2cSlaveDiary
            DebugDiaryEntry( 0, kStartRcv, TW_STATUS );
#endif
            standby();
            break;

        case TW_SR_DATA_ACK:            // Data has been received; ACK has been returned
        case TW_SR_GCALL_DATA_ACK:      // Data has been received; ACK has been returned
            gI2cBuffer[ gI2cBufferIndex++ ] = TWDR;
            if ( gI2cBufferIndex < kI2cBufferSize - 1 )
            {
#ifdef DEBUG_I2cSlaveDiary
                DebugDiaryEntry( gI2cBuffer[ gI2cBufferIndex - 1 ], kDataRcv, TW_STATUS );
#endif
                getNextByteWithACK();
            }
            else
            {
                // Next byte will be the last one that fits; respond with NACK
#ifdef DEBUG_I2cSlaveDiary
                DebugDiaryEntry( gI2cBuffer[ gI2cBufferIndex - 1 ], kDataRcvLastByte, TW_STATUS );
#endif
                getNextByteWithNACK();
            }
            break;

        case TW_SR_STOP:                // A STOP condition or repeated START condition
            gI2cMsgSize = I2cSlave::processI2cMessage( gI2cBuffer, gI2cBufferIndex );
            gI2cBusy = false;
            if ( gI2cStatus == I2cSlave::kI2cInProgress )
            {
                gI2cStatus = I2cSlave::kI2cCompletedOk;
#ifdef DEBUG_I2cSlaveDiary
                DebugDiaryEntry( 0, kStopWhileInProgress, TW_STATUS );
#endif
            }
#ifdef DEBUG_I2cSlaveDiary
            else
            {
                DebugDiaryEntry( 0, kStopOtherwise, TW_STATUS );
            }
#endif
            standby();
            break;


        case TW_SR_DATA_NACK:           // Addressed with own SLA+W; data received; NOT ACK returned
        case TW_SR_GCALL_DATA_NACK:     // Addressed with general call; data received; NOT ACK returned
        case TW_ST_LAST_DATA:           // Last byte has been transmitted (TWEA = 0); ACK has been received
            // This case is not used and (TWEA = 0 not sent on last byte) so this shouldn't happen
        case TW_NO_INFO:                // No relevant state information available; TWINT = 0
        case TW_BUS_ERROR:              // Bus error due to an illegal START or STOP condition
        default:
            gI2cBusy = false;
            gI2cStatus = TW_STATUS | I2cSlave::kI2cError;
#ifdef DEBUG_I2cSlaveDiary
            DebugDiaryEntry( 0, kErrorStandby, TW_STATUS );
#endif
            standby();
    }
}


#ifdef DEBUG_I2cSlaveDiary


namespace
{

    void DebugDiaryEntry( uint8_t content, uint8_t control, uint8_t status )
    {
        debugContent[ debugPtr ] = content;
        debugControl[ debugPtr ] = control;
        debugStatus[ debugPtr ] = status;
        ++debugPtr;
    }

};



void I2cSlave::clearDebugI2cDiary()
{
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        for ( uint8_t i = 0; i < DEBUG_I2cSlaveDiaryBufferSize; ++i )
        {
            debugContent[ i ] = 0;
            debugControl[ i ] = 0;
            debugStatus[ i ] = 0;
        }
        debugPtr = 0;
    }
}



void I2cSlave::dumpDebugI2cDiary()
{
    // Can't ATOMIC_BLOCK because need to use serial output, so temporarily pause I2C

    pauseI2c();

    debugSout->println( "I2C diary follows..." );
    debugSout->println( "#, status, sent, sent, next" );
    for ( uint8_t i = 0; i < debugPtr; ++i )
    {
        debugSout->print( i );
        debugSout->print( ", " );
        debugSout->print( debugStatus[ i ], kHex );
        debugSout->print( ", " );
        debugSout->print( debugContent[ i ] );
        debugSout->print( ", " );
        debugSout->print( static_cast<char>( debugContent[ i ] ) );
        debugSout->print( ", " );
        debugSout->println( debugControl[ i ] );
    }
    debugSout->println();

    enableI2c();
}



void I2cSlave::setDebugSout( Serial0* s )
{
    debugSout = s;
}

#endif

