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

      if (parameters.logs) {
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
            else {
                  errorTermination("Invalid search model: " + parameters.searModel + "\nAvailable search models: 2P-R-GRASP, 2P-R-HGRASP-DP, 2P-R-GRASP-DP\n");
            }

            fileEvol.open(base_directory + "evolution/" + instance + "_" + std::to_string(parameters.seed) + "_" + parameters.searModel + ".txt");
            if (!fileEvol) {
                  errorTermination("The fileEvol file can not be opened.");
            }

            fileSolu.open(base_directory + "solutions/" + instance + "_" + std::to_string(parameters.seed) + "_" + parameters.searModel + ".txt");
            if (!fileSolu) {
                  errorTermination("The fileSolu file can not be opened.");
            }

            fileCost.open(base_directory + "objectives/" + instance + "_" + std::to_string(parameters.seed) + "_" + parameters.searModel + ".txt");
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