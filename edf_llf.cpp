#include "edf_llf.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <map>

void edf(std::vector<taskPeriodic> &perTasks,
         std::vector<taskAperiodic> &aperTasks, int simTime,
         std::ofstream &oFile) {
    // Track various important values
    // For preemption and other needs
    std::string lastTaskID = "";
    int lastTaskRemaining = 0;

    // For summary
    int totalPreemptions = 0;
    int totalMisses = 0;
    int totalAperiodicResponseTime = 0;
    int compAperiodics = 0;
    std::map<std::string, int> taskMisses;

    // Time simulation
    for (int t = 0; t < simTime; t++) {
        // Release logic
        for (auto &p : perTasks) {
            if (t == p.nextRelease) {
                p.remainingTime = p.compTime;
                p.deadline = t + p.period;
                p.nextRelease += p.period;
            }
        }
        for (auto &a : aperTasks) {
            if (t == a.releaseTime) {
                a.remainingTime = a.compTime;
                a.deadline = t + 500;
            }
        }

        // Selection logic
        // Separate task selection for periodic and aperiodic
        taskPeriodic *chosenPeriodic = nullptr;
        taskAperiodic *chosenAperiodic = nullptr;
        int minDeadline = std::numeric_limits<int>::max();

        // Check Periodics
        for (auto &p : perTasks) {
            if (p.remainingTime > 0) {
                if (p.deadline < minDeadline) {
                    minDeadline = p.deadline;
                    chosenPeriodic = &p;
                    chosenAperiodic = nullptr;
                } else if (p.deadline == minDeadline && chosenPeriodic &&
                           p.id < chosenPeriodic->id) {
                    chosenPeriodic = &p;
                }
            }
        }

        // Check Aperiodics
        for (auto &a : aperTasks) {
            if (a.remainingTime > 0 && t >= a.releaseTime) {
                // Compares with current minDeadline
                if (a.deadline < minDeadline) {
                    minDeadline = a.deadline;
                    chosenAperiodic = &a;
                    chosenPeriodic = nullptr;
                }
                // If equal but current minDeadline is periodic, then aperiodic
                // is favored
                else if (a.deadline == minDeadline) {
                    if (chosenPeriodic != nullptr) {
                        chosenAperiodic = &a;
                        chosenPeriodic = nullptr;
                    }
                    // If current minDeadline is aperiodic, use ID tie-break
                    else if (chosenAperiodic != nullptr &&
                             a.id < chosenAperiodic->id) {
                        chosenAperiodic = &a;
                    }
                }
            }
        }

        // Execution logic
        // Track current task for logging and preemption
        std::string currentID = "";
        int currentRemaining = 0;

        if (chosenAperiodic) {
            currentID = chosenAperiodic->id;
            chosenAperiodic->remainingTime--;
            currentRemaining = chosenAperiodic->remainingTime;

            if (currentRemaining == 0) {
                totalAperiodicResponseTime +=
                    ((t + 1) - chosenAperiodic->releaseTime);
                compAperiodics++;
            }
        } else if (chosenPeriodic) {
            currentID = chosenPeriodic->id;
            chosenPeriodic->remainingTime--;
            currentRemaining = chosenPeriodic->remainingTime;
        }
        // For idle state
        else {
            currentID = "";
            currentRemaining = 0;
        }

        // Output logic
        // Start line with current time
        oFile << "t=" << t << ": ";

        // Check for deadline misses
        for (auto &p : perTasks) {
            if (t == p.deadline && p.remainingTime > 0) {
                oFile << "deadline miss " << p.id << " ";
                taskMisses[p.id]++;
                p.deadlineMisses++;
            }
        }
        for (auto &a : aperTasks) {
            if (t == a.deadline && a.remainingTime > 0) {
                oFile << "deadline miss " << a.id << " ";
                taskMisses[a.id]++;
                a.deadlineMisses++;
            }
        }

        // Log the running task
        if (currentID != "") {
            oFile << "running task " << currentID;
        } else {
            oFile << "idle";
        }

        // Check for preemption
        if (lastTaskID != "" && currentID != "" && lastTaskID != currentID) {
            if (lastTaskRemaining > 0) {
                totalPreemptions++;
                oFile << " preemption";
                for (auto &p : perTasks) {
                    if (p.id == lastTaskID) {
                        p.preemptions++;
                    }
                }
                for (auto &a : aperTasks) {
                    if (a.id == lastTaskID) {
                        a.preemptions++;
                    }
                }
            }
        }
        oFile << "\n";

        // Update trackers for next tick
        lastTaskID = currentID;
        lastTaskRemaining = currentRemaining;
    }

    // Summary logic
    // Calculate final stats and output to file
    double avgResponse =
        (compAperiodics > 0)
            ? (double)totalAperiodicResponseTime / compAperiodics
            : 0;
    oFile << "\n--- EDF Summary ---\n";

    oFile << "Task\tPreemptions\tDeadline Misses" << std::endl;

    for (taskPeriodic task : perTasks) {
        oFile << task.id << "\t\t" << task.preemptions << "\t\t\t"
              << task.deadlineMisses << std::endl;
        totalMisses += task.deadlineMisses;
    }

    for (taskAperiodic task : aperTasks) {
        oFile << task.id << "\t\t" << task.preemptions << "\t\t\t"
              << task.deadlineMisses << std::endl;
        totalMisses += task.deadlineMisses;
    }
    oFile << "Total\t" << totalPreemptions << "\t\t\t" << totalMisses;

    oFile << "\nAverage Aperiodic Response Time: " << avgResponse << " msec";
}

void llf(std::vector<taskPeriodic> &perTasks,
         std::vector<taskAperiodic> &aperTasks, int simTime,
         std::ofstream &oFile) {
    // Track various important values
    // For preemption and other needs
    std::string lastTaskID = "";
    int lastTaskRemaining = 0;

    // For summary
    int totalPreemptions = 0;
    int totalMisses = 0;
    int totalAperiodicResponseTime = 0;
    int compAperiodics = 0;
    std::map<std::string, int> taskMisses;

    // Time simulation
    for (int t = 0; t < simTime; t++) {
        // Release logic
        for (auto &p : perTasks) {
            if (t == p.nextRelease) {
                p.remainingTime = p.compTime;
                p.deadline = t + p.period;
                p.nextRelease += p.period;
            }
        }
        for (auto &a : aperTasks) {
            if (t == a.releaseTime) {
                a.remainingTime = a.compTime;
                a.deadline = t + 500;
            }
        }

        // Selection logic
        // Separate task selection for periodic and aperiodic
        taskPeriodic *chosenPeriodic = nullptr;
        taskAperiodic *chosenAperiodic = nullptr;
        int minLaxity = std::numeric_limits<int>::max();

        // Check Periodic Tasks first
        for (auto &p : perTasks) {
            if (p.remainingTime > 0) {
                int laxity = (p.deadline - t) - p.remainingTime;
                if (laxity < minLaxity) {
                    minLaxity = laxity;
                    chosenPeriodic = &p;
                }
                // If there is a tie, go based off of ID
                else if (laxity == minLaxity && chosenPeriodic) {
                    if (p.deadline < chosenPeriodic->deadline) {
                        chosenPeriodic = &p;
                    } else if (p.deadline == chosenPeriodic->deadline &&
                               p.id < chosenPeriodic->id) {
                        chosenPeriodic = &p;
                    }
                }
            }
        }

        // Check Aperiodic Tasks
        for (auto &a : aperTasks) {
            if (a.remainingTime > 0 && t >= a.releaseTime) {
                int aLaxity = (a.deadline - t) - a.remainingTime;

                // Compare Laxity
                if (aLaxity < minLaxity) {
                    minLaxity = aLaxity;
                    chosenAperiodic = &a;
                    chosenPeriodic = nullptr;
                }
                // If tied
                else if (aLaxity == minLaxity) {
                    // If the current minLaxity is Periodic, Aperiodic wins the
                    // tie-break
                    if (chosenPeriodic != nullptr) {
                        chosenAperiodic = &a;
                        chosenPeriodic = nullptr;
                    }
                    // If already another Aperiodic, check Deadline, then ID
                    else if (chosenAperiodic != nullptr) {
                        if (a.deadline < chosenAperiodic->deadline) {
                            chosenAperiodic = &a;
                        } else if (a.deadline == chosenAperiodic->deadline &&
                                   a.id < chosenAperiodic->id) {
                            chosenAperiodic = &a;
                        }
                    }
                }
            }
        }

        // Execution logic
        // Track current task for logging and preemption
        std::string currentID = "";
        int currentRemaining = 0;

        if (chosenAperiodic) {
            currentID = chosenAperiodic->id;
            chosenAperiodic->remainingTime--;
            currentRemaining = chosenAperiodic->remainingTime;

            if (currentRemaining == 0) {
                totalAperiodicResponseTime +=
                    ((t + 1) - chosenAperiodic->releaseTime);
                compAperiodics++;
            }
        } else if (chosenPeriodic) {
            currentID = chosenPeriodic->id;
            chosenPeriodic->remainingTime--;
            currentRemaining = chosenPeriodic->remainingTime;
        }
        // For idle state
        else {
            currentID = "";
            currentRemaining = 0;
        }

        // Output logic
        // Start line with current time
        oFile << "t=" << t << ": ";

        // Check for deadline misses
        for (auto &p : perTasks) {
            if (t == p.deadline && p.remainingTime > 0) {
                oFile << "deadline miss " << p.id << " ";
                taskMisses[p.id]++;
                p.deadlineMisses++;
            }
        }
        for (auto &a : aperTasks) {
            if (t == a.deadline && a.remainingTime > 0) {
                oFile << "deadline miss " << a.id << " ";
                taskMisses[a.id]++;
                a.deadlineMisses++;
            }
        }

        // Log the running task
        if (currentID != "") {
            oFile << "running task " << currentID;
        } else {
            oFile << "idle";
        }

        // Check for preemption
        if (lastTaskID != "" && currentID != "" && lastTaskID != currentID) {
            if (lastTaskRemaining > 0) {
                totalPreemptions++;
                oFile << " preemption";
                for (auto &p : perTasks) {
                    if (p.id == lastTaskID) {
                        p.preemptions++;
                    }
                }
                for (auto &a : aperTasks) {
                    if (a.id == lastTaskID) {
                        a.preemptions++;
                    }
                }
            }
        }
        oFile << "\n";

        // Update trackers for next tick
        lastTaskID = currentID;
        lastTaskRemaining = currentRemaining;
    }

    // Summary logic
    // Calculate final stats and output to file
    double avgResponse =
        (compAperiodics > 0)
            ? (double)totalAperiodicResponseTime / compAperiodics
            : 0;
    oFile << "\n--- LLF Summary ---\n";
    oFile << "Task\tPreemptions\tDeadline Misses" << std::endl;

    for (taskPeriodic task : perTasks) {
        oFile << task.id << "\t\t" << task.preemptions << "\t\t\t"
              << task.deadlineMisses << std::endl;
        totalMisses += task.deadlineMisses;
    }

    for (taskAperiodic task : aperTasks) {
        oFile << task.id << "\t\t" << task.preemptions << "\t\t\t"
              << task.deadlineMisses << std::endl;
        totalMisses += task.deadlineMisses;
    }
    oFile << "Total\t" << totalPreemptions << "\t\t\t" << totalMisses;

    oFile << "\nAverage Aperiodic Response Time: " << avgResponse << " msec\n";
}

void resetTasks(std::vector<taskPeriodic> &perTasks,
                std::vector<taskAperiodic> &aperTasks) {
    for (auto &p : perTasks) {
        p.remainingTime = 0;
        p.nextRelease = 0;
        p.deadline = 0;
        p.deadlineMisses = 0;
        p.preemptions = 0;
    }
    for (auto &a : aperTasks) {
        a.remainingTime = 0;
        a.deadline = 0;
        a.deadlineMisses = 0;
        a.preemptions = 0;
    }
}
