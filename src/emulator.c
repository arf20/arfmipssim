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

    emulator.c: Instruction interpreter

*/

#include <stdlib.h>
#include <string.h>

#include "types.h"

#include "emulator.h"

typedef struct {
    union {
        word_t instruction;
        struct {
            /* All */
            union {
                struct {
                    union {
                        struct {
                            /* R */
                            uint8_t func : 6;
                            uint8_t shamt : 5;
                            uint8_t rd : 5;                            
                        };
                        struct {
                            /* I */
                            int16_t imm;
                        };
                    };
                    /* R or I */
                    uint8_t rt : 5;
                    uint8_t rs : 5;
                };
                /* J */
                uint32_t j : 26;
            };
            uint8_t op : 6;
        };
    };
} instruction_t;

machine_t *
machine_new(segment_t *segs) {
    machine_t *m = malloc(sizeof(machine_t));
    m->cycle_c = 0;
    /* Alloc and init register bank */
    m->regs = malloc(sizeof(registers_t));
    memset(m->regs, 0, sizeof(registers_t));
    m->regs->pc = ORG_TEXT;
    /* Attach segments */
    m->segs = segs;
    
    return m;
}

void
machine_destroy(machine_t *m) {
    free(m->regs);
    free(m);
}

void
machine_step(machine_t *m) {
    int ic = 0; /* instruction cycle */

    instruction_t IR;
    word_t MDR, A, B, ALUOut;


    /* Cycle 0 */
    /*   Fetch */
    IR.instruction = *(word_t*)&m->segs[SEG_TEXT].data[m->regs->pc - ORG_TEXT];
    /*   Increment PC */
    m->regs->pc += 4;

    ic++;

    /* Cycle 1 */
    /*   Read register bank speculatively */
    A = m->regs->bank[IR.rs];
    B = m->regs->bank[IR.rt];

}