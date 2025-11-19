/*
      Header: GraspSolution.h
      File: modifiers.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

void Solution::addVertex(uint16_t vIndex, uint16_t cIndex) {

      // Association cost modification
      for (uint16_t v = 0; v < instance.nV; v++) {
            delta[v][cIndex] += instance.D[v][vIndex] + instance.D[vIndex][v];
      }

      // Partition weights modification
      for (uint16_t t = 0; t < instance.nT; t++) {
            w[cIndex][t] += instance.W[vIndex][t];
      }

      // Infeasibility score modification
      sigma[cIndex] = 0;
      for (uint16_t t = 0; t < instance.nT; t++) {
            if (w[cIndex][t] + parameters.eps < instance.Wl[cIndex][t]) {
                  sigma[cIndex] += instance.Wl[cIndex][t] - w[cIndex][t];
            }
            else if (instance.Wu[cIndex][t] < w[cIndex][t] - parameters.eps) {
                  sigma[cIndex] += w[cIndex][t] - instance.Wu[cIndex][t];
            }
      }
}