//
// Created by 最上川 on 2022/8/9/009.
//

#ifndef HOMURA_JOBSYSTEM_H
#define HOMURA_JOBSYSTEM_H

namespace Base
{
    struct Job;
    typedef void (*JobFunction)(Job*, const void*);

    struct Job
    {
        JobFunction mFunction;
        Job* mParent;
        int mUnfinishedJobs;
        char mPadding[];
    };
}

#endif //HOMURA_JOBSYSTEM_H
