/*
      File: Island.h
      Author: Nanashi
*/

#include "Island.h"

// Constructor/destructor

Island::Island() {
      metaheuristics = new Metaheuristic*[parameters.nMetaheuristic];
      for (uint16_t i = 0; i < parameters.nMetaheuristic; i++) {
            metaheuristics[i] = new GRASP();
      }
}

Island::~Island() {
      for (uint16_t i = 0; i < parameters.nMetaheuristic; i++) {
            delete metaheuristics[i];
      }
      delete[] metaheuristics;
}

void Island::Iterate() {
      for (uint16_t i = 0; i < parameters.nMetaheuristic; i++) {
            metaheuristics[i] -> Iterate();
      }
}

void Island::Transform() {
      // if (parameters.searModel == "") {
      //       for (uint16_t i = 0; i < parameters.nMetaheuristic; i++) {
      //             // GRASP -> GRASP_DP
      //       }
      // }
}
