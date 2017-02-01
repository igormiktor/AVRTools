/*
    SimpleDelays.S - Simple delay functions.
    This is part of the AVRTools library.
    Copyright (c) 2016 Igor Mikolic-Torreira.  All right reserved.

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

#if F_CPU == 16000000L

#include "SimpleDelays16MHz.inc.S"

#elif F_CPU == 12000000L

#include "SimpleDelays12MHz.inc.S"

#elif F_CPU == 8000000L

#include "SimpleDelays8MHz.inc.S"

#else

#error "SimpleDelays only implemented for CPU speeds of 8 MHz, 12 MHz, or 16 MHz."

#endif

