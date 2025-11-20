/*
      File: DpSolution.h
      Author: Nanashi
*/

#include "../includes.h"

class Solution {

public:

      // Constructor, destructor
      Solution();
      virtual ~Solution();

      // Operators redefinition
      bool operator < (const Solution &solution) const {
            return objective < solution.objective;
      }
      bool operator > (const Solution &solution) const {
            return objective > solution.objective;
      }

      // Retrieval functions
      std::vector<std::vector<uint16_t>> getPartitions() {
            return partitions;
      }
      double getObjective() {
            return objective;
      }

      // Validation functions
      bool isNormalized() {
            return normalized;
      }
      void normalize();
      bool validate();

private:

      // Private attributes
      std::vector<std::vector<uint16_t>> partitions;
      double objective;
      bool normalized;

      // Algorithmic attributes
      std::vector<std::vector<std::pair<uint16_t, uint16_t>>> bounds;
};
