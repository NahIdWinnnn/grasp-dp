/*
      File: HybridGraspPhase2.h
      Author: Nanashi
*/

#pragma once
#include "../interfaces/IOperatorPhase2.h"

class HybridGraspPhase2: public IOperatorPhase2 {
public:
      bool improve(BaseSolution* solution) override;
};