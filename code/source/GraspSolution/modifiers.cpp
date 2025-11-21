/*
      Header: GraspSolution.h
      File: modifiers.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

void GraspSolution::addVertex(uint16_t vIndex, uint16_t cIndex) {

      // Attributes modification
      partitions[cIndex].emplace_back(vIndex);
      objective += delta[vIndex][cIndex];

      // Association cost modification
      for (uint16_t v = 0; v < instance.nV; v++) {
            delta[v][cIndex] += instance.D[v][vIndex] + instance.D[vIndex][v];
      }

      // Infeasibility score modification
      infeasibility -= sigma[cIndex];
      sigma[cIndex] = 0;
      for (uint16_t t = 0; t < instance.nT; t++) {
            w[cIndex][t] += instance.W[vIndex][t];
            if (w[cIndex][t] + parameters.eps < instance.Wl[cIndex][t]) {
                  sigma[cIndex] += instance.Wl[cIndex][t] - w[cIndex][t];
            }
            else if (instance.Wu[cIndex][t] < w[cIndex][t] - parameters.eps) {
                  sigma[cIndex] += w[cIndex][t] - instance.Wu[cIndex][t];
            }
      }
      infeasibility += sigma[cIndex];
}

void GraspSolution::insertVertex(uint16_t vPos, uint16_t source, uint16_t target) {
      uint16_t vIndex = partitions[source][vPos];

      // Attributes modification
      partitions[target].emplace_back(vIndex);
      std::swap(partitions[source][vPos], partitions[source].back());
      partitions[source].pop_back();
      objective += delta[vIndex][target] - delta[vIndex][source];

      // Association cost modification
      for (uint16_t v = 0; v < instance.nV; v++) {
            delta[v][source] -= instance.D[v][vIndex] + instance.D[vIndex][v];
            delta[v][target] += instance.D[v][vIndex] + instance.D[vIndex][v];
      }

      // Infeasibility score modification
      infeasibility -= sigma[source] + sigma[target];
      sigma[source] = sigma[target] = 0;
      for (uint16_t t = 0; t < instance.nT; t++) {
            w[source][t] -= instance.W[vIndex][t];
            w[target][t] += instance.W[vIndex][t];

            if (w[target][t] + parameters.eps < instance.Wl[target][t]) {
                  sigma[target] += instance.Wl[target][t] - w[target][t];
            }
            else if (instance.Wu[target][t] < w[target][t] - parameters.eps) {
                  sigma[target] += w[target][t] - instance.Wu[target][t];
            }

            if (w[source][t] + parameters.eps < instance.Wl[source][t]) {
                  sigma[source] += instance.Wl[source][t] - w[source][t];
            }
            else if (instance.Wu[source][t] < w[source][t] - parameters.eps) {
                  sigma[source] += w[source][t] - instance.Wu[source][t];
            }
      }
      infeasibility += sigma[source] + sigma[target];
}

void GraspSolution::exchangeVertex(uint16_t fPos, uint16_t fClus, uint16_t sPos, uint16_t sClus) {
      uint16_t fVertex = partitions[fClus][fPos];
      uint16_t sVertex = partitions[sClus][sPos];

      // Attributes modification
      std::swap(partitions[fClus][fPos], partitions[sClus][sPos]);
      objective += (delta[fVertex][sClus] - delta[fVertex][fClus]) + (delta[sVertex][fClus] - delta[sVertex][sClus]) - 2 * (instance.D[fVertex][sVertex] + instance.D[sVertex][fVertex]);

      // Association cost modification
      for (uint16_t v = 0; v < instance.nV; v++) {
            delta[v][fClus] += (instance.D[v][sVertex] + instance.D[sVertex][v]) - (instance.D[v][fVertex] + instance.D[fVertex][v]);
            delta[v][sClus] += (instance.D[v][fVertex] + instance.D[fVertex][v]) - (instance.D[v][sVertex] + instance.D[sVertex][v]);
      }

      // Infeasibility score modification
      infeasibility -= sigma[fClus] + sigma[sClus];
      sigma[fClus] = sigma[sClus] = 0;
      for (uint16_t t = 0; t < instance.nT; t++) {
            w[fClus][t] += instance.W[sVertex][t] - instance.W[fVertex][t];
            w[sClus][t] += instance.W[fVertex][t] - instance.W[sVertex][t];

            if (w[fClus][t] + parameters.eps < instance.Wl[fClus][t]) {
                  sigma[fClus] += instance.Wl[fClus][t] - w[fClus][t];
            }
            else if (instance.Wu[fClus][t] < w[fClus][t] - parameters.eps) {
                  sigma[fClus] += w[fClus][t] - instance.Wu[fClus][t];
            }

            if (w[sClus][t] + parameters.eps < instance.Wl[sClus][t]) {
                  sigma[sClus] += instance.Wl[sClus][t] - w[sClus][t];
            }
            else if (instance.Wu[sClus][t] < w[sClus][t] - parameters.eps) {
                  sigma[sClus] += w[sClus][t] - instance.Wu[sClus][t];
            }
      }
      infeasibility += sigma[fClus] + sigma[sClus];
}
