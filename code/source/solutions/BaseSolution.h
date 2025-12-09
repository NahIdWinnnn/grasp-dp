/*
      File: BaseSolution.h
      Author: Nanashi
*/

#pragma once
#include "../includes.h"

class BaseSolution {

public:
      std::vector<std::vector<uint16_t>> partitions;
      double objective;
      double infeasibility;

      virtual ~BaseSolution() = default;

      virtual void validate() = 0;
      virtual bool checkFeasibility() = 0;
      virtual void printDetailedDebug() = 0;
      virtual void construct(double alpha) = 0;
      virtual bool explore(bool objectiveSearch) = 0;
      virtual void reconstruct(std::vector<std::vector<uint16_t>> &newPartitions) = 0;
};
