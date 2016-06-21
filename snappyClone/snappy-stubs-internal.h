
//TODO Note this file needs to be changed inorder to make the code architecture independent.
//This currently works only for windows.

#ifndef SNAPPY_STUBS_INTERNAL
#define SNAPPY_STUBS_INTERNAL

#include "snappy-stubs-public.h"
#include <cassert>
#include <cstdlib>
#include <cstring>

#if defined(__x86_64__)
// Enable 64-bit optimized versions of some routines
#define ARCH_K8 1
#endif

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

//this is safe because this header file is not included in any of the public header files
using namespace std;

//The size of an array, if known at compile time,
//will give unexpected results, if used on a pointer.
//We will undefine it first, as many compilers already have a definition.

#ifdef ARRAY_SIZE
#undef ARRAY_SIZE
#endif
#define ARRAY_SIZE(s) (sizeof(s) / sizeof(*(a)))


//static prediction hints
//http://stackoverflow.com/questions/30130930/is-there-a-compiler-hint-for-gcc-to-force-branch-prediction-to-always-go-a-certa
#ifdef HAVE_BUILTIN_EXPECT
#define PREDICT_FALSE(x) (__builtin_expect(x, 0))
#define PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define PREDICT_FALSE(x) x
#define PREDICT_TRUE(x) x
#endif

//This is only used for recomputing the tag byte table used during
//during decompression;
#define DEFINE_bool(flag_name, default_value, description) \
	bool FLAGS_ ## flag_name = default_value
#define DECLARE_bool(flag_name) \
	extern bool FLAGS_ ## flag_name

namespace snappy {
	static const uint32 kuint32max = static_cast<uint32>(0xFFFFFFFF);
	static const int64 kint64max = static_cast<int64>(0x7FFFFFFFFFFFFFFFLL);

	//windows does not support unaligned load and stores
	//TODO address other architectures.
	//Refer: https://en.wikipedia.org/wiki/Data_structure_alignment

	inline uint16 UNALIGNED_LOAD16(const void* p)
	{
		uint16 t;
		memcpy(&t, p, sizeof t);
		return t;
	}

	inline uint32 UNALIGNED_LOAD32(const void * p)
	{
		uint32 t;
		memcpy(&t, p, sizeof t);
		return t;
	}

	inline uint64 UNALIGNED_LOAD64(const void * p)
	{
		uint64 t;
		memcpy(&t, p, sizeof t);
		return t;
	}

	inline void UNALIGNED_STORE16(void *p, uint16 v)
	{
		memcpy(p, &v, sizeof v);
	}

	inline void UNALIGNED_STORE32(void *p, uint32 v)
	{
		memcpy(p, &v, sizeof v);
	}

	inline void UNALIGNED_STORE64(void *p, uint64 v)
	{
		memcpy(p, &v, sizeof v);
	}

	//the following function can be more efficient than
	//UNALIGNED_LOAD64 + UNALIGNED_STORE64 on some platforms
	//particularly ARM
	inline void UnalignedCopy64(const void *src, void *dst)
	{
		if (sizeof(void *) == 8) {
			UNALIGNED_STORE64(dst, UNALIGNED_LOAD64(src));
		}
		else {
			const char *src_char = reinterpret_cast<const char*>(src);
			char *dst_char = reinterpret_cast<char*>(dst);
			UNALIGNED_STORE32(dst_char, UNALIGNED_LOAD32(src_char));
			UNALIGNED_STORE32(dst_char + 4, UNALIGNED_LOAD32(src_char + 4));
		}
	}

#ifdef WORDS_BIGENDIAN

#ifdef HAVE_SYS_BYTEORDER_H
#include <sys/byteorder.h>
#endif

#ifdef HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif

#ifdef _MSC_VER
#include<stdlib.h>
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)


//TODO address this for APPLE and freeBSD, write now writing just else
#elif defined(__APPLE__)
	//define apple swap function

#elif defined(bswap32)
	//define freebsd swaps, included in sys/endian.h

#elif defined(BSWAP_64)
	//case for solaris 10

#else
	inline uint16 bswap_16(uint16 x)
	{
		return (x << 8) | (x >> 8);
	}

	inline uint32 bswap_32(uint32 x)
	{
		x = ((x & 0xff00ff00UL) >> 8) | ((x & 0x00ff00ffUL) << 8);
		return (x << 16) | (x >> 16);
	}

	inline uint64 bswap_64(uint64 x)
	{
		x = ((x & 0xff00ff00ff00ff00ULL) >> 8)  | ((x & 0x00ff00ff00ff00ffULL) << 8);
		x = ((x & 0xffff0000ffff0000ULL) >> 16) | ((x & 0x0000ffff0000ffffULL) << 16);
		return (x << 32) | (x >> 32);
	}
#endif //end of all platform bswap

#endif //End of words_bigendian

//	Convert to little-endian storage, opposite of network format
//	Convert x from host to little endian; x = LittleEndian.fromHost(x);
//	Convert x from little endian to host; x = LittleEndian.ToHost(x);
//	Store values into unaligned memory converting to little endian order;
//		LittleEndian.Store16(p, x);
//	Load unaligned values stored in little endian converting to host order;
//	x = LittleEndian.Load16(x);

	class LittleEndian {
	public:

#ifdef  WORDS_BIGENDIAN
		static uint16 FromHost16(uint16 x) { return bswap_16(x); }
		static uint16 ToHost16(uint16 x) { return bswap_16(x); }

		static uint32 FromHost32(uint32 x) { return bswap_32(x); }
		static uint32 ToHost32(uint32 x) { return bswap_64(x); }

		static bool IsLittleEndian( return false; )
#else	//!defined(WORDS_BIGENDIAN)
		static uint16 FromHost16(uint16 x) { return x; }
		static uint16 ToHost16(uint16 x) { return x; }

		static uint16 FromHost32(uint32 x) { return x; }
		static uint16 ToHost32(uint32 x) { return x; }

		static bool IsLittleEndian() { return true; }
#endif

		//functions to do unaligned load and store in little Endian
		static uint16 Load16(const void * p)
		{
			return ToHost16(UNALIGNED_LOAD16(p));
		}

		static void Store16(void * p, uint16 v)
		{
			UNALIGNED_STORE16(p, FromHost16(v));
		}

		static uint32 Load32(const void * p)
		{
			return ToHost32(UNALIGNED_LOAD32(p));
		}

		static void Store32(void *p, uint32 v)
		{
			UNALIGNED_STORE32(p, FromHost32(v));
		}

	};

	//some bit manipulation functions

	class Bits {

	public:
		//return floor(log2(n)) for positive integer n, Returns -1 iff n == 0
		static int Log2Floor(uint32 n);

		//Returns the first set least/most significant bit, 0-indexed.
		//Returns an undefined value if n == 0
		//FindLSBSetNonZero() is similar to ffs() except that it's 0-indexed.
		static int FindLSBSetNonZero(uint32 n);
		static int FindMSBSetNonZero64(uint64 n);

	private:
		DISALLOW_COPY_AND_ASSIGN(Bits);
	};

	inline int Bits::Log2Floor(uint32 n)
	{
		if (n == 0)
			return -1;
		int log = 0;
		uint32 value = n;
		for (int i = 4; i >= 0; --i) {
			int shift = (1 << i);
			uint32 x = value >> shift;
			if (x != 0) {
				value = x;
				log += shift;
			}
		}
		assert(value == 1);
		return log;
	}

	inline int Bits::FindLSBSetNonZero(uint32 n)
	{
		int rc = 31;
		for (int i = 4, shift = 1 << 4; i >= 0; --i) {
			uint32 x = n << shift;
			if (x != 0) {
				n = x;
				rc -= shift;
			}
			shift >>= 1;
		}
		return rc;
	}

	inline int Bits::FindMSBSetNonZero64(uint64 n)
	{
		const uint32 last32bits = static_cast<uint32>(n);
		if (last32bits == 0) {
			return 32 + FindLSBSetNonZero(static_cast<uint32>(n >> 32));
		}
		else {
			return FindLSBSetNonZero(last32bits);
		}
	}

	//variable length integer encoding
	class Varint {
	public:
		//maximum length of Varint encoding of uint32
		static const int kMax32 = 5;

		//Purpose:	Encode a uint32_t "v" into "ptr" and returns a pointer
		//			and returns a pointer to the byte just past the last
		//			encoded byte.
		//Requires: "ptr" points to a buffer of length sufficient to hold
		//			value "v"
		//Note:		Ex: v = 456, instead of 4 bytes, we can store "v" at buffer
		//			pointed by "ptr" in just two bytes. as rest of the two bytes
		//			would be 0.
		//			456 = 0b 00000000 00000000 00000001 11001000
		static char* Encode32(char *ptr, uint32_t v);

		//Purpose:	Attempts to parse a varint32 from a prefix of bytes in [ptr, limit-1].
		//			Never reads a character at or beyond the limit. If a valid/terminated
		//			varint32 was found in range, stores it in *output, and returns a pointer
		//			just past the last byte of the varint32. Else returns NULL. On success,
		//			ptr <= limit.
		static const char* Parse32WithLimit(const char* ptr, const char* limit, uint32* output);

		//Purpose:	Append the varint representation of "value" to "*s"
		static void Append32(string *s, uint32 value);
	};

	inline char* Varint::Encode32(char *ptr, uint32 v)
	{
		unsigned char * uptr = reinterpret_cast<unsigned char*>(ptr);
		static const int B = 0xFF;
		if (v <= 0xFF) {
			*(uptr++) = v;
		}
		else if (v <= 0xFFFF) {
			*(uptr++) = (v & B);
			*(uptr++) = (v >> 8);
		}
		else if (v <= 0xFFFFFF){
			*(uptr++) = (v & B);
			*(uptr++) = (v >> 8) | B;
			*(uptr++) = (v >> 16);
		}
		else {
			*(uptr++) = (v & B);
			*(uptr++) = (v >> 8) | B;
			*(uptr++) = (v >> 16) | B;
			*(uptr++) = (v >> 24);
		}
		return reinterpret_cast<char*>(uptr);
	}

	inline const char* Varint::Parse32WithLimit(const char * ptr, const char * limit, uint32* output)
	{
		const unsigned char* uptr = reinterpret_cast<const unsigned char*>(ptr);
		const unsigned char* ulimit = reinterpret_cast<const unsigned char*>(limit);
		uint32 b = 0, result = 0;
		int itr = 0;
		if (ulimit - uptr > 4 || ulimit - uptr <= 0) {
			return nullptr;
		}
		while (itr < 4 && uptr < ulimit) {
			b = *(uptr++);
			if (b == 0) break;
			result |= ((b & 0xFF) << (itr * 8));
			++itr;
		}
		*output = result;
		return reinterpret_cast<const char*>(uptr);
	}

	inline char* string_as_array(string *str)
	{
		return str->empty() ? nullptr : &*str->begin();
	}
} // end of namespace snappy.
#endif