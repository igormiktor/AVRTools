/*
    I2cLcd.h - Tools for using an I2C-based LCD such as the
    Adafruit RGB 16x2 LCD Shield .
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
 * \brief This file defines a class that provides a high-level interface to an LCD offering an I2C interface.
 * The most common variant of this is HD44780U controlled LCD driven via an MCP23017 that offers an I2C
 * interface (such LCDs are available from Adafruit and SparkFun).  To use this class you must also use and
 * properly initialize the I2C Master package from I2cMaster.h
 *
 * To use these features, include I2cLcd.h in your source code and link against I2cLcd.cpp and I2cMaster.cpp.
 *
 */



#ifndef I2cLcd_h
#define I2cLcd_h

#include <inttypes.h>
#include "Writer.h"




/*!
 * \brief This class provides a high-level interface via I2C to an LCD such as those offered by AdaFruit
 * and SparkFun.  Specifically, it communicates via I2C with an MCP23017 that drives an HD44780U controlling
 * an LCD.  It also lets you detect button presses on the 5-button keypad generally assocaited with such devices.
 *
 * This class derives from Writer, allowing you to write to the LCD much as it if were a serial device.
 *
 * To use these features, include I2cLcd.h in your source code and link against I2cLcd.cpp and I2cMaster.cpp, and
 * initialize the I2C hardware by calling I2cMaster::start().
 */

class I2cLcd : public Writer
{
public:

    /*!
     *  These constants are used to identify the five buttons.
     */
    enum
    {
        kButton_Select   = 0x01,        //!< the Select button       \hideinitializer
        kButton_Right    = 0x02,        //!< the Right button        \hideinitializer
        kButton_Down     = 0x04,        //!< the Down button         \hideinitializer
        kButton_Up       = 0x08,        //!< the Up button           \hideinitializer
        kButton_Left     = 0x10         //!< the Left button         \hideinitializer
    };


    /*!
     * These constants are used to set the backlight color on the LCD.
     */
    enum
    {
        kBacklight_Red      = 0x1,        //!< Backlight red            \hideinitializer
        kBacklight_Yellow   = 0x3,        //!< Backlight yellow         \hideinitializer
        kBacklight_Green    = 0x2,        //!< Backlight green          \hideinitializer
        kBacklight_Teal     = 0x6,        //!< Backlight teal           \hideinitializer
        kBacklight_Blue     = 0x4,        //!< Backlight blue           \hideinitializer
        kBacklight_Violet   = 0x5,        //!< Backlight violet         \hideinitializer
        kBacklight_White    = 0x7         //!< Backlight white          \hideinitializer
    };


    /*!
     * \brief Constructor simply initializes some internal bookkeeping
     */
    I2cLcd();


    /*!
     * \brief Initialize the I2cLcd object.  This must be called before using the I2cLcd, or
     * calling any of the other member functions.  The I2C system must be initialized
     * before calling this function (by calling I2cMaster::start() from I2cMaster.h).
     *
     * The LCD display is initialized in 16-column, 2-row mode.
     */
    int init();


    /*!
     * \brief Clear the display (all rows, all columns).
     */
    void clear();


    /*!
     * \brief Move the cursor home (the top row, left column).
     */
    void home();


    /*!
     * \brief Display a C-string on the top row.
     *
     * \arg \c str the C-string to display.
     */
    void displayTopRow( const char* str );


    /*!
     * \brief Display a C-string on the bottom row.
     *
     * \arg \c str the C-string to display.
     */
    void displayBottomRow( const char* str );


    /*!
     * \brief Clear the top row.
     */
    void clearTopRow();


    /*!
     * \brief Clear the bottom row.
     */
    void clearBottomRow();



    /*!
     * \brief Turn the display off.
     */
    void displayOff();


    /*!
     * \brief Turn the display on.
     */
    void displayOn();


    /*!
     * \brief Do not blink the cursor.
     */
    void blinkOff();


    /*!
     * \brief Blink the cursor.
     */
    void blinkOn();


    /*!
     * \brief Hide the cursor.
     */
    void cursorOff();


    /*!
     * \brief Display the cursor.
     */
    void cursorOn();


    /*!
     * \brief Scroll the display to the left.
     */
    void scrollDisplayLeft();


    /*!
     * \brief Scroll the display to the right.
     */
    void scrollDisplayRight();


    /*!
     * \brief Turn on automatic scrolling of the display.
     */
    void autoscrollOn();


    /*!
     * \brief Turn off automatic scrolling of the display.
     */
    void autoscrollOff();


    /*!
     * \brief Move the cursor the a particular row and column.
     *
     * \arg \c row the row to move the cursor to (numbering starts at 0).
     * \arg \c col the column to move the cursor to (numbering starts at 0).
     */
    void setCursor( uint8_t row, uint8_t col );


    /*!
     * \brief Set the backlight to a given color.  Set a black-and-white LCD display to White if
     * you want to have a backlight.
     *
     * \arg \c color the color to set the backlight.  Pass one of kBacklight_Red, kBacklight_Yellow,
     *  kBacklight_Green, kBacklight_Teal, kBacklight_Blue, kBacklight_Violet, or kBacklight_White.
     */
    int setBacklight( uint8_t color );


    /*!
     * \brief Pass a command to the LCD.
     *
     * \arg \c cmd a valid command to send to the HD44780U.
     */
    void command( uint8_t cmd );


    /*!
     * \brief Read the state of the buttons associated with the LCD display.
     *
     * \returns a byte with flags set corresponding to the buttons that are depressed.  You must
     * "and" the return value with kButton_Right, kButton_Left, kButton_Down, kButton_Up, or
     * kButton_Select to determine which buttons have been pressed.
     */
    uint8_t readButtons();



    /*!
     * \brief Write a single character to the LCD at the current cursor location.
     * This implements the pure virtual function Writer::write( char c ).
     *
     * \arg \c  the character to be written.
     *
     * \returns the number of bytes written.
     */
    virtual size_t write( char c );


    /*!
     * \brief Write a C-string to the LCD at the current cursor location.
     * This implements the pure virtual function Writer::write( const char* str ).
     *
     * \arg \c  the C-string to be written.
     *
     * \returns the number of bytes written.
     */
    virtual size_t write( const char* str );


    /*!
     * \brief Write a given number of characters from a buffer to the LCD at the current cursor location.
     * This implements the pure virtual function Writer::write( const char* buffer, size_t size ).
     *
     * \arg \c buffer the buffer of characters to write.
     * \arg \c size the number of characters to write
     *
     * \returns the number of bytes written.
     */
    virtual size_t write( const char* buffer, size_t size );


    /*!
     * \brief Write a given number of bytes from a buffer to the LCD at the current cursor location.
     * This implements the pure virtual function Writer::write( const uint8_t* buffer, size_t size ).
     *
     * \arg \c buffer the buffer of bytes to write.
     * \arg \c size the number of bytes to write
     *
     * \returns the number of bytes written.
     */
    virtual size_t write( const uint8_t* buffer, size_t size );


    /*!
     * \brief This function does nothing.  It simply implements the pure virtual
     * function Writer::flush().
     */
    virtual void flush();


private:

    enum
    {
        kWriteFourBitsSendChar = 0,
        kWriteFourBitsSendCommand = 1
    };

    int initMCP23017();
    int initHD44780U();
    size_t write( uint8_t value );
    int writeFourBitsToLcd( uint8_t value, uint8_t gpioB );
    int sendCharOrCmdToLcd( uint8_t value, bool isCommand );

    int sendCommand( uint8_t cmd )
    {
        return sendCharOrCmdToLcd( cmd, kWriteFourBitsSendCommand );
    }

    int sendCharToDisplay( uint8_t value )
    {
        return sendCharOrCmdToLcd( value, kWriteFourBitsSendChar );
    }

    uint8_t             mDisplayControl;
    uint8_t             mDisplayMode;
    uint8_t             mCurrLine;
    volatile uint8_t    mI2cStatus;
};

#endif
