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

    idecode.c: Instruction decoding helpers

*/

#include "idecode.h"

op_t
get_op(word_t i) {
    return (i & 0b11111100000000000000000000000000) >> 26;
}

uint8_t
get_rs(word_t i) {
    return (i & 0b00000011111000000000000000000000) >> 21;
}

uint8_t
get_rt(word_t i) {
    return (i & 0b00000000000111110000000000000000) >> 16;
}

uint8_t
get_rd(word_t i) {
    return (i & 0b00000000000000001111100000000000) >> 11;
}

uint8_t
get_shamt(word_t i) {
    return (i & 0b00000000000000000000011111000000) >> 6;
}

func_t
get_func(word_t i) {
    return (i & 0b00000000000000000000000000111111) >> 0;
}

int16_t
get_imm(word_t i) {
    return (i & 0b00000000000000001111111111111111) >> 0;
}

uint32_t
get_j(word_t i) {
    return (i & 0b00000011111111111111111111111111) >> 0;
}

int
get_msb(word_t w) {
    return (w & 0b10000000000000000000000000000000) >> 31;
}

const char *
op_str(op_t op) {
    switch (op) {
        case OP_ALU: return "";
        case OP_ORI: return "ori ";
        case OP_LW: return "lw ";
        case OP_SW: return "sw ";
        case OP_LUI: return "lui ";
        case OP_BEQ: return "beq ";
        case OP_J: return "j ";
        default: return "Unknown op.";
    }
}

const char *
func_str(func_t func) {
    switch (func) {
        case FUNC_AND: return "and";
        case FUNC_OR: return "or";
        case FUNC_ADD: return "add";
        case FUNC_SUB: return "sub";
        case FUNC_SLT: return "slt";
        default: return "Unknown func.";
    }
}

const char *
reg_str(uint8_t r) {
    static const char *reg_strs[] = {
        "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
        "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
        "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
        "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
    };
    if (r >= 0 && r <= 31)
        return reg_strs[r];
    else return "Unknown register.";
}

