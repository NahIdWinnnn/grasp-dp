/*
      File: Metaheuristic.h
      Author: Nanashi
*/

#pragma once
#include "../includes.h"

class Metaheuristic {

public:

      // Constructor, destructor
      Metaheuristic();
      virtual ~Metaheuristic();

      // Operators definition
      bool operator < (const Metaheuristic &other) const {
            return getObjective() < other.getObjective();
      }

      // Public attributes
      uint32_t it;

      // Public functions
      virtual void Iterate() = 0;
      virtual double getObjective() const = 0;
      virtual const std::vector<std::vector<uint16_t>>& getPartitions() const = 0;
};