/*
      File: HybridGraspPhase1.h
      Author: Nanashi
*/

#include "../interfaces/IOperatorPhase1.h"

class HybridGraspPhase1: public IOperatorPhase1 {

public:

      // [u][v]: Association strength between vertex u and vertex v
      std::vector<std::vector<double>> edgePheromone;

      // [u][k]: Association strength between vertex u and cluster k
      std::vector<std::vector<double>> assignPheromone;

      // --- MMAS Parameters ---
      double tauMax;
      double tauMin;
      double rho;       // Evaporation rate
      double Q;         // Deposit factor
      double alpha;     // MMAS alpha
      double beta;      // MMAS beta
      double gamma;     // MMAS gamma

      // Constructor, destructor
      HybridGraspPhase1();
      virtual ~HybridGraspPhase1() = default;

      // Override
      BaseSolution* construct(double alpha) override;

      // Functions
      void updatePheromones(BaseSolution *solution);
      void resetPheromones();
};