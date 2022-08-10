//
// Created by 最上川 on 2022/8/10/010.
//
// https://www.zhihu.com/people/zhang-yan-99-47/posts

#ifndef HOMURA_WORKSTEALQUEUE_H
#define HOMURA_WORKSTEALQUEUE_H
#include <atomic>

namespace Base
{
    template<typename TYPE, size_t COUNT>
    class WorkStealQueue
    {
        static_assert(!(COUNT & (COUNT - 1)), "count must be a power of two");
        static constexpr size_t MASK = COUNT - 1u;
        // steal at pop
        std::atomic<int> mTop;
        // posh, pop at bottom
        std::atomic<int> mBottom;
        TYPE mQueue[COUNT];
    public:
        void push(TYPE item);
        TYPE pop();
        TYPE steal();
    };
}
#endif //HOMURA_WORKSTEALQUEUE_H
