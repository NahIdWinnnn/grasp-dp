/*
      File: GraspPhase1.h
      Author: Nanashi
*/

#pragma once
#include "../interfaces/IOperatorPhase1.h"

class GraspPhase1: public IOperatorPhase1 {
public:
      BaseSolution* construct(double alpha) override;
};