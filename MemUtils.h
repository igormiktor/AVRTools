/*
    MemUtils.h - Memory-related utilities
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
 * \brief This file provides functions that provide information on the available memory in SRAM.
 *
 * To use these functions, include MemUtils.h in your source code and link against MemUtils.cpp.
 *
 * These functions are wrapped in namespace MemUtils to avoid namespace collisions.
 *
 */



#ifndef MemUtils_h
#define MemUtils_h



/*!
 * \brief A namespace providing encapsulation for functions that report the available memory in SRAM
 */

namespace MemUtils
{

/*!
 * \brief Get the number of free bytes remaining in SRAM.
 *
 * \returns The number of free bytes remaining in SRAM.
 *
 */

unsigned int freeRam();



/*!
 * \brief Get a quick estimate of the number of free bytes remaining in SRAM.
 *
 * This provides a quicker, but perhaps slightly inaccurate, estimate of the amount of free memory.
 *
 * \returns The number of free bytes remaining in SRAM.
 *
 */

unsigned int freeRamQuickEstimate();

};

#endif