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

    emulator.c: Instruction decoding

*/

#include "types.h"

typedef enum {
    OP_ALU = 0b000000,
    OP_ORI = 0b001101,
    OP_LW  = 0b100011,
    OP_SW  = 0b101011,
    OP_LUI = 0b001111,
    OP_BEQ = 0b000100,
    OP_J   = 0b000010
} op_t;

typedef enum {
    FUNC_AND = 0b100100,
    FUNC_OR  = 0b100101,
    FUNC_ADD = 0b100000,
    FUNC_SUB = 0b100010,
    FUNC_SLT = 0b101010
} func_t;

op_t get_op(word_t i);
uint8_t get_rs(word_t i);
uint8_t get_rt(word_t i);
uint8_t get_rd(word_t i);
uint8_t get_shamt(word_t i);
func_t get_func(word_t i);
int16_t get_imm(word_t i);
uint32_t get_j(word_t i);

int get_msb(word_t w);

/*
    <                struct                 >       ^
          <        struct            >      ^
                    <    struct    > ^
    op    rs   rt   rd   shamtfunc
    ######*****#####*****#####******  union          
                    imm                      union   union
                    **************** v
          j
          **************************        v       
    instruction
    ################################                v

    is this cursed? - probably
*/
