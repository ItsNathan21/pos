/**
 * @file base_page_map.c
 * @author Nathan Porter
 * @brief Contains the implementation of the 32-bit PAE
 * page table map.
 *
 * The map is the barebones mapping required to get the processor
 * from 32 bit mode into 64 bit long mode, and consists of this:
 *
 * The kernel code and data itself is identity mapped with the
 * 2MB page entries, then, for later use, the "upper half" is
 * mapped as
 *
 * Physical Address + UPPER_HALF_OFFSET = Virtual Address, where
 *
 * These upper pages are used because the linking for the 64-bit
 * code is used with these virtual addresses, and to make room
 * for the standard user mode code which lives in the lower half
 * of the 64 bit address space. These upper half pages also use the
 * 2MB page entries.
 *
 * Reference for work: Intel 64 and IA-32 Architectures Software Developer's Manual
 *
 *
 */
