//
// Created by 最上川 on 2022/8/9/009.
//

#ifndef HOMURA_JOBSYSTEM_H
#define HOMURA_JOBSYSTEM_H
#include <atomic>
#include <thread>
#include <vector>
#include <workStealQueue.h>
#include <allocator.h>

namespace Base
{
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t MAX_JOB_COUNT = 4096;
    struct Job;
    using JobFunction = void(*)(Job*, const void*);
    using WorkQueue = WorkStealQueue<uint16_t, MAX_JOB_COUNT>;
    using JobPool = allocator<Job>;

    struct alignas(CACHE_LINE_SIZE) Job
    {
        JobFunction mFunction;
        Job* mParent;
        std::atomic<int> mUnfinishedJobs;
        std::atomic<int> mContinuationCount;
        Job* mContinuations[15];
        char mData[];
    };

    template<typename TYPE, size_t COUNT>
    struct Worker {
        Worker()
        {
            mPool = mAllocator.allocate(COUNT);
            mIndex = -1;
        }

        TYPE* createJob()
        {
            mIndex++;
            return &mPool[mIndex & (COUNT - 1u)];
        }

        size_t getLoad()
        {
            return mQueue.getSize();
        }
    private:
        std::thread mThread;
        WorkQueue mQueue;
        TYPE* mPool;
        allocator<TYPE> mAllocator;
        int mIndex;
    };

    class JobSystem
    {
    public:
        using JobWorker = Worker<Job, MAX_JOB_COUNT>;
        JobSystem();
        Job* createJob(Job* parent, JobFunction function);
    private:
        Job* allocateJob();
        JobWorker* selectMinLoadWorker();
    private:
        std::vector<JobWorker*> mWorker;
    };
}

#endif //HOMURA_JOBSYSTEM_H
