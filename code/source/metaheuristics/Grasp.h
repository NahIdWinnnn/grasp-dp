/*
      File: Grasp.h
      Author: Nanashi
*/

#pragma once
#include "BaseGrasp.h"
#include "operators/phase1/GraspPhase1.h"
#include "operators/phase2/GraspPhase2.h"

class Grasp: public BaseGrasp {

public:
      Grasp(): BaseGrasp() {
            // Phase 1:
            phase1 = new GraspPhase1();

            // Phase 2:
            phase2 = new GraspPhase2();
      }

protected:
      BaseSolution* findNewSolution(double alpha) {

            // Phase 1: Find feasible solution
            BaseSolution* newSolution = phase1 -> construct(alpha);

            newSolution -> validate();
            if (!newSolution -> checkFeasibility()) {
                  errorTermination("Infeasible solution after Phase 2");
            }

            // Phase 2: Improvement
            phase2 -> improve(newSolution);

            newSolution -> validate();
            if (!newSolution -> checkFeasibility()) {
                  errorTermination("Infeasible solution after Phase 2");
            }

            return newSolution;
      }
};