/*

    arfmipssim: Simulator for UM ETC base MIPS-based CPU
    Copyright (C) 2023 arf20 (Ángel Ruiz Fernandez)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef _EMULATOR_H
#define _EMULATOR_H

#include "types.h"

#define ORG_DATA    0x10010000
#define ORG_TEXT    0x00400000

typedef struct {
    union {
        word_t bank[32]; /* index bank access */
        struct {
            word_t /* register names */
              zero, at, v0, v1, a0, a1, a2, a3,
                t0, t1, t2, t3, t4, t5, t6, t7,
                s0, s1, s2, s3, s4, s5, s6, s7,
                t8, t9, k0, k1, gp, sp, fp, ra;
        };
    };
    addr_t pc;  /* Program Counter is a special separate register in the CPU */
    word_t ir,  /* Instruction Register */
        a, b,   /* Values from register bank */
        mdr,    /* Memory Data Register, read from memory */
        aluout; /* ALU accumulator */
    struct {
        int overflow, zero; /* ALU flags (overflow unused) */
    } flags;
} registers_t;

/* This represents de state of the CPU */
typedef struct {
    segment_t *segs;
    registers_t *regs;
    uint64_t cycle_c; /* cycle counter */
    uint64_t inst_c;  /* instruction counter */
} machine_t;

typedef enum { EXCEPT_UNKNOWN_OP } except_t;

machine_t *machine_new(segment_t *segs);
void machine_destroy(machine_t *m);
except_t machine_step(machine_t *m);


#endif /* _EMULATOR_H */
