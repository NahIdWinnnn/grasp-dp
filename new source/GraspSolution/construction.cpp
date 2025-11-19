/*
      Header: GraspSolution.h
      File: construction.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

void Solution::constructGreedy(double alpha) {

      // Candidate list initialization
      std::vector<uint16_t> candidate_list(instance.nV);
      std::iota(candidate_list.begin(), candidate_list.end(), 0);
      randomShuffle(candidate_list);

      // First assignment
      for (uint16_t i = 0; i < instance.nK; i++) {
            partitions[i].emplace_back(candidate_list.back());
            candidate_list.pop_back();
      }

      // Final assignment
      std::vector<std::tuple<double, uint16_t, uint16_t>> extended_candidate_list;
      for (uint16_t v = 0; v < instance.nV; v++) {
            for (uint16_t c = 0; c < instance.nK; c++) {
                  extended_candidate_list.emplace_back(0, v, c);
            }
      }

      for (size_t it = 0; it < instance.nK; it++) {
            if (extended_candidate_list.empty()) {
                  errorTermination("Runtime error: Extended candidate list empty!");
            }

            if (parameters.GRASPver == "random-greedy") {
                  random_greedySelection(extended_candidate_list, alpha);
            }
            else if (parameters.GRASPver == "greedy-random") {
                  greedy_randomSelection(extended_candidate_list, alpha);
            }
            else {
                  errorTermination("Invalid configuration: GRASP version \"" + parameters.GRASPver + "\" is not available!");
            }
      }
}

void Solution::random_greedySelection(std::vector<std::tuple<double, uint16_t, uint16_t>> &extended_candidate_list, double alpha) {

      // Initialization
      uint32_t s = std::max((1.0 - alpha) * extended_candidate_list.size(), 1.0);
      double min = std::numeric_limits<double>::max();

      std::vector<std::reference_wrapper<std::tuple<double, uint16_t, uint16_t>>> restricted_candidate_list;
      std::sample(extended_candidate_list.begin(), extended_candidate_list.end(), std::back_inserter(restricted_candidate_list), s, rng[omp_get_thread_num()]);

      // Choose pair [vertex, cluster]
      auto* best_candidate_ptr = &restricted_candidate_list[0].get(); 
      for (auto &wrapped_p : restricted_candidate_list) {
            auto& current_tuple = wrapped_p.get();
            if (current_tuple < *best_candidate_ptr) {
                  best_candidate_ptr = &current_tuple;
            }
      }
      auto [cCost, cVertex, cCluster] = *best_candidate_ptr;

      // Update solution data
      partitions[cCluster].emplace_back(cVertex);
      objective += cCost;
      addVertex(cVertex, cCluster);

      // Make extended candidate list reusable
      extended_candidate_list.erase(std::partition(extended_candidate_list.begin(), extended_candidate_list.end(), [&](std::tuple<double, uint16_t, uint16_t> &x) -> bool {
            return std::get<1>(x) != cVertex;
      }), extended_candidate_list.end());
      for (auto &[cost, vertex, cluster] : extended_candidate_list) {
            if (cluster == cCluster) {
                  cost = delta[vertex][cCluster];
            }
      }
}

void Solution::greedy_randomSelection(std::vector<std::tuple<double, uint16_t, uint16_t>> &extended_candidate_list, double alpha) {

      // Initialization
      double min = std::numeric_limits<double>::max(), max = std::numeric_limits<double>::min();
      for (auto &[cost, vertex, cluster] : extended_candidate_list) {
            min = std::min(min, cost);
            max = std::max(max, cost);
      }

      double threshold = min + (max - min) * alpha;
      std::vector<int> restricted_candidate_list;
      for (int i = 0; i < int(extended_candidate_list.size()); i++) {
            if (std::get<0>(extended_candidate_list[i]) + parameters.eps <= threshold) {
                  restricted_candidate_list.emplace_back(i);
            }
      }
      assert(!restricted_candidate_list.empty());

      // Choose pair [vertex, cluster]
      int index = randomElement(restricted_candidate_list);
      auto [cCost, cVertex, cCluster] = extended_candidate_list[index];

      // Update solution data
      partitions[cCluster].emplace_back(cVertex);
      objective += cCost;
      addVertex(cVertex, cCluster);

      // Make extended candidate list reusable
      extended_candidate_list.erase(std::partition(extended_candidate_list.begin(), extended_candidate_list.end(), [&](std::tuple<double, uint16_t, uint16_t> &x) -> bool {
            return std::get<1>(x) != cVertex;
      }), extended_candidate_list.end());
      for (auto &[cost, vertex, cluster] : extended_candidate_list) {
            if (cluster == cCluster) {
                  cost = delta[vertex][cCluster];
            }
      }
}

void Solution::constructRandomized() {

      // Candidate list initialization
      std::vector<uint16_t> candidate_list(instance.nV);
      std::iota(candidate_list.begin(), candidate_list.end(), 0);
      randomShuffle(candidate_list);

      // Assignment
      uint16_t min_size = instance.nV / instance.nK;
      for (uint16_t i = 0; i < instance.nV; i += min_size) {
            if (i + min_size <= instance.nV) {
                  for (uint16_t j = 0; j < instance.nK; j++) {
                        partitions[j].emplace_back(candidate_list[i + j]);
                  }
            }
            else {
                  for (uint16_t j = 0; i + j < instance.nV; j++) {
                        partitions[randomUnsignedInt(0, instance.nK)].emplace_back(candidate_list[i + j]);
                  }
            }
      }
}
