/**
 * @file pit_timer.h
 * @author your name (you@domain.com)
 * @brief Contains definitions used for
 * initializing and using the PIT within the
 * IA32.
 *
 * @note yes, this file has the word "timer"
 * in it twice, I wanted the second one so I'm not
 * like "what is a pit? oh, timer."
 *
 */

#ifndef _PIT_TIMER_H_
#define _PIT_TIMER_H_

/** @brief the rate of the timer, in Hz */
#define TIMER_RATE 1193182

/** @brief the port number to program the timer's period in */
#define TIMER_PERIOD_IO_PORT 0x40
/** @brief the port to program the timer's mode in  */
#define TIMER_MODE_IO_PORT 0x43

#define TIMER_SQUARE_WAVE 0x36

/**
 * @brief Time period presets (TIMER_RATE is the input clock frequency).
 * Adjust the static constant timer_cycles to change the timer frequency.
 */
#define TIMER_10MS (TIMER_RATE / 100)  /** ~10ms */
#define TIMER_5MS (TIMER_RATE / 200)   /** ~5ms */
#define TIMER_2_5MS (TIMER_RATE / 400) /** ~2.5ms */
#define TIMER_2MS (TIMER_RATE / 500)   /** ~2ms */

#endif /* _PIT_TIMER_H_  */