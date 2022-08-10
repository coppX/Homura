//
// Created by 最上川 on 2022/8/10/010.
//
#include "workStealQueue.h"

namespace Base
{
    template<typename TYPE, size_t COUNT>
    void WorkStealQueue<TYPE, COUNT>::push(TYPE item)
    {
        int bottom = mBottom.load(std::memory_order_relaxed);
        mQueue[bottom & MASK] = item;
        mBottom.store(bottom + 1, std::memory_order_release);
    }

    template<typename TYPE, size_t COUNT>
    TYPE WorkStealQueue<TYPE, COUNT>::pop()
    {
        int bottom = mBottom.fetch_sub(1, std::memory_order_seq_cst) - 1;
        int top = mTop.load(std::memory_order_seq_cst);
        TYPE item;
        if (top <= bottom)
        {
            item = mQueue[bottom & MASK];
            if (top != bottom)
            {
                return item;
            }

            if(mTop.compare_exchange_strong(top, top + 1, std::memory_order_seq_cst, std::memory_order_relaxed))
            {
                top++;
            }
            else
            {
                item = nullptr;
            }
        }
        mBottom.store(top, std::memory_order_relaxed);
        return item;
    }

    template<typename TYPE, size_t COUNT>
    TYPE WorkStealQueue<TYPE, COUNT>::steal()
    {
        while(true)
        {
            int top = mTop.load(std::memory_order_seq_cst);
            int bottom = mBottom.load(std::memory_order_seq_cst);
            if (top < bottom)
            {
                TYPE item = mQueue[top & MASK];
                if (mTop.compare_exchange_strong(top, top + 1, std::memory_order_seq_cst, std::memory_order_relaxed))
                {
                    return item;
                }
            }

            return nullptr;
        }
    }
}