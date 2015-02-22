/*
    RingBufferT.h - A ring buffer template class for AVR processors.
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
 * \brief This file provides a very flexible, template-based ring buffer implementation.
 *
 * Ring buffers are versatile storage structures.  This file provides a template-based
 * ring buffer implementation that can store different kinds of objects in
 * buffers of whatever size is needed.
 */


#ifndef RingBufferT_h
#define RingBufferT_h


#include <util/atomic.h>


/*!
 * \brief a template-based ring buffer class that can store different kinds of objects in
 * buffers of whatever size is needed.
 *
 * The implementation of RingBufferT is interrupt safe: the key operations are atomic, allowing for RingBuffer
 * objects to be shared between interrupt functions and ordinary code.
 *
 * The template-based RingBufferT class provides a very flexible ring buffer implementation; however
 * different instantiations of RingBufferT (e.g., RingBufferT\< char, int, 32 \> and RingBufferT\< char, int, 16 \>)
 * result in replicated code for each instantiation, even when they could logically share code.  For
 * a more efficient ring buffer that avoids such code bloat but can only store bytes, use RingBuffer.
 *
 * \tparam T is the type of object that will be stored in the RingBufferT instantiation.
 * \tparam N is the integer type that will be used to index the RingBufferT elements.
 * \tparam SIZE is an integer indicating the size of the RingBufferT instantiation.
 *
 */

template< typename T, typename N, unsigned int SIZE > class RingBufferT
{

public:

    /*!
     * \brief Construct a ring buffer to store elements of type T indexed by
     * integer type N, with size SIZE.  All of these are passed as template parameters.     *
     */
    RingBufferT()
        : mSize( SIZE ), mLength( 0 ), mIndex( 0 )
        {}


    /*!
     * \brief Extract the next (first) element from the ring buffer.
     *
     * \note There is no
     * general purpose safe value to return to indicate an empty buffer, so before
     * calling pull() be sure to check the ring buffer is not empty.
     *
     * \returns the next element.
     */
    T pull()
    {
        T element = 0;
        ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
        {
            if ( mLength )
            {
                element = mBuffer[ mIndex ];
                mIndex++;
                if ( mIndex >= mSize )
                {
                    mIndex -= mSize;
                }
                --mLength;
            }
        }
        return element;
    }


    /*!
     * \brief Examine an element in the ring buffer.
     *
     * \arg \c index the element to examine; 0 means the first (= next) element in the buffer.
     * The default if the argument is omitted is to return the first element.
     *
     * \note There is no
     * general purpose safe value to return to indicate an empty element, so before
     * calling peek() be sure the element exists.
     *
     * \returns the next element.
     */
    T peek( N index = 0 )
    {
        T element;
        ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
        {
            element = mBuffer[ ( mIndex + index ) % mSize ];
        }
        return element;
    }


    /*!
     * \brief Push an element into the ring buffer.  The element is appended to the back
     * of the buffer.
     *
     * \arg \c element is the item to append to the ring buffer.
     *
     * \returns 0 (false) if it succeeds; 1 (true) if it fails because the buffer is full.
     */
    bool push( T element )
    {
        ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
        {
            if ( mLength < mSize )
            {
                mBuffer[ ( mIndex + mLength ) % mSize ] = element;
                ++mLength;
                return 0;
            }
        }
        // True = failure
        return 1;
    }


    /*!
     * \brief Determine if the buffer is empty .
     *
     * \returns true if the buffer is empty; false if not.
     */
    bool isEmpty()
    {
        return !static_cast<bool>( mLength );
    }


    /*!
     * \brief Determine if the buffer is not empty.
     *
     * \returns true if the buffer is not empty; false if it is empty.
     */
    bool isNotEmpty()
    {
        return static_cast<bool>( mLength );
    }


    /*!
     * \brief Determine if the buffer is full and cannot accept more bytes.
     *
     * \returns true if the buffer is full; false if not.
     */
    bool isFull()
    {
        ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
        {
            return ( mSize - mLength ) <= 0;
        }
    }


    /*!
     * \brief Determine if the buffer is not full and can accept more bytes.
     *
     * \returns true if the buffer is not full; false if it is full.
     */
    bool isNotFull()
    {
        ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
        {
            return ( mSize - mLength ) > 0;
        }
    }


    /*!
     * \brief discard a number of elements from the front of the ring buffer.
     *
     * \arg \c nbrElements the number of elements to discard.
     */
    void discardFromFront( N nbrElements )
    {
        ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
        {
            if ( nbrElements < mLength )
            {
                mIndex += nbrElements;
                if( mIndex >= mSize )
                {
                    mIndex -= mSize;
                }
                mLength -= nbrElements;
            }
            else
            {
                // flush the whole buffer
                mLength = 0;
            }
        }
    }


    /*!
     * \brief Clear the ring buffer, leaving it empty.
     */
    void clear()
    {
        ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
        {
            mLength = 0;
        }
    }



private:

    T mBuffer[ SIZE ] ;
    volatile N mSize;
    volatile N mLength;
    volatile N mIndex;

};


#endif


