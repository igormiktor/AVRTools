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

/*!
 * \file
 *
 * \brief This file provides a generic interface to incoming data streams of any kind.  It is designed
 * around how serial streams are generally used, but can be used with any system that provides
 * a sequential input of bytes that can be interpreted as strings and/or numbers.
 */


#ifndef Reader_h
#define Reader_h


#include <stddef.h>
#include <stdint.h>


#ifndef SERIAL_INPUT_EOL
#define SERIAL_INPUT_EOL    '\n'
#endif


/*!
 * \brief This is an abstract class defining a generic interface to read numbers and strings from a sequential
 * stream of bytes (such as a serial device).
 *
 * It implements functions to convert a sequence of bytes into various integers
 * and floating point numbers (so it is not a pure interface class).  These functions depend on a small
 * set of lower-level functions that are purely abstract and must be implemented by classes deriving from
 * Reader.
 *
 * Serial0 is an example of a class that derives from Reader by implementating the purely
 * abstract functions in Reader.
 *
 * \note Use of the timeout feature requires linking against SystemClock.cpp and calling initSystemClock()
 * from your start-up code.  If you do not wish to use the system clock and link against SystemClock.cpp,
 * then define the macro USE_READER_WITHOUT_SYSTEM_CLOCK.  This means that calls will never timeout, and
 * you are likely to lock your system if you read input that doesn't naturally terminate parsing (e.g.,
 * if you read numbers and the last number isn't followed by a newline).
 */


class Reader
{

public:


    /*!
     * \brief Constructor.  It sets the default timeout to 1 second.
     *
     */
    Reader();



    // Virtual methods (pure -- need to be implmented by derived classes)

    /*!
     * \brief Pure virtual function that reads and removes the next byte from the input stream.
     *
     * \returns the next byte, or -1 if there is nothing to read in the input stream
     * before timeout expires.
     */
    virtual int read() = 0;


    /*!
     * \brief Pure virtual function that examines the next byte from the input stream, without removing it.
     *
     * \returns the next byte, or -1 if there is nothing to read in the input stream
     * before timeout expires.
     */
    virtual int peek() = 0;


    /*!
     * \brief Pure virtual function that determines if data is available in the input stream.
     *
     * \returns True if data is available in the stream before timeout expires; false if timeout
     * expires before any data appears in the stream.
     */
    virtual bool available() = 0;




    // Parsing methods

    /*!
     * \brief Sets maximum milliseconds to wait for stream data, default is 1 second.
     *
     * \arg \c milliseconds the length of the timeout period in milliseconds.
     */
    void setTimeout( unsigned long milliseconds )
    { mTimeOut = milliseconds; }


    /*!
     * \brief Read data from the input stream until the target string is found.
     *
     * \arg \c target is the string the function seeks in the input stream.
     *
     * \returns true if target string is found before timeout, false otherwise.
     */
    bool find( const char *target )
    { return findUntil( target, 0 ); }


    /*!
     * \brief Read data from the stream until the target string of given length is found.
     *
     * \arg \c target is a string, the first length bytes of which the function seeks in the input stream.
     * \arg \c length is the number of bytes of the string to use for comparison.
     *
     * \returns true if target string of given length is found, false if the function times out before
     * finding the target string.
     */
    bool find( const char *target, size_t length )
    { return findUntil( target, length, NULL, 0 ); }


    /*!
     * \brief Read data from the stream until the target string is found, or the
     * terminator string is found, or the function times out.
     *
     * This function is like find() but the search ends if the terminator string is found first.
     *
     * \arg \c target is the string the function seeks in the input stream.
     * \arg \c terminator is the string that stops the search.
     *
      * \returns true if target string is found before the terminator is encountered and before
      * the function times out; false otherwise.
     */
    bool findUntil( const char *target, const char *terminator );


    /*!
     * \brief Read data from the stream until the target string of given length is found, or the
     * terminator string of given length is found, or the function times out.
     *
     * This function is like find() but the search ends if the terminator string is found first.
     *
     * \arg \c target is the string the function seeks in the input stream.
     * \arg \c targetLen is the number of bytes in target that the function seeks in the input stream.
     * \arg \c terminator is the string that stops the search.
     * \arg \c termLen is the number of bytes in the terminator that
     *
      * \returns true if target string is found before the terminator is encountered and before
      * the function times out; false otherwise.
     */
    bool findUntil( const char *target, size_t targetLen, const char *terminate, size_t termLen );


    /*!
     * \brief Return the first valid long integer value from the stream.
     *
     * Initial characters that are not digits (or the minus sign) are skipped;
     * the integer is terminated by the first character that is not a digit.
     *
     * \arg \c result is a pointer to where the long integer will be stored.
     *
     * \returns true if a valid integer is found prior to timeout; false otherwise.
     */
    bool readLong( long* result );


    /*!
     * \brief Return the first valid float value from the stream.
     *
     * Initial characters that are not digits (or the minus sign) are skipped;
     * the float is terminated by the first character that is not a digit.
     *
     * \arg \c result is a pointer to where the float will be stored.
     *
     * \returns true if a valid float is found prior to timeout; false otherwise.
     */
    bool readFloat( float* result );


    /*!
     * \brief Return the first valid long integer value from the stream, ignoring
     * selected characters.
     *
     * Initial characters that are not digits (or the minus sign) are skipped;
     * the integer is terminated by the first character that is not a digit and is not
     * one of the skip characters.  This allows format characters (typically commas)
     * to be ignored on input.
     *
     * \arg \c result is a pointer to where the long integer will be stored.
     * \arg \c skipChar is a character that will be ignored on input.
     *
     * \returns true if a valid long integer is found prior to timeout; false otherwise.
     */
    bool readLong( long* result, char skipChar );


    /*!
     * \brief Return the first valid float value from the stream, ignoring
     * selected characters.
     *
     * Initial characters that are not digits (or the minus sign) are skipped;
     * the float is terminated by the first character that is not a digit and is not
     * one of the skip characters.  This allows format characters (typically commas)
     * to be ignored on input.
     *
     * \arg \c result is a pointer to where the float will be stored.
     * \arg \c skipChar is a character that will be ignored on input.
     *
     * \returns true if a valid float is found prior to timeout; false otherwise.
     */
    bool readFloat( float* result, char skipChar );


    /*!
     * \brief Read characters from the input stream into a buffer, terminating if length characters
     * have been read or the function times out.  The result is \e NOT null-termimated.
     *
     * \arg \c buffer a pointer to where the characters read will be stored.
     * \arg \c length the maximum number of characters to read.
     *
     * \returns the number of characters placed in the buffer (0 means no data were read prior to
     * timeout).
     */
    size_t readBytes( char *buffer, size_t length );


    /*!
     * \brief Read characters from the input stream into a buffer, terminating  when the terminator
     * charactor is encountered, or if length characters
     * have been read, or if the function times out.  The result is \e NOT null-terminated.
     *
     * \arg \c terminator a character that when encountered causes the function to return.
     * \arg \c buffer a pointer to where the characters read will be stored.
     * \arg \c length the maximum number of characters to read.
     *
     * \returns the number of characters placed in the buffer (0 means no data were read prior to
     * timeout or detecting the terminator character).
     */
   size_t readBytesUntil( char terminator, char* buffer, size_t length );


    /*!
     * \brief Read bytes (uint8_t) from the input stream into a buffer, terminating if length bytes
     * have been read or the function times out.
     *
     * \arg \c buffer a pointer to where the bytes read will be stored.
     * \arg \c length the maximum number of bytes to read.
     *
     * \returns the number of bytes placed in the buffer (0 means no data were read prior to
     * timeout).
     */
    size_t readBytes( uint8_t* buffer, size_t length )
    { return readBytes( reinterpret_cast<char*>(buffer), length ); }



    /*!
     * \brief Read bytes (uint8_t) from the input stream into a buffer, terminating  when the terminator
     * byte is encountered, or if length bytes
     * have been read, or if the function times out.
     *
     * \arg \c terminator a byte that when encountered causes the function to return.
     * \arg \c buffer a pointer to where the bytes read will be stored.
     * \arg \c length the maximum number of bytes to read.
     *
     * \returns the number of bytes placed in the buffer (0 means no data were read prior to
     * timeout or detecting the terminator character).
     */
   size_t readBytesUntil( uint8_t terminator, uint8_t* buffer, size_t length )
   { return readBytesUntil( static_cast<char>(terminator), reinterpret_cast<char*>(buffer), length ); }


    /*!
     * \brief Read characters from the input stream into a buffer, until it reaches EOL, or if
     * length characters have been read, or if it times out.
     * The result \e IS null-termimated.
     *
     * \arg \c buffer a pointer to where the characters read will be stored.
     * \arg \c length the maximum number of characters to read.
     *
     * \returns the number of characters placed in the buffer (0 means no data were read prior to
     * timeout or detecting EOL).
     */
    size_t readLine( char *buffer, size_t length );


    /*!
     * \brief Consumes whitespace characters until the first non-whitespace character is encountered or
     * the function times out.
     */
    void consumeWhiteSpace();


private:

    // Number of milliseconds to wait for the next char before aborting timed read
    unsigned long mTimeOut;

    int timedRead();            // private method to read stream with timeout
    int timedPeek();            // private method to peek stream with timeout
    int peekNextDigit();        // returns the next numeric digit in the stream or -1 if timeout
};


#endif
