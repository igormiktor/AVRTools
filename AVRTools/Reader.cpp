/*
    Reader.cpp - a base class for reading data
    For AVR ATMega328p (Arduino Uno) and ATMega2560 (Arduino Mega).
    This is part of the AVRTools library.
    Copyright (c) 2014 Igor Mikolic-Torreira.  All right reserved.
    Functions readLong() and readFloat() adapted from Arduino code that
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



#include "Reader.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


#ifndef USE_READER_WITHOUT_SYSTEM_CLOCK
#include "SystemClock.h"
#endif

#ifndef DEFAULT_READER_TIMEOUT
#define DEFAULT_READER_TIMEOUT      1000        // milliseconds
#endif

#define NO_SKIP_CHAR  1     // Magic char not found in a valid ASCII numeric field




int read()
{
    // Default does nothing
    return 0;
}


int peek()
{
    // Default does nothing
    return 0;
}

bool available()
{
    // Default does nothing
    return false;
}





Reader::Reader()
: mTimeOut( DEFAULT_READER_TIMEOUT )
{
    // Nothing else to do
}


int Reader::timedRead()
{
#ifndef USE_READER_WITHOUT_SYSTEM_CLOCK

    unsigned long stop = millis() + mTimeOut;
    do
    {
        int c = read();
        if ( c >= 0 )
        {
            return c;
        }
    }
    while ( millis() < stop );

    return -1;

#else

    return read();

#endif
}



int Reader::timedPeek()
{
#ifndef USE_READER_WITHOUT_SYSTEM_CLOCK

    unsigned long stop = millis() + mTimeOut;
    do
    {
        int c = peek();
        if ( c >= 0 )
        {
            return c;
        }
    }
    while ( millis() < stop );

    return -1;

#else

    return peek();

#endif
}



int Reader::peekNextDigit()
{
    while ( 1 )
    {
        int c = timedPeek();
        if ( c < 0 )
        {
            // timeout
            return c;
        }
        if ( c == '-' )
        {
            return c;
        }
        if ( c >= '0' && c <= '9' )
        {
            return c;
        }

        // discard non-numeric
        read();
    }
}




bool Reader::findUntil( const char *tgt, const char *term )
{
    return findUntil( tgt, (tgt ? strlen( tgt ) : 0 ), term, ( term ? strlen( term ) : 0 ) );
}



bool Reader::findUntil( const char *target, size_t targetLen, const char *terminator, size_t termLen )
{
    size_t index = 0;           // maximum target string length is 64k bytes!
    size_t termIndex = 0;

    if( *target == 0 )
    {
        return true;
    }

    int c;
    while( ( c = timedRead() ) > 0 )
    {

        if ( c != target[ index ] )
        {
            // Reset index if any char does not match
            index = 0;
        }

        if ( c == target[ index ] )
        {
            if ( ++index >= targetLen )
            {
                // return true if all chars in the target match
                return true;
            }
        }

        if ( termLen > 0 && c == terminator[ termIndex ] )
        {
            if ( ++termIndex >= termLen )
            {
                // return false if terminate string found before target string
                return false;
            }
        }
        else
        {
            termIndex = 0;
        }
    }

    return false;
}




bool Reader::readLong( long* result )
{
    return readLong( result, NO_SKIP_CHAR );
}



bool Reader::readLong( long* result, char skipChar )
{
    bool isNegative = false;
    bool noDigitsYet = true;
    long value = 0;

    // Ignore non-numeric leading characters
    int c = peekNextDigit();

    if ( c < 0 )
    {
        // Return false if time out without getting at least one digit
        return noDigitsYet ? false : true;
    }

    do
    {
        if ( c == skipChar )
        {
            // Ignore this charactor
        }
        else if ( c == '-' )
        {
            isNegative = true;
        }
        else if( c >= '0' && c <= '9' )
        {
            value = value * 10 + c - '0';
            noDigitsYet = false;
        }

        // Consume the character we got with peek
        read();

        c = timedPeek();
        if ( c < 0 )
        {
            // Return false if time out without getting at least one digit
            return noDigitsYet ? false : true;
        }
    }
    while ( ( c >= '0' && c <= '9' ) || c == skipChar );

    if ( c == SERIAL_INPUT_EOL )
    {
        // Consume the character we got with peek
        read();
    }

    if ( isNegative )
    {
        value = -value;
    }
    *result = value;

    return true;
}




bool Reader::readFloat( float* result )
{
    return readFloat( result, NO_SKIP_CHAR );
}


bool Reader::readFloat( float* result, char skipChar )
{
    bool isNegative = false;
    bool isFraction = false;
    long value = 0;
    float fraction = 1.0;
    bool noDigitsYet = true;

    // Ignore non-numeric leading characters
    int c = peekNextDigit();

    if ( c < 0 )
    {
        // Return false if time out without getting at least one digit
        return noDigitsYet ? false : true;
    }

    do
    {
        if( c == skipChar )
        {
            // Ignore
        }
        else if ( c == '-' )
        {
            isNegative = true;
        }
        else if ( c == '.' )
        {
            isFraction = true;
        }
        else if ( c >= '0' && c <= '9' )
        {
            // c is a digit
            value = value * 10 + c - '0';
            noDigitsYet = false;

            if ( isFraction )
            {
                fraction *= 0.1;
            }
        }

        // Consume the character we got with peek
        read();

        c = timedPeek();
        if ( c < 0 )
        {
            // Return false if time out without getting at least one digit
            return noDigitsYet ? false : true;
        }
    }
    while ( ( c >= '0' && c <= '9' )  || c == '.' || c == skipChar );

    if ( c == SERIAL_INPUT_EOL )
    {
        // Consume the character we got with peek
        read();
    }

    if ( isNegative )
    {
        value = -value;
    }

    if ( isFraction )
    {
        *result = value * fraction;
    }
    else
    {
        *result = value;
    }

    return true;
}



// Read characters from stream into buffer
// Terminates if length characters have been read, or timeout (see setTimeout)
// Returns the number of characters placed in the buffer
// The buffer is NOT null terminated.

size_t Reader::readBytes( char *buffer, size_t length )
{
    if ( !buffer || !length )
    {
        return 0;
    }
    size_t count = 0;
    while ( count < length )
    {
        int c = timedRead();
        if ( c < 0 )
        {
            break;
        }
        *buffer++ = static_cast<char>( c );
        ++count;
    }
    return count;
}



// readBytes() with terminator character
// Terminates if length characters have been read, timeout, or if the terminator character  detected
// Returns the number of characters placed in the buffer (0 means no valid data found)
// The buffer is NOT null terminated.

size_t Reader::readBytesUntil( char terminator, char *buffer, size_t length )
{
    if ( !buffer || !length )
    {
        return 0;
    }
    size_t index = 0;
    while ( index < length )
    {
        int c = timedRead();
        if ( c < 0 || c == terminator )
        {
            break;
        }
        *buffer++ = static_cast<char>( c );
        ++index;
    }
    return index;
}



// readBytesUntil() with EOLterminator character, but returns null-teminated string.
// Returns the number of characters placed in the buffer (0 means no valid data found)
// Result is null-terminated

size_t Reader::readLine( char *buffer, size_t length )
{
    if ( !buffer || !length )
    {
        return 0;
    }
    size_t index = 0;
    while ( index < length - 1 )
    {
        int c = timedRead();
        if ( c < 0 || c == SERIAL_INPUT_EOL )
        {
            break;
        }
        *buffer++ = static_cast<char>( c );
        ++index;
    }

    *buffer = 0;
    return index; // return number of characters, not including null terminator
}




void Reader::consumeWhiteSpace()
{
    int c = timedPeek();
    while ( isspace ( c ) )
    {
        // Consume peeked character
        read();

        // Peek again
        c = timedPeek();
    }
}
