/*
      Header: AntSolution.h
      File: AntSolution.cpp
      Author: Nanashi
*/

#include "AntSolution.h"

AntSolution::AntSolution(
      double metaAlpha,
      const std::vector<std::vector<double>> &edgePheromone,
      const std::vector<std::vector<double>> &assignPheromone,
      double alphaFactor,
      double betaFactor,
      double gammaFactor): tau(edgePheromone), psi(assignPheromone), alpha(alphaFactor), beta(betaFactor), gamma(gammaFactor) {
      construct(metaAlpha);
}

void AntSolution::construct(double alpha) {

      // Data assignment
      partitions.assign(instance.nK, {});
      delta.assign(instance.nV, std::vector<double>(instance.nK));
      pheromoneSum.assign(instance.nV, std::vector<double>(instance.nK));
      w.assign(instance.nK, std::vector<double>(instance.nT));
      sigma.assign(instance.nK, double(0));
      objective = 0;
      infeasibility = 0;

      // Assertion
      if (instance.nV < instance.nK) {
            errorTermination("Invalid input: Number of vertices(" + std::to_string(instance.nV) + ") must be greater than number of clusters(" + std::to_string(instance.nK) + ")!");
      }

      // Construction
      constructGreedy(alpha);
}

bool AntSolution::explore(bool objective) {
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

double AntSolution::score(uint16_t vIndex, uint16_t cIndex) {
      return std::pow(pheromoneSum[vIndex][cIndex], alpha) * std::pow(psi[vIndex][cIndex], beta) * std::pow(1.0 / (1.0 + delta[vIndex][cIndex]), gamma);
}



void AntSolution::reconstruct(std::vector<std::vector<uint16_t>> &newPartitions) {
      // Data assignment
      partitions.assign(instance.nK, {});
      delta.assign(instance.nV, std::vector<double>(instance.nK));
      pheromoneSum.assign(instance.nV, std::vector<double>(instance.nK));
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