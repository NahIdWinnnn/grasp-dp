/*
      Header: HybridGraspPhase1.h
      File: HybridGraspPhase1.cpp
      Author: Nanashi
*/

#include "HybridGraspPhase1.h"

HybridGraspPhase1::HybridGraspPhase1() {
      rho = 0.1;
      Q = 100.0;
      alpha = 1.2;
      beta = 0.5;
      gamma = 1.0;
      tauMax = 5.0;
      tauMin = tauMax / 200.0;

      edgePheromone.assign(instance.nV, std::vector<double>(instance.nV, tauMax));
      assignPheromone.assign(instance.nV, std::vector<double>(instance.nK, tauMax));
}

void HybridGraspPhase1::updatePheromones(BaseSolution *solution) {

      double cost = solution -> objective;
      if (!solution -> checkFeasibility()) {
            cost += 1000.0 * solution -> infeasibility;
      }

      if (cost < parameters.eps) {
            cost = parameters.eps;
      }

      // MMAS: Update tauMax, tauMin dynamically
      double newTauMax = 1.0 / (rho * cost);
      tauMax = Q * newTauMax;
      tauMin = tauMax / 200.0;

      // MMAS: Evaporating
      for (auto &row : edgePheromone) {
            for (auto &val : row) {
                  val *= (1.0 - rho);
            }
      }
      for (auto &row : assignPheromone) {
            for (auto &val : row) {
                  val *= (1.0 - rho);
            }
      }

      // MMAS: Depositing
      double reward = tauMax / cost;

      const auto &partitions = solution -> partitions;
      for (uint16_t cIndex = 0; cIndex < instance.nK; cIndex++) {
            const auto &cluster = partitions[cIndex];

            // A. Update assign pheromone
            for (const uint16_t &vIndex : cluster) {
                  assignPheromone[vIndex][cIndex] += reward;
            }

            // B. Update edge pheromone
            for (size_t i = 0; i < cluster.size(); i++) {
                  for (size_t j = i + 1; j < cluster.size(); j++) {
                        uint16_t u = cluster[i];
                        uint16_t v = cluster[j];
                        edgePheromone[u][v] += reward;
                        edgePheromone[v][u] += reward;
                  }
            }
      }

      // MMAS: Clamping
      for (auto &row : edgePheromone) {
            for (auto &val : row) {
                  if (val > tauMax) {
                        val = tauMax;
                  }
                  if (val < tauMin) {
                        val = tauMin;
                  }
            }
      }
      for (auto &row : assignPheromone) {
            for (auto &val : row) {
                  if (val > tauMax) {
                        val = tauMax;
                  }
                  if (val < tauMin) {
                        val = tauMin;
                  }
            }
      }
}

void HybridGraspPhase1::resetPheromones() {
      for (auto &row : edgePheromone) {
            std::fill(row.begin(), row.end(), tauMax);
      }
      for (auto &row : assignPheromone) {
            std::fill(row.begin(), row.end(), tauMax);
      }

      std::cerr << "[HybridGraspPhase1] Pheromones Reset!" << std::endl;
}

BaseSolution* HybridGraspPhase1::construct(double metaAlpha) {
      bool feasible = false;
      BaseSolution *newSolution = nullptr;

      while (!feasible) {
            if (newSolution) {
                  delete newSolution;
            }
            newSolution = new AntSolution(metaAlpha, edgePheromone, assignPheromone, alpha, beta, gamma);
            while (newSolution -> explore(false)) {
                  continue;
            }
            if (std::abs(newSolution -> infeasibility) < parameters.eps) {
                  feasible = true;
            }
      }
      return newSolution;
}