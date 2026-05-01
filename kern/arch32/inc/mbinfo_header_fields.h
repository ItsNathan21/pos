/**
 * @file mbinfo_header_fields.h
 * @author Nathan Porter
 * @brief Contains definitions ONLY for the bit fields within
 * the GRUB multiboot header structure, defined here
 * https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state
 *
 * This file can be included in assembly files (.S), and thus
 * should not contain anything beyond macros
 *
 */
#ifndef _MBINFO_HEADER_FIELDS_H_
#define _MBINFO_HEADER_FIELDS_H_

#define ALIGN_4K (1 << 0)
#define GET_MEM (1 << 1)
#define VIDEO_MODE_TABLE (1 << 2)
#define EXTRA_FIELDS (1 << 16)

#endif /* _MBINFO_HEADER_FIELDS_H_ */
