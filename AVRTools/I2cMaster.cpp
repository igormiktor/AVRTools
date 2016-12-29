/*
    I2cMaster.cpp - An I2C master library
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



#include "I2cMaster.h"

#include <stdint.h>
#include <string.h>

#include <util/atomic.h>
#include <util/twi.h>
#include <util/delay.h>

#include "ArduinoPins.h"





#if defined( DEBUG_I2cMasterBuffer ) || defined( DEBUG_I2cMasterDiary )

#include "USART0.h"

namespace
{
    Serial0*    debugSout;
};

#endif


#ifdef DEBUG_I2cMasterDiary

#ifndef DEBUG_I2cMasterDiaryBufferSize
#define DEBUG_I2cMasterDiaryBufferSize         32
#endif

#if DEBUG_I2cMasterDiaryBufferSize > 255
#error "DEBUG_I2cMasterDiaryBufferSize exceeds size of a uint8_t"
#endif



namespace
{

    enum
    {
        kSendNextByte               = 1,
        kSendRestartSameMsg         = 2,
        kSendRestartNewMsg          = 3,
        kFinished                   = 4,
        kSentAddressSendNextByte    = 8,
        kSentAddressReadNextByte    = 9,
        kGetNextByteAckAfterSLAR    = 11,
        kGetNextByteNAckAfterSLAR   = 12,
        kGetNextByteAck             = 13,
        kGetNextByteNAck            = 14,
        kRcvDoneRestart             = 15,
        kRcvDoneStop                = 16,
        kTryStartAgain              = 17,
        kTryStartAgainError         = 18,
        kArbLostRestart             = 90,
        kErrorStop                  = 91,
        kErrorStartStop             = 92
    };


    uint8_t     debugContent[ DEBUG_I2cMasterDiaryBufferSize ];
    uint8_t     debugControl[ DEBUG_I2cMasterDiaryBufferSize ];
    uint8_t     debugStatus[ DEBUG_I2cMasterDiaryBufferSize ];
    uint8_t     debugPtr;

    void DebugDiaryEntry( uint8_t content, uint8_t control, uint8_t status );

};

#endif






namespace
{

    enum I2cTxMode
    {
        kI2cWrite               = 0x01,
        kI2cRead                = 0x02,
        kI2cWriteRestartRead    = ( kI2cWrite | kI2cRead ),

        kI2cWriteMask           = 0x01,
        kI2cReadMask            = 0x02
    };


    const uint8_t kMaxMsgLen    = I2C_MASTER_MAX_TX_MSG_LEN;
    const uint8_t kMaxNbrMsgs   = I2C_MASTER_MAX_TX_MSG_NBR;



    class BufferI2cTx
    {
    public:


        BufferI2cTx();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

        uint8_t getCurrentAddress()
        {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
            {
                return mAddress[ mCurrentMsg ];
            }
        }

        uint8_t getCurrentTxMode()
        {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
            {
                return mTxMode[ mCurrentMsg ];
            }
        }

        uint8_t setCurrentTxMode( I2cTxMode mode )
        {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
            {
                mTxMode[ mCurrentMsg ] = static_cast<uint8_t>( mode );
            }
        }

        uint8_t* getCurrentRxBufferPtr()
        {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
            {
                return mRxBuffer[ mCurrentMsg ];
            }
        }

        uint8_t getCurrentRxBufferSize()
        {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
            {
                return mRxBufferSize[ mCurrentMsg ];
            }
        }

        volatile uint8_t* getCurrentRxBufferCtr()
        {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
            {
                return mRxCounter[ mCurrentMsg ];
            }
        }

        volatile uint8_t* getCurrentStatus()
        {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
            {
                return mStatus[ mCurrentMsg ];
            }
        }

        int getCurrentByte()
        {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
            {
                return mCurrentByte < mTxBufferSize[ mCurrentMsg ]
                        ? mTxBuffer[ mCurrentMsg ][ mCurrentByte++ ] : -1;
            }
        }

        bool isFull()
        {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
            {
                return mNbrMsgs >= kMaxNbrMsgs;
            }
        }

        bool isNotFull()
        {
            ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
            {
                return mNbrMsgs < kMaxNbrMsgs;
            }
        }

#pragma GCC diagnostic pop

        bool noMessages()
        { return !static_cast<bool>( mNbrMsgs ); }

        bool messagePending()
        { return static_cast<bool>( mNbrMsgs ); }



        bool doneWithCurrentMessage();

        uint8_t push( uint8_t address, I2cTxMode txMode, const uint8_t* txdata, uint8_t txLen,
                        uint8_t* rxData, uint8_t rxLen, volatile uint8_t* rxCnt,
                        volatile uint8_t* rxStatus );

        void clear();



#ifdef DEBUG_I2cMasterBuffer

        void dumpBufferContents();

#endif


    private:

        uint8_t             mTxBuffer[ kMaxNbrMsgs ][ kMaxMsgLen ];
        uint8_t*            mRxBuffer[ kMaxNbrMsgs ];
        volatile uint8_t*   mRxCounter[ kMaxNbrMsgs ];
        volatile uint8_t*   mStatus[ kMaxNbrMsgs ];
        uint8_t             mTxBufferSize[ kMaxNbrMsgs ];
        uint8_t             mRxBufferSize[ kMaxNbrMsgs ];
        uint8_t             mTxMode[ kMaxNbrMsgs ];
        uint8_t             mAddress[ kMaxNbrMsgs ];

        volatile uint8_t    mCurrentByte;
        volatile uint8_t    mNbrMsgs;
        volatile uint8_t    mCurrentMsg;

    };

};









BufferI2cTx::BufferI2cTx()
: mCurrentByte( 0 ), mNbrMsgs( 0 ), mCurrentMsg( 0 )
{
}


bool BufferI2cTx::doneWithCurrentMessage()
{
    bool retVal = false;

    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        --mNbrMsgs;
        if ( mNbrMsgs )
        {
            // Point to the next message
            ++mCurrentMsg;
            if ( mCurrentMsg >= kMaxNbrMsgs )
            {
                mCurrentMsg -= kMaxNbrMsgs;
            }

            // We have another msg, so return true;
            retVal = true;
        }

        // Else just leave the current message where it is (but it is invalid)

        // No matter what, reset to point to the beginning of the next message's data
        mCurrentByte = 0;
    }

    return retVal;
}





uint8_t BufferI2cTx::push( uint8_t address, I2cTxMode txMode, const uint8_t* txData, uint8_t txLen,
                           uint8_t* rxBuf, uint8_t rxLen, volatile uint8_t* rxCnt,
                           volatile uint8_t* status )
{
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        if ( mNbrMsgs >= kMaxNbrMsgs )
        {
            // If buffer is full, ignore the push() but return code to indicate a problem
            return I2cMaster::kI2cErrTxBufferFull;
        }

        if ( txLen > kMaxMsgLen )
        {
            // If data is too long, ignore the push() but return code to indicate a problem
            return I2cMaster::kI2cErrMsgTooLong;
        }

        if ( !status )
        {
            // If no status, ignore the push() but return code to indicate a problem
            return I2cMaster::kI2cErrNullStatusPtr;
        }

        if ( ( txMode == kI2cWrite || txMode == kI2cWriteRestartRead ) && ( !txData || !txLen ) )
        {
            // If writing and no data provided, ignore the push() but return code to indicate a problem
            return I2cMaster::kI2cErrWriteWithoutData;
        }

        if ( ( txMode == kI2cRead || txMode == kI2cWriteRestartRead )
                        && ( !rxBuf || !rxLen || !rxCnt ) )
        {
            // If reading and no storage provided, ignore the push() but return code to indicate a problem
            return I2cMaster::kI2cErrReadWithoutStorage;
        }

        // If we get here, things are okay

        // Figure out the slot
        uint8_t slot = ( mCurrentMsg + mNbrMsgs ) % kMaxNbrMsgs;

        // Put in the data
        mAddress[ slot ] = address;
        mTxMode[ slot ] = static_cast<uint8_t>( txMode );

        mTxBufferSize[ slot ] = txLen;
        if ( txData && txLen )
        {
            memcpy( mTxBuffer[ slot ], txData, txLen );
        }
        else
        {
            // Just to make sure things are consistent
            mTxBufferSize[ slot ] = 0;
        }

        mRxBuffer[ slot ] = rxBuf;
        mRxBufferSize[ slot ] = rxLen;
        *rxCnt = 0;
        mRxCounter[ slot ] = rxCnt;
        mStatus[ slot ] = status;
        *status = I2cMaster::kI2cNotStarted;

        if ( !( rxLen && rxBuf && rxCnt ) )
        {
            // Just to make sure things are consistent
            mRxBufferSize[ slot ] = 0;
        }

        //  We now have one more message in the buffer
        ++mNbrMsgs;
    }

    return I2cMaster::kI2cNoError;
}





void BufferI2cTx::clear()
{
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        mNbrMsgs = 0;
    }
}




#ifdef DEBUG_I2cMasterBuffer

#if DEBUG_I2cMasterBuffer > 255
#error "DEBUG_I2cMasterBuffer exceeds size of a uint8_t"
#endif


/*
    uint8_t             mTxBuffer[ kMaxNbrMsgs ][ kMaxMsgLen ];
    uint8_t*            mRxBuffer[ kMaxNbrMsgs ];
    volatile uint8_t*   mRxCounter[ kMaxNbrMsgs ];
    volatile uint8_t*   mStatus[ kMaxNbrMsgs ];
    uint8_t             mTxBufferSize[ kMaxNbrMsgs ];
    uint8_t             mRxBufferSize[ kMaxNbrMsgs ];
    uint8_t             mTxMode[ kMaxNbrMsgs ];
    uint8_t             mAddress[ kMaxNbrMsgs ];
    volatile uint8_t    mCurrentByte;
    volatile uint8_t    mNbrMsgs;
    volatile uint8_t    mCurrentMsg;
*/

void BufferI2cTx::dumpBufferContents()
{
    debugSout->print( "Nbr msgs:  " ); debugSout->println( mNbrMsgs );
    if ( mNbrMsgs )
    {
        for ( uint8_t i = 0; i < mNbrMsgs ; ++i )
        {
            debugSout->print( "Msg " ); debugSout->println( i );
            debugSout->print( "Address: " ); debugSout->println( mAddress[ (mCurrentMsg + i) % kMaxNbrMsgs ] );
            debugSout->print( "Tx mode: " ); debugSout->println( mTxMode[ (mCurrentMsg + i) % kMaxNbrMsgs ] );
            uint8_t n = mTxBufferSize[ (mCurrentMsg + i) % kMaxNbrMsgs ];
            debugSout->print( "Tx Size:    " ); debugSout->println( n );
            debugSout->print( "Tx Msg:     " );
            char* msg = reinterpret_cast<char*>( mTxBuffer[ (mCurrentMsg + i) % kMaxNbrMsgs ] );
            if ( n && msg )
            {
                for ( uint8_t j = 0; j < n; ++j )
                {
                    debugSout->print( msg[ j ] );
                }
            }
            debugSout->println( "" );
            uint8_t m = mRxBufferSize[ (mCurrentMsg + i) % kMaxNbrMsgs ];
            debugSout->print( "Rx Size:    " ); debugSout->println( m );
            debugSout->print( "Rx Ptr:     " ); debugSout->println( *mRxCounter[ (mCurrentMsg + i) % kMaxNbrMsgs ] );
        }
    }
}


#endif








namespace
{

    BufferI2cTx     gI2cBuffer;

    bool            gI2cBusy;

#if I2C_MASTER_SLA_NACK_SPECIAL_HANDLING
    uint8_t         gRetries;
#endif


    void waitForCompletion( volatile uint8_t& status )
    {

        // Wait for completion
        while ( status == I2cMaster::kI2cNotStarted || status == I2cMaster::kI2cInProgress )
        {
            // Just spin
        }
    }



    void startI2c()
    {
        if ( !gI2cBusy && !(TWCR & (1<<TWIE)) )
        {
            while ( TWCR & (1<<TWSTO) )
            {
                // spin
            }

            gI2cBusy = 1;

#if I2C_MASTER_SLA_NACK_SPECIAL_HANDLING
            gRetries = 0;
#endif

            TWCR = (1<<TWEN)        // TWI Interface enabled
                    | (1<<TWIE)     // Enable Interupt
                    | (1<<TWINT)    // Clear the interrupt flag
                    | (0<<TWEA)     // ACK doesn't matter
                    | (1<<TWSTA);   // Initiate a start condition
        }
    }

}


void I2cMaster::start( uint8_t speed )
{
    // Initialize our internal flags
    gI2cBusy = false;

#if I2C_MASTER_SLA_NACK_SPECIAL_HANDLING
    gRetries = 0;
#endif

    // Activate internal pull-ups
    pullups( kPullupsOn );

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
            | (0<<TWIE)     // Enable Interupt
            | (1<<TWINT)    // Clear the interrupt flag
            | (0<<TWEA)     // ACK doesn't matter
            | (0<<TWSTA);   // Initiate a start condition
}



void I2cMaster::stop()
{
    TWCR = 0;
}



void I2cMaster::pullups( uint8_t set )
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



bool I2cMaster::busy()
{
    return ( gI2cBusy || ( TWCR & (1<<TWIE) ) );
}





// Asynchronous

uint8_t I2cMaster::writeAsync( uint8_t address, uint8_t registerAddress, volatile uint8_t* status )
{
    while ( gI2cBuffer.isFull() )
    {
        // Delay 2 I2C cycles at 400 KHz
        _delay_us( 5 );
    }

    uint8_t err = gI2cBuffer.push( address, kI2cWrite, &registerAddress, 1, 0, 0, 0, status );

#ifdef DEBUG_I2cMasterBuffer
    debugSout->print( "writeAsync1 pushed into buffer err= " );
    debugSout->println( err );
    gI2cBuffer.dumpBufferContents();
#endif

    if ( !err )
    {
        startI2c();
    }

    return err;
}


uint8_t I2cMaster::writeAsync( uint8_t address, uint8_t registerAddress, uint8_t data, volatile uint8_t* status )
{
    uint8_t temp[2];
    temp[0] = registerAddress;
    temp[1] = data;

    while ( gI2cBuffer.isFull() )
    {
        // Delay 2 I2C cycles at 400 KHz
        _delay_us( 5 );
    }

    uint8_t err = gI2cBuffer.push( address, kI2cWrite, temp, 2, 0, 0, 0, status );

#ifdef DEBUG_I2cMasterBuffer
    debugSout->print( "writeAsync2 pushed into buffer err= " );
    debugSout->println( err );
    gI2cBuffer.dumpBufferContents();
#endif

    if ( !err )
    {
        startI2c();
    }

    return err;
}


uint8_t I2cMaster::writeAsync( uint8_t address, uint8_t registerAddress, const char* data, volatile uint8_t* status )
{
    size_t nbrBytes = strlen( data );

    uint8_t temp[ nbrBytes + 1 ];
    temp[0] = registerAddress;
    memcpy( temp + 1, data, nbrBytes );

    // TODO Optimize with strcpy -> nbrBytes

    while ( gI2cBuffer.isFull() )
    {
        // Delay 2 I2C cycles at 400 KHz
        _delay_us( 5 );
    }

    uint8_t err = gI2cBuffer.push( address, kI2cWrite, temp, nbrBytes + 1, 0, 0, 0, status );

#ifdef DEBUG_I2cMasterBuffer
    debugSout->print( "writeAsync3 pushed into buffer err= " );
    debugSout->println( err );
    gI2cBuffer.dumpBufferContents();
#endif

    if ( !err )
    {
        startI2c();
    }

    return err;
}


uint8_t I2cMaster::writeAsync( uint8_t address, uint8_t registerAddress, uint8_t* data, uint8_t numberBytes,
                        volatile uint8_t* status )
{
    uint8_t temp[ numberBytes + 1 ];
    temp[0] = registerAddress;
    memcpy( temp + 1, data, numberBytes );

    while ( gI2cBuffer.isFull() )
    {
        // Delay 2 I2C cycles at 400 KHz
        _delay_us( 5 );
    }

    uint8_t err = gI2cBuffer.push( address, kI2cWrite, temp, numberBytes + 1, 0, 0, 0, status );

#ifdef DEBUG_I2cMasterBuffer
    debugSout->print( "writeAsync4 pushed into buffer err= " );
    debugSout->println( err );
    gI2cBuffer.dumpBufferContents();
#endif

    if ( !err )
    {
        startI2c();
    }

    return err;
}



uint8_t I2cMaster::readAsync( uint8_t address, uint8_t numberBytes, volatile uint8_t* destination,
                    volatile uint8_t* bytesRead, volatile uint8_t* status )
{
    while ( gI2cBuffer.isFull() )
    {
        // Delay 2 I2C cycles at 400 KHz
        _delay_us( 5 );
    }

    uint8_t err = gI2cBuffer.push( address, kI2cRead, 0, 0, const_cast<uint8_t*>( destination ), numberBytes, bytesRead,
                        status );

#ifdef DEBUG_I2cMasterBuffer
    debugSout->print( "readAsync1 pushed into buffer err= " );
    debugSout->println( err );
    gI2cBuffer.dumpBufferContents();
#endif

    if ( !err )
    {
        startI2c();
    }

    return err;
}


uint8_t I2cMaster::readAsync( uint8_t address, uint8_t registerAddress, uint8_t numberBytes,
                    volatile uint8_t* destination, volatile uint8_t* bytesRead,
                    volatile uint8_t* status )
{
    while ( gI2cBuffer.isFull() )
    {
        // Delay 2 I2C cycles at 400 KHz
        _delay_us( 5 );
    }

    uint8_t err = gI2cBuffer.push( address, kI2cWriteRestartRead, &registerAddress, 1, const_cast<uint8_t*>( destination ),
                        numberBytes, bytesRead, status );

#ifdef DEBUG_I2cMasterBuffer
    debugSout->print( "readAsync2 pushed into buffer err= " );
    debugSout->println( err );
    gI2cBuffer.dumpBufferContents();
#endif

    if ( !err )
    {
        startI2c();
    }

    return err;
}




// Synchronous

int I2cMaster::writeSync( uint8_t address, uint8_t registerAddress )
{
    volatile uint8_t status;
    uint8_t err = writeAsync( address, registerAddress, &status );
    if ( !err )
    {
        waitForCompletion( status );
        return -static_cast<int>( status );
    }
    return err;
}


int I2cMaster::writeSync( uint8_t address, uint8_t registerAddress, uint8_t data )
{
    volatile uint8_t status;
    uint8_t err = writeAsync( address, registerAddress, data, &status );
    if ( !err )
    {
        waitForCompletion( status );
        return -static_cast<int>( status );
    }
    return err;
}


int I2cMaster::writeSync( uint8_t address, uint8_t registerAddress, const char* data )
{
    volatile uint8_t status;
    uint8_t err = writeAsync( address, registerAddress, data, &status );
    if ( !err )
    {
        waitForCompletion( status );
        return -static_cast<int>( status );
    }
    return err;
}


int I2cMaster::writeSync( uint8_t address, uint8_t registerAddress, uint8_t* data, uint8_t numberBytes )
{
    volatile uint8_t status;
    uint8_t err = writeAsync( address, registerAddress, data, numberBytes, &status );
    if ( !err )
    {
        waitForCompletion( status );
        return -static_cast<int>( status );
    }
    return err;
}



int I2cMaster::readSync( uint8_t address, uint8_t numberBytes, uint8_t* destination )
{
    volatile uint8_t status;
    volatile uint8_t bytesRead;
    uint8_t err = readAsync( address, numberBytes, destination, &bytesRead, &status );
    if ( !err )
    {
        waitForCompletion( status );
        return -static_cast<int>( status );
    }
    return err;
}


int I2cMaster::readSync( uint8_t address, uint8_t registerAddress, uint8_t numberBytes, uint8_t* destination )
{
    volatile uint8_t status;
    volatile uint8_t bytesRead;
    uint8_t err = readAsync( address, registerAddress, numberBytes, destination, &bytesRead, &status );
    if ( !err )
    {
        waitForCompletion( status );
        return -static_cast<int>( status );
    }
    return err;
}






#define SLA_W( address )        ( address << 1 )        // = ( address << 1 | TW_WRITE )
#define SLA_R( address )        ( ( address << 1 ) | TW_READ )


#define sendNextByte()          TWCR =    (1<<TWEN)     /* TWI Interface enabled */         \
                                        | (1<<TWIE)     /* Enable Interupt */               \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (1<<TWEA)     /* ACK doesn't matter */            \
                                        | (0<<TWSTA)                                        \
                                        | (0<<TWSTO)                                        \
                                        | (0<<TWWC)


#define sendStop()              TWCR =    (1<<TWEN)     /* TWI Interface enabled */         \
                                        | (0<<TWIE)     /* Disable Interupt */              \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (0<<TWEA)     /* ACK doesn't matter */            \
                                        | (0<<TWSTA)                                        \
                                        | (1<<TWSTO)    /* Initiate a stop condition */     \
                                        | (0<<TWWC)


#define sendStart()             TWCR =    (1<<TWEN)     /* TWI Interface enabled */         \
                                        | (1<<TWIE)     /* Enable Interupt */               \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (0<<TWEA)     /* ACK doesn't matter */            \
                                        | (1<<TWSTA)    /* Initiate a start condition */    \
                                        | (0<<TWSTO)                                        \
                                        | (0<<TWWC)


#define sendRestart()           TWCR =    (1<<TWEN)     /* TWI Interface enabled */         \
                                        | (1<<TWIE)     /* Enable Interupt */               \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (0<<TWEA)     /* ACK doesn't matter */            \
                                        | (1<<TWSTA)    /* Initiate a start condition */    \
                                        | (0<<TWSTO)                                        \
                                        | (0<<TWWC)


#define sendStopAndStart()      TWCR =    (1<<TWEN)     /* TWI Interface enabled */         \
                                        | (1<<TWIE)     /* Enable Interupt */               \
                                        | (1<<TWINT)    /* Clear the interrupt flag */      \
                                        | (0<<TWEA)     /* ACK doesn't matter */            \
                                        | (1<<TWSTA)    /* Initiate a start (second) */     \
                                        | (1<<TWSTO)    /* Initiate a stop (first) */       \
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





ISR( TWI_vect )
{
    volatile uint8_t* n;
    int b;

    switch ( TW_STATUS )
    {
        case TW_START:              // START has been transmitted
        case TW_REP_START:          // Repeated START has been transmitted
            // Send the address of the node we want to communicate with
            if ( gI2cBuffer.getCurrentTxMode() & kI2cWriteMask )
            {
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( SLA_W(gI2cBuffer.getCurrentAddress()), kSentAddressSendNextByte, TW_STATUS );
#endif
                TWDR = SLA_W( gI2cBuffer.getCurrentAddress() );
            }
            else
            {
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( SLA_R(gI2cBuffer.getCurrentAddress()), kSentAddressReadNextByte, TW_STATUS );
#endif
                TWDR = SLA_R( gI2cBuffer.getCurrentAddress() );
            }
            *(gI2cBuffer.getCurrentStatus()) = I2cMaster::kI2cInProgress;
#if I2C_MASTER_SLA_NACK_SPECIAL_HANDLING
            gRetries = 0;
#endif
            gI2cBusy = true;
            sendNextByte();
            break;

        case TW_MT_SLA_ACK:         // SLA+W has been tramsmitted and ACK received
        case TW_MT_DATA_ACK:        // Data byte has been tramsmitted and ACK received
            // Ready to send another byte...
            b = gI2cBuffer.getCurrentByte();
            if ( b != -1 )
            {
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( b, kSendNextByte, TW_STATUS );
#endif
                // Send the next byte
                TWDR = static_cast<uint8_t>( b );
                sendNextByte();
            }
            else                    // Send STOP or RESTART after last byte
            {
                if ( gI2cBuffer.getCurrentTxMode() == kI2cWriteRestartRead )
                {
#ifdef DEBUG_I2cMasterDiary
                    DebugDiaryEntry( 0, kSendRestartSameMsg, TW_STATUS );
#endif
                    // Need to set a restart and update txMode accordingly
                    gI2cBuffer.setCurrentTxMode( kI2cRead );
                    sendRestart();
                }
                else
                {
                    // Done with this message
                   *(gI2cBuffer.getCurrentStatus()) = I2cMaster::kI2cCompletedOk;
                    // Is there another message?
                    if ( gI2cBuffer.doneWithCurrentMessage() )
                    {
#ifdef DEBUG_I2cMasterDiary
                        DebugDiaryEntry( 0, kSendRestartNewMsg, TW_STATUS );
#endif
                        // Keep control of the bus and restart a new msg
                        sendRestart();
                    }
                    else
                    {
#ifdef DEBUG_I2cMasterDiary
                        DebugDiaryEntry( 0, kFinished, TW_STATUS );
#endif
                        // Done for now
                        gI2cBusy = false;
                        sendStop();
                    }
                }
            }
            break;

        case TW_MR_SLA_ACK:         // SLA+R has been tramsmitted and ACK received
            if ( gI2cBuffer.getCurrentRxBufferSize() > 1 )
            {
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( 0, kGetNextByteAckAfterSLAR, TW_STATUS );
#endif
                // Next byte is not the last one; send an ACK when we get it
                getNextByteWithACK();
            }
            else
            {
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( 0, kGetNextByteNAckAfterSLAR, TW_STATUS );
#endif
                // Only 1 byte to get; next byte is last one so have to send NACK when we get it
                getNextByteWithNACK();
            }
            break;

        case TW_MR_DATA_ACK:        // Data byte has been received and ACK tramsmitted
            // Store the byte, if there is room
            n = gI2cBuffer.getCurrentRxBufferCtr();
            *( gI2cBuffer.getCurrentRxBufferPtr() + *n ) = TWDR;
            ++(*n);
            if ( *n < gI2cBuffer.getCurrentRxBufferSize() - 1 )
            {
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( *(gI2cBuffer.getCurrentRxBufferPtr() + *n - 1), kGetNextByteAck, TW_STATUS );
#endif
                // Next byte is not the last one, so send an ACK when we get it
                getNextByteWithACK();
            }
            else
            {
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( *(gI2cBuffer.getCurrentRxBufferPtr() + *n - 1), kGetNextByteNAck, TW_STATUS );
#endif
                // Next byte is the last one, so send a NACK when we get it
                getNextByteWithNACK();
            }
            break;

        case TW_MR_DATA_NACK:       // Data byte has been received and NACK tramsmitted
            n = gI2cBuffer.getCurrentRxBufferCtr();
            *( gI2cBuffer.getCurrentRxBufferPtr() + *n ) = TWDR;
            ++(*n);
            // Done with this message
            *(gI2cBuffer.getCurrentStatus()) = I2cMaster::kI2cCompletedOk;
            if ( gI2cBuffer.doneWithCurrentMessage() )
            {
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( *(gI2cBuffer.getCurrentRxBufferPtr() + *n - 1), kRcvDoneRestart, TW_STATUS );
#endif
                // Keep control of the bus and restart a new msg
                sendRestart();
            }
            else
            {
                // Done for now
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( *(gI2cBuffer.getCurrentRxBufferPtr() + *n - 1), kRcvDoneStop, TW_STATUS );
#endif
                gI2cBusy = false;
                sendStop();
            }
            break;

        case TW_MT_ARB_LOST:        // Arbitration lost (same as TW_MR_ARB_LOST)
            // Try it again
#ifdef DEBUG_I2cMasterDiary
            DebugDiaryEntry( 0, kArbLostRestart, TW_STATUS );
#endif
            sendRestart();
            break;

        case TW_MT_SLA_NACK:        // SLA+W has been tramsmitted and NACK received
        case TW_MR_SLA_NACK:        // SLA+R has been tramsmitted and NACK received
            // Slave may still be resetting.  Try again (but stay on current msg).
#if I2C_MASTER_SLA_NACK_SPECIAL_HANDLING
            if ( gRetries++ < 3 )
            {
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( 0, kTryStartAgain, TW_STATUS );
#endif
                _delay_us( 5 );         // Two cycles at 400KHz
                sendStart();
            }
            else
            {
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( 0, kTryStartAgainError, TW_STATUS );
#endif
                gRetries = 0;
                *(gI2cBuffer.getCurrentStatus()) = ( TW_STATUS | I2cMaster::kI2cError );
                gI2cBusy = false;
                sendStop();
            }
            break;
#endif
        // Intentional fall-through if the special handling code above is not turned on.

        case TW_MT_DATA_NACK:       // Data byte has been tramsmitted and NACK received
        case TW_NO_INFO:            // No relevant state information available
        case TW_BUS_ERROR:          // Bus error due to an illegal START or STOP condition
        default:
            // Report the error
            *(gI2cBuffer.getCurrentStatus()) = ( TW_STATUS | I2cMaster::kI2cError );
            if ( gI2cBuffer.doneWithCurrentMessage() )
            {
                // Keep control of the bus and restart a new msg
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( 0, kErrorStartStop, TW_STATUS );
#endif
                sendStopAndStart();
            }
            else
            {
                // Done for now
#ifdef DEBUG_I2cMasterDiary
                DebugDiaryEntry( 0, kErrorStop, TW_STATUS );
#endif
                gI2cBusy = false;
                sendStop();
            }
            break;
    }
}





//********************************************************


#ifdef DEBUG_I2cMasterDiary


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



void I2cMaster::clearDebugI2cDiary()
{
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
        for ( uint8_t i = 0; i < DEBUG_I2cMasterDiaryBufferSize; ++i )
        {
            debugContent[ i ] = 0;
            debugControl[ i ] = 0;
            debugStatus[ i ] = 0;
        }
        debugPtr = 0;
    }
}



void I2cMaster::dumpDebugI2cDiary()
{
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
}


#endif



#if defined( DEBUG_I2cMasterBuffer ) || defined( DEBUG_I2cMasterDiary )

void I2cMaster::setDebugSout( Serial0* s )
{
    debugSout = s;
}

#endif



#ifdef DEBUG_I2cMasterBuffer

void I2cMaster::dumpBufferContents()
{
    gI2cBuffer.dumpBufferContents();
}

#endif

