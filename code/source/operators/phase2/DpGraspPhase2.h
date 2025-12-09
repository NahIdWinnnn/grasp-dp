/*
      File: DpGraspPhase2.h
      Author: Nanashi
*/

#pragma once
#include "../interfaces/IOperatorPhase2.h"

class DpGraspPhase2: public IOperatorPhase2 {
public:
      bool improve(BaseSolution* solution) override;
};