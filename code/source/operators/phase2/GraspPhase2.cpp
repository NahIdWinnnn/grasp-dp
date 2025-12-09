/*
      File: GraspPhase2.cpp
      Author: Nanashi
*/

#include "GraspPhase2.h"

bool GraspPhase2::improve(BaseSolution* solution) {
      bool improved = false;
      while (solution -> explore(true)) {
            improved = true;
            solution -> validate();
            if (!solution -> checkFeasibility()) {
                  errorTermination("Infeasible solution after Phase 2");
            }
      }
      return improved;
}