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
};