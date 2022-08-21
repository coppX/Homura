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
        mThread = std::thread(&Worker::execute, this);
        mIndex = -1;
    }

    template<typename TYPE, size_t COUNT>
    Worker<TYPE, COUNT>::~Worker()
    {
        mAllocator->deallocate(mPool);
        if (mThread.joinable())
        {
            mThread.join();
        }
    }

    template<typename TYPE, size_t COUNT>
    TYPE* Worker<TYPE, COUNT>::createJob()
    {
        mIndex++;
        return &mPool[mIndex & (COUNT - 1u)];
    }

    template<typename TYPE, size_t COUNT>
    void Worker<TYPE,COUNT>::run(Job* job)
    {
        assert(job);
        size_t index = job - mPool;
        assert(index >= 0 && index < MAX_JOB_COUNT);
        mQueue->push(index);
        mCv.notify_one();
    }

    template<typename TYPE, size_t COUNT>
    size_t Worker<TYPE,COUNT>::getLoad()
    {
        return mQueue->getSize();
    }

    template<typename TYPE, size_t COUNT>
    bool Worker<TYPE, COUNT>::loop()
    {
        TYPE* job = mQueue->pop();
        if (!job)
        {
            job = mQueue->steal();
        }

        if (job)
        {
            (job->mFunction)(job, job->mData);
            finish(job);
        }
        return job != nullptr;
    }

    template<typename TYPE, size_t COUNT>
    void Worker<TYPE, COUNT>::finish(TYPE* job)
    {
        int unfinished = job->mUnfinishedJobs.fetch_sub(1, std::memory_order_acquire);
        if (unfinished == 0)
        {
            if (job->mParent)
            {
                finish(job->mParent);
            }
        }
        // delete job
    }

    template<typename TYPE, size_t COUNT>
    void Worker<TYPE, COUNT>::execute()
    {
        while(!mExit.load())
        {
            if (!loop())
            {
                // wait
                std::unique_lock<std::mutex> lock(mMutex);
                mCv.wait(lock);
            }
        }
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

    Job* JobSystem::createJob(Base::Job* parent, Base::JobFunction function)
    {
        // 选择负载最小的worker, 在其上面创建任务
        JobWorker* worker = selectMinLoadWorker();
        Job* job = worker->createJob();
        if (parent != nullptr)
        {
            parent->mUnfinishedJobs.fetch_add(1, std::memory_order_relaxed);
            job->mParent = parent;
        }
        job->mFunction = function;
        job->mUnfinishedJobs = 1;
        worker->run(job);
        return job;
    }

    JobWorker* JobSystem::selectMinLoadWorker()
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

    Job *JobSystem::getJob()
    {
        return nullptr;
    }

    bool JobSystem::hasCompleted(Job *job)
    {
        return job->mUnfinishedJobs.load(std::memory_order_acquire) <= 0;
    }

    void JobSystem::run(Job *job)
    {

    }

    void JobSystem::wait(Job *job)
    {
        while (!hasCompleted(job))
        {

        }
    }

    template<typename T, typename S>
    Job *JobSystem::parallel_for(T *data, unsigned int count, void (*f)(T *, unsigned int), const S &splitter)
    {
        return nullptr;
    }
}