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

      // Public attributes
      uint32_t it;

      // Public functions
      virtual void Iterate() = 0;
      virtual double getBest() = 0;
};