/**
 * @file timer.h
 * @author Nathan Porter
 * @brief defines the function to install the timer tickback
 * function internally with the timer
 *
 */

#ifndef _TIMER_H_
#define _TIMER_H_

void timer_install_tickback(void (*tickback)(void));

#endif /* _TIMER_H_ */
