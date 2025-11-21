/*
      Header: GraspSolution.h
      File: exchanges.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

bool GraspSolution::exploreExchange(bool objective) {

      // Initialization
      std::vector<uint16_t> fClusSet(instance.nK), sClusSet(instance.nK);
      for (uint16_t i = 0; i < instance.nK; i++) {
            fClusSet[i] = sClusSet[i] = i;
      }
      randomShuffle(fClusSet);
      randomShuffle(sClusSet);

      std::vector<std::vector<uint16_t>> orders(instance.nK);
      for (uint16_t i = 0; i < instance.nK; i++) {
            orders[i].assign(partitions[i].size(), 0);
            std::iota(orders[i].begin(), orders[i].end(), 0);
            randomShuffle(orders[i]);
      }

      // Choose best [fPos, fClus, sPos, sClus]
      bool improved = false;
      double bestDer = 0;
      uint16_t bestFPos, bestFClus, bestSPos, bestSClus;

      for (uint16_t &fClus : fClusSet) {
            std::vector<uint16_t> &fPosSet = orders[fClus];

            for (uint16_t &fPos : fPosSet) {
                  uint16_t fVertex = partitions[fClus][fPos];

                  for (uint16_t &sClus : sClusSet) {
                        if (fClus == sClus) {
                              continue;
                        }

                        std::vector<uint16_t> &sPosSet = orders[sClus];

                        for (uint16_t &sPos : sPosSet) {
                              uint16_t sVertex = partitions[sClus][sPos];

                              if (fVertex >= sVertex) {
                                    continue;
                              }

                              if (objective) {
                                    if (isFeasibleExchange(fPos, fClus, sPos, sClus)) {
                                          double newDer = evaluateExchange(objective, fPos, fClus, sPos, sClus);
                                          if (newDer < bestDer) {
                                                std::swap(bestDer, newDer), bestFPos = fPos, bestFClus = fClus, bestSPos = sPos, bestSClus = sClus;
                                                improved = true;
                                          }
                                    }
                              }
                              else {
                                    double newDer = evaluateExchange(objective, fPos, fClus, sPos, sClus);
                                    if (newDer < bestDer) {
                                          std::swap(bestDer, newDer), bestFPos = fPos, bestFClus = fClus, bestSPos = sPos, bestSClus = sClus;
                                          improved = true;
                                    }
                              }

                              if (improved) {
                                    if (!objective and parameters.consMoveStrat == "first") {
                                          goto end_loops;
                                    }
                                    if (objective and parameters.searMoveStrat == "first") {
                                          goto end_loops;
                                    }
                              }
                        }
                  }
            }
      }

      end_loops:;

      // Update solution data
      if (improved) {
            exchangeVertex(bestFPos, bestFClus, bestSPos, bestSClus);
      }

      return improved;
}

bool GraspSolution::isFeasibleExchange(uint16_t fPos, uint16_t fClus, uint16_t sPos, uint16_t sClus) {
      uint16_t fVertex = partitions[fClus][fPos];
      uint16_t sVertex = partitions[sClus][sPos];
      for (uint16_t t = 0; t < instance.nT; t++) {
            double aux_w = w[fClus][t] - instance.W[fVertex][t] + instance.W[sVertex][t];
            if (aux_w + parameters.eps < instance.Wl[fClus][t] or instance.Wu[fClus][t] < aux_w - parameters.eps) {
                  return false;
            }
            aux_w = w[sClus][t] - instance.W[sVertex][t] + instance.W[fVertex][t];
            if (aux_w + parameters.eps < instance.Wl[sClus][t] or instance.Wu[sClus][t] < aux_w - parameters.eps) {
                  return false;
            }
      }
      return true;
}

double GraspSolution::evaluateExchange(bool objective, uint16_t fPos, uint16_t fClus, uint16_t sPos, uint16_t sClus) {
      uint16_t fVertex = partitions[fClus][fPos];
      uint16_t sVertex = partitions[sClus][sPos];
      if (!objective) {
            // Evaluating infeasibility
            double delta_w = 0;
            for (uint16_t t = 0; t < instance.nT; t++) {
                  double aux_w = w[fClus][t] - instance.W[fVertex][t] + instance.W[sVertex][t];
                  if (aux_w + parameters.eps < instance.Wl[fClus][t]) {
                        delta_w += instance.Wl[fClus][t] - aux_w;
                  }
                  else if (instance.Wu[fClus][t] < aux_w - parameters.eps) {
                        delta_w += aux_w - instance.Wu[fClus][t];
                  }

                  aux_w = w[sClus][t] - instance.W[sVertex][t] + instance.W[fVertex][t];
                  if (aux_w + parameters.eps < instance.Wl[sClus][t]) {
                        delta_w += instance.Wl[sClus][t] - aux_w;
                  }
                  else if (instance.Wu[sClus][t] < aux_w - parameters.eps) {
                        delta_w += aux_w - instance.Wu[sClus][t];
                  }
            }
            return delta_w - sigma[fClus] - sigma[sClus];
      }
      else {
            // Evaluating objective
            return (delta[fVertex][sClus] - delta[fVertex][fClus]) + (delta[sVertex][fClus] - delta[sVertex][sClus]) - 2 * (instance.D[fVertex][sVertex] + instance.D[sVertex][fVertex]);
      }
}
