/*
    MemUtils.cpp - Memory-related utilities
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



#include "MemUtils.h"

#include <stddef.h>
#include <avr/io.h>

#include "USART0.h"


extern unsigned int         __heap_start;
extern void*                __brkval;
extern struct __freelist*   __flp;          // The head of the free list structure


/*
 * The free list structure as maintained by the
 * avr-libc memory allocation routines.
 */

struct __freelist
{
    size_t sz;
    struct __freelist *nx;
};


namespace
{

    // Calculates the size of the free list
    unsigned int freeListSize()
    {
        struct __freelist* current;
        unsigned int total = 0;

        for ( current = __flp; current; current = current->nx )
        {
            total += 2;         // Add two bytes for the memory block's header
            total += static_cast<int>( current->sz );
        }

        return total;
    }

};


unsigned int MemUtils::freeRam()
{
    unsigned int free_memory;

    if ( int( __brkval ) == 0 )
    {
        free_memory = reinterpret_cast<unsigned int>( &free_memory ) - reinterpret_cast<unsigned int>( &__heap_start );
    }
    else
    {
        free_memory = reinterpret_cast<unsigned int>( &free_memory ) - reinterpret_cast<unsigned int>( __brkval );
        free_memory += freeListSize();
  }
  return free_memory;
}



unsigned int MemUtils::freeRamQuickEstimate()
{
    // Extra "-2" at the end makes freeRamQuickEstimate() agree 99% of the time with freeRam()
    unsigned int v;
    return reinterpret_cast<unsigned int>( &v ) -
        ( __brkval == 0 ? reinterpret_cast<unsigned int>( &__heap_start ) : reinterpret_cast<unsigned int>( __brkval ) ) - 2;
}
