/*
      File: GraspSolution.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

GraspSolution::GraspSolution(double alpha) {
      construct(alpha);
}

void GraspSolution::reconstruct(std::vector<std::vector<uint16_t>> &newPartitions) {
      // Data assignment
      partitions.assign(instance.nK, {});
      delta.assign(instance.nV, std::vector<double>(instance.nK));
      w.assign(instance.nK, std::vector<double>(instance.nT));
      sigma.assign(instance.nK, double(0));
      objective = 0;
      infeasibility = 0;

      for (uint16_t cIndex = 0; cIndex < instance.nK; cIndex++) {
            for (uint16_t &vIndex : newPartitions[cIndex]) {
                  addVertex(vIndex, cIndex);
            }
      }
}

void GraspSolution::construct(double alpha) {

      // Data assignment
      partitions.assign(instance.nK, {});
      delta.assign(instance.nV, std::vector<double>(instance.nK));
      w.assign(instance.nK, std::vector<double>(instance.nT));
      sigma.assign(instance.nK, double(0));
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
