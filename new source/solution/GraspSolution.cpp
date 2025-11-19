/*
      File: GraspSolution.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

Solution::Solution(double alpha) {
      construct(alpha);
}

void Solution::normalize() {
      if (normalized) {
            return;
      }
}

void Solution::validate() {
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
                  if (accumulatedAttributes[t] + parameters.eps < instance.Wl[i][t] or instance.Wu[i][t] < accumulatedAttributes[t] - parameters.eps) {
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

void Solution::construct(double alpha) {
      partitions.assign(instance.nK, std::vector<uint16_t>());
}