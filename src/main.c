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

    main.c: Program entry point

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "types.h"
#include "loader.h"

void
usage(const char *name) {
    fprintf(stderr, "Usage: %s [options] basename\nOptions\n"
    "  -g\tGraphical visualization\n", name);
}

char *
read_whole_file(FILE *f, size_t *sz) {
    fseek(f, 0, SEEK_END);
    *sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buff = malloc(*sz);
    fread(buff, *sz, 1, f);

    fclose(f);

    return buff;
}

int
main(int argc, char **argv) {
    if (argc < 2) {
        usage(*argv);
        return 1;
    }

    /* Command line options */
    int graphic = 0;
    const char *in = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            /* Argument */
            switch (argv[i][1]) {
                case 'g': graphic = 1; break;
            }
        } else {
            if (in == NULL) in = argv[i];
            else {
                usage(*argv);
                return 1;
            }
        }
    }

    if (!in) {
        usage(*argv);
        return 1;
    }

    char fnbuff[256];

    /* Load input */
    symbol_table_t symbols;
    segment_t segments[2];

    strncpy(fnbuff, in, 256);
    strncat(fnbuff, ".data", 255);
    FILE *dataf = fopen(fnbuff, "r");
    if (!dataf) {
        fprintf(stderr, "Cannot open data segment %s: %s\n", fnbuff,
            strerror(errno));
        return 1;
    }
    segments[SEG_DATA].data = read_whole_file(dataf, &segments[SEG_DATA].size);

    strncpy(fnbuff, in, 256);
    strncat(fnbuff, ".text", 255);
    FILE *textf = fopen(fnbuff, "r");
    if (!dataf) {
        fprintf(stderr, "Cannot open text segment %s: %s\n", fnbuff,
            strerror(errno));
        return 1;
    }
    segments[SEG_TEXT].data = read_whole_file(textf, &segments[SEG_TEXT].size);

    printf("Loaded %s with %dB of .data and %dB of .text\n", in,
        segments[SEG_DATA].size, segments[SEG_TEXT].size);

    strncpy(fnbuff, in, 256);
    strncat(fnbuff, ".sym", 255);
    FILE *symf = fopen(fnbuff, "r");
    size_t symfs;
    if (symf) {
        char *symfc = read_whole_file(symf, &symfs);
        load_symbols(symfc, symfs, &symbols);
        free(symfc);
    }

    printf("Loaded %d symbols\n", symbols.size);
}
