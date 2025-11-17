/*
      File: Island.h
      Author: Nanashi
*/

#include "../metaheuristics/GRASP-DP.h"
#include "../metaheuristics/GRASP.h"

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