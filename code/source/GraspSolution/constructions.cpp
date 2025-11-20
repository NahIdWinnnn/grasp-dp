/*
      Header: GraspSolution.h
      File: constructions.cpp
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
            addVertex(candidate_list.back(), i);
            candidate_list.pop_back();
      }

      // Final assignment
      std::vector<std::pair<uint16_t, uint16_t>> extended_candidate_list;
      for (uint16_t v = 0; v < instance.nV; v++) {
            for (uint16_t c = 0; c < instance.nK; c++) {
                  extended_candidate_list.emplace_back(v, c);
            }
      }

      for (size_t it = 0; instance.nK + it < instance.nV; it++) {
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

      if (!extended_candidate_list.empty()) {
            errorTermination("Logic error: ECL is not empty after procedure!");
      }
}

void Solution::random_greedySelection(std::vector<std::pair<uint16_t, uint16_t>> &extended_candidate_list, double alpha) {

      // Initialization
      uint32_t s = std::max((1.0 - alpha) * extended_candidate_list.size(), 1.0);
      double min = std::numeric_limits<double>::max();

      std::vector<std::reference_wrapper<std::pair<uint16_t, uint16_t>>> restricted_candidate_list;
      std::sample(extended_candidate_list.begin(), extended_candidate_list.end(), std::back_inserter(restricted_candidate_list), s, rng[omp_get_thread_num()]);

      // Choose pair [vertex, cluster]
      auto [cVertex, cCluster] = restricted_candidate_list[0].get();
      for (auto &wrapped_p : restricted_candidate_list) {
            auto &current_pair = wrapped_p.get();
            if (delta[current_pair.first][current_pair.second] < delta[cVertex][cCluster]) {
                  cVertex = current_pair.first;
                  cCluster = current_pair.second;
            }
      }

      // Update solution data
      addVertex(cVertex, cCluster);

      // Make extended candidate list reusable
      extended_candidate_list.erase(std::partition(extended_candidate_list.begin(), extended_candidate_list.end(), [&](std::pair<uint16_t, uint16_t> &x) -> bool {
            return x.first != cVertex;
      }), extended_candidate_list.end());
}

void Solution::greedy_randomSelection(std::vector<std::pair<uint16_t, uint16_t>> &extended_candidate_list, double alpha) {

      // Initialization
      double min = std::numeric_limits<double>::max(), max = std::numeric_limits<double>::min();
      for (auto &[vertex, cluster] : extended_candidate_list) {
            min = std::min(min, delta[vertex][cluster]);
            max = std::max(max, delta[vertex][cluster]);
      }

      double threshold = min + (max - min) * alpha;
      std::vector<int> restricted_candidate_list;
      for (auto it = extended_candidate_list.begin(); it != extended_candidate_list.end(); ++it) {
            if (delta[it -> first][it -> second] + parameters.eps <= threshold) {
                  restricted_candidate_list.emplace_back(int(it - extended_candidate_list.begin()));
            }
      }
      assert(!restricted_candidate_list.empty());

      // Choose pair [vertex, cluster]
      int index = randomElement(restricted_candidate_list);
      auto [cVertex, cCluster] = extended_candidate_list[index];

      // Update solution data
      addVertex(cVertex, cCluster);

      // Make extended candidate list reusable
      extended_candidate_list.erase(std::partition(extended_candidate_list.begin(), extended_candidate_list.end(), [&](std::pair<uint16_t, uint16_t> &x) -> bool {
            return x.first != cVertex;
      }), extended_candidate_list.end());
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
