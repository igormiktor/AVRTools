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
    (equal to 16383.75 microseconds or ~16.4 milliseconds).  The loop executes
    four CPU cycles per iteration, not including the function call overhead.
*/

delayQuartersOfMicroSeconds:

; Register r24 is passed as parameter (the number of quarter
; microseconds to delay)

; Function call/return overhead takes about 1+1/8 microseconds, or 18 cycles,
; or between 4 and 5 quarter microseconds.
; Takes 4 cycles (= 1 quarter microsecond) to determine the requested delay is small,
; so return for any delay of less than 7 quarter microseconds;

.set kMinDelay,     6

    cpi r25, 0                  ; 1 cycle
    brne 1f                     ; 1 cycle if false, 2 cycles if true
    cpi r24, (kMinDelay + 1)    ; 1 cycle
    brge 2f                     ; 1 cycle if false, 2 cycles if true
    ret                         ; Getting to here takes 4 cycles = 1 quarter microsecond
                                ; or 22 cycles including the function call overhead
                                ; We are two cycles short if r24 == 6, but long if r24 < 6

1:
    nop                         ; need to add a little extra delay if we branched at first brne
    nop

2:
    sbiw r24, kMinDelay         ; 2 cycles

3:                              ; Getting to here takes 7 cycles (1 short of 2 delay loops),
                                ; for a total of 25 cycles including function call overhead.
                                ; We're always 1 cycle short on loop exit, so things balance out.
    sbiw 24, 1                  ; 2 cycles
    brne 3b                     ; 1 cycle on exit, 2 cycles on loop
    ret







/*
    delayWholeMilliSeconds subroutine

    Takes a single uint8_t argument passed in r24
    (as per avr-gcc parameter passing rules).

    That parameter is the number of milliseconds
    to delay.

    Each individual millisecond delay loop is
    accurate to within a few microseconds at 16MHz.

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
    10-20 microseconds at 16 MHz, the call overhead and the loop for
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


; Executing the following combination of inner and outer loop cycles takes almost precisely 1 millisecond
.set kOuterCount,           2
.set kInnerCount,           1997


; Top of loop for number of milliseconds
1:
    ; Initialize outer loop (uses a byte counter and counts down)
    ldi r25, kOuterCount

; Top of outer loop
2:
    ; Initialze inner loop (uses a word counter and counts down)
    ldi r26, lo8( kInnerCount )
    ldi r27, hi8( kInnerCount )

; Top of inner loop
3:
    ; Decrement and test inner loop
    sbiw r26, 1
    brne 3b
    ; Done with inner loop

    ; Decrement and test outer loop
    subi r25, 1
    brne 2b
    ; Done with inner loop

    ; Decrement and test millisecond loop
    subi r24, 1
    brne 1b
    ; Done with the requested number of milliseconds

    ret








/*
    delayTenthsOfSeconds subroutine

    Delay loop using an 8-bit counter that iterates a set of inner loops.
    The set of inner loops takes 1/10 second to execute.

    The 8-bit parameter is the number of tenths of a second to delay, for
    a maximum of 256 tenths of a second or 25.6 secondz.  Note that the
    value 256 would have to be passed as 0.

    Although the 1/10 second timing loop itself is accurate to within
    10-20 microseconds at 16 MHz, the call overhead and the loop for
    multiple tenths of a second will increase the inaccuracy, but the
    error will remain at most a few hundreds of microseconds.

    Takes a single uint8_t argument passed in r24
    (as per avr-gcc parameter passing rules.

    That parameter is the number of tenths of seconds
    to delay.

    Each individual tenth-of-a-second delay loop is
    accurate to within a few microseconds at 16MHz.

    The call overhead and the loop for multiple tenths of
    a second will increase inaccuracy, but the error will
    remain at most a few hundreds of microseconds.
*/

delayTenthsOfSeconds:

; Register r24 (tenthOfSecCounter) is passed as parameter

; Executing the following combination of inner and outer loop cycles takes almost precisely 0.1 seconds
.set kOuterCount,           63
.set kInnerCount,           63491

; r24 = number of tenths-of-seconds to count (comes in as argument)
;     = number of times to execute the outer+inner loops combined
; r25 = outer loop counter byte
; r26 = low byte of inner loop counter word
; r27 = high byte of inner loop counter word



; Top of loop for number of tenths-of-seconds
1:
    ; Initialize outer loop (uses a byte counter and counts down)
    ldi r25, kOuterCount

; Top of outer loop
2:
    ; Initialze inner loop (uses a word counter and counts down)
    ldi r26, lo8( kInnerCount )
    ldi r27, hi8( kInnerCount )

; Top of inner loop
3:
    ; Decrement and test inner loop
    sbiw r26, 1
    brne 3b
    ; Done with inner loop

    ; Decrement and test outer loop
    subi r25, 1
    brne 2b
    ; Done with inner loop

    ; Decrement and test tenth-of-second loop
    subi r24, 1
    brne 1b
    ; Done with the requested number of tenths-of-seconds

    ret

