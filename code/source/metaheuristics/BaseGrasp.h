/*
      File: BaseGrasp.h
      Author: Nanashi
*/

#pragma once
#include "Metaheuristic.h"
#include "../solutions/BaseSolution.h"
#include "../operators/interfaces/IOperatorPhase1.h"
#include "../operators/interfaces/IOperatorPhase2.h"

struct BaseGrasp: public Metaheuristic {

public:

      // Constructor, destructor
      BaseGrasp();
      virtual ~BaseGrasp() override;

      void Iterate() override;
      double getObjective() const override;
      const std::vector<std::vector<uint16_t>>& getPartitions() const override;

protected:

      BaseSolution* solution;

      IOperatorPhase1* phase1;
      IOperatorPhase2* phase2;

      virtual BaseSolution* findNewSolution(double alpha) = 0;

      uint32_t noImprovementCount;

private:

      // Parameters for Reactive GRASP.
      uint32_t m, block, metaDelta;
      std::vector<uint32_t> n;
      std::vector<double> value_alpha, probabilities, sum, q;
};