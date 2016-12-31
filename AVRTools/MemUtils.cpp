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



// avr-libc internal globals used to manage the heap

extern char                 __heap_start;   // Start of the heap
extern char*                __brkval;       // Top of the heap (if non-zero)
extern struct __freelist*   __flp;          // The head of the free list structure


// The free list structure as maintained by the avr-libc memory allocation routines

struct __freelist
{
    size_t sz;
    struct __freelist *nx;
};


// Utility macros

#define STACK           reinterpret_cast<char*>( AVR_STACK_POINTER_REG )

#define HEAP_START      static_cast<char*>( &__heap_start )

#define HEAP_TOP        static_cast<char*>( __brkval )






size_t MemUtils::memoryAvailableOnFreeList()
{
    struct __freelist* current;
    size_t total = 0;

    for ( current = __flp; current; current = current->nx )
    {
        // Memory in the header (the "size" slot) is not available for allocation, so don't count it
        total += current->sz;
    }

    return total;
}





size_t MemUtils::getFreeListStats( int* nbrBlocks, size_t* sizeSmallestBlock, size_t* sizeLargestBlock )
{
    struct __freelist* current;
    size_t total = 0;
    int nbr = 0;
    size_t max = 0;
    size_t min = static_cast<size_t>( RAMEND ) - static_cast<size_t>( RAMSTART );

    for ( current = __flp; current; current = current->nx )
    {
        size_t thisSize = current->sz;
        ++nbr;
        if ( thisSize > max )
        {
            max = thisSize;
        }
        if ( thisSize < min )
        {
            min = thisSize;
        }
        total += thisSize;
    }

    if ( !nbr )
    {
        // Avoid returning something stupid
        min = 0;
    }

    *nbrBlocks = nbr;
    *sizeSmallestBlock = min;
    *sizeLargestBlock = max;

    return total;
}





size_t MemUtils::freeSRAM()
{
    size_t freeMemory;

    if ( HEAP_TOP )
    {
        // We have a heap allocation, so need to sum space beyond top of heap and the free list space
        freeMemory = STACK - HEAP_TOP;
        freeMemory += memoryAvailableOnFreeList();
    }
    else
    {
        // No heap allocations, so it's free memory from heap start all the way to the stack end
        freeMemory = STACK - HEAP_START;
    }

    return freeMemory;
}





size_t MemUtils::freeMemoryBetweenHeapAndStack()
{
    size_t freeMemory;

    if ( HEAP_TOP )
    {
        // We have a heap allocation, so measure from the top of heap
        freeMemory = STACK - HEAP_TOP;
    }
    else
    {
        // No heap allocations, so measure from the heap start
        freeMemory = STACK - HEAP_START;
    }

    return freeMemory;
}





void MemUtils::resetHeap()
{
    // This resets the free-list to "no free-list"
    __flp = 0;

    // This "forgets" any existing heap allocations
    __brkval = 0;
}
