#include "rate.hpp"
#include <algorithm>
#include <iostream>

void rateMonotic(std::vector<taskPeriodic> &perTasks,
                 std::vector<taskAperiodic> &aperTasks, int simTime) {

    std::sort(perTasks.begin(), perTasks.end(),
              [](const taskPeriodic &a, const taskPeriodic &b) {
                  return a.period < b.period;
              });

    for (int t = 0; t < simTime; t++) {

        // Release tasks
        for (taskPeriodic task : perTasks) {
            if (t == task.nextRelease) {
                task.remainingTime = task.compTime;
                task.deadline = t + task.period;
                task.nextRelease += task.period;
            }
        }

        for (taskAperiodic task : aperTasks) {
            if (t == task.releaseTime) {
                task.remainingTime = task.compTime;
                task.deadline = t + 500;
            }
        }

        // Pick task to run

        // Check for preemption

        // Print this line

        // Check deadlines
    }
}
