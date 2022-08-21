//
// Created by 最上川 on 2022/8/9/009.
//

#ifndef HOMURA_JOBSYSTEM_H
#define HOMURA_JOBSYSTEM_H
#include <atomic>
#include <thread>
#include <vector>
#include <memory>
#include <condition_variable>

namespace Base
{
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t MAX_JOB_COUNT = 4096;

    struct Job;
    template<typename TYPE, size_t COUNT> class WorkStealQueue;
    template<typename TYPE, size_t COUNT> struct Worker;
    template<typename TYPE> class allocator;

    using JobFunction = void(*)(Job*, const void*);
    using JobWorker = Worker<Job, MAX_JOB_COUNT>;

    struct alignas(CACHE_LINE_SIZE) Job
    {
        Job() {}
        Job(const Job&) = delete;
        Job(Job&&) = delete;
        JobFunction mFunction;
        Job* mParent;
        // If the counter is greater than 0, either the job itself or any of its child jobs hasn’t finished
        std::atomic<int> mUnfinishedJobs;
        std::atomic<int> mContinuationCount;
        Job* mContinuations[15];
        char mData[];
    };

    template<typename TYPE, size_t COUNT>
    struct Worker {
        using WorkQueue = WorkStealQueue<size_t, COUNT>;
        using alloc = allocator<TYPE>;
        Worker();
        ~Worker();
        TYPE* createJob();
        void run(Job* job);
        size_t getLoad();
        bool loop();
        void execute();
        void finish(TYPE* job);
        TYPE* mPool;
        int mIndex;
        std::thread mThread;
        std::condition_variable mCv;
        std::mutex mMutex;
        std::shared_ptr<WorkQueue> mQueue;
        std::shared_ptr<alloc> mAllocator;
        std::atomic<bool> mExit;
    };

    class JobSystem
    {
    public:
        JobSystem();
        Job* createJob(Job* parent, JobFunction function);
        void run(Job* job);
        void wait(Job* job);
        Job* getJob();
        bool hasCompleted(Job* job);

        template<typename T, typename S>
        Job* parallel_for(T* data, unsigned int count, void(*f)(T*, unsigned int), const S& splitter);
    private:
        JobWorker* selectMinLoadWorker();
    private:
        std::vector<JobWorker*> mWorker;
    };
}

#endif //HOMURA_JOBSYSTEM_H
