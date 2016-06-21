#ifndef SNAPPY_DEST_SOURCE_H
#define SNAPPY_DEST_SOURCE_H

#include <cstddef>

namespace snappy {

	//Destination is an interface that consumes a sequence of bytes
	class Destination {
	public:
		Destination() { }
		virtual ~Destination();

		//Purpose:	Append bytes[0, n-1] to this.
		virtual void Append(const char* bytes, size_t n) = 0;

		//Purpose:	Returns a writable buffer of the specified length for appending.
		//			May return a pointer to the caller-own scratch buffer which must
		//			have atleast the indicated length. The returned buffer is only
		//			valid until the next operation on this destination.
	};
}

#endif