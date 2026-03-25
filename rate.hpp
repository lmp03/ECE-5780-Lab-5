#ifndef RATE_HPP
#define RATE_HPP

#include "structDef.hpp"
#include <fstream>
#include <vector>

void rateMonotonic(std::vector<taskPeriodic> &perTasks,
                   std::vector<taskAperiodic> &aperTasks, int simTime,
                   std::ofstream &oFile);

#endif
