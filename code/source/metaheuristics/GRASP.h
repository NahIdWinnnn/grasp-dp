/*
      File: GRASP.h
      Author: Nanashi
*/

#pragma once
#include "Metaheuristic.h"
#include "../GraspSolution/GraspSolution.h"

struct GRASP: public Metaheuristic {

public:

      // Constructor, destructor
      GRASP();
      ~GRASP() override;

      void Iterate() override;
      double getObjective() const override;
      const std::vector<std::vector<uint16_t>>& getPartitions() const override;

private:

      // Private attributes
      uint32_t m, block, metaDelta;
      std::vector<uint32_t> n;
      std::vector<double> value_alpha, probabilities, sum, q;
      GraspSolution* solution;
};