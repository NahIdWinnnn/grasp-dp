/*
      File: Algorithm.h
      Author: Nanashi
*/

#include "Island.h"

class Algorithm {

public:

// Constructor, destructor
      Algorithm(const std::string& pathInstance);
      virtual ~Algorithm();

// Public attributes
      std::vector<double> accumulated_time_cpu;
      std::vector<double> min_cost;

// Public functions
      void Iterate();

private:
      
// Private attributes
      Island **islands;

      std::ofstream fileEvol;
      std::ofstream fileCost;
      std::ofstream fileSolu;

};