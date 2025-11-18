/*
      File: GraspSolution.h
      Author: Nanashi
*/

#include "../includes.h"

class Solution {

public:

      // Constructor, destructor
      Solution();
      virtual ~Solution();

      // Operators redefinition
      bool operator < (const Solution &solution) const {
            return objective < solution.objective;
      }
      bool operator > (const Solution &solution) const {
            return objective > solution.objective;
      }

      // Retrieval functions
      std::vector<std::vector<uint16_t>> getPartitions() {
            return partitions;
      }
      double getObjective() {
            return objective;
      }

      // Validation functions
      bool isNormalized() {
            return normalized;
      }
      void normalize();
      void validate() {
            for (uint16_t i = 0; i < instance.nK; i++) {
                  if (partitions[i].empty()) {
                        errorTermination("Invalid solution detected: Empty cluster!");
                  }
                  double accumulatedAttributes[instance.nT] = {};
                  for (uint16_t &index : partitions[i]) {
                        if (index < 0 or instance.nV <= index) {
                              errorTermination("Invalid solution detected: Vertex does not exists!");
                        }
                        for (uint16_t t = 0; t < instance.nT; t++) {
                              accumulatedAttributes[t] += instance.W[index][t];
                        }
                  }
                  for (uint16_t t = 0; t < instance.nT; t++) {
                        if (accumulatedAttributes[t] + (1e-9) < instance.Wl[i][t] and instance.Wu[i][t] < accumulatedAttributes[t] - (1e-9)) {
                              errorTermination("Invalid solution detected: Out of bound!");
                        }
                  }
            }

            std::vector<uint16_t> used(instance.nV);
            for (uint16_t i = 0; i < instance.nK; i++) {
                  for (uint16_t &index : partitions[i]) {
                        used[index] += 1;
                        if (used[index] > 1) {
                              errorTermination("Invalid solution detected: Duplicate vertices!");
                        }
                  }
            }

            if (int count = std::accumulate(used.begin(), used.end(), 0); count != instance.nV) {
                  assert(count < instance.nV);
                  errorTermination("Invalid solution detected: Insufficient vertex count in solution!");
            }
      }

private:

      // Private attributes
      std::vector<std::vector<uint16_t>> partitions;
      double objective;
      bool normalized;

      // Algorithmic attributes
      std::vector<std::vector<std::pair<uint16_t, uint16_t>>> bounds;
};
