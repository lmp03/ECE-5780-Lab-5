#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "rate.hpp"
#include "edf_lst.hpp"

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

int main(int argc, char** argv) {
        std::string iFileName(argv[1]);
        std::string oFileName(argv[2]);

        std::ifstream iFile(iFileName);
        std::ofstream oFile(oFileName);

        std::string line;

        std::getline(iFile, line);
        int numTasks = stoi(line);
        std::getline(iFile, line);
        int simTime = stoi(line);

        std::vector<taskPeriodic> periodicTaskList;

        for (int i = 0; i < numTasks; i++) {
                std::getline(iFile, line);

                std::stringstream ss(line);
                std::string temp;
                std::string id;
                int compTime;
                int period;

                std::getline(ss, id, ',');
                std::getline(ss >> std::ws, temp, ',');
                compTime = stoi(temp);
                std::getline(ss >> std::ws, temp);
                period = stoi(temp);

                taskPeriodic t = {id, compTime, period};
                periodicTaskList.emplace_back(t);
        }

        for (taskPeriodic t : periodicTaskList) {
                std::cout << t.id << std::endl;
                std::cout << t.compTime << std::endl;
                std::cout << t.period << std::endl;
        }

        std::getline(iFile, line);
        int numTasksAperiodic = stoi(line);

        std::vector<taskAperiodic> aperiodicTaskList;

        for (int i = 0; i < numTasksAperiodic; i++) {
                std::getline(iFile, line);

                std::stringstream ss(line);
                std::string temp;
                std::string id;
                int compTime;
                int releaseTime;

                std::getline(ss, id, ',');
                std::getline(ss >> std::ws, temp, ',');
                compTime = stoi(temp);
                std::getline(ss >> std::ws, temp);
                releaseTime = stoi(temp);

                taskAperiodic t = {id, compTime, releaseTime};
                aperiodicTaskList.emplace_back(t);
        }

        for (taskAperiodic t : aperiodicTaskList) {
                std::cout << t.id << std::endl;
                std::cout << t.compTime << std::endl;
                std::cout << t.releaseTime << std::endl;
        }
}
