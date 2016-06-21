
//TODO this file needs to be changed inorder to make the code architecture independent.
//This currently works only for windows.

#ifndef SNAPPY_STUBS_PUBLIC_H
#define SNAPPY_STUBS_PUBLIC_H

#include <cstdint>
#include <cstddef>

#define SNAPPY_MAJOR 1
#define SNAPPY_MINOR 0
#define SNAPPY_PATCHLEVEL 4
#define SNAPPY_VERSION \
	((SNAPPY_MAJOR << 16) | (SNAPPY_MINOR << 8) | SNAPPY_PATCHLEVEL)

#include <string>

namespace snappy {
	typedef int8_t int8;
	typedef uint8_t uint8;
	typedef int16_t int16;
	typedef uint16_t uint16;
	typedef int32_t int32;
	typedef uint32_t uint32;
	typedef int64_t int64;
	typedef uint64_t uint64;

	typedef std::string string;

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(Typename)	\
	Typename(const Typename&);				\
	void operator=(const Typename&)
#endif


//windows does not have iovec type so let us define one in our own namespace
	struct iovec {
		void* iov_base;
		size_t iov_len;
	};
} //namespace snappy
#endif //end of SNAPPY_STUBS_PUBLIC_H