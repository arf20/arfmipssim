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
#include <stdio.h>

#include "types.h"
#include "idecode.h"

#include "emulator.h"

#define CHECK_ADDR(addr) \
    if (m->regs->aluout - ORG_DATA < 0 || \
        m->regs->aluout - ORG_DATA > (m->segs[SEG_DATA].size - 1)) \
    { \
        return (except_t){ EXCEPT_OUTOFBOUNDS, m->regs->aluout }; \
    } \
    if (m->regs->aluout % 4) \
        return (except_t){ EXCEPT_UNALIGNED, m->regs->aluout }; 


const char *
except_str(except_idx_t e) {
    static const char *except_strs[] = {
        "No exception.",
        "Unknown op.",
        "Out of bounds memory access.",
        "Unaligned memory access."
    };

    return except_strs[e];
}


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

except_t
machine_step(machine_t *m) {
    int ic = 0; /* instruction cycle */

    /* Cycle 0 */
    /*   Fetch */
    m->regs->ir = *(word_t*)&m->segs[SEG_TEXT].data[m->regs->pc - ORG_TEXT];
    /*   Increment PC */
    m->regs->pc += 4;
    ic++;

    /* Cycle 1 */
    /*   Read register bank speculatively */
    m->regs->a = m->regs->bank[get_rs(m->regs->ir)];
    m->regs->b = m->regs->bank[get_rt(m->regs->ir)];
    /*   Calculate jump address specilatively */
    m->regs->aluout = m->regs->pc + get_imm(m->regs->ir) << 2;
    ic++;

    /* Cycle 2 */
    switch (get_op(m->regs->ir)) {
        case OP_ALU: {
            /* Execute ALU operation */
            switch (get_func(m->regs->ir)) {
                case FUNC_AND: m->regs->aluout = m->regs->a & m->regs->b; break;
                case FUNC_OR : m->regs->aluout = m->regs->a | m->regs->b; break;
                case FUNC_ADD: m->regs->aluout = m->regs->a + m->regs->b; break;
                case FUNC_SUB: m->regs->aluout = m->regs->a - m->regs->b; break;
                case FUNC_SLT: m->regs->aluout = m->regs->a < m->regs->b; break;
            }

            /* Flags */
            int addfun = FUNC_ADD || FUNC_SUB;
            m->regs->flags.overflow = 
              !addfun && !get_msb(m->regs->a) && !get_msb(m->regs->b) &&  get_msb(m->regs->aluout) 
            | !addfun &&  get_msb(m->regs->a) &&  get_msb(m->regs->b) && !get_msb(m->regs->aluout)
            |  addfun && !get_msb(m->regs->a) &&  get_msb(m->regs->b) &&  get_msb(m->regs->aluout) 
            |  addfun &&  get_msb(m->regs->a) && !get_msb(m->regs->b) && !get_msb(m->regs->aluout);

            m->regs->flags.zero = (m->regs->aluout == 0); 
        } break;
        case OP_ORI: {
            /* Immediate OR */
            m->regs->aluout = m->regs->a | get_imm(m->regs->ir);
        } break;
        case OP_LW: {
            /* Source address */
            m->regs->aluout = (addr_t)m->regs->a + get_imm(m->regs->ir);
        } break;
        case OP_SW: {
            /* Destination address */
            m->regs->aluout = (addr_t)m->regs->b + get_imm(m->regs->ir);
        } break;
        case OP_LUI: {
            /* Write immediate */
            m->regs->bank[get_rt(m->regs->ir)] = get_imm(m->regs->ir);
        } break;
        case OP_BEQ: {
            if (m->regs->a == m->regs->b)
                /* Address calculated speculatively in Cycle 1 */
                m->regs->pc = m->regs->aluout;
        } break;
        case OP_J: {
            if (m->regs->a == m->regs->b)
                /* Address calculated speculatively in Cycle 1 */
                m->regs->pc = get_imm(m->regs->ir) << 2;
        } break;
        default: {
            return (except_t){ EXCEPT_UNKNOWN_OP, 0 };
        }
    }
    ic++;

    /* Cycle 3 */
    switch (get_op(m->regs->ir)) {
        case OP_ALU: {
            m->regs->bank[get_rd(m->regs->ir)] = m->regs->aluout;
            ic++;
        } break;
        case OP_ORI: {
            m->regs->bank[get_rt(m->regs->ir)] = m->regs->aluout;
            ic++;
        } break;
        case OP_LW: {
            CHECK_ADDR(m->regs->aluout);
            m->regs->mdr = *(word_t*)&m->segs[SEG_DATA].data[m->regs->aluout];
            ic++;
        } break;
        case OP_SW: {
            CHECK_ADDR(m->regs->aluout);
            *(word_t*)&m->segs[SEG_DATA].data[m->regs->aluout - ORG_DATA]
                = m->regs->a;
            ic++;
        } break;
    }

    /* Cycle 4 */
    switch (get_op(m->regs->ir)) {
        case OP_LW: {
            m->regs->bank[get_rt(m->regs->ir)] = m->regs->mdr;
            ic++;
        } break;
    }

    m->cycle_c += ic;
    m->inst_c++;

    return (except_t){ EXCEPT_NO, 0 };
}
