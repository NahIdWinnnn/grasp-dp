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

Metaheuristic* Island::getBest() {
      Metaheuristic* result = metaheuristics[0];
      for (uint16_t i = 1; i < parameters.nMetaheuristic; i++) {
            if (*metaheuristics[i] < *result) {
                  result = metaheuristics[i];
            }
      }
      return result;
}

void Island::Transform() {
      // if (parameters.searModel == "") {
      //       for (uint16_t i = 0; i < parameters.nMetaheuristic; i++) {
      //             // GRASP -> GRASP_DP
      //       }
      // }
}
