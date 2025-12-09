/*
      File: BaseGrasp.cpp
      Author: Nanashi
*/

#include "BaseGrasp.h"

BaseGrasp::BaseGrasp(): Metaheuristic() {
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
      solution = nullptr;
      phase1 = nullptr;
      phase2 = nullptr;
}

BaseGrasp::~BaseGrasp() {
      if (solution) {
            delete solution;
      }
      if (phase1) {
            delete phase1;
      }
      if (phase2) {
            delete phase2;
      }
}

void BaseGrasp::Iterate() {
      uint32_t alpha_index = (it < m + 1 ? it : selectFromDistribution(probabilities));
      n[alpha_index] += 1;

      double alpha = value_alpha[alpha_index];

      BaseSolution* newSolution = findNewSolution(alpha);

      // Evaluate alpha
      sum[alpha_index] += newSolution -> objective;
      if ((it + 1) % block == 0) {
            double sum_q = 0;
            for (uint32_t i = 0; i < m + 1; i++) {
                  double avg = sum[i] / n[i];
                  q[i] = std::pow(solution -> objective / avg, metaDelta);
                  sum_q += q[i];
            }
            for (uint32_t i = 0; i < m + 1; i++) {
                  probabilities[i] = q[i] / sum_q;
            }
      }

      // Clear up
      if (!solution or newSolution -> objective + parameters.eps < solution -> objective) {
            std::swap(solution, newSolution);
            noImprovementCount = 0;
      }
      else {
            noImprovementCount += 1;
      }
      delete newSolution;

      it += 1;
}

double BaseGrasp::getObjective() const {
      return solution -> objective;
}

const std::vector<std::vector<uint16_t>>& BaseGrasp::getPartitions() const {
      return solution -> partitions;
}