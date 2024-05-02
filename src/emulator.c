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
#include "idecode.h"

#include "emulator.h"

machine_t *
machine_new(segment_t *segs) {
    machine_t *m = malloc(sizeof(machine_t));
    m->cycle_c = 0;
    m->inst_c = 0;
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

int
addr_check(addr_t a, machine_t *m) {
    if (a < ORG_DATA || a > ORG_DATA + m->segs[SEG_DATA].size) {
        /* Segmentation fault */
        return -1;
    } else if (a % 4) {
        /* Unaligned access */
        return -1;
    }
}

void
machine_step(machine_t *m) {
    int ic = 0; /* instruction cycle */

    word_t IR;
    int32_t MDR, A, B, ALUOut;


    /* Cycle 0 */
    /*   Fetch */
    IR = *(word_t*)&m->segs[SEG_TEXT].data[m->regs->pc - ORG_TEXT];
    /*   Increment PC */
    m->regs->pc += 4;
    ic++;

    /* Cycle 1 */
    /*   Read register bank speculatively */
    A = m->regs->bank[get_rs(IR)];
    B = m->regs->bank[get_rt(IR)];
    /*   Calculate jump address specilatively */
    ALUOut = m->regs->pc + get_imm(IR) << 2;
    ic++;

    /* Cycle 2 */
    switch (get_op(IR)) {
        case OP_ALU: {
            /* Execute ALU operation */
            switch (get_func(IR)) {
                case FUNC_AND: ALUOut = A & B; break;
                case FUNC_OR : ALUOut = A | B; break;
                case FUNC_ADD: ALUOut = A + B; break;
                case FUNC_SUB: ALUOut = A - B; break;
                case FUNC_SLT: ALUOut = A < B; break;
            }

            /* Flags */
            int addfun = FUNC_ADD || FUNC_SUB;
            m->regs->flags.overflow = 
              !addfun && !get_msb(A) && !get_msb(B) &&  get_msb(ALUOut) 
            | !addfun &&  get_msb(A) &&  get_msb(B) && !get_msb(ALUOut)
            |  addfun && !get_msb(A) &&  get_msb(B) &&  get_msb(ALUOut) 
            |  addfun &&  get_msb(A) && !get_msb(B) && !get_msb(ALUOut);

            m->regs->flags.zero = ALUOut == 0; 
        } break;
        case OP_ORI: {
            /* Immediate OR */
            ALUOut = A | get_imm(IR);
        } break;
        case OP_LW: {
            /* Source address */
            ALUOut = (addr_t)A + get_imm(IR);
        } break;
        case OP_SW: {
            /* Destination address */
            ALUOut = (addr_t)B + get_imm(IR);
        } break;
        case OP_LUI: {
            /* Write immediate */
            m->regs->bank[get_rt(IR)] = get_imm(IR);
        } break;
        case OP_BEQ: {
            if (A == B)
                /* Address calculated speculatively in Cycle 1 */
                m->regs->pc = ALUOut;
        } break;
    }
    ic++;

    /* Cycle 3 */
    switch (get_op(IR)) {
        case OP_ALU: {
            m->regs->bank[get_rd(IR)] = ALUOut;
            ic++;
        } break;
        case OP_ORI: {
            m->regs->bank[get_rt(IR)] = ALUOut;
            ic++;
        } break;
        case OP_LW: {
            addr_check(ALUOut, m);
            MDR = *(word_t*)&m->segs[SEG_DATA].data[(addr_t)ALUOut];
            ic++;
        } break;
        case OP_SW: {
            addr_check(ALUOut, m);
            *(word_t*)&m->segs[SEG_DATA].data[(addr_t)ALUOut] = A;
            ic++;
        } break;
    }

    /* Cycle 4 */
    switch (get_op(IR)) {
        case OP_LW: {
            m->regs->bank[get_rt(IR)] = MDR;
            ic++;
        } break;
    }

    m->cycle_c += ic;
    m->inst_c++;
}
