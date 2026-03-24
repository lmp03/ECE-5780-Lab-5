#include "rate.hpp"
#include <algorithm>
#include <iostream>

void rateMonotonic(std::vector<taskPeriodic> &perTasks,
                   std::vector<taskAperiodic> &aperTasks, int simTime,
                   std::ofstream &oFile) {

    std::sort(perTasks.begin(), perTasks.end(),
              [](const taskPeriodic &a, const taskPeriodic &b) {
                  return a.period < b.period;
              });

    taskPeriodic *currentTask = nullptr;
    taskAperiodic *currentATask = nullptr;
    bool preempt = false;

    for (int t = 0; t < simTime; t++) {

        preempt = false;

        // Release tasks
        for (taskPeriodic &task : perTasks) {
            if (t == task.nextRelease) {
                if (task.remainingTime > 0) {
                    task.deadlineMisses++;
                    oFile << "deadline miss " << task.id << " ";
                }
                task.remainingTime = task.compTime;
                task.deadline = t + task.period;
                task.nextRelease = t + task.period;
            }
        }

        for (taskAperiodic &task : aperTasks) {
            if (t > task.deadline && task.remainingTime > 0) {
                task.deadlineMisses++;
                oFile << "t=" << t << ": deadline miss " << task.id << " ";

                task.remainingTime = 0;
            }
            if (t == task.releaseTime) {
                task.remainingTime = task.compTime;
                task.deadline = t + 500;
                task.released = true;
            }
        }

        // Pick task to run

        taskPeriodic *nextTask = nullptr;

        for (taskPeriodic &task : perTasks) {
            if (task.remainingTime > 0) {
                nextTask = &task;
                break;
            }
        }

        taskAperiodic *nextATask = nullptr;

        // If no periodic to run, run aperiodic
        if (nextTask == nullptr) {
            for (taskAperiodic &task : aperTasks) {
                if (task.released && task.remainingTime > 0) {
                    nextATask = &task;
                    break;
                }
            }
        }

        // Check for preemption

        if (currentTask != nullptr && nextTask != currentTask) {
            if (currentTask->remainingTime > 0) {
                currentTask->preemptions++;
                preempt = true;
            }
        } else if (currentATask != nullptr && nextTask != nullptr) {
            if (currentATask->remainingTime > 0) {
                currentATask->preemptions++;
                preempt = true;
            }
        }

        // Print this line
        if (nextTask != nullptr) {
            currentTask = nextTask;
            currentATask = nullptr;
            currentTask->remainingTime--;
            oFile << "t=" << t << ": running task " << currentTask->id;
            if (preempt) {
                oFile << " preemption ";
            }
        } else if (nextATask != nullptr) {

            currentTask = nullptr;
            currentATask = nextATask;

            currentATask->remainingTime--;
            oFile << "t=" << t << ": running task " << currentATask->id;
            if (preempt) {
                oFile << " preemption ";
            }
        }

        else {
            currentTask = nullptr;
            currentATask = nullptr;
            oFile << "t=" << t << ": idle";
        }

        oFile << std::endl;
    }

    int totalPreemptions = 0;
    int totalMisses = 0;
    oFile << "Task\tPreemptions\tDeadline Misses" << std::endl;
    for (taskPeriodic task : perTasks) {
        oFile << task.id << "\t\t" << task.preemptions << "\t\t\t"
              << task.deadlineMisses << std::endl;
        totalPreemptions += task.preemptions;
        totalMisses += task.deadlineMisses;
    }

    for (taskAperiodic task : aperTasks) {
        oFile << task.id << "\t\t" << task.preemptions << "\t\t\t"
              << task.deadlineMisses << std::endl;
        totalPreemptions += task.preemptions;
        totalMisses += task.deadlineMisses;
    }
    oFile << "Total\t" << totalPreemptions << "\t\t\t" << totalMisses
          << std::endl;
}
