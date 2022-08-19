//
// Created by 最上川 on 2022/8/9/009.
//

#include <jobSystem.h>
#include <allocator.h>
#include <workStealQueue.h>

namespace Base
{
    template<typename TYPE, size_t COUNT>
    Worker<TYPE, COUNT>::Worker()
    {
        mQueue = std::make_shared<WorkQueue>();
        mAllocator = std::make_shared<allocator<TYPE>>();
        mPool = mAllocator->allocate(COUNT);
        mIndex = -1;
    }

    template<typename TYPE, size_t COUNT>
    Worker<TYPE, COUNT>::~Worker()
    {
        mAllocator->deallocate(mPool);
    }

    template<typename TYPE, size_t COUNT>
    TYPE* Worker<TYPE, COUNT>::createJob()
    {
        mIndex++;
        return &mPool[mIndex & (COUNT - 1u)];
    }

    template<typename TYPE, size_t COUNT>
    size_t Worker<TYPE,COUNT>::getLoad()
    {
        return mQueue->getSize();
    }

    JobSystem::JobSystem()
    {
        int threadCounts = std::thread::hardware_concurrency() - 1;
        for (int i = 0; i < threadCounts; i++)
        {
            JobWorker* worker{};
            mWorker.push_back(worker);
        }
    }

    Job* JobSystem::allocateJob()
    {
        // 选择负载最小的worker, 在其上面创建任务
        JobWorker* worker = selectMinLoadWorker();
        return worker->createJob();
    }

    Job* JobSystem::createJob(Base::Job* parent, Base::JobFunction function)
    {
        Job* job = allocateJob();
        if (parent != nullptr)
        {
            parent->mUnfinishedJobs.fetch_add(1, std::memory_order_relaxed);
            job->mParent = parent;
        }
        job->mFunction = function;
        job->mUnfinishedJobs = 1;
        return job;
    }

    JobSystem::JobWorker*
    JobSystem::selectMinLoadWorker()
    {
        JobWorker* worker = mWorker[0];
        for (int i = 1; i < mWorker.size(); i++)
        {
            if (mWorker[i]->getLoad() < worker->getLoad())
            {
                worker = mWorker[i];
                break;
            }
        }
        return worker;
    }
}