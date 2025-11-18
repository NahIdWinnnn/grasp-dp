/*
      File: Island.h
      Author: Nanashi
*/

#include "../metaheuristics/GRASP.h"
#include "../metaheuristics/GRASP_DP.h"

class Island {

public:
      
// Constructor/destructor
      Island();
      virtual ~Island();

// Public functions
      void Iterate();

private:

// Private attributes
      Metaheuristic **metaheuristics;

};