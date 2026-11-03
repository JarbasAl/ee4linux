/* PKWARE Data Compression Library ("implode"/"explode") stream format: an implementation written for this project.
 * NOT a reconstruction of PKWARE's code and not byte-matched: library code with no FUNCTION markers and no matching
 * credit (owner decision, 2026-11-03). It replaces the original's DCL 1.11 objects (Low-Level Engine.dll
 * 0x10085bf0..0x10086d44), which the relink used to carve from the original image. Same entry points and
 * signatures; the work buffers fit the sizes UDataCompression allocates.
 *
 * Stream: byte 0 = literal mode (PK_BINARY / PK_ASCII), byte 1 = log2(dictionary size) - 6 (4, 5, 6 for 1024, 2048,
 * 4096), then a bit stream read from bit 0 of each byte upwards. Each token starts with one bit:
 *   0: a literal, 8 raw bits (binary) or the byte's code from kAsciiCode/kAsciiBits (ASCII)
 *   1: a copy: length code (kLengthCode/kLengthBits), kLengthExtraBits extra bits added to kLengthBase; length =
 *      that + 2 (2..518; 519 ends the stream); then the distance's upper 6 bits as a code (kDistCode/kDistBits) and
 *      its low bits (2 for a length-2 copy, else the dictionary bits); distance = value + 1.
 * Codes are stored in the order they are read (first bit in bit 0). */
#ifndef PKDCL_H
#define PKDCL_H

typedef unsigned int (*PKReadFn)(char *buf, unsigned int *size, void *param);
typedef void (*PKWriteFn)(char *buf, unsigned int *size, void *param);

/* results (the values UDataCompression and the original library use) */
#define PK_OK 0
#define PK_BAD_DICTIONARY 1
#define PK_BAD_MODE 2
#define PK_BAD_DATA 3
#define PK_ABORT 4

#define PK_BINARY 0
#define PK_ASCII 1

#define PK_MIN_COPY 2
#define PK_MAX_COPY 518
#define PK_END_LENGTH 519           /* the copy length that marks the end of the stream */
#define PK_MAX_DICTIONARY 0x1000

/* work buffer sizes the callers allocate (UDataCompression: new char[0x8dd8], new char[0x3134]) */
#define PK_IMPLODE_WORK_SIZE 0x8dd8
#define PK_EXPLODE_WORK_SIZE 0x3134

#define PK_STATIC_ASSERT(name, cond) typedef char name[(cond) ? 1 : -1]

/* the format's code tables (explode.c) */
extern const unsigned char kDistBits[64];
extern const unsigned char kDistCode[64];
extern const unsigned char kLengthExtraBits[16];
extern const unsigned short kLengthBase[16];
extern const unsigned char kLengthBits[16];
extern const unsigned char kLengthCode[16];
extern const unsigned char kAsciiBits[256];
extern const unsigned short kAsciiCode[256];

unsigned int implode(PKReadFn read_buf, PKWriteFn write_buf, char *work_buf, void *param, unsigned int *type,
                     unsigned int *dsize);
unsigned int explode(PKReadFn read_buf, PKWriteFn write_buf, char *work_buf, void *param);

#endif
