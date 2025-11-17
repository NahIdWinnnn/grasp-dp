/*
      File: Algorithm.cpp
      Author: Nanashi
*/

#include "Algorithm.h"

// Constructor, destructor

Algorithm::Algorithm(const std::string& pathInstance) {
      islands = new Island *[parameters.nIsland];
      for (size_t i = 0; i < parameters.nIsland; i++) {
            islands[i] = new Island();
      }

      if (parameters.logs) {
      }
}