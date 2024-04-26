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
    return (i & 0b11111100000000000000000000000000) >> 27;
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
