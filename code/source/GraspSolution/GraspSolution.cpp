/*
      File: GraspSolution.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

GraspSolution::GraspSolution(double alpha) {
      construct(alpha);
}

void GraspSolution::validate() {
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

void GraspSolution::construct(double alpha) {

      // Data assignment
      partitions.assign(instance.nK, {});
      delta.assign(instance.nV, std::vector<double>(instance.nK));
      w.assign(instance.nK, std::vector<double>(instance.nT));
      sigma.assign(instance.nV, double(0));
      objective = 0;
      infeasibility = 0;

      // Assertion
      if (instance.nV < instance.nK) {
            errorTermination("Invalid input: Number of vertices(" + std::to_string(instance.nV) + ") must be greater than number of clusters(" + std::to_string(instance.nK) + ")!");
      }

      // Construction
      if (parameters.consModel == "greedy-grasp") {
            constructGreedy(alpha);
      }
      else if (parameters.consModel == "random-grasp") {
            constructRandomized();
      }
      else {
            errorTermination("Invalid configuration: Construction model \"" + parameters.consModel + "\" is not available!");
      }
}

bool GraspSolution::explore(bool objective) {
      if (parameters.searMove == "insert") {
            return exploreInsert(objective);
      }
      else if (parameters.searMove == "exchange") {
            return exploreExchange(objective);
      }
      else {
            assert(parameters.searMove == "extended");
            return exploreExtended(objective);
      }
}
