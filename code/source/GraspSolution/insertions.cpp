/*
      Header: GraspSolution.h
      File: insertions.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

bool GraspSolution::exploreInsert(bool objective) {

      // Initialization
      std::vector<uint16_t> sourceSet(instance.nK), targetSet(instance.nK);
      for (uint16_t i = 0; i < instance.nK; i++) {
            sourceSet[i] = targetSet[i] = i;
      }

      // Choose best [vPos, source, target]
      bool improved = false;
      double bestDer = 0;
      uint16_t bestPos, bestSource, bestTarget;
      for (uint16_t &source : sourceSet) {
            std::vector<uint16_t> vPosSet(partitions[source].size());
            std::iota(vPosSet.begin(), vPosSet.end(), 0), randomShuffle(vPosSet);

            for (uint16_t &vPos : vPosSet) {
                  for (uint16_t &target : targetSet) {
                        if (source == target) {
                              continue;
                        }
                        if (objective) {
                              if (isFeasibleInsert(vPos, source, target)) {
                                    double newDer = evaluateInsert(objective, vPos, source, target);
                                    if (newDer < bestDer) {
                                          std::swap(bestDer, newDer), bestPos = vPos, bestSource = source, bestTarget = target;
                                    }
                              }
                        }
                        else {
                              double newDer = evaluateInsert(objective, vPos, source, target);
                              if (newDer < bestDer) {
                                    std::swap(bestDer, newDer), bestPos = vPos, bestSource = source, bestTarget = target;
                              }
                        }

                        if (improved) {
                              if (!objective and parameters.consMoveStrat == "first") {
                                    break;
                              }
                              if (objective and parameters.searMoveStrat == "first") {
                                    break;
                              }
                        }
                  }
                  if (improved) {
                        if (!objective and parameters.consMoveStrat == "first") {
                              break;
                        }
                        if (objective and parameters.searMoveStrat == "first") {
                              break;
                        }
                  }
            }
            if (improved) {
                  if (!objective and parameters.consMoveStrat == "first") {
                        break;
                  }
                  if (objective and parameters.searMoveStrat == "first") {
                        break;
                  }
            }
      }

      // Update solution data
      if (improved) {
            insertVertex(bestPos, bestSource, bestTarget);
      }

      return improved;
}

bool GraspSolution::isFeasibleInsert(uint16_t vPos, uint16_t source, uint16_t target) {
      uint16_t vIndex = partitions[source][vPos];
      for (uint16_t t = 0; t < instance.nT; t++) {
            if (w[source][t] - instance.W[vIndex][t] + parameters.eps < instance.Wl[source][t] or instance.Wu[target][t] < w[target][t] + instance.W[vIndex][t] - parameters.eps) {
                  return false;
            }
      }
      return true;
}

double GraspSolution::evaluateInsert(bool objective, uint16_t vPos, uint16_t source, uint16_t target) {
      uint16_t vIndex = partitions[source][vPos];
      if (!objective) {
            // Evaluating infeasibility
            double delta_w = 0;
            for (uint16_t t = 0; t < instance.nT; t++) {
                  double aux_w = w[source][t] - instance.W[vIndex][t];
                  if (aux_w + parameters.eps < instance.Wl[source][t]) {
                        delta_w += instance.Wl[source][t] - aux_w;
                  }
                  else if (instance.Wu[source][t] < aux_w - parameters.eps) {
                        delta_w += aux_w - instance.Wu[source][t];
                  }

                  aux_w = w[target][t] + instance.W[vIndex][t];
                  if (aux_w + parameters.eps < instance.Wl[target][t]) {
                        delta_w += instance.Wl[target][t] - aux_w;
                  }
                  else if (instance.Wu[target][t] < aux_w - parameters.eps) {
                        delta_w += aux_w - instance.Wu[target][t];
                  }
            }
            return delta_w;
      }
      else {
            // Evaluating objective
            return delta[vIndex][target] - delta[vIndex][source];
      }
}