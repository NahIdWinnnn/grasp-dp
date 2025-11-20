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
