/*
      File: GRASP_DP.h
      Author: Nanashi
*/

#pragma once
#include "Metaheuristic.h"
#include "../DPsolution/DpSolution.h"

struct GRASP_DP: public Metaheuristic {

public:

      // Constructor, destructor
      GRASP_DP();
      ~GRASP_DP() override;

      void Iterate() override;
      double getObjective() const override;

private:

      DpSolution* solution;
};