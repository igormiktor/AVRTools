/*
    abi.h - C++ ABI support missing from avr-gcc.
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
 * \brief This file provides certain functions needed to complete the avr-gcc C++ ABI. You
 * never need to include this file, and you only need to link against abi.cpp if you encounter
 * certain link errors.
 *
 * If when building your project you get link-time errors
 * about undefined references to symbols of the form \c __cxa_XXX (e.g., \c __cxa_pure_virtual), then you
 * should link your project against abi.cpp (there is no need to include abi.h in
 * any of your sources.
 *
 * If you don't encounter such errors, you can completely disregard both abi.h and abi.cpp.
 *
 */


#ifndef abi_h
#define abi_h


#include <stdlib.h>

__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C"
{
int __cxa_guard_acquire( __guard* );
void __cxa_guard_release ( __guard* );
void __cxa_guard_abort ( __guard* );

void __cxa_pure_virtual() __attribute__ ((__noreturn__));
void __cxa_deleted_virtual() __attribute__ ((__noreturn__));
}





#endif
