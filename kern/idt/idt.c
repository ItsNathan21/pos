/**
 * @file idt.c
 * @author Nathan Porter
 * @brief Implements the IDT interface functions
 * to use for installing and modifying contents in
 * the IDT.
 *
 */

#include <panic.h>
#include <stddef.h>

#include "idt.h"

/**
 * @brief the limit is always number of bytes the idt
 * contains, minus 1 for indexing.
 *
 * If an exception is mentioned outside of this limit then
 * a GP fault is triggered (try not to do this)
 */
#define IDT_LIMIT ((IDT_ENTRIES * 8) - 1)

/**
 * @brief Stores the contents of the described IDT
 * into the appropriate IDTR.
 *
 * @param idt the new idt to store
 */
void store_idt(x86_IDT *idt)
{
    assert(idt != NULL);

    struct idtr idtr;
    idtr.base = (uint32_t)idt;
    idtr.limit = IDT_LIMIT;
    /* do the actual storing into the idt register */
    __store_idtr(&idtr);
}

x86_IDT *idt_base(void)
{
    struct idtr idtr;
    __get_idtr(&idtr);

    /*
        Nobody should have touched what we're
        doing in between stores and loads...
    */
    assert(idtr.limit == IDT_LIMIT);

    return (x86_IDT *)idtr.base;
}
