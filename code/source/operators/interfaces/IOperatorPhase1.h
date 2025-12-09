/*
      File: IOperatorPhase1.h
      Author: Nanashi
*/

#pragma once
#include "../../solutions/BaseSolution.h"
#include "../../solutions/GraspSolution/GraspSolution.h"
#include "../../solutions/AntSolution/AntSolution.h"

class IOperatorPhase1 {
public:
      virtual ~IOperatorPhase1() = default;

      virtual BaseSolution* construct(double alpha) = 0;
};