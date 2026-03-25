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
        oFile << "t=" << t << ": ";
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
                oFile << "deadline miss " << task.id << " ";

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
        // If task to run next is different than next task to run
        // and current task running still has work to be done
        // and current task wasn't released this timestep
        // (don't want to think it's a preemption because there's work to still
        // do on the task if last timestep we finished the last period's work
        // and this timestep we're starting a new period)
        if (currentTask != nullptr && nextTask != currentTask) {
            if (currentTask->remainingTime > 0 &&
                currentTask->deadline - currentTask->period != t) {
                currentTask->preemptions++;
                preempt = true;
            }
            // If we were running an aperiodic task, and it get preempted by a
            // periodic task
        } else if (currentATask != nullptr && nextTask != nullptr) {
            if (currentATask->remainingTime > 0) {
                currentATask->preemptions++;
                preempt = true;
            }
        }
        // Periodic tasks can't be preempted by aperiodic tasks in RMA, so no
        // need to check that case

        // Print this line
        if (nextTask != nullptr) {
            currentTask = nextTask;
            currentATask = nullptr;
            currentTask->remainingTime--;
            oFile << "running task " << currentTask->id;
            if (preempt) {
                oFile << " preemption ";
            }
        } else if (nextATask != nullptr) {

            currentTask = nullptr;
            currentATask = nextATask;

            currentATask->remainingTime--;
            oFile << "running task " << currentATask->id;
            if (preempt) {
                oFile << " preemption ";
            }
            if (currentATask->remainingTime == 0) {
                currentATask->responseTime = (t - currentATask->releaseTime);
            }
        }

        else {
            currentTask = nullptr;
            currentATask = nullptr;
            oFile << "idle";
        }

        oFile << std::endl;
    }

    int totalPreemptions = 0;
    int totalMisses = 0;
    int numAper = 0;
    double averageResponseTime = 0;
    oFile << "\n--- RMA Summary ---" << std::endl;
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
        averageResponseTime += static_cast<double>(task.responseTime);
        numAper++;
    }
    if (numAper != 0) {
        averageResponseTime /= static_cast<double>(numAper);
    }
    oFile << "Total\t" << totalPreemptions << "\t\t\t" << totalMisses;
    if (numAper != 0) {
        oFile << "\nAverage Aperiodic Response Time " << averageResponseTime
              << " msec";
    }
    oFile << std::endl << std::endl;
}
