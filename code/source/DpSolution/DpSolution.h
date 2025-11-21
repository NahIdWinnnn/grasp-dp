/*
      File: DpSolution.h
      Author: Nanashi
*/

#pragma once
#include "../includes.h"

class DpSolution {

public:

      // Constructor, destructor
      DpSolution();

      // Operators redefinition
      bool operator < (const DpSolution &solution) const {
            return objective < solution.objective;
      }

      // Retrieval functions
      std::vector<std::vector<uint16_t>> getPartitions() {
            return partitions;
      }
      double getObjective() {
            return objective;
      }

      // Validation functions
      void validate();

private:

      // Private attributes
      std::vector<std::vector<uint16_t>> partitions;
      double objective;

      // Algorithmic attributes
      std::vector<std::vector<std::pair<uint16_t, uint16_t>>> bounds;
};
