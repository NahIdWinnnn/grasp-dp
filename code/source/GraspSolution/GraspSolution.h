/*
      File: GraspSolution.h
      Author: Nanashi
*/

#pragma once
#include "../includes.h"

class GraspSolution {

public:

      // Constructor, destructor
      GraspSolution(double alpha);

      // Operators redefinition
      bool operator < (const GraspSolution &solution) const {
            return objective < solution.objective;
      }
      bool operator > (const GraspSolution &solution) const {
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
      void validate();
      void construct(double alpha);

private:

      // Private attributes
      std::vector<std::vector<uint16_t>> partitions;
      double objective;
      double infeasibility;

      // Algorithmic attributes
      std::vector<std::vector<double>> delta;               // Association cost:          delta[vertex][cluster]
      std::vector<std::vector<double>> w;                   // Partition weights:         w[cluster][attribute]
      std::vector<double> sigma;                            // Infeasibility score:       sigma[cluster]

      // 1. Constructions:
      void constructGreedy(double alpha);
      void constructRandomized();
      void greedy_randomSelection(std::vector<std::pair<uint16_t, uint16_t>> &extended_candidate_list, double alpha);
      void random_greedySelection(std::vector<std::pair<uint16_t, uint16_t>> &extended_candidate_list, double alpha);

      // 2. Data modifiers:
      void addVertex(uint16_t vIndex, uint16_t cIndex);
};
