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

      // Operators definition
      bool operator < (const GraspSolution &solution) const {
            return objective < solution.objective;
      }

      // Retrieval functions
      const std::vector<std::vector<uint16_t>>& getPartitions() const {
            return partitions;
      }
      double getObjective() const {
            return objective;
      }
      double getInfeasibility() const {
            return infeasibility;
      }

      // Validation functions
      void validate();
      void checkFeasibility();
      void construct(double alpha);
      bool explore(bool objective);

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
      void insertVertex(uint16_t vPos, uint16_t source, uint16_t target);
      void exchangeVertex(uint16_t fPos, uint16_t fClus, uint16_t sPos, uint16_t sClus);

      // 3.1. Insertions:
      std::tuple<double, uint16_t, uint16_t, uint16_t> exploreInsert(bool objective);
      bool isFeasibleInsert(uint16_t vPos, uint16_t source, uint16_t target);
      double evaluateInsert(bool objective, uint16_t vPos, uint16_t source, uint16_t target);

      // 3.2. Exchanges:
      std::tuple<double, uint16_t, uint16_t, uint16_t, uint16_t> exploreExchange(bool objective);
      bool isFeasibleExchange(uint16_t fPos, uint16_t fClus, uint16_t sPos, uint16_t sClus);
      double evaluateExchange(bool objective, uint16_t fPos, uint16_t fClus, uint16_t sPos, uint16_t sClus);
};
