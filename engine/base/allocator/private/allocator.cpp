//
// Created by Administrator on 2022/8/11/011.
//

#include "allocator.h"
namespace Base
{
    template<typename TYPE>
    TYPE* allocator<TYPE>::allocate(size_t n)
    {
        TYPE* p = aligned_alloc(n * sizeof(TYPE), alignof(TYPE));
        return p;
    }

    template<typename TYPE>
    void allocator<TYPE>::deallocate(TYPE *p)
    {
        aligned_free(p);
    }
}