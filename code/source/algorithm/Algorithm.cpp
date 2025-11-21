/*
      File: Algorithm.cpp
      Author: Nanashi
*/

#include "Algorithm.h"

// Constructor, destructor

Algorithm::Algorithm(const std::string& pathInstance) {
      islands = new Island *[parameters.nIsland];
      for (size_t i = 0; i < parameters.nIsland; i++) {
            islands[i] = new Island();
      }

      if (parameters.logs > 0) {
            std::vector<std::string> tokens = split(pathInstance + '/', '/');
            tokens = split(tokens.back(), '.');
            std::string instance = tokens[0];

            std::string base_directory = "results/logs/pred_logs/";

            if (parameters.searModel == "2P-R-HGRASP-DP") {
                  base_directory = "results/logs/dp_logs/";
            }
            else if (parameters.searModel == "2P-R-GRASP-DP") {
                  base_directory = "results/logs/hybrid_logs/";
            }
            else if (parameters.searModel != "2P-R-GRASP") {
                  errorTermination("Invalid search model: " + parameters.searModel + "\nAvailable search models: 2P-R-GRASP, 2P-R-HGRASP-DP, 2P-R-GRASP-DP\n");
            }

            fileEvol.open(base_directory + "evolution/" + instance + "_" + std::to_string(parameters.seed) + ".txt");
            if (!fileEvol) {
                  errorTermination("The fileEvol file can not be opened.");
            }

            fileSolu.open(base_directory + "solutions/" + instance + "_" + std::to_string(parameters.seed) + ".txt");
            if (!fileSolu) {
                  errorTermination("The fileSolu file can not be opened.");
            }

            fileCost.open(base_directory + "objectives/" + instance + "_" + std::to_string(parameters.seed) + ".txt");
            if (!fileEvol) {
                  errorTermination("The fileCost file can not be opened.");
            }
      }
}

Algorithm::~Algorithm() {
      for (size_t i = 0; i < parameters.nIsland; i++) {
            delete islands[i];
      }
      delete[] islands;

      if (parameters.logs) {
            fileEvol.close();
            fileSolu.close();
            fileCost.close();
      }
}

// Public functions

void Algorithm::Iterate() {
      #pragma omp parallel for if(parameters.parallelEnabled)
      for (uint32_t i = 0; i < parameters.nIsland; i++) {
            islands[i] -> Iterate();
      }
}

Metaheuristic* Algorithm::getBest() {
      Metaheuristic* result = islands[0] -> getBest();
      for (uint16_t i = 1; i < parameters.nIsland; i++) {
            Metaheuristic* candidate = islands[i] -> getBest();
            if (*candidate < *result) {
                  std::swap(result, candidate);
            }
      }
      return result;
}

void Algorithm::printLogs(uint32_t &iter, double &time_used) {
      accumulated_time_cpu.emplace_back(time_used);
      min_cost.emplace_back(std::vector<double>(parameters.nIsland));
      auto &vector_cost = min_cost.back();
      for (uint16_t i = 0; i < parameters.nIsland; i++) {
            vector_cost[i] = islands[i] -> getBest() -> getObjective();
      }

      // Print logs every "parameters.logs" iters.
      if (iter % parameters.logs == 0) {
            std::cout << "  Iteration" << std::setw(12) << iter << "      Cost";
            for (uint16_t i = 0; i < parameters.nIsland; i++) {
                  std::cout << std::setw(16) << std::fixed << std::setprecision(6) << vector_cost[i] << std::endl;
            }
      }
}

void Algorithm::saveLogs() {

      // Saves the algorithm evolution to file
      for (uint32_t i = 0; i < accumulated_time_cpu.size(); i++) {
            fileEvol << std::setw(10) << std::fixed << std::setprecision(4) << accumulated_time_cpu[i];
            for (double &cost : min_cost[i]) {
                  fileEvol << std::setw(16) << std::fixed << std::setprecision(6) << cost;
            }
            fileEvol << std::endl;
      }

      // Saves the best cost to file
      Metaheuristic* best = getBest();
      fileCost << std::fixed << std::setprecision(2) << best -> getObjective();

      // Saves the best solution to file
      const auto &partitions = best -> getPartitions();
      assert(partitions.size() == instance.nK);
      for (uint16_t cIndex = 0; cIndex < instance.nK; cIndex++) {
            for (size_t i = 0; i < size_t(partitions[cIndex].size()); i++) {
                  std::cout << partitions[cIndex][i] << " \n"[i + 1 == partitions[cIndex].size()];
            }
      }
}

// Utils

std::vector<std::string> Algorithm::split(const std::string& str, char delimiter) {
      std::vector<std::string> tokens;
      std::stringstream ss(str);
      std::string token;
      while (getline(ss, token, delimiter)) {
            tokens.push_back(token);
      }
      return tokens;
}