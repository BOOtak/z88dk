#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ticks.h"
#include "debug.h"

static symbol  *symbols[65536] = {0};
static symbol  *symbols_byname = NULL;



static int demangle_filename(const char *input, char *filename, char *funcname, int *lineno, int *level, int *scope)
{
    *lineno = -1;
    *level = -1;
    *scope = -1;
    *funcname = 0;

    // Full packed format
    if ( sscanf(input,"%[^:]::%[^:]::%d::%d:%d",filename,funcname, level, scope, lineno) == 5 ) {
        return 0;
    }

    // Classic in function format: adv_a.c::CHKAWAY:2206
    if ( sscanf(input,"%[^:]::%[^:]:%d",filename,funcname, lineno) == 3 ) {
        return 0;
    }

    // Just a waypoint file:line
    if ( sscanf(input,"%[^:]:%d",filename,lineno) == 2 ) {
        return 0;
    }
    return -1;
}


static int symbol_compare(const void *p1, const void *p2)
{
    const symbol *s1 = p1, *s2 = p2;

    return s2->address - s1->address;
}




void read_symbol_file(char *filename)
{
    char  buf[256];
    FILE *fp = fopen(filename,"r");

    if ( fp != NULL ) {
        while ( fgets(buf, sizeof(buf), fp) != NULL ) {
            int argc;
            char **argv = parse_words(buf,&argc);

            // Ignore
            if ( argc < 10 ) {
                if ( argc >= 3 ) {
                    // We've got at least 3, do something (it's an old format)
                    symbol *sym = calloc(1,sizeof(*sym));
                    sym->name = strdup(argv[0]);
                    sym->address = strtol(!isxdigit(argv[2][0]) ? &argv[2][1] : argv[2], NULL, 16);
                    sym->symtype = SYM_ADDRESS;
                    if ( sym->address >= 0 && sym->address <= 65535 ) {
                        LL_APPEND(symbols[sym->address], sym);
                    }
                    HASH_ADD_KEYPTR(hh, symbols_byname, sym->name, strlen(sym->name), sym);
                }
                free(argv);
                continue;
            }
            if ( strncmp(argv[0],"__CDBINFO__",11) == 0 ) {
                debug_add_info_encoded(argv[0] + 11);
            } else if ( strncmp(argv[0], "__C_LINE_",9) && strncmp(argv[0], "__ASM_LINE_",11) ) {
                symbol *sym = calloc(1,sizeof(*sym));

                sym->name = strdup(argv[0]);

                {
                    char fname[FILENAME_MAX];
                    if (sscanf(argv[9], "%[^:]", fname) == 1) {
                        sym->file = strdup(fname);
                    } else {
                        sym->file = NULL;
                    }
                }

                sym->section = strdup(argv[8]); // TODO, comma
                sym->islocal = 0;
                if ( strcmp(argv[5], "local,")) {
                    sym->islocal = 1;
                }
                sym->symtype = SYM_ADDRESS;
                if ( strcmp(argv[4],"const,") == 0 ) {
                    sym->symtype = SYM_CONST;
                }
                sym->address = strtol(argv[2] + 1, NULL, 16);
                if ( sym->address >= 0 && sym->address <= 65535 ) {
                    LL_APPEND(symbols[sym->address], sym);
                }
                HASH_ADD_KEYPTR(hh, symbols_byname, sym->name, strlen(sym->name), sym);
            } else if ( argc > 9 ) {
                /* It's a cline/asmline symbol */
                char   filename[FILENAME_MAX+1];
                char   funcname[FILENAME_MAX+1];
                int    lineno;
                int    level;;
                int    scope_block;
                char  *ptr;

                demangle_filename(argv[9], filename, funcname, &lineno,&level, &scope_block);;
                debug_add_cline(filename, funcname, lineno, level, scope_block, argv[2]);
            }
            free(argv);
        }
    }
}

symbol *find_symbol_byname(const char *name)
{
    symbol *sym;

    HASH_FIND_STR(symbols_byname, name, sym);

    return sym;
}

int symbol_resolve(char *name)
{
    symbol *sym;
    char   *ptr;

    HASH_FIND_STR(symbols_byname, name, sym);
    if ( sym != NULL ) {
        return sym->address;
    }

    return -1;
}


// Find a symbol lower than where we were
symbol* symbol_find_lower(int addr, symboltype preferred_type, uint16_t* offset)
{
    symbol *sym;
    int     original_address = addr;

    while ( 1 ) {
        if ( addr < 0 ) {
            return NULL;
        }
    
        while ( (sym = symbols[addr % 65536]) == NULL && addr > 0 ) {
            addr--;
        }

        while ( sym != NULL ) {
            if ( preferred_type == SYM_ANY ) {
                break;
            }
            // Skip over internal labels?
            if ( preferred_type == sym->symtype && strncmp(sym->name,"i_",2)) {
                break;
            }
            sym = sym->next;
        }

        if ( sym != NULL ) {
            *offset = original_address - addr;
            return sym;
        } 
        addr--;
    }

    return NULL;
}

const char *find_symbol(int addr, symboltype preferred_type)
{
    symbol *sym;

    if ( addr < 0 ) {
        return NULL;
    }
    
    sym = symbols[addr % 65536];

    while ( sym != NULL ) {
        if ( preferred_type == SYM_ANY ) {
            return sym->name;
        }
        if ( preferred_type == sym->symtype ) {
            return sym->name;
        }
        sym = sym->next;
    }
    return NULL;
}

char **parse_words(char *line, int *argc)
{
    int                 i = 0, j = 0 , n = 0;
    int                 len = strlen(line);
    int                 in_single_quotes = 0, in_double_quotes = 0;
    char              **args;

    while ( isspace(line[i] ) ) {
        i++;
    }

    for ( ; i <= len; i++) {
        switch (line[i]) {
        case '"':
            if (in_single_quotes) {
                line[j++] = line[i];
                break;
            }
            in_double_quotes = !in_double_quotes;
            break;
        case '\'':
            if (in_double_quotes) {
                line[j++] = line[i];
                break;
            }
            in_single_quotes = !in_single_quotes;
            break;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        case 0:
            if (!in_single_quotes && !in_double_quotes) {
                line[j++] = 0;
                n++;
                i++;
                /* Try to find the start of the next word */
                while (i <= len && (line[i] == 0 || isspace(line[i])) ) {
                    i++;
                }
                i--;
                break;
            }
            line[j++] = line[i];
            break;
        case '\\':
            switch (line[i + 1]) {
            case '"':
            case '\'':
            case '\\':
                line[j++] = line[i + 1];
                break;
            case ' ':
                if (in_single_quotes || in_double_quotes) {
                    line[j++] = line[i];
                }
                line[j++] = line[i + 1];
                break;
            default:
                line[j++] = line[i];
                line[j++] = line[i + 1];
                break;
            }
            i++;
            break;
        default:
            line[j++] = line[i];
            break;
        }
    }

    args = (char **)malloc(sizeof(char *) * (n + 1) );
    n = 0;
    args[n++] = line;
    j--;

    for (i = 0; i < j; i++) {
        if (line[i] == 0) {
            args[n++] = line + i + 1;
        }
    }

    *argc = n;

    return args;
}
