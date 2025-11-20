/*
      File: DpSolution.cpp
      Author: Nanashi
*/

#include "DpSolution.h"

DpSolution::DpSolution() {
}

void DpSolution::validate() {
      for (uint16_t i = 0; i < instance.nK; i++) {
            if (partitions[i].empty()) {
                  errorTermination("Invalid solution detected: Empty cluster!");
            }
            double accumulated_attributes[instance.nT] = {};
            for (uint16_t &index : partitions[i]) {
                  if (index < 0 or instance.nV <= index) {
                        errorTermination("Invalid solution detected: Vertex does not exists!");
                  }
                  for (uint16_t t = 0; t < instance.nT; t++) {
                        accumulated_attributes[t] += instance.W[index][t];
                  }
            }
            for (uint16_t t = 0; t < instance.nT; t++) {
                  if (accumulated_attributes[t] + parameters.eps < instance.Wl[i][t] or instance.Wu[i][t] < accumulated_attributes[t] - parameters.eps) {
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