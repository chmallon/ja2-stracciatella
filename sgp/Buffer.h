#ifndef BUFFER_H
#define BUFFER_H

#include "MemMan.h"


namespace SGP
{
	template<typename T> class Buffer
	{
		public:
			explicit Buffer() : buf_(0) {}

			explicit Buffer(size_t const n) : buf_(MALLOCN(T, n)) {}

			~Buffer() { if (buf_) MemFree(buf_); }

			Buffer& Allocate(size_t const n)
			{
				if (buf_)
				{
					MemFree(buf_);
					buf_ = 0;
				}
				buf_ = MALLOCN(T, n);
				return *this;
			}

			T* Release()
			{
				T* const buf = buf_;
				buf_ = 0;
				return buf;
			}

			operator T*()             { return buf_; }
			operator T const*() const { return buf_; }

		private:
			T* buf_;

			Buffer(const Buffer&);          /* no copy */
			void operator =(const Buffer&); /* no assignment */
	};
}

#endif
