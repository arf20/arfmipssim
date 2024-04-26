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

    word_t IR;
    word_t MDR, A, B, ALUOut;


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
    }
    ic++;

    /* Cycle 3 */
    switch (get_op(IR)) {
        case OP_ALU: {
            m->regs->bank[get_rd(IR)] = ALUOut;
            ic++;
        } break;
    }

    m->cycle_c += ic;
    m->inst_c++;
}
