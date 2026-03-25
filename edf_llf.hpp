#ifndef EDF_LLF_HPP
#define EDF_LLF_HPP

#include "structDef.hpp"
#include <vector>
#include <fstream>

void edf(std::vector<taskPeriodic> &perTasks, std::vector<taskAperiodic> &aperTasks, int simTime, std::ofstream &oFile);

void llf(std::vector<taskPeriodic> &perTasks, std::vector<taskAperiodic> &aperTasks, int simTime, std::ofstream &oFile);

// Helper to clear task states between EDF and LLF runs
void resetTasks(std::vector<taskPeriodic> &perTasks, std::vector<taskAperiodic> &aperTasks);

#endif
