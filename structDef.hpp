#ifndef STRUCT_DEF_HPP
#define STRUCT_DEF_HPP

#include <string>

struct taskPeriodic {
    std::string id;
    int compTime;
    int period;

    int remainingTime = 0;
    int nextRelease = 0;
    int deadline = 0;

    int preemptions = 0;
    int deadlineMisses = 0;

    int priority;
};

struct taskAperiodic {
    std::string id;
    int compTime;
    int releaseTime;

    int remainingTime = 0;
    int deadline = 0;

    int preemptions = 0;
    int deadlineMisses = 0;

    int priority;
};

#endif
