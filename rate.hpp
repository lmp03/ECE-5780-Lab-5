#ifndef RATE_HPP
#define RATE_HPP

#include "structDef.hpp"
#include <vector>

void rateMonotic(std::vector<taskPeriodic> &perTasks,
                 std::vector<taskAperiodic> &aperTasks, int simTime);

#endif
