/*
      File: Island.h
      Author: Nanashi
*/

#pragma once
#include "../metaheuristics/Grasp.h"
#include "../metaheuristics/HybridGrasp.h"

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