/*
    SimpleDelays12Mhz.inc.S - Simple delay functions for 12 MHz CPUs.
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



#include <avr/io.h>


.section .text
.global delayQuartersOfMicroSeconds
.global delayWholeMilliSeconds
.global delayTenthsOfSeconds





/*
    delayQuartersOfMicroSeconds subroutine

    Takes a single uint16_t argument passed in r25:r24
    (as per avr-gcc parameter passing rules).

    That parameter is the number of quarter milliseconds to delay.

    The delay loop using a 16-bit counter, so up to 65535 iterations are possible
    (equal to 32762 microseconds or ~32.8 milliseconds).  The loop executes
    six CPU cycles per iteration, not including the function call overhead.
*/

delayQuartersOfMicroSeconds:

; Register r25:24 is passed as parameter (the number of quarter
; microseconds to delay)

; r24 = LSB quarter microseconds to delay
; r25 = MSB quarter microseconds to delay

#define rDelayL     r24
#define rDelayH     r25

; Function call/return overhead takes about 1+1/2 microseconds, or 18 cycles,
; or 6 quarter microseconds.
; Takes 4 cycles (= 1/3 microsecond) to determine the requested delay is small,
; for total time of 22 cycles or 1.83 us or slightly over 7 quarter seconds
; so return for any delay of less or equal to 8 quarter microseconds;

#define kMinDelay       10

    cpi rDelayH, 0                  ; 1 cycle
    brne 2f                         ; 1 cycle if false, 2 cycles if true
    cpi rDelayL, (kMinDelay + 2)    ; 1 cycle
    brge 3f                         ; 1 cycle if false, 2 cycles if true


                                    ; For smallish delays we do some special adjustments
    cpi rDelayL, (kMinDelay - 2)    ; 1 cycle
    brge 1f                         ; 1 cycle if false, 2 cycles if true


    ret                             ; Getting to here takes 6 cycles = 1/2 microsecond
                                    ; or 24 cycles (2 us) including the function call overhead
                                    ; We are exact if r24 == 8, but long if r24 < 8

1:
    nop                             ; Getting to here takes 7 cycles or 25 cycles total
    nop                             ; and we know 8 < r24 < 12
    nop                             ; Split the difference and assume r24 = 10 us or 30 cycles
    nop                             ; so we add 5 NOPs
    nop

    ret                             ; Getting to here takes 30 cycles total
                                    ; We are 3 cycles short if r24 == 11, but 3 cycles long if r24 == 9


2:
    nop                             ; need to add a little extra delay if we branched at first brne
    nop


3:                                  ; Getting here takes 5 cycles for larger values of r24
    sbiw rDelayL, kMinDelay         ; 2 cycles

    nop                             ; Need a NOP here to get the overall delay right

                                    ; 8 cycles to here for larger values of r24

                                    ; If rDelayL is odd, we need to add a little extra delay
    sbrs rDelayL, 0                 ; 1 cycle if false (even), 2 cycles if true (odd)
    rjmp 4f                         ; 2 cycles

    nop                             ; Add 3 cycles of extra delay in the odd case
    nop
    nop


4:                                  ; Each loop is 6 cycles or 2 quarter microseconds, so divide loop count in half
    lsr rDelayH                     ; 1 cycle
    ror rDelayL                     ; 1 cycle
                                    ; Because of the (kMinDelay + 2) above, know rDelayL will at least be 1


                                    ; This is a 6-cycle loop (or 2 quarter microseconds)
5:
    nop                             ; 1 cycle
    nop                             ; 1 cycle
    sbiw rDelayL, 1                 ; 2 cycles
    brne 5b                         ; 2 cycles on loop, 1 cycle on exit

    ret

#undef rDelayL
#undef rDelayH
#undef kMinDelay





/*
    delayWholeMilliSeconds subroutine

    Takes a single uint8_t argument passed in r24
    (as per avr-gcc parameter passing rules).

    That parameter is the number of milliseconds
    to delay.

    Each individual millisecond delay loop is
    accurate to within a few microseconds at 12MHz.

    The call overhead and the loop for multiple
    millisecond  will increase inaccuracy, but the
    error will remain at most a few hundreds of
    microseconds.

    Delay loop using an 8-bit counter that iterates a set of inner loops.
    The set of inner loops takes 1 millisecond to execute.

    The 8-bit parameter is the number of milliseconds to delay, for a
    maximum of 256 milliseconds or about a quarter of a second.  Note
    that the value 256 would have to be passed as 0.

    Although the 1 millisecond timing loop itself is accurate to within
    10-20 microseconds at 12 MHz, the call overhead and the loop for
    multiple milliseconds will increase the inaccuracy, but the error
    will remain at most a few hundreds of microseconds.
*/

delayWholeMilliSeconds:

; Register r24 (milliSecCounter) is passed as parameter

; r24 = number of milliseconds to count (comes in as argument)
;     = number of times to execute the outer+inner loops combined
; r25 = outer loop counter byte
; r26 = low byte of inner loop counter word
; r27 = high byte of inner loop counter word

#define rMillis         r24
#define rOuter          r25
#define rInnerL         r26
#define rInnerH         r27

; Executing the following combination of inner and outer loop cycles takes almost precisely 1 millisecond at 12 MHz
#define kOuterCount     2
#define kInnerCount     1499


; Top of loop for number of milliseconds
1:
    ; Initialize outer loop (uses a byte counter and counts down)
    ldi rOuter, kOuterCount

; Top of outer loop
2:
    ; Initialze inner loop (uses a word counter and counts down)
    ldi rInnerL, lo8( kInnerCount )
    ldi rInnerH, hi8( kInnerCount )

; Top of inner loop
3:
    ; Decrement and test inner loop
    sbiw rInnerL, 1
    brne 3b
    ; Done with inner loop

    ; Decrement and test outer loop
    dec rOuter
    brne 2b
    ; Done with outer loop

    ; Decrement and test millisecond loop
    dec rMillis
    brne 1b
    ; Done with the requested number of milliseconds

    ret

#undef rMillis
#undef rOuter
#undef rInnerL
#undef rInnerH
#undef kOuterCount
#undef kInnerCount








/*
    delayTenthsOfSeconds subroutine

    Delay loop using an 8-bit counter that iterates a set of inner loops.
    The set of inner loops takes 1/10 second to execute.

    The 8-bit parameter is the number of tenths of a second to delay, for
    a maximum of 256 tenths of a second or 25.6 secondz.  Note that the
    value 256 would have to be passed as 0.

    Although the 1/10 second timing loop itself is accurate to within
    10-20 microseconds at 12 MHz, the call overhead and the loop for
    multiple tenths of a second will increase the inaccuracy, but the
    error will remain at most a few hundreds of microseconds.

    Takes a single uint8_t argument passed in r24
    (as per avr-gcc parameter passing rules.

    That parameter is the number of tenths of seconds
    to delay.

    Each individual tenth-of-a-second delay loop is
    accurate to within a few microseconds at 12MHz.

    The call overhead and the loop for multiple tenths of
    a second will increase inaccuracy, but the error will
    remain at most a few hundreds of microseconds.
*/

delayTenthsOfSeconds:

; Register r24 (tenthOfSecCounter) is passed as parameter
; r24 = number of tenths-of-seconds to count (comes in as argument)
;     = number of times to execute the outer+inner loops combined
; r25 = outer loop counter byte
; r26 = low byte of inner loop counter word
; r27 = high byte of inner loop counter word

#define r10ths          r24
#define rOuter          r25
#define rInnerL         r26
#define rInnerH         r27

; Executing the following combination of inner and outer loop cycles takes almost precisely 0.1 seconds at 12 Mhz
#define kOuterCount     7
#define kInnerCount     42856



; Top of loop for number of tenths-of-seconds
1:
    ; Initialize outer loop (uses a byte counter and counts down)
    ldi rOuter, kOuterCount

; Top of outer loop
2:
    ; Initialze inner loop (uses a word counter and counts down)
    ldi rInnerL, lo8( kInnerCount )
    ldi rInnerH, hi8( kInnerCount )

; Top of inner loop
3:
    ; Decrement and test inner loop
    sbiw rInnerL, 1
    brne 3b
    ; Done with inner loop

    ; Decrement and test outer loop
    dec rOuter
    brne 2b
    ; Done with outer loop

    ; Decrement and test tenth-of-second loop
    dec r10ths
    brne 1b
    ; Done with the requested number of tenths-of-seconds

    ret

#undef r10ths
#undef rOuter
#undef rInnerL
#undef rInnerH
#undef kInnerCount
#undef kOuterCount
