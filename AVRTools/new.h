/*
    new.cpp - operator new implementations not provided with avr-gcc.
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
 * \brief This file provides \c operator \c new and \c operator \c delete.  You
 * only need this file if you use \c new and \c delete to manage objects on the heap.
 *
 * If you do use \c new and \c delete, then include new.h in your source files
 * and link your project against new.cpp.
 *
 * \note The AVRTools library does not itself make any use of heap storage
 * or the `new` or `delete` operators.
 *
 */


#ifndef new_h
#define new_h


#include <stdlib.h>

void* operator new( size_t size );
void* operator new[]( size_t size );


void operator delete( void* ptr );
void operator delete[]( void* ptr );

#if __cplusplus >= 201402L

void operator delete ( void* ptr, size_t sz );
void operator delete[]( void* ptr, size_t sz );

#endif


// Placement new & delete operators

inline void* operator new( size_t, void* ptr )
{ return ptr; }
inline void* operator new[]( size_t, void* ptr )
{ return ptr; }

inline void operator delete( void* , void* )
{ }
inline void operator delete[]( void* , void* )
{ }


#endif

