/*
      File: GraspPhase1.cpp
      Author: Nanashi
*/

#include "GraspPhase1.h"

BaseSolution* GraspPhase1::construct(double alpha) {
      bool feasible = false;
      BaseSolution *newSolution = nullptr;
      while (!feasible) {
            if (newSolution) {
                  delete newSolution;
            }
            newSolution = new GraspSolution(alpha);
            while (newSolution -> explore(false)) {
                  continue;
            }
            if (std::abs(newSolution -> infeasibility) < parameters.eps) {
                  feasible = true;
            }
      }
      return newSolution;
}