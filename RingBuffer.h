/*
    RingBuffer.h - A ring buffer class for AVR processors.
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
 * \brief This file provides an efficient ring buffer implementation for storing bytes.
 *
 * Ring buffers are particularly useful for memory constrained microcontrollers such as the
 * ATmega328 and ATmega2650.
 */



#ifndef RingBuffer_h
#define RingBuffer_h


#include <util/atomic.h>



/*!
 * \brief This class provides an efficient ring buffer implementation for storing bytes.  Ring
 * buffers are particularly useful for memory constrained microcontrollers such as the
 * ATmega328 and ATmega2650.  For maximum efficiency, this class is focused on the storgage of bytes, providing
 * a single code base that is shared by all instances of this class.
 *
 * For maximum flexiblity, the caller must provide the storage to be used for each RingBuffer object
 * instantiated (this allows the use of different sized ring bufferss without having to make dynamic
 * memory allocations).
 *
 * The implementation of RingBuffer is interrupt safe: the key operations are atomic, allowing for RingBuffer
 * objects to be shared between interrupt functions and ordinary code.
 *
 * The template-based RingBufferT class provides a more flexible ring buffer implementation that
 * can store a variety of data types.  However, this comes at the cost of replicating code for each
 * template instantiation of RingBufferT.
 */

class RingBuffer
{
public:

    /*!
     * \brief Construct a ring buffer by providing the storage area for the ring buffer.
     *
     * \arg \c buffer the storage for the ring buffer.
     * \arg \c size the size of the storage for the ring buffer.
     */
    RingBuffer( unsigned char *buffer, unsigned short size );

    /*!
     * \brief Extract the next (first) byte from the ring buffer.
     *
     * \returns the next byte, or -1 if the ring buffer is empty.
     */
    int pull();

    /*!
     * \brief Examine an element in the ring buffer.
     *
     * \arg \c index the element to examine; 0 means the first (= next) element in the buffer.
     * The default if the argument is omitted is to return the first element.
     *
     * \returns the next element or -1 if there is no such element.
     */
    int peek( unsigned short index = 0 );

    /*!
     * \brief Push a byte into the ring buffer.  The element is appended to the back
     * of the buffer.
     *
     * \arg \c element is the byte to append to the ring buffer.
     *
     * \returns 0 (false) if it succeeds; 1 (true) if it fails because the buffer is full.
     */
    bool push( unsigned char element );


    /*!
     * \brief Determine if the buffer is full and cannot accept more bytes.
     *
     * \returns true if the buffer is full; false if not.
     */
    bool isFull();

    /*!
     * \brief Determine if the buffer is not full and can accept more bytes.
     *
     * \returns true if the buffer is not full; false if it is full.
     */
    bool isNotFull();

    /*!
     * \brief Determine if the buffer is empty .
     *
     * \returns true if the buffer is empty; false if not.
     */
    bool isEmpty()
    { return !static_cast<bool>( mLength ); }

    /*!
     * \brief Determine if the buffer is not empty.
     *
     * \returns true if the buffer is not empty; false if it is empty.
     */
    bool isNotEmpty()
    { return static_cast<bool>( mLength ); }

    /*!
     * \brief Clear the ring buffer, leaving it empty.
     */
    void clear();


private:

    unsigned char *mBuffer;
    volatile unsigned short mSize;
    volatile unsigned short mLength;
    volatile unsigned short mIndex;

};


#endif


