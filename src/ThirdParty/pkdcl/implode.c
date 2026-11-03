// UNIT-FLAGS: -O2 -G6 -MD
/* implode: encoder for the PKWARE Data Compression Library stream format (pkdcl.h). Written for this project; not
 * PKWARE's code and not byte-matched (library code: no FUNCTION markers, no matching credit). Its output is a valid
 * DCL stream that the original explode decodes, but it is not byte-identical to the original implode's.
 * Kept from the original DCL 1.11: the result codes (the dictionary size is checked before the literal mode), the
 * stream header, a 2-byte copy only within 256 bytes, a copy within the dictionary size, the end code after the
 * data, output handed to write_buf in 0x800-byte pieces with the rest at the end.
 * Match search: hash chains over byte pairs in a window of 0x1000 bytes of history plus 0x1000 bytes of new input
 * (plus room for the longest copy), longest match first, the nearest of equal length; one step of lazy matching. */
#include <string.h>
#include "pkdcl.h"

#define HISTORY 0x1000                      /* history kept for copies (the largest dictionary) */
#define WINDOW (HISTORY + 0x1000 + PK_MAX_COPY)
#define HASH_SIZE 0x800
#define NO_POS 0xffff
#define MAX_CHAIN 0x1000                    /* candidates tried per position (chains stop at the dictionary anyway) */
#define OUT_PIECE 0x800

typedef struct {
    PKReadFn read;
    PKWriteFn write;
    void *param;
    unsigned int mode;
    unsigned int dict_size;
    unsigned int dict_bits;
    unsigned long bits;                     /* output bits not yet in out_buf, the first in bit 0 */
    unsigned int bit_count;
    unsigned int out_len;
    unsigned int end;                       /* window[0..end) holds data */
    unsigned int next_insert;               /* the first position not yet in the hash chains */
    unsigned short head[HASH_SIZE];         /* newest position of each byte-pair hash */
    unsigned short prev[WINDOW];            /* the next older position with the same hash */
    unsigned char window[WINDOW];
    unsigned char out_buf[OUT_PIECE + 4];
} Imploder;

PK_STATIC_ASSERT(imploder_fits, sizeof(Imploder) <= PK_IMPLODE_WORK_SIZE);

static void PutBits(Imploder *s, unsigned long value, unsigned int count)
{
    s->bits |= value << s->bit_count;
    s->bit_count += count;
    while (s->bit_count >= 8) {
        s->out_buf[s->out_len++] = (unsigned char)s->bits;
        s->bits >>= 8;
        s->bit_count -= 8;
        if (s->out_len == OUT_PIECE) {
            unsigned int size = OUT_PIECE;
            s->write((char *)s->out_buf, &size, s->param);
            s->out_len = 0;
        }
    }
}

static void PutLiteral(Imploder *s, unsigned int c)
{
    PutBits(s, 0, 1);
    if (s->mode == PK_BINARY)
        PutBits(s, c, 8);
    else
        PutBits(s, kAsciiCode[c], kAsciiBits[c]);
}

/* A copy of `length` bytes (2..518, or PK_END_LENGTH without a distance) from `distance` bytes back. */
static void PutCopy(Imploder *s, unsigned int length, unsigned int distance)
{
    unsigned int value = length - PK_MIN_COPY;
    unsigned int code = 15;
    unsigned int low_bits;
    while (kLengthBase[code] > value)
        code--;
    PutBits(s, 1, 1);
    PutBits(s, kLengthCode[code], kLengthBits[code]);
    PutBits(s, value - kLengthBase[code], kLengthExtraBits[code]);
    if (length == PK_END_LENGTH)
        return;
    distance--;
    low_bits = length == PK_MIN_COPY ? 2 : s->dict_bits;
    PutBits(s, kDistCode[distance >> low_bits], kDistBits[distance >> low_bits]);
    PutBits(s, distance & ((1u << low_bits) - 1), low_bits);
}

static unsigned int Hash(const unsigned char *p)
{
    return (((unsigned int)p[0] << 3) ^ p[1]) & (HASH_SIZE - 1);
}

/* Adds the positions up to `pos` (inclusive) that have a following byte to the hash chains. */
static void InsertUpTo(Imploder *s, unsigned int pos)
{
    for (; s->next_insert <= pos && s->next_insert + 1 < s->end; s->next_insert++) {
        unsigned int h = Hash(s->window + s->next_insert);
        s->prev[s->next_insert] = s->head[h];
        s->head[h] = (unsigned short)s->next_insert;
    }
}

/* The longest copy for position `pos` (its length, 0 if none; the distance in *distance). Only positions before
 * `pos` are in the chains. */
static unsigned int FindCopy(Imploder *s, unsigned int pos, unsigned int *distance)
{
    const unsigned char *here = s->window + pos;
    unsigned int limit = s->end - pos;
    unsigned int best = 0, tries = MAX_CHAIN;
    unsigned int candidate;
    if (limit > PK_MAX_COPY)
        limit = PK_MAX_COPY;
    if (limit < PK_MIN_COPY)
        return 0;
    for (candidate = s->head[Hash(here)]; candidate != NO_POS && tries != 0; candidate = s->prev[candidate], tries--) {
        const unsigned char *there = s->window + candidate;
        unsigned int back = pos - candidate;
        unsigned int length;
        if (back > s->dict_size)
            break;                          /* the chains run from the newest position back */
        if (there[best] != here[best] || there[0] != here[0] || there[1] != here[1])
            continue;
        for (length = 2; length < limit && there[length] == here[length]; length++)
            ;
        if (length == PK_MIN_COPY && back > 256)
            continue;                       /* a 2-byte copy has only 8 distance bits */
        if (length > best) {
            best = length;
            *distance = back;
            if (length == limit)
                break;
        }
    }
    return best;
}

/* Drops the window's oldest bytes so that `pos` keeps HISTORY bytes of history; returns the shift. */
static unsigned int Slide(Imploder *s, unsigned int pos)
{
    unsigned int shift = pos - HISTORY;
    unsigned int i;
    memmove(s->window, s->window + shift, s->end - shift);
    memmove(s->prev, s->prev + shift, (s->end - shift) * sizeof(s->prev[0]));
    s->end -= shift;
    s->next_insert -= shift;
    for (i = 0; i < HASH_SIZE; i++)
        s->head[i] = (unsigned short)(s->head[i] == NO_POS || s->head[i] < shift ? NO_POS : s->head[i] - shift);
    for (i = 0; i < s->end; i++)
        s->prev[i] = (unsigned short)(s->prev[i] == NO_POS || s->prev[i] < shift ? NO_POS : s->prev[i] - shift);
    return shift;
}

static void Compress(Imploder *s)
{
    unsigned int pos = 0;
    int input_ended = 0;
    unsigned int i;
    s->end = 0;
    s->next_insert = 0;
    for (i = 0; i < HASH_SIZE; i++)
        s->head[i] = NO_POS;
    for (;;) {
        unsigned int stop;
        while (!input_ended && s->end < WINDOW) {
            unsigned int size = WINDOW - s->end;
            unsigned int got = s->read((char *)s->window + s->end, &size, s->param);
            if (got == 0)
                input_ended = 1;
            else
                s->end += got;
        }
        /* before the input ends, only positions with a full longest copy of lookahead */
        stop = input_ended ? s->end : s->end - PK_MAX_COPY;
        while (pos < stop) {
            unsigned int distance = 0, next_distance = 0;
            unsigned int length = FindCopy(s, pos, &distance);
            if (length != 0 && length < 8 && pos + 1 < s->end) {
                /* lazy: a longer copy one byte later wins (by one byte only when this copy is far) */
                unsigned int next;
                InsertUpTo(s, pos);
                next = FindCopy(s, pos + 1, &next_distance);
                if (next > length && (next > length + 1 || distance > 0x81))
                    length = 0;
            }
            if (length == 0) {
                PutLiteral(s, s->window[pos]);
                InsertUpTo(s, pos);
                pos++;
            } else {
                PutCopy(s, length, distance);
                InsertUpTo(s, pos + length - 1);
                pos += length;
            }
        }
        if (input_ended)
            break;
        if (pos > HISTORY)
            pos -= Slide(s, pos);
    }
    PutCopy(s, PK_END_LENGTH, 0);
    if (s->bit_count != 0)
        PutBits(s, 0, 8 - s->bit_count);
    if (s->out_len != 0) {
        unsigned int size = s->out_len;
        s->write((char *)s->out_buf, &size, s->param);
    }
}

unsigned int implode(PKReadFn read_buf, PKWriteFn write_buf, char *work_buf, void *param, unsigned int *type,
                     unsigned int *dsize)
{
    Imploder *s = (Imploder *)work_buf;
    s->read = read_buf;
    s->write = write_buf;
    s->param = param;
    s->dict_size = *dsize;
    s->mode = *type;
    switch (s->dict_size) {
    case 0x400:
        s->dict_bits = 4;
        break;
    case 0x800:
        s->dict_bits = 5;
        break;
    case 0x1000:
        s->dict_bits = 6;
        break;
    default:
        return PK_BAD_DICTIONARY;
    }
    if (s->mode != PK_BINARY && s->mode != PK_ASCII)
        return PK_BAD_MODE;
    s->out_buf[0] = (unsigned char)s->mode;
    s->out_buf[1] = (unsigned char)s->dict_bits;
    s->out_len = 2;
    s->bits = 0;
    s->bit_count = 0;
    Compress(s);
    return PK_OK;
}
