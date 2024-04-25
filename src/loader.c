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

    loader.c: Load files

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#include "loader.h"


symbol_table_t *
symbol_table_new() {
    symbol_table_t *st = malloc(sizeof(symbol_table_t));
    st->table = malloc(SYMBOL_TABLE_INIT_SIZE * sizeof(symbol_t));
    st->size = 0;
    st->capacity = SYMBOL_TABLE_INIT_SIZE;
    return st;
}

void
symbol_table_destroy(symbol_table_t *st) {
    for (int i = 0; i < st->size; i++)
        free(st->table[i].label);
    free(st->table);
    st->capacity = 0;
    st->size = 0;
    free(st);
}

void
symbol_table_push(symbol_table_t *st, symbol_t sym) {
    /* Grow table by double */
    if (st->size == st->capacity) {
        st->table = realloc(st->table, 2*st->capacity);
        st->capacity *= 2;
    }
    /* Insert at end */
    st->table[st->size++] = sym;
}


void
load_symbols(const char *str, size_t len, symbol_table_t *st) {
    symbol_t s;
    const char *ptr = str;
    while (ptr && *ptr && ptr - str < len) {
        const char *colon = strchr(ptr, ':');
        if (!colon) break;
        size_t labellen = colon - ptr;
        s.label = malloc(labellen + 1);
        strncpy(s.label, ptr, labellen);
        ptr = colon + 1;
        s.address = strtol(ptr, NULL, 16);
        ptr = strchr(ptr, '\n');

        symbol_table_push(st, s);
    }
}
