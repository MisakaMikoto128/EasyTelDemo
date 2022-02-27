#ifndef _BYTES_UTIL_H_
#define _BYTES_UTIL_H_

/*
Naming Rule:

*/

// Select the appropriate platform nefore writing other code,the default configuration is 32-bit machines
//  #define _8_BIT_SYSTEM
//  #define _16_BIT_SYSTEM
//  #define _32_BIT_SYSTEM
//  #define _64_BIT_SYSTEM
#define _32_BIT_SYSTEM

#ifdef _8_BIT_SYSTEM // TODO : the following definitions are for 8-bit maybe wrong
#define bu_uint8 unsigned char
#define bu_int8 char
#define bu_uint16 unsigned short
#define bu_int16 short
#define bu_uint32 unsigned long
#define bu_int32 long
#define bu_uint64 unsigned long long
#define bu_int64 long long
#define bu_float float
#define bu_double double
#define bu_long_double long double

#define bu_byte bu_uint8
#define bu_word bu_uint16
#define bu_dword bu_uint32
#define bu_qword bu_uint64

#define bu_size_t bu_uint32
#endif

#ifdef _16_BIT_SYSTEM
#define bu_uint8 unsigned char
#define bu_int8 char
#define bu_uint16 unsigned short
#define bu_int16 short
#define bu_uint32 unsigned long
#define bu_int32 long
#define bu_uint64 unsigned long long
#define bu_int64 long long
#define bu_float float
#define bu_double double
#define bu_long_double long double

#define bu_byte bu_uint8
#define bu_word bu_uint16
#define bu_dword bu_uint32
#define bu_qword bu_uint64

#define bu_size_t bu_uint32
#endif

#ifdef _32_BIT_SYSTEM
#define bu_uint8 unsigned char
#define bu_int8 char
#define bu_uint16 unsigned short
#define bu_int16 short
#define bu_uint32 unsigned int
#define bu_int32 int
#define bu_uint64 unsigned long long
#define bu_int64 long long
#define bu_float float
#define bu_double double
#define bu_long_double long double

#define bu_byte bu_uint8
#define bu_word bu_uint16
#define bu_dword bu_uint32
#define bu_qword bu_uint64

#define bu_size_t bu_uint32
#endif

#ifdef _64_BIT_SYSTEM
#define bu_uint8 unsigned char
#define bu_int8 char
#define bu_uint16 unsigned short
#define bu_int16 short
#define bu_uint32 unsigned int
#define bu_int32 int
#define bu_uint64 unsigned long long
#define bu_int64 long long
#define bu_float float
#define bu_double double
#define bu_long_double long double

#define bu_byte bu_uint8
#define bu_word bu_uint16
#define bu_dword bu_uint32
#define bu_qword bu_uint64

#define bu_size_t bu_uint32
#endif

#define REVERSE_2_BYTES_P(dst, src) \
    do                              \
    {                               \
        dst[0] = src[1];            \
        dst[1] = src[0];            \
    } while (0)

#define REVERSE_4_BYTES_P(dst, src) \
    do                              \
    {                               \
        dst[0] = src[3];            \
        dst[1] = src[2];            \
        dst[2] = src[1];            \
        dst[3] = src[0];            \
    } while (0)

#define REVERSE_8_BYTES_P(dst, src) \
    do                              \
    {                               \
        dst[0] = src[7];            \
        dst[1] = src[6];            \
        dst[2] = src[5];            \
        dst[3] = src[4];            \
        dst[4] = src[3];            \
        dst[5] = src[2];            \
        dst[6] = src[1];            \
        dst[7] = src[0];            \
    } while (0)

#define REVERSE_16_BYTES_P(dst, src) \
    do                               \
    {                                \
        dst[0] = src[15];            \
        dst[1] = src[14];            \
        dst[2] = src[13];            \
        dst[3] = src[12];            \
        dst[4] = src[11];            \
        dst[5] = src[10];            \
        dst[6] = src[9];             \
        dst[7] = src[8];             \
        dst[8] = src[7];             \
        dst[9] = src[6];             \
        dst[10] = src[5];            \
        dst[11] = src[4];            \
        dst[12] = src[3];            \
        dst[13] = src[2];            \
        dst[14] = src[1];            \
        dst[15] = src[0];            \
    } while (0)

#define REVERSE_2_BYTES_R(value) (((value) >> 8) | ((value) << 8))

#define REVERSE_4_BYTES_R(value) (((value) >> 24) | (((value) >> 8) & 0x0000FF00L) | (((value) << 8) & 0x00FF0000L) | ((value) << 24))

#define REVERSE_8_BYTES_R(value) (((value) >> 56) | (((value) >> 40) & 0x000000000000FF00LL) | (((value) >> 24) & 0x0000000000FF0000LL) | (((value) >> 8) & 0x00000000FF000000LL) | (((value) << 8) & 0x000000FF00000000LL) | (((value) << 24) & 0x0000FF0000000000LL) | (((value) << 40) & 0x00FF000000000000LL) | ((value) << 56))

#define CAST_2_CONST_BYTE_P(obj) ((const bu_byte *)(obj))

enum Endian{
    LLITTLE_ENDIAN = 0,
    LBIG_ENDIAN = 1
};

#define ENDIA_JUDGE_DATA 0x20220204

class BytesUtil
{
public:
    /**
     * @brief  get the endian of the system using the data ENDIA_JUDGE_DATA.
     *
     * @param data : ENDIA_JUDGE_DATA bytes come from other system.
     * @return Endian
     *
     */
    static Endian getEndian(const bu_byte * data)
    {
        if (data[0] == 0x04)
        {
            return LLITTLE_ENDIAN;
        }
        else
        {
            return LBIG_ENDIAN;
        }
    }

    /**
     * @brief get self endian.
     *
     * @return Endian
     */
    static Endian getSelfEndian()
    {
        bu_uint32 data = ENDIA_JUDGE_DATA;
        bu_byte *p = (bu_byte *)&data;
        if (p[0] == 0x04)
        {
            return LLITTLE_ENDIAN;
        }
        else
        {
            return LBIG_ENDIAN;
        }
    }

    /**
     * @brief Get the Bytes object
     *
     * @tparam T : the type of the object, support char, short, int, long, long long, float, double, long double,
     * unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long,wchar_t
     * @param obj : the object to be converted to Bytes object
     * @param dst : save the bytes result of obj.
     * @param change_endian : if true, the bytes will be changed to the opposite endian of current machine.
     */
    template <typename T>
    static void getBytes(const T &obj, bu_byte *dst, bool change_endian)
    {
        if (change_endian)
        {
            switch (sizeof(T))
            {
            case 2:
                REVERSE_2_BYTES_P(dst, (bu_byte *)&obj);
                break;
            case 4:
                REVERSE_4_BYTES_P(dst, (bu_byte *)&obj);
                break;
            case 8:
                REVERSE_8_BYTES_P(dst, (bu_byte *)&obj);
                break;
            case 16:
                REVERSE_16_BYTES_P(dst, (bu_byte *)&obj);
                break;
            default:
                break;
            }
        }
        else
        {
            *((T *)dst) = obj;
        }
    }

    /**
     * @brief Get the Obj object
     * 
     * @tparam T :  the type of the object, support char, short, int, long, long long, float, double, long double,
     * unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long,wchar_t
     * @param src : source bytes
     * @param change_endian : if true, the bytes will be changed to the opposite endian of current machine.
     * @return T : the T object of the bytes.
     */

    template <typename T>
    static T getObj(const bu_byte *src, bool change_endian)
    {
        union
        {
            T obj;
            bu_byte bytes[sizeof(T)];
        } u;
        if (change_endian)
        {
            switch (sizeof(T))
            {
            case 2:
                REVERSE_2_BYTES_P(u.bytes, src);
                break;
            case 4:
                REVERSE_4_BYTES_P(u.bytes, src);
                break;
            case 8:
                REVERSE_8_BYTES_P(u.bytes, src);
                break;
            case 16:
                REVERSE_16_BYTES_P(u.bytes, src);
                break;
            default:
                break;
            }
        }
        else
        {
            *((T *)&u) = *((T *)src);
        }
        return u.obj;
    }



};



#endif // _BYTES_UTIL_H_