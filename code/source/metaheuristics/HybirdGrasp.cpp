#pragma once
#include "HybridGrasp.h"

BaseSolution* HybridGrasp::findSolutionGRASP_DP(double alpha) {
      // Phase 1: Find feasible solution
      BaseSolution* newSolution = phase1 -> construct(alpha);

      newSolution -> validate();
      if (!newSolution -> checkFeasibility()) {
            errorTermination("Infeasible solution after Phase 2");
      }

      // Phase 2: Improvement
      extension -> improve(newSolution);
      newSolution -> validate();
      if (!newSolution -> checkFeasibility()) {
            errorTermination("Infeasible solution after Phase 2");
      }

      phase2 -> improve(newSolution);

      newSolution -> validate();
      if (!newSolution -> checkFeasibility()) {
            errorTermination("Infeasible solution after Phase 2");
      }

      return newSolution;
}

BaseSolution* HybridGrasp::findSolutionHybridGrasp(double alpha) {
      // Phase 1: Find feasible solution
      BaseSolution* newSolution = phase1 -> construct(alpha);

      newSolution -> validate();
      if (!newSolution -> checkFeasibility()) {
            errorTermination("Infeasible solution after Phase 2");
      }

      // // Phase 2: Improvement
      // phase2 -> improve(newSolution);

      // newSolution -> validate();
      // if (!newSolution -> checkFeasibility()) {
      //       errorTermination("Infeasible solution after Phase 2");
      // }

      // Phase 2: Improvement
      extension -> improve(newSolution);
      newSolution -> validate();
      if (!newSolution -> checkFeasibility()) {
            errorTermination("Infeasible solution after Phase 2");
      }

      phase2 -> improve(newSolution);

      newSolution -> validate();
      if (!newSolution -> checkFeasibility()) {
            errorTermination("Infeasible solution after Phase 2");
      }

      // Insert solution to local memory
      if (localBest == nullptr or newSolution -> objective + parameters.eps < localBest -> objective) {
            delete localBest;
            AntSolution* ptr = dynamic_cast<AntSolution*>(newSolution);
            localBest = new AntSolution(*ptr);
      }

      // Update pheromones
      if (it % 50 == 49) {
            HybridGraspPhase1* ptr = dynamic_cast<HybridGraspPhase1*>(phase1);
            ptr -> updatePheromones(localBest);
            delete localBest;
            localBest = nullptr;
      }

      // Reset pheromones
      if (noImprovementCount > parameters.stagnation_limit) {
            noImprovementCount = 0;
            HybridGraspPhase1* ptr = dynamic_cast<HybridGraspPhase1*>(phase1);
            ptr -> resetPheromones();
      }

      return newSolution;
}
