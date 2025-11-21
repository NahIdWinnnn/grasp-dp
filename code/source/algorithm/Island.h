/*
      File: Island.h
      Author: Nanashi
*/

#pragma once
#include "../metaheuristics/GRASP.h"
#include "../metaheuristics/GRASP_DP.h"

class Island {

public:

// Constructor/destructor
      Island();
      virtual ~Island();

// Public functions
      void Iterate();
      Metaheuristic* getBest();

      void Transform();

private:

// Private attributes
      Metaheuristic **metaheuristics;

};