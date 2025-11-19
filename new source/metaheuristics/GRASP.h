/*
      File: GRASP.h
      Author: Nanashi
*/

#include "Metaheuristic.h"
#include "../GraspSolution/GraspSolution.h"

struct GRASP: public Metaheuristic {

public:

      // Constructor, destructor
      GRASP();
      ~GRASP() override;

      void Iterate() override;
};