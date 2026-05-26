/**
 * @file bounded_queue.h
 * @author your name (you@domain.com)
 * @brief Contains the macros used to implement bounded queues
 * as a ringbuffer.
 *
 * Macros are used here because in a normal "function" setting,
 * the user would have to declare their own buffer of storage,
 * or have it be dynamically allocated, which is not an option
 * for certain use of bounded queues (e.g, keyboard scancode queue
 * should come pre-allocated). Because of this, I employed macros
 * to do the work of declaration,
 */

#ifndef _BOUNDED_QUEUE_H_
#define _BOUNDED_QUEUE_H_

#include "bounded_queue_internals.h"

#include <stddef.h>
#include <string.h>

/**
 * @def BQUEUE_NEW(RESULT_TYPE_NAME, ELEMENT_TYPE, NUM_ELEMENTS)
 *
 * gives a new type with name RESULT_TYPE_NAME which represents
 * a bounded queue with elements of type ELEMENT_TYPE and NUM_ELEMENTS
 * elements.
 *
 */
#define BQUEUE_NEW(RESULT_TYPE_NAME, ELEMENT_TYPE, NUM_ELEMENTS)       \
    typedef struct                                                     \
    {                                                                  \
        size_t element_size, num_elements, head, tail;                 \
        uint8_t element_buffer[sizeof(ELEMENT_TYPE) * (NUM_ELEMENTS)]; \
    } RESULT_TYPE_NAME;

/**
 * @def BQUEUE_INITIALIZE(RESULT_VALUE_NAME, BQUEUE_TYPE, ELEMENT_TYPE, NUM_ELEMENTS)
 *
 * Initializes the bounded queue value necessary before use. The RESULT_VALUE_NAME is the name
 * of the value which has type BQUEUE_TYPE, initialized with the remaining fields. Undefined
 * behavior is encountered if ELEMENT_TYPE and NUM_ELEMENTS does not match what was in the
 * declaration.
 *
 * @note This is done seperate from the declaration, which brings up issues with the
 * type and number of elements not matching in between, being a safety concern. This is
 * mostly because initializing the element and creating the bqueue is possible within one
 * macro, but then this eliminates the use of creating many values of the same
 * bqueue type (since each declaration comes with an instantiation).
 *
 * Instead, blame is put on the user if they somehow don't manage to keep type and
 * size consistent between these two macros, which are higihly likely to be used
 * immediately next to eachother.
 *
 */
#define BQUEUE_INITIALIZE(RESULT_VALUE_NAME, BQUEUE_TYPE, ELEMENT_TYPE, NUM_ELEMENTS) \
    BQUEUE_TYPE RESULT_VALUE_NAME =                                                   \
        {                                                                             \
            .element_size = sizeof(ELEMENT_TYPE),                                     \
            .num_elements = (NUM_ELEMENTS),                                           \
            .head = 0,                                                                \
            .tail = 0,                                                                \
    }

/**
 * @def BQUEUE_NEW_AND_INITIALIZE(RESULT_VALUE_NAME, RESULT_TYPE_NAME, ELEMENT_TYPE, NUM_ELEMENTS)
 *
 * To make up for discrepancies between BQUEUE_NEW and BQUEUE_INITIALIZE, this is created
 * to do both at once, with no possibility of misinputting size or type. read the according
 * other macros to learn what they do specifically.
 *
 */
#define BQUEUE_NEW_AND_INITIALIZE(RESULT_VALUE_NAME, RESULT_TYPE_NAME, ELEMENT_TYPE, NUM_ELEMENTS) \
    BQUEUE_NEW(RESULT_TYPE_NAME, ELEMENT_TYPE, NUM_ELEMENTS);                                      \
    BQUEUE_INITIALIZE(RESULT_VALUE_NAME, RESULT_TYPE_NAME, ELEMENT_TYPE, NUM_ELEMENTS);

/**
 * @def BQUEUE_FULL(BQ)
 *
 * Expression which computes to whether or not the queue is full
 *
 */
#define BQUEUE_FULL(BQ) \
    (__BQUEUE_NEXT_HEAD(BQ) == (BQ)->tail)

/**
 * @def BQUEUE_EMPTY(BQ)
 * Expression which computes whether or not the queue is empty
 */
#define BQUEUE_EMPTY(BQ) \
    ((BQ)->head == (BQ)->tail)

/**
 * @def BQUEUE_ENQUEUE(BQ, ELEMENT, ENQUEUE_SUCCESSFUL)
 *
 * Enqueues ELEMENT onto the queue BQ.
 *
 * ENQUEUE_SUCCESSFUL must be an int * which will be set to
 * 0 on successful enqueue, or -1 on failure (the queue is full).
 *
 * ELEMENT must be a pointer to the desired element to be enqueued.
 *
 */
#define BQUEUE_ENQUEUE(BQ, ELEMENT, ENQUEUE_SUCCESSFUL)                 \
    do                                                                  \
    {                                                                   \
        if (!BQUEUE_FULL(BQ))                                           \
        {                                                               \
            char *byte_buf = (char *)(BQ)->element_buffer + (BQ)->head; \
            memcpy(byte_buf, ELEMENT, (BQ)->element_size);              \
            (BQ)->head = __BQUEUE_NEXT_HEAD(BQ);                        \
            *ENQUEUE_SUCCESSFUL = 0;                                    \
        }                                                               \
        else                                                            \
            *ENQUEUE_SUCCESSFUL = -1;                                   \
    } while (0)

/**
 * @def BQUEUE_ENQUEUE_ONE_LEFT(BQ, ELEMENT, ENQUEUE_SUCCESSFUL)
 *
 * Same exact logic as a normal enqueue, but leaves space for
 * a single element at the end of the bounded queue, and does not
 * enqueue over it.
 *
 */
#define BQUEUE_ENQUEUE_ONE_LEFT(BQ, ELEMENT, ENQUEUE_SUCCESSFUL)            \
    do                                                                      \
    {                                                                       \
        if (__BQUEUE_ADVANCE_IDX(BQ, __BQUEUE_NEXT_HEAD(BQ)) != (BQ)->tail) \
        {                                                                   \
            char *byte_buf = (char *)(BQ)->element_buffer + (BQ)->head;     \
            memcpy(byte_buf, ELEMENT, (BQ)->element_size);                  \
            (BQ)->head = __BQUEUE_NEXT_HEAD(BQ);                            \
            *ENQUEUE_SUCCESSFUL = 0;                                        \
        }                                                                   \
        else                                                                \
            *ENQUEUE_SUCCESSFUL = -1;                                       \
    } while (0)

/**
 * @def BQUEUE_DEQUEUE(BQ, RESULT_PTR, DEQUEUE_SUCCESSFUL)
 *
 * Dequeues an element from the queue, and stores it into RESULT_PTR
 * if non-NULL. Result will be ignored if NULL, though element will
 * still be de-queued.
 *
 * DEQUEUE_SUCCESSFUL must be an int * which will be set to 0 if the
 * result placed in RESULT_PTR is a valid dequeued result, or -1 otherwise.
 *
 */
#define BQUEUE_DEQUEUE(BQ, RESULT_PTR, DEQUEUE_SUCCESSFUL)                                              \
    do                                                                                                  \
    {                                                                                                   \
        if (!BQUEUE_EMPTY(BQ))                                                                          \
        {                                                                                               \
            char *byte_buf = (char *)(BQ)->element_buffer + (BQ)->tail;                                 \
            if (RESULT_PTR != NULL)                                                                     \
                memcpy(RESULT_PTR, byte_buf, (BQ)->element_size);                                       \
            (BQ)->tail = ((BQ)->tail + (BQ)->element_size) % ((BQ)->num_elements * (BQ)->element_size); \
            *DEQUEUE_SUCCESSFUL = 0;                                                                    \
        }                                                                                               \
        else                                                                                            \
            *DEQUEUE_SUCCESSFUL = -1;                                                                   \
    } while (0)

/**
 * @def BQUEUE_CPY_FROM_QUEUE(BQ, DEST, NUM_ELEMS, COPIED_OUT)
 *
 * Copies at most NUM_ELEMS elements from the tail of BQ into DEST,
 * dequeueing each element as it is copied. Subsequent calls to
 * BQUEUE_DEQUEUE or BQUEUE_CPY_FROM_QUEUE will not see data that
 * was already copied out.
 *
 * DEST must point to a buffer of at least
 * sizeof(element_type) * NUM_ELEMS bytes.
 *
 * COPIED_OUT must be an int * and will be set to the number of
 * elements actually copied, which may be less than NUM_ELEMS if
 * the queue is exhausted first.
 *
 */
#define BQUEUE_CPY_FROM_QUEUE(BQ, DEST, NUM_ELEMS, COPIED_OUT)              \
    do                                                                      \
    {                                                                       \
        *(COPIED_OUT) = 0;                                                  \
        if ((NUM_ELEMS) > 0)                                                \
        {                                                                   \
            while (!BQUEUE_EMPTY(BQ) && *(COPIED_OUT) < (NUM_ELEMS))        \
            {                                                               \
                char *_bq_src = (char *)(BQ)->element_buffer + (BQ)->tail;  \
                char *_bq_dest = (char *)(DEST) +                           \
                                 (*(COPIED_OUT) * (int)(BQ)->element_size); \
                memcpy(_bq_dest, _bq_src, (BQ)->element_size);              \
                (BQ)->tail = ((BQ)->tail + (BQ)->element_size) %            \
                             ((BQ)->num_elements * (BQ)->element_size);     \
                (*(COPIED_OUT))++;                                          \
            }                                                               \
        }                                                                   \
    } while (0)

#endif /* _BOUNDED_QUEUE_H_ */
