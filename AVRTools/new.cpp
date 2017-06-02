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


/*
    NOTE:  The C++ Standard requires that operator new() always return a valid
    pointer, even if the size is 0.
*/


#include "new.h"


void* operator new( size_t size )
{
    if ( size == 0 )
    {
        size = 1;
    }
    return malloc( size );
}


void* operator new[]( size_t size )
{
    if ( size == 0 )
    {
        size = 1;
    }
    return malloc( size );
}


void operator delete( void* ptr )
{
    if ( ptr )
    {
        free( ptr );
    }
}


void operator delete[]( void* ptr )
{
    if ( ptr )
    {
        free( ptr );
    }
}



#if __cplusplus >= 201402L

void operator delete( void* ptr, size_t sz )
{
    if ( ptr )
    {
        free( ptr );
    }
}


void operator delete[]( void* ptr, size_t sz )
{
    if ( ptr )
    {
        free( ptr );
    }
}

#endif
