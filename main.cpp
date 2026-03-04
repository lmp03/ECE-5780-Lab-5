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
        std::getline(iFile, line);

        // Helpers to store task info before it's added to the vector
        std::stringstream ss(line);
        std::string temp;
        std::string id;
        int compTime;
        int period;

        // Read from line until first comma, save as id
        std::getline(ss, id, ',');
        // Read from first comma until second comma, save as compTime
        std::getline(ss >> std::ws, temp, ',');
        compTime = stoi(temp);
        // Read from second comma until end, save as period
        std::getline(ss >> std::ws, temp);
        period = stoi(temp);

        // Create task and add to vector
        taskPeriodic t = {id, compTime, period};
        periodicTaskList.emplace_back(t);
    }

    // Debug print
    for (taskPeriodic t : periodicTaskList) {
        std::cout << t.id << std::endl;
        std::cout << t.compTime << std::endl;
        std::cout << t.period << std::endl;
    }

    // Number of Aperiodic tasks
    int numTasksAperiodic;

    // Read in number of Aperiodic tasks, check validity
    std::getline(iFile, line);
    try {
        numTasksAperiodic = stoi(line, &pos);
        if (pos != line.length()) {
            std::cerr << "Line specifying number of aperiodic tasks must "
                         "contain a single integer"
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }
    } catch (const std::invalid_argument &) {
        std::cerr << "Line specifying number of aperiodic tasks must contain a "
                     "single integer"
                  << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (const std::out_of_range &) {
        std::cerr << "Line specifying number of aperiodic tasks is not a valid "
                     "integer"
                  << std::endl;
    }

    // Store list of Aperiodic tasks
    std::vector<taskAperiodic> aperiodicTaskList;

    // Read in Aperiodic tasks
    for (int i = 0; i < numTasksAperiodic; i++) {
        // Read in line
        std::getline(iFile, line);

        // Helpers to store task info before it's added to the vector
        std::stringstream ss(line);
        std::string temp;
        std::string id;
        int compTime;
        int releaseTime;

        // Read from start of line to first comma, save as id
        std::getline(ss, id, ',');
        // Read from first comma to second comma, save as compTime
        std::getline(ss >> std::ws, temp, ',');
        compTime = stoi(temp);
        // Read from second comma to end, save as release time
        std::getline(ss >> std::ws, temp);
        releaseTime = stoi(temp);

        // Create task and add to vector
        taskAperiodic t = {id, compTime, releaseTime};
        aperiodicTaskList.emplace_back(t);
    }

    // Debug print
    for (taskAperiodic t : aperiodicTaskList) {
        std::cout << t.id << std::endl;
        std::cout << t.compTime << std::endl;
        std::cout << t.releaseTime << std::endl;
    }
}
