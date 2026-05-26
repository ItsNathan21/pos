/**
 * @file bounded_queue_internals.h
 * @author your name (you@domain.com)
 * @brief Contains internal macros and definitions used
 * for the bounded queue implementation. These are seperate
 * so they don't interfere with the data structures API,
 * but they are visible to see for clarity if you really want to.
 */

#define __BQUEUE_ADVANCE_IDX(BQ, IDX) \
    (((IDX) + (BQ)->element_size) % ((BQ)->num_elements * (BQ)->element_size))

/**
 * @def __BQUEUE_NEXT_HEAD(BQ)
 *
 * internal macro to get the value for the next head of the bqueue,
 *
 */
#define __BQUEUE_NEXT_HEAD(BQ) \
    __BQUEUE_ADVANCE_IDX(BQ, (BQ)->head)
