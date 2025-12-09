/*
      File: IOperatorPhase2.h
      Author: Nanashi
*/

#pragma once
#include "../../solutions/BaseSolution.h"
#include "../../solutions/GraspSolution/GraspSolution.h"
#include "../../solutions/AntSolution/AntSolution.h"

class IOperatorPhase2 {
public:
      virtual ~IOperatorPhase2() = default;

      virtual bool improve(BaseSolution* solution) = 0;
};