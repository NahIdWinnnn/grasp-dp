/*
      File: GRASP_DP.h
      Author: Nanashi
*/

#include "Metaheuristic.h"
#include "../solution/DpSolution.h"

struct GRASP_DP: public Metaheuristic {

public:

      // Constructor, destructor
      GRASP_DP();
      ~GRASP_DP() override;

      void Iterate() override;
};