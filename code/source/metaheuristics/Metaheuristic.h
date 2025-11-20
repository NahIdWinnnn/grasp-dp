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
      int      k = 0;

// Public functions
      virtual void Iterate() = 0;
};