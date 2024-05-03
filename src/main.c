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
#include "emulator.h"
#include "idecode.h"

void
usage(const char *name) {
    fprintf(stderr, "Usage: %s [options] basename\nOptions\n"
    "  -g\tGraphical visualization\n", name);
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
    segment_t segments[2];

    strncpy(fnbuff, in, 256);
    strncat(fnbuff, ".data", 255);
    FILE *dataf = fopen(fnbuff, "r");
    if (!dataf) {
        fprintf(stderr, "Cannot open data segment %s: %s\n", fnbuff,
            strerror(errno));
        return 1;
    }

    strncpy(fnbuff, in, 256);
    strncat(fnbuff, ".text", 255);
    FILE *textf = fopen(fnbuff, "r");
    if (!dataf) {
        fprintf(stderr, "Cannot open text segment %s: %s\n", fnbuff,
            strerror(errno));
        return 1;
    }

    load_segments(dataf, textf, segments);

    printf("Loaded %s with %dB of .data and %dB of .text\n", in,
        segments[SEG_DATA].size, segments[SEG_TEXT].size);

    symbol_table_t *symbols = NULL;
    strncpy(fnbuff, in, 256);
    strncat(fnbuff, ".sym", 255);
    FILE *symf = fopen(fnbuff, "r");
    if (symf) {
        symbols = symbol_table_new();
        load_symbols(symf, symbols);
    }

    printf("Loaded %d symbols\n", symbols->size);

    machine_t *m = machine_new(segments);

    while (m->regs->pc - ORG_TEXT < segments[SEG_TEXT].size) {
        except_t e = machine_step(m);

        printf("Executing 0x%.8x at 0x%.8x: ", m->regs->ir, m->regs->pc - 4);

        printf("%s", op_str(get_op(m->regs->ir)));
        switch (get_op(m->regs->ir)) {
            case OP_ALU: {
                printf("%s ", func_str(get_func(m->regs->ir)));
                printf("$%s [%d], $%s [%d], $%s [%d]",
                    reg_str(get_rd(m->regs->ir)), m->regs->aluout,
                    reg_str(get_rs(m->regs->ir)), m->regs->a,
                    reg_str(get_rt(m->regs->ir)), m->regs->b);
            } break;
            case OP_ORI: {
                printf("$%s [%d], $%s [%d], %d|0x%.4x",
                    reg_str(get_rt(m->regs->ir)), m->regs->b,
                    reg_str(get_rs(m->regs->ir)), m->regs->a,
                    get_imm(m->regs->ir), get_imm(m->regs->ir));
            } break;
            case OP_LW: {
                printf("$%s [%d], %d($%s [%d])",
                    reg_str(get_rt(m->regs->ir)), m->regs->b,
                    get_imm(m->regs->ir),
                    reg_str(get_rs(m->regs->ir)), m->regs->a);
            } break;
            case OP_SW: {
                printf("$%s [%d], %d($%s [%d])",
                    reg_str(get_rs(m->regs->ir)), m->regs->a,
                    get_imm(m->regs->ir),
                    reg_str(get_rt(m->regs->ir)), m->regs->b);
            } break;
        }
        
        printf("\n");

        if (e.e != EXCEPT_NO) {
            printf("Exception at 0x%.8x executing 0x%.8x: %s reason: %d\n",
                m->regs->pc - 4, m->regs->ir, except_str(e.e), e.why);
        }
    }

    /* Deinit */
    machine_destroy(m);
    free(segments[SEG_DATA].data);
    free(segments[SEG_TEXT].data);
    if (symf)
        symbol_table_destroy(symbols);

    return 0;
}
