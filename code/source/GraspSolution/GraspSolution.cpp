/*
      File: GraspSolution.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

GraspSolution::GraspSolution(double alpha) {
      if (alpha > 1) {
            objective = std::numeric_limits<double>::max();
      }
      construct(alpha);
}

void GraspSolution::validate() {
      for (uint16_t i = 0; i < instance.nK; i++) {
            if (partitions[i].empty()) {
                  errorTermination("Invalid solution detected: Empty cluster!");
            }
            double accumulated_attributes[instance.nT] = {};
            for (uint16_t &index : partitions[i]) {
                  if (instance.nV <= index) {
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
            auto [der, vPos, source, target] = exploreInsert(objective);
            if (der < 0) {
                  insertVertex(vPos, source, target);
                  return true;
            }
      }
      else if (parameters.searMove == "exchange") {
            auto [der, fPos, fClus, sPos, sClus] = exploreExchange(objective);
            if (der < 0) {
                  exchangeVertex(fPos, fClus, sPos, sClus);
                  return true;
            }
      }
      else {
            assert(parameters.searMove == "extended");
            auto [derI, vPos, source, target] = exploreInsert(objective);
            auto [derE, fPos, fClus, sPos, sClus] = exploreExchange(objective);
            if (derI < 0 or derE < 0) {
                  if (derI < derE) {
                        insertVertex(vPos, source, target);
                  }
                  else {
                        exchangeVertex(fPos, fClus, sPos, sClus);
                  }
                  return true;
            }
      }

      return false;
}
