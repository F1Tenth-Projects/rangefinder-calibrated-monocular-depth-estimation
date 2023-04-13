#ifndef ENDIAN_H
#define ENDIAN_H

#define htobe32(value) __builtin_bswap32(value)
#define be32toh(value) __builtin_bswap32(value)
#define htobe16(value) __builtin_bswap16(value)
#define be16toh(value) __builtin_bswap16(value)

#endif
