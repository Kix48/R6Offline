#pragma once

typedef          __int64 ll;
typedef unsigned __int64 ull;

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;
typedef ll              int64;
typedef ll              sint64;
typedef ull             uint64;

template<class T> T __ROL__(T value, int count)
{
    const uint nbits = sizeof(T) * 8;

    if (count > 0)
    {
        count %= nbits;
        T high = value >> (nbits - count);
        if (T(-1) < 0) // signed value
            high &= ~((T(-1) << count));
        value <<= count;
        value |= high;
    }
    else
    {
        count = -count % nbits;
        T low = value << (nbits - count);
        value >>= count;
        value |= low;
    }
    return value;
}

inline uint8  __ROL1__(uint8  value, int count) { return __ROL__((uint8)value, count); }
inline uint16 __ROL2__(uint16 value, int count) { return __ROL__((uint16)value, count); }
inline uint32 __ROL4__(uint32 value, int count) { return __ROL__((uint32)value, count); }
inline uint64 __ROL8__(uint64 value, int count) { return __ROL__((uint64)value, count); }
inline uint8  __ROR1__(uint8  value, int count) { return __ROL__((uint8)value, -count); }
inline uint16 __ROR2__(uint16 value, int count) { return __ROL__((uint16)value, -count); }
inline uint32 __ROR4__(uint32 value, int count) { return __ROL__((uint32)value, -count); }
inline uint64 __ROR8__(uint64 value, int count) { return __ROL__((uint64)value, -count); }

//Dumb glow decryption stuff
//https://www.codeproject.com/Articles/1274943/IEEE-754-Conversion

#define NTH_BIT(b, n) ((b >> n) & 0x1)

#define BYTE_TO_BIN(b)   (( b & 0x80 ) ) |\
            (( b & 0x40 ) ) |\
            (( b & 0x20 ) ) |\
            (( b & 0x10 ) ) |\
            (( b & 0x08 ) ) |\
            (( b & 0x04 ) ) |\
            (( b & 0x02 ) ) |\
            ( b & 0x01 )

#define MANTISSA_TO_BIN(b)  (( b & 0x400000 ) ) |\
             (( b & 0x200000 ) ) |\
             (( b & 0x100000 ) ) |\
             (( b &  0x80000 ) ) |\
             (( b &  0x40000 ) ) |\
             (( b &  0x20000 ) ) |\
             (( b &  0x10000 ) ) |\
             (( b &  0x8000 ) ) |\
             (( b &  0x4000 ) ) |\
             (( b &  0x2000 ) ) |\
             (( b &  0x1000 ) ) |\
             (( b &  0x800 ) ) |\
             (( b &  0x400 ) ) |\
             (( b &  0x200 ) ) |\
             (( b &  0x100 ) ) |\
             (( b &  0x80 ) ) |\
             (( b &  0x40 ) ) |\
             (( b &  0x20 ) ) |\
             (( b &  0x10 ) ) |\
             (( b &  0x08 ) ) |\
             (( b &  0x04 ) ) |\
             (( b &  0x02 ) ) |\
              ( b & 0x01 )

typedef union IEEE754
{
    struct
    {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } raw;
    float f;
} IEEE754;

inline uint32_t FloatToHex(float f)
{
    IEEE754 ieee754;
    uint32_t floatingToIntValue = 0;
    ieee754.f = f;
    floatingToIntValue = (((NTH_BIT(ieee754.raw.sign, 0) << 8) |
        (BYTE_TO_BIN(ieee754.raw.exponent))) << 23) | MANTISSA_TO_BIN(ieee754.raw.mantissa);
    return floatingToIntValue;
}

inline float HexToFloat(uint32_t floatingToIntValue)
{
    IEEE754 ieee754;
    unsigned int mantissa = 0;
    unsigned int exponent = 0;
    unsigned int sign = 0;

    sign = NTH_BIT(floatingToIntValue, 31);
    for (int ix = 0; ix < 8; ix++)
        exponent = (exponent | (NTH_BIT(floatingToIntValue, (30 - ix)))) << 1;
    exponent = exponent >> 1;
    for (int ix = 0; ix < 23; ix++)
        mantissa = (mantissa | (NTH_BIT(floatingToIntValue, (22 - ix)))) << 1;
    mantissa = mantissa >> 1;

    ieee754.raw.sign = sign;
    ieee754.raw.exponent = exponent;
    ieee754.raw.mantissa = mantissa;
    return ieee754.f;
}