/*
      File: Island.h
      Author: Nanashi
*/

#include "Island.h"

// Constructor/destructor

Island::Island() {
      metaheuristics = new *Metaheuristic[parameters.nMetaheuristic];
      if (parameters.searModel == "2P-R-GRASP") {
            for (size_t i = 0; i < parameters.nMetaheuristic; i++) {
                  metaheuristics[i] = new GRASP();
            }
      }
      else if (parameters.searModel == "2P-R-HGRASP-DP") {
            for (size_t i = 0; i < parameters.nMetaheuristic; i++) {
                  metaheuristics[i] = new GRASP_DP();
            }
      }
      else {
            assert(parameters.searModel == "2P-R-GRASP-DP");
      }
}