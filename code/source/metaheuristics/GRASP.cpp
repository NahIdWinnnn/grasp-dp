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
      solution = new GraspSolution(1);
}

GRASP::~GRASP() {
      delete solution;
}

void GRASP::Iterate() {
      uint32_t alpha_index = (it < m ? it : selectFromDistribution(probabilities));
      n[alpha_index] += 1;

      double alpha = value_alpha[alpha_index];

      bool feasible = false;
      while (!feasible) {
            solution -> construct(alpha);
            solution -> validate();

            // to be continued
      }
}

double GRASP::getBest() {
      // to be continued
}