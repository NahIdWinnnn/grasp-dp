/*
      File: GRASP.cpp
      Author: Nanashi
*/

#include "GRASP.h"

GRASP::GRASP(): Metaheuristic() {
      it = 0;
      m = parameters.GRASPalphaDiv;
      block = parameters.GRASPblock;
      metaDelta = parameters.GRASPdelta;
      n.assign(m + 1, 0);
      value_alpha.assign(m + 1, 0);
      for (int i = 0; i < m + 1; i++) {
            value_alpha[i] = double(i) / m;
      }
      probabilities.assign(m + 1, double(1) / (m + 1));
      sum.assign(m + 1, 0);
      q.assign(m + 1, 0);
      solution = new GraspSolution(2);
}

GRASP::~GRASP() {
      delete solution;
}

void GRASP::Iterate() {
      uint32_t alpha_index = (it < m + 1 ? it : selectFromDistribution(probabilities));
      n[alpha_index] += 1;

      double alpha = value_alpha[alpha_index];

      GraspSolution* newSolution = nullptr;

      // Phase 1: Find feasible solution
      bool feasible = false;
      while (!feasible) {
            delete newSolution;
            newSolution = new GraspSolution(alpha);
            newSolution -> validate();
            while (newSolution -> explore(false)) {
                  newSolution -> validate();
                  continue;
            }
            newSolution -> validate();
            if (std::abs(newSolution -> getInfeasibility()) < parameters.eps) {
                  feasible = true;
            }
      }
      newSolution -> validate();
      if (!newSolution->checkFeasibility()) {
            errorTermination("Invalid solution detected: Infeasible solution after Phase 1!");
      }

      // Phase 2: Optimize solution
      while (newSolution -> explore(true)) {
            newSolution -> validate();
            if (!newSolution ->checkFeasibility()) {
                  errorTermination("Invalid solution detected: Infeasible solution during phase 2!");
            }
            continue;
      }
      newSolution -> validate();
      if (!newSolution ->checkFeasibility()) {
            errorTermination("Invalid solution detected: Infeasible solution after phase 2!");
      }

      // Evaluate alpha
      sum[alpha_index] += newSolution -> getObjective();
      if ((it + 1) % block == 0) {
            double sum_q = 0;
            for (uint32_t i = 0; i < m + 1; i++) {
                  double avg = sum[i] / n[i];
                  q[i] = std::pow(solution -> getObjective() / avg, metaDelta);
                  sum_q += q[i];
            }
            for (uint32_t i = 0; i < m + 1; i++) {
                  probabilities[i] = q[i] / sum_q;
            }
      }

      // Clear up
      if (*newSolution < *solution) {
            std::swap(solution, newSolution);
      }
      delete newSolution;

      if (!solution ->checkFeasibility()) {
            errorTermination("Invalid solution detected: Infeasible final solution!");
      }

      it += 1;
}

double GRASP::getObjective() const {
      return solution -> getObjective();
}

const std::vector<std::vector<uint16_t>>& GRASP::getPartitions() const {
      return solution -> getPartitions();
}