/*
    ArduinoPins.h - Macros naming the Arduino pins (selects the appropriate variant).
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
 * \brief This file is the primary one that users should include to access and
 * use the pin name macros.
 *
 * Including this file will automatically include either the default Arduino Uno
 * pin names (by including ArduinoUnoPins.h) or the default Arduino Mega pin names
 * (by including ArduinoMegaPins.h).
 *
 * The standard Arduino digital pins will be defined in the form pPinNN (where
 * NN = 00 through 13 for Arduino Uno, and 00 through 53 for Arduino Mega).
 *
 * The standard Arduino analog pins will be defined in the form pPinAxx (where
 * xx = 00 through 07 for Arduino Uno, and xx = 00 through 15 for Arduino Mega).
 *
 */




#ifndef ArduinoPins_h
#define ArduinoPins_h


#if defined(__AVR_ATmega328P__)

#include "ArduinoUnoPins.h"

#elif defined(__AVR_ATmega2560__)

#include "ArduinoMegaPins.h"

#else

#error "Undefined AVR processor type"

#endif


#endif

