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

#ifndef _TYPES_H
#define _TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef uint32_t addr_t;
typedef uint32_t word_t;
typedef uint8_t reg_t;

typedef struct {
    addr_t address;
    char *label;
} symbol_t;

typedef struct {
    symbol_t *table;
    size_t size;
    size_t capacity;
} symbol_table_t;

#endif /* _TYPES_H */
