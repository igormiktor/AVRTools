/*
    Writer.h - a base class for writing data.
    For AVR ATMega328p (Arduino Uno) and ATMega2560 (Arduino Mega).
    This is part of the AVRTools library.
    Copyright (c) 2014 Igor Mikolic-Torreira.  All right reserved.
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
 * \brief This file provides a generic interface to outgoing data streams of any kind.  It is designed
 * around how serial streams are generally used, but can be used with any system that requires converting strings
 * and/or numbers into a sequential output of bytes.
 */



#ifndef Writer_h
#define Writer_h

#include <stdint.h>
#include <stddef.h>



#ifndef SERIAL_OUTPUT_EOL
#define SERIAL_OUTPUT_EOL    '\n'
#endif




/*!
 * \brief This is an abstract class defining a generic interface to write numbers and strings to a sequential
 * stream of bytes (such as a serial output device).
 *
 * It implements functions to convert various integers
 * and floating point numbers into a sequence of bytes (so it is not a pure interface class).  These functions depend on a small
 * set of lower-level functions that are purely abstract and must be implemented by classes deriving from
 * Writer.
 *
 * Serial0 is an example of a class that derives from Writer by implementating the purely
 * abstract functions in Writer.
 */


class Writer
{
public:


    /*!
     * \brief An enumeration that defines the number that will be used as the base for representing
     * integer quantities as a string of characters.
     */

    enum IntegerOutputBase
    {
        kBin    = 2,        //!< Produce a binary representation of integers (e.g., 11 is output as 0b1011)
        kOct    = 8,        //!< Produce an octal representation of integers (e.g, 11 is output as 013)
        kDec    = 10,       //!< Produce a decimal representation of integers (e.g., 11 is output as 11
        kHex    = 16        //!< Produce a hexadecimal representation of integers (e.g., 11 is output as 0x0b)
    };



    /*!
     * \brief Pure virtual function that writes a single character to the output stream.
     *
     * \arg \c c the character to be written.
     *
     * \returns the number of bytes written.
     */
    virtual size_t write( char c ) = 0;


    /*!
     * \brief Pure virtual function that writes a null-terminated string to the output stream.
     *
     * \arg \c str the string to be written.
     *
     * \returns the number of bytes written.
     */
    virtual size_t write( const char* str ) = 0;


    /*!
     * \brief Pure virtual function that writes a given number of characters from a buffer to the output stream.
     *
     * \arg \c buffer the buffer of characters to write.
     * \arg \c size the number of characters to write
     *
     * \returns the number of bytes written.
     */
    virtual size_t write( const char* buffer, size_t size ) = 0;


    /*!
     * \brief Pure virtual function that writes a given number of bytes from a buffer to the output stream.
     *
     * \arg \c buffer the buffer of bytes to write.
     * \arg \c size the number of bytes to write
     *
     * \returns the number of bytes written.
     */
    virtual size_t write( const uint8_t* buffer, size_t size ) = 0;


    /*!
     * \brief Pure virtual function to flush the output stream.  When this function returns, all previously
     * written data will have been transmitted through the underlying output stream.
     */
    virtual void flush() = 0;




    /*!
     * \brief Print a null-terminated string to the output stream, with or without adding
     * a new line character at the end.
     *
     * \arg \c str is the null-terminated string to output.
     * \arg \c addLn if true, a new line character is added at the end of the output; the default is false.
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t print( const char* str, bool addLn = false );


    /*!
     * \brief Print a number of bytes to the output stream, with or without adding
     * a new line character at the end.
     *
     * \arg \c buf is the buffer containing bytes to output.
     * \arg \c size is the number of bytes from the buffer to output.
     * \arg \c addLn if true, a new line character is added at the end of the output; the default is false.
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t print( const uint8_t* buf, size_t size, bool addLn = false );


    /*!
     * \brief Print a single character to the output stream, with or without adding
     * a new line character at the end.
     *
     * \arg \c c is the character to output.
     * \arg \c addLn if true, a new line character is added at the end of the output; the default is false.
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t print( char c, bool addLn = false );


    /*!
     * \brief Print an integer to the output stream, with or without adding
     * a new line character at the end.
     *
     * \arg \c n is the integer to output.
     * \arg \c base is the base used to represent the number; should be one of IntegerOutputBase; defaults to
     * decimal representation (kDec).
     * \arg \c addLn if true, a new line character is added at the end of the output; the default is false.
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t print( int n, int base = kDec, bool addLn = false )
    { return print( static_cast<long>( n ), base, addLn ); }


    /*!
     * \brief Print an unsigned integer to the output stream, with or without adding
     * a new line character at the end.
     *
     * \arg \c n is the unsigned integer to output.
     * \arg \c base is the base used to represent the number; should be one of IntegerOutputBase; defaults to
     * decimal representation (kDec).
     * \arg \c addLn if true, a new line character is added at the end of the output; the default is false.
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t print( unsigned int n, int base = kDec, bool addLn = false )
    { return print( static_cast<unsigned long>( n ), base, addLn ); }


    /*!
     * \brief Print a long integer to the output stream, with or without adding
     * a new line character at the end.
     *
     * \arg \c n is the long integer to output.
     * \arg \c base is the base used to represent the number; should be one of IntegerOutputBase; defaults to
     * decimal representation (kDec).
     * \arg \c addLn if true, a new line character is added at the end of the output; the default is false.
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t print( long n, int base = kDec, bool addLn = false );


    /*!
     * \brief Print an unsigned long integer to the output stream, with or without adding
     * a new line character at the end.
     *
     * \arg \c n is the unsigned long integer to output.
     * \arg \c base is the base used to represent the number; should be one of IntegerOutputBase; defaults to
     * decimal representation (kDec).
     * \arg \c addLn if true, a new line character is added at the end of the output; the default is false.
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t print( unsigned long n, int base = kDec, bool addLn = false );


    /*!
     * \brief Print a floating point number to the output stream, with or without adding
     * a new line character at the end.
     *
     * \arg \c d is the floating point number to output.
     * \arg \c digits is the number of decimal digits to output; the default is 2.
     * \arg \c addLn if true, a new line character is added at the end of the output; the default is false.
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t print( double d, int digits = 2, bool addLn = false );


    /*!
     * \brief Print a null-terminated string to the output stream, adding
     * a new line character at the end.
     *
     * \arg \c str is the null-terminated string to output.
     *
     * \returns the number of bytes sent to the output stream.
     */
    size_t println( const char* str )                       { return print( str, true ); }

    /*!
     * \brief Print a number of bytes to the output stream, adding
     * a new line character at the end.
     *
     * \arg \c buf is the buffer containing bytes to output.
     * \arg \c size is the number of bytes from the buffer to output.
     *
     * \returns the number of bytes sent to the output stream.
     */
    size_t println( const uint8_t* buf, size_t size )       { return print( buf, size, true ); }

    /*!
     * \brief Print a single character to the output stream, adding
     * a new line character at the end.
     *
     * \arg \c c is the character to output.
     *
     * \returns the number of bytes sent to the output stream.
     */
    size_t println( char c )                                { return print( c, true ); }

    /*!
     * \brief Print an unsigned character to the output stream, adding
     * a new line character at the end.
     *
     * \arg \c n is the unsigned character to output.
     * \arg \c base is the base used to represent the number; should be one of IntegerOutputBase; defaults to
     * decimal representation (kDec).
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t println( unsigned char n, int base = kDec )      { return print( static_cast<unsigned long>( n ), base, true ); }

    /*!
     * \brief Print an integer to the output stream, adding
     * a new line character at the end.
     *
     * \arg \c n is the integer to output.
     * \arg \c base is the base used to represent the number; should be one of IntegerOutputBase; defaults to
     * decimal representation (kDec).
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t println( int n, int base = kDec )                { return print( n, base, true ); }

    /*!
     * \brief Print an unsigned integer to the output stream, adding
     * a new line character at the end.
     *
     * \arg \c n is the unsigned integer to output.
     * \arg \c base is the base used to represent the number; should be one of IntegerOutputBase; defaults to
     * decimal representation (kDec).
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t println( unsigned int n, int base = kDec )       { return print( n, base, true ); }

    /*!
     * \brief Print a long integer to the output stream, adding
     * a new line character at the end.
     *
     * \arg \c n is the long integer to output.
     * \arg \c base is the base used to represent the number; should be one of IntegerOutputBase; defaults to
     * decimal representation (kDec).
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t println( long n, int base = kDec )               { return print( n, base, true ); }

    /*!
     * \brief Print an unsigned long integer to the output stream, adding
     * a new line character at the end.
     *
     * \arg \c n is the unsigned long integer to output.
     * \arg \c base is the base used to represent the number; should be one of IntegerOutputBase; defaults to
     * decimal representation (kDec).
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t println( unsigned long n, int base = kDec )      { return print( n, base, true ); }

    /*!
     * \brief Print a floating point number to the output stream, adding
     * a new line character at the end.
     *
     * \arg \c d is the flaoting point number to output.
     * \arg \c digits is the number of decimal digits to output; the default is 2.
     *
     * \returns the number of bytes sent to the output stream.
     *
     * \hideinitializer
     */
    size_t println( double d, int digits = 2 )              { return print( d, digits, true ); }

    /*!
     * \brief Print a new line to the output stream.
     */
    size_t println();

private:

    size_t printNumber( unsigned long n, uint8_t base );
    size_t printFloat( double d, uint8_t digits );
};

#endif

