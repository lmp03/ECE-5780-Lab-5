#ifndef STRUCT_DEF_HPP
#define STRUCT_DEF_HPP

#include <string>

struct taskPeriodic {
        std::string id;
        int compTime;
        int period;
};

struct taskAperiodic {
        std::string id;
        int compTime;
        int releaseTime;
};

#endif
