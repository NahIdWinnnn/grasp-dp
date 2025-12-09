/*
      File: HybridGrasp.h
      Author: Nanashi
*/

#pragma once
#include "BaseGrasp.h"
#include "operators/phase1/GraspPhase1.h"
#include "operators/phase1/HybridGraspPhase1.h"
#include "operators/phase2/GraspPhase2.h"
#include "operators/phase2/DpGraspPhase2.h"
#include "operators/phase2/HybridGraspPhase2.h"

class HybridGrasp: public BaseGrasp {

public:
      HybridGrasp(): BaseGrasp() {
            // Phase 1:
            if (parameters.searModel == "2P-R-GRASP-DP") {
                  std::cerr << "Phase 1: Using GraspPhase1.\n";
                  phase1 = new GraspPhase1();
            }
            else {
                  std::cerr << "Phase 1: Using HybridGraspPhase1.\n";
                  phase1 = new HybridGraspPhase1();
            }

            // Phase 2:
            if (parameters.searModel == "2P-R-GRASP-DP") {
                  std::cerr << "Phase 2: Using DpGraspPhase2.\n";
                  phase2 = new DpGraspPhase2();
            }
            else {
                  std::cerr << "Phase 2: Using HDpPhase2.\n";
                  phase2 = new HybridGraspPhase2();
            }

            // Extensions:
            extension = new GraspPhase2();

            localBest = nullptr;
      }

      ~HybridGrasp() {
            if (extension) {
                  delete extension;
            }
            if (localBest) {
                  delete localBest;
            }
      }

private:
      IOperatorPhase2* extension;

      BaseSolution *localBest;

      BaseSolution* findSolutionGRASP_DP(double alpha);
      BaseSolution* findSolutionHybridGrasp(double alpha);

protected:
      BaseSolution* findNewSolution(double alpha) {
            if (parameters.searModel == "2P-R-GRASP-DP") {
                  std::cerr << "Running DP\n";
                  return findSolutionGRASP_DP(alpha);
            }
            else {
                  std::cerr << "Running HDP\n";
                  return findSolutionHybridGrasp(alpha);
            }
      }
};