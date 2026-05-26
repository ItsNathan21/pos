/**
 * @file keyboard.c
 * @author your name (you@domain.com)
 * @brief Contains the PS/2 functions dealing
 * with keyboard interrupts, and the implementations
 * of the keyboard interface.
 *
 */

#include "keyboard.h"
#include "scancodes.h"

#include <pic.h>
#include <bounded_queue.h>
#include <panic.h>
#include <console.h>
#include <x86_asm.h>

#define SCANCODE_QUEUE_LENGTH (256)

BQUEUE_NEW_AND_INITIALIZE(scancode_queue, scancode_queue_t, int, SCANCODE_QUEUE_LENGTH);

/** @brief if the user is typing more than an entire screen of characters
 * before a newline, it might be time to reconsider that filename...
 */
#define KEYBOARD_QUEUE_LENGTH (CONSOLE_WIDTH * CONSOLE_HEIGHT)

BQUEUE_NEW_AND_INITIALIZE(keyboard_queue, keyboard_queue_t, char, KEYBOARD_QUEUE_LENGTH);

/**
 * @brief indicator to let readchar know
 * that readline is calling it, so we know
 * whether to pop off of the linebuffer queue
 * or read new scancodes
 */
static int readline_active = 0;

/**
 * @brief interrupt wrapper for the keyboard, this has been
 *
 *
 */
void keyboard_wrapper(void)
{
    int scancode, success;
    scancode = inb(PS2_KBD_PORT);

    BQUEUE_ENQUEUE(&scancode_queue, &scancode, &success);

    pic_send_eoi(IRQ_KEYBOARD);
}

/**
 * @brief Returns a byte pressed by the keyboard.
 *
 * @return char
 */
char keyboard_getbyte(void)
{
    /*
        If there are some old characters from an
        overflowed line buffer, we should
        prioritize those first, since they were
        typed first.
    */
    if (!readline_active && !BQUEUE_EMPTY(&keyboard_queue))
    {
        char char_read;
        int dequeue_successful;

        BQUEUE_DEQUEUE(&keyboard_queue, &char_read, &dequeue_successful);

        if (dequeue_successful < 0)
            return -1;

        return (int)char_read;
    }
    /*
        Since its possible the keyboard is pressed
        during the dequeuing process, we don't want
        any conflicting information
        about the contents of the buffer, so we
        should disable them temporarily to get an
        accurate reading.
    */
    while (!BQUEUE_EMPTY(&scancode_queue))
    {
        /*
            We might need to pop a few scancodes off
            of the queue to get a single char, hence
            the loop. This loop will always terminate
            though, as the scancode queue is bounded,
            and will either run out of scancodes,
            or a char is finally reached with a sequence
            of scancodes.
        */
        disable_interrupts();

        int scancode, dequeue_successful;
        BQUEUE_DEQUEUE(&scancode_queue, &scancode, &dequeue_successful);

        enable_interrupts();

        if (dequeue_successful < 0)
            return -1;

        key_event_t key_event = scancode_process(scancode);

        if (KEY_EVENT_IS_MAKE(key_event) && KEY_EVENT_HAS_CHAR(key_event))
            return KEY_EVENT_CHAR(key_event);
    }

    return -1;
}

int keyboard_readline(char *buf, int len)
{
    if (len <= 0 || len > KEYBOARD_QUEUE_LENGTH)
        return -1;

    readline_active = 1;

    /*
        If we still have leftover
        data from the previous line read,
        we should priotize it just like readchar.
        In this case, we know the rest of the line
        is stored there (since we did not store into the
        queue until one was encountered).
    */
    if (!BQUEUE_EMPTY(&keyboard_queue))
    {
        readline_active = 0;
        int num_copied;
        BQUEUE_CPY_FROM_QUEUE(&keyboard_queue, buf, len, &num_copied);
        return num_copied;
    }

    char byte_read;

    while ((byte_read = (char)keyboard_getbyte()) != '\n')
    {
        if (byte_read < 0)
            continue;
        /*
            Delete from the front of the
            queue in case of backspaces, letting
            the user not reach the buffers capacity
            in case they make frequent typing misaeks
        */
        if (byte_read == '\b')
        {
            if (!BQUEUE_EMPTY(&keyboard_queue))
            {
                int dequeue_successful;

                BQUEUE_DEQUEUE(&keyboard_queue, NULL, &dequeue_successful);

                assert(dequeue_successful >= 0);

                /*
                    Echo the typing feedback back to the user
                */
                console_putc(byte_read);
            }
            continue;
        }

        /*
            Leaving room for the newline at the end,
            we enqueue the character we recieved.
            If we've run out of buffer space before hitting
            a newline, thats okay, just don't give the
            user feedback, and don't overflow
            any old characters. Wait for the newline.
        */
        int enqueue_successful;

        BQUEUE_ENQUEUE_ONE_LEFT(&keyboard_queue, &byte_read, &enqueue_successful);

        if (enqueue_successful == 0)
            console_putc(byte_read);
    }

    /*
        Newline recieved, enqueue it (safe since we left
        a hole in the while loop), and place it on the screen
        for user feedback.
    */

    int enqueue_successful;

    BQUEUE_ENQUEUE(&keyboard_queue, &byte_read, &enqueue_successful);

    affirm(enqueue_successful == 0);

    console_putc(byte_read);

    readline_active = 0;

    int num_copied_out;

    BQUEUE_CPY_FROM_QUEUE(&keyboard_queue, buf, len, &num_copied_out);

    return num_copied_out;
}
