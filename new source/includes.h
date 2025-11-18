/*
      File: include.h
      Author: Nanashi
*/

#include "bits/stdc++.h"
#include <omp.h>

struct Instance {
      char type;                                      // Instance type: [p] (pollster), [t] (tsplib)

      uint16_t nV;                                    // Number of vertices
      uint16_t nK;                                    // Number of clusters
      uint16_t nT;                                    // Number of attributes

      std::vector<std::vector<double>> D;             // Distance matrix
      std::vector<std::vector<double>> W;             // Weight of attributes for each node

      std::vector<std::vector<double>> Wl, Wu;        // Lower bound and Upper bound for the weight of attributes in each cluster
};

struct Parameters {
      // Phase 1: Construction
      std::string       consModel = "2P-R-GRASP";     // Construction:              2P-R-GRASP, Rnd-GRASP
      std::string       consMove = "extended";        // Move:                      insert, exchange, extended
      std::string       consMoveStrat = "first";      // Strategy:                  first, best

      // Phase 2: Local search
      std::string       searModel = "2P-R-HGRASP-DP"; // Search:                    2P-R-GRASP, 2P-R-HGRASP-DP, 2P-R-GRASP-DP

      // GRASP
      std::string       GRASPver = "random-greedy";   // Version:                   greedy-random, random-greedy
      int               GRASPalphaDiv = 4;            // Alpha division
      int               GRASPblock = 40;              // Block of iterations
      int               GRASPdelta = 1;               // Delta exponent

      // Configuration
      std::string       terminationCriteria = "iter"; // Termination criteria:      iter, tcpu
      double            terminationValue = 1000;      // Termination value:         number of iteration (iter), time in seconds (tcpu)
      uint32_t          seed = 1;                     // Seed
      bool              logs = true;                  // Logs

      // Fixed configuration
      uint16_t          nIsland = 1;                  // Number of Islands
      uint16_t          nMetaheuristic = 1;           // Number of Metaheuristics
      bool              parallelEnabled = true;       // Parallel island calculation
};

extern Instance   instance;
extern Parameters parameters;

using high_clock_t = std::chrono::high_resolution_clock;
