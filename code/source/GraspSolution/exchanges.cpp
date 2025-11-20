/*
      Header: GraspSolution.h
      File: exchanges.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

bool GraspSolution::exploreExchange(bool objective) {

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
            return delta_w;
      }
      else {
            // Evaluating objective
            return (delta[fVertex][sClus] - delta[fVertex][fClus]) + (delta[sVertex][fClus] - delta[sVertex][sClus]) - 2 * (instance.D[fVertex][sVertex] + instance.D[sVertex][fVertex]);
      }
}
