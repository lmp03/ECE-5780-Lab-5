#include "edf_lst.hpp"
#include "rate.hpp"
#include "structDef.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
    // Check that input and output files were specified
    if (argc != 3) {
        std::cerr << "Invalid number of arguments" << std::endl;
        std::cerr << "1st argument should be an input file" << std::endl;
        std::cerr << "2nd argument should be an output file" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string iFileName(argv[1]);
    std::string oFileName(argv[2]);

    // Open input file
    std::ifstream iFile(iFileName);
    if (!iFile) {
        std::cerr << "Could not open input file " << iFileName << std::endl;
        std::cerr << "Are you sure it exists?" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Open output file
    std::ofstream oFile(oFileName);
    if (!oFile) {
        std::cerr << "Could not open output file " << oFileName << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Helpers to read in lines
    std::string line;
    size_t pos;

    // Number of tasks - first line of input
    int numTasks;
    // Simulation time - second line of input
    int simTime;

    // Read in first line
    std::getline(iFile, line);
    try {
        numTasks = stoi(line, &pos);
        if (pos != line.length()) {
            std::cerr << "First line of input must contain a single integer"
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }
    } catch (const std::invalid_argument &) {
        std::cerr << "First line of input must contain a single integer"
                  << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (const std::out_of_range &) {
        std::cerr << "First line of input is not a valid integer" << std::endl;
    }

    // Read in second line
    std::getline(iFile, line);
    try {
        simTime = stoi(line, &pos);
        if (pos != line.length()) {
            std::cerr << "Second line of input must contain a single integer"
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }
    } catch (const std::invalid_argument &) {
        std::cerr << "Second line of input must contain a single integer"
                  << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (const std::out_of_range &) {
        std::cerr << "Second line of input is not a valid integer" << std::endl;
    }

    // Store periodic tasks
    std::vector<taskPeriodic> periodicTaskList;

    // Read in periodic tasks
    for (int i = 0; i < numTasks; i++) {
        // Read in line

        if (!std::getline(iFile, line)) {
            std::cerr << "Not enough tasks specified. " << numTasks
                      << " tasks expected" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Helpers to store task info before it's added to the vector
        std::stringstream ss(line);
        std::string temp;
        std::string id;
        int compTime;
        int period;

        // Read from line until first comma, save as id
        // ss is a stringstream copy of line, getline reads from current
        // position in ss to the first comma if it fails, there was no line to
        // read, or there was no comma if it succeeds, whatever it reads is
        // stored as the id
        if (!std::getline(ss, id, ',')) {
            std::cerr << "No id to read, or no comma to separate id from other "
                         "task information. \n Line: "
                      << line << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Read from first comma until second comma, save as compTime
        // read same as before, ss remembers starting position from last getline
        // call
        // >> std::ws removes leading whitespace
        // save the value in temp. We'll convert it to an int later
        if (!std::getline(ss >> std::ws, temp, ',')) {
            std::cerr
                << "No computation time to read, or no comma to separate "
                   "computation time from other task information. \n Line:"
                << line << std::endl;
            std::exit(EXIT_FAILURE);
        }

        try {
            compTime = std::stoi(temp, &pos);
            if (pos != temp.length() || compTime <= 0) {
                throw std::invalid_argument("Invalid computation time");
            }
        } catch (...) {
            std::cerr << "Invalid computation time in line: " << line
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Read period
        if (!std::getline(ss >> std::ws, temp)) {
            std::cerr << "No period to read following second comma. \n Line: "
                      << line << std::endl;
            std::exit(EXIT_FAILURE);
        }

        try {
            period = std::stoi(temp, &pos);
            if (pos != temp.length() || period <= 0) {
                throw std::invalid_argument("Invalid period");
            }
        } catch (...) {
            std::cerr << "Invalid period in line: " << line << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Check for extra stuff after period
        // if ss >> temp is true, there was still something in ss to be put into
        // temp.
        if (ss >> temp) {
            std::cerr << "Extra data in task line: " << line << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // We've checked everything, it's all ok
        // Create task and add to vector
        taskPeriodic t = {id, compTime, period};
        periodicTaskList.emplace_back(t);
    }

    // Number of Aperiodic tasks
    int numTasksAperiodic;

    // Read in number of Aperiodic tasks, check validity
    // If there's nothing to read, that means that there's only periodic tasks
    // If so, just set numTasksAperiodic to 0
    if (std::getline(iFile, line)) {
        if (!line.empty()) {
            try {
                numTasksAperiodic = stoi(line, &pos);
                if (pos != line.length()) {
                    std::cerr
                        << "Line specifying number of aperiodic tasks must "
                           "contain a single integer\n It is possible you have "
                           "given more periodic tasks than expected"
                        << std::endl;
                    std::exit(EXIT_FAILURE);
                }
            } catch (const std::invalid_argument &) {
                std::cerr << "Line specifying number of aperiodic tasks must "
                             "contain a single integer\n It is possible you "
                             "have given more periodic tasks than expected"
                          << std::endl;
                std::exit(EXIT_FAILURE);
            } catch (const std::out_of_range &) {
                std::cerr << "Line specifying number of aperiodic tasks is not "
                             "a valid integer"
                          << std::endl;
            }
        } else {
            numTasksAperiodic = 0;
        }
    } else {
        numTasksAperiodic = 0;
    }

    // Store list of Aperiodic tasks
    std::vector<taskAperiodic> aperiodicTaskList;

    // Read in Aperiodic tasks
    for (int i = 0; i < numTasksAperiodic; i++) {
        // Read in line
        if (!std::getline(iFile, line)) {
            std::cerr << "Not enough aperiodic tasks specified. "
                      << numTasksAperiodic << " tasks expected" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Helpers to store task info before it's added to the vector
        std::stringstream ss(line);
        std::string temp;
        std::string id;
        int compTime;
        int releaseTime;

        // Same process to read in tasks as with periodic tasks
        // But with release time instead of period

        if (!std::getline(ss, id, ',')) {
            std::cerr << "No id to read, or no comma to separate id from other "
                         "task information. \n Line: "
                      << line << std::endl;
            std::exit(EXIT_FAILURE);
        }

        if (!std::getline(ss >> std::ws, temp, ',')) {
            std::cerr
                << "No computation time to read, or no comma to separate "
                   "computation time from other task information. \n Line:"
                << line << std::endl;
            std::exit(EXIT_FAILURE);
        }

        try {
            compTime = std::stoi(temp, &pos);
            if (pos != temp.length() || compTime <= 0) {
                throw std::invalid_argument("Invalid computation time");
            }
        } catch (...) {
            std::cerr << "Invalid computation time in line: " << line
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }

        if (!std::getline(ss >> std::ws, temp)) {
            std::cerr
                << "No release time to read following second comma. \n Line: "
                << line << std::endl;
            std::exit(EXIT_FAILURE);
        }

        try {
            releaseTime = std::stoi(temp, &pos);
            if (pos != temp.length() || releaseTime < 0) {
                throw std::invalid_argument("Invalid release time");
            }
        } catch (...) {
            std::cerr << "Invalid release time in line: " << line << std::endl;
            std::exit(EXIT_FAILURE);
        }

        if (ss >> temp) {
            std::cerr << "Extra data in task line: " << line << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Create task and add to vector
        taskAperiodic t = {id, compTime, releaseTime};
        aperiodicTaskList.emplace_back(t);
    }

    std::cout << "Tasks read in successfully" << std::endl;
    rateMonotonic(periodicTaskList, aperiodicTaskList, simTime, oFile);
}
