//
// Created by Administrator on 2022/8/11/011.
//

#ifndef HOMURA_ALLOCATOR_H
#define HOMURA_ALLOCATOR_H
#include <cassert>

#if defined(WIN32)
#include <malloc.h>
#endif
namespace Base
{
    inline void* aligned_alloc(size_t size, size_t align) noexcept
    {
        // 'align' must be a power of two and a multiple of sizeof(void*)
        align = (align < sizeof(void*)) ? sizeof(void*) : align;
        assert(align && !(align & align - 1));
        assert((align % sizeof(void*)) == 0);
        void* p = nullptr;

#if defined(WIN32)
        p = ::_aligned_malloc(size, align);
#else
        ::posix_memalign(&p, align, size);
#endif
        return p;
    }

    inline void aligned_free(void* p)
    {
#if defined(WIN32)
        ::_aligned_free(p);
#else
        ::free(p);
#endif
    }

    template<typename TYPE>
    class allocator
    {
    public:

        typedef ptrdiff_t   different_type;
        typedef TYPE*       pointer;
        typedef const TYPE* const_pointer;
        typedef TYPE        value_type;
    public:
        TYPE* allocate(size_t n);
        void deallocate(TYPE* p);
    };
}

#endif //HOMURA_ALLOCATOR_H
