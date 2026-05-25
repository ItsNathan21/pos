/**
 * @file eflags.h
 * @author Nathan Porter
 * @brief Contains macros used for names of eflags fields
 *
 */

#ifndef _EFLAGS_H_
#define _EFLAGS_H_

/** @brief Carry flag — set on unsigned arithmetic borrow/carry out */
#define EFLAGS_CF (1 << 0)

/** @brief Reserved — always reads as 1 */
#define EFLAGS_R1 (1 << 1)

/** @brief Parity flag — set if the low byte of the result has even parity */
#define EFLAGS_PF (1 << 2)

/** @brief Auxiliary carry flag — carry out of bit 3, used for BCD arithmetic */
#define EFLAGS_AF (1 << 4)

/** @brief Zero flag — set if the result of an operation is zero */
#define EFLAGS_ZF (1 << 6)

/** @brief Sign flag — set to the most significant bit of the result */
#define EFLAGS_SF (1 << 7)

/** @brief Trap flag — enables single-step debugging mode when set */
#define EFLAGS_TF (1 << 8)

/** @brief Interrupt enable flag — enables hardware IRQs when set */
#define EFLAGS_IF (1 << 9)

/** @brief Direction flag — controls string instruction direction (0=up, 1=down) */
#define EFLAGS_DF (1 << 10)

/** @brief Overflow flag — set if signed arithmetic result overflowed */
#define EFLAGS_OF (1 << 11)

/** @brief I/O privilege level — minimum CPL required to execute I/O instructions */
#define EFLAGS_IOPL (3 << 12)

/** @brief Nested task flag — indicates current task is linked to a previous one via TSS */
#define EFLAGS_NT (1 << 14)

/** @brief Resume flag — suppresses debug exceptions on the next instruction */
#define EFLAGS_RF (1 << 16)

/** @brief Virtual-8086 mode flag — enables v8086 mode when set in protected mode */
#define EFLAGS_VM (1 << 17)

/** @brief Alignment check flag — enables alignment fault checking at CPL 3 */
#define EFLAGS_AC (1 << 18)

/** @brief Virtual interrupt flag — virtual image of IF, used in v8086 mode */
#define EFLAGS_VIF (1 << 19)

/** @brief Virtual interrupt pending — indicates a virtual interrupt is pending */
#define EFLAGS_VIP (1 << 20)

/** @brief ID flag — if software can toggle this, CPUID instruction is supported */
#define EFLAGS_ID (1 << 21)

#endif /* _EFLAGS_H_ */
