/*
      File: HybridGraspPhase2.cpp
      Author: Nanashi
*/

#include "HybridGraspPhase2.h"

uint16_t selectNextNodeProbabilistic(uint16_t current, const std::vector<uint16_t>& candidates, const std::vector<bool>& visited, const std::vector<std::vector<double>>& tau, double alpha_power = 1.0) {

      std::vector<uint16_t> valid_candidates;
      std::vector<double> weights;

      for (uint16_t next : candidates) {
            if (!visited[next]) {
                  valid_candidates.emplace_back(next);
                  double w = std::max(1e-6, std::pow(tau[current][next], alpha_power));
                  weights.emplace_back(w);
            }
      }

      if (valid_candidates.empty()) {
            return 0;
      }

      std::discrete_distribution<int> dist(weights.begin(), weights.end());

      int selected_idx = dist(rng[omp_get_thread_num()]);

      return valid_candidates[selected_idx];
}

bool HybridGraspPhase2::improve(BaseSolution *initSolution) {
      AntSolution *solution = dynamic_cast<AntSolution *>(initSolution);
      if (!solution) {
            return false;
      }

      const auto &tau = solution -> tau;
      std::vector<std::vector<uint16_t>> partitions = solution -> partitions;
      uint16_t n = instance.nV;
      uint16_t k = instance.nK;

      std::vector<uint16_t> arr;
      arr.reserve(n);
      std::vector<bool> visited(n, false);

      std::vector<uint16_t> partition_orders(k);
      std::iota(partition_orders.begin(), partition_orders.end(), 0);
      randomShuffle(partition_orders);

      for (uint16_t cPos = 0; cPos < k; cPos++) {
            uint16_t cIndex = partition_orders[cPos];
            const auto &clusterNodes = partitions[cIndex];

            if (clusterNodes.empty()) {
                  continue;
            }

            uint16_t startNode;
            if (arr.empty()) {
                  startNode = clusterNodes[0];
            }
            else {
                  uint16_t lastNode = arr.back();
                  startNode = selectNextNodeProbabilistic(lastNode, clusterNodes, visited, tau, solution -> alpha);
            }

            arr.emplace_back(startNode);
            visited[startNode] = true;
            uint16_t curr = startNode;

            for (size_t i = 1; i < clusterNodes.size(); i++) {
                  uint16_t next = selectNextNodeProbabilistic(curr, clusterNodes, visited, tau, solution -> alpha);

                  if (visited[next]) {
                        for (uint16_t fallback : clusterNodes) {
                              if (!visited[fallback]) {
                                    next = fallback;
                                    break;
                              }
                        }
                  }

                  visited[next] = true;
                  arr.emplace_back(next);
                  curr = next;
            }
      }

      assert(arr.size() == size_t(n));

      std::vector<std::vector<double>> cost(n, std::vector<double>(2));
      for (uint16_t len = 2; len <= n; len++) {
            for (uint16_t s = 0; s + len <= n; s++) {
                  cost[s].emplace_back(cost[s][len - 1] + cost[s + 1][len - 1] - cost[s + 1][len - 2] + (instance.D[arr[s]][arr[s + len - 1]] + instance.D[arr[s + len - 1]][arr[s]]));
            }
      }

      // std::vector<std::vector<double>> ps(n + 1, std::vector<double>(instance.nT));
      // for (int i = 0; i < n; i++) {
      //       for (int j = 0; j < instance.nT; j++) {
      //             ps[i + 1][j] += ps[i][j] + instance.W[arr[i]][j];
      //       }
      // }

      // auto check = [&](int l, int r, std::vector<double> &low, std::vector<double> &up) -> bool {
      //       for (uint16_t t = 0; t < instance.nT; t++) {
      //             double w = ps[r][t] - ps[l][t];
      //             if (w < low[t] - parameters.eps or w > up[t] + parameters.eps) {
      //                   return false;
      //             }
      //       }
      //       return true;
      // };

      // std::vector<int> pl(k + 1);
      // for (int cPos = 0; cPos < k; cPos++) {
      //       pl[cPos + 1] = pl[cPos] + partitions[partition_orders[cPos]].size();
      // }

      std::vector<std::vector<int16_t>> prev(k, std::vector<int16_t>(n + 1, -1));
      std::vector<double> dp(n + 1, std::numeric_limits<double>::max() / 10);
      dp[0] = 0;

      for (uint16_t cPos = 0; cPos < k; cPos++) {
            uint16_t cIndex = partition_orders[cPos];

            // sum over all loops: O(k * n * t)
            std::vector<int16_t> s(n + 1, -1), e(n + 1, -1);
            {
                  std::vector<double> sum(instance.nT, 0);
                  for (int16_t i = 1, l = 0; i <= n; i++) {
                        uint16_t vIndex = arr[i - 1];

                        for (uint16_t t = 0; t < instance.nT; t++) {
                              sum[t] += instance.W[vIndex][t];
                        }

                        for (uint16_t t = 0; t < instance.nT; t++) {
                              while (sum[t] > instance.Wu[cIndex][t] + parameters.eps) {
                                    for (uint16_t nt = 0; nt < instance.nT; nt++) {
                                          sum[nt] -= instance.W[arr[l]][nt];
                                    }
                                    l += 1;
                              }
                              assert(l <= i);
                        }

                        s[i] = l;
                  }
                  sum.assign(instance.nT, 0);
                  for (int16_t i = n, r = n; i > 0; i--) {
                        uint16_t vIndex = arr[i - 1];

                        for (uint16_t t = 0; t < instance.nT; t++) {
                              while (r > 0 and sum[t] + instance.W[arr[r - 1]][t] < instance.Wl[cIndex][t] - parameters.eps) {
                                    r -= 1;
                                    for (uint16_t nt = 0; nt < instance.nT; nt++) {
                                          sum[nt] += instance.W[arr[r]][nt];
                                    }
                              }
                              assert(r <= i);
                        }

                        for (uint16_t t = 0; t < instance.nT; t++) {
                              sum[t] -= instance.W[vIndex][t];
                        }

                        e[i] = r;
                  }
            }

            std::vector<double> new_dp(n + 1, std::numeric_limits<double>::max() / 10);
            {
                  for (uint16_t j = 1; j <= n; j++) {
                        for (int16_t pj = s[j]; pj < e[j]; pj++) {
                              if (new_dp[j] - parameters.eps > dp[pj] + cost[pj][j - pj]) {
                                    new_dp[j] = dp[pj] + cost[pj][j - pj];
                                    prev[cPos][j] = pj;
                              }
                        }
                  }
            }
            std::swap(dp, new_dp);
      }

      if (std::abs(dp[n] - solution -> objective) < parameters.eps) {
            return false;
      }

      if (dp[n] + parameters.eps < solution -> objective) {
            assert(prev[k - 1][n] != -1);
            std::vector<uint16_t> seperator(k);
            uint16_t pos = n;
            for (int16_t cPos = k - 1; cPos >= 0; cPos--) {
                  seperator[cPos] = pos;
                  assert(prev[cPos][pos] >= 0 and prev[cPos][pos] < pos);
                  pos = prev[cPos][pos];
            }
            assert(pos == 0);

            std::vector<std::vector<uint16_t>> newPartitions(k);
            for (uint16_t cPos = 0; cPos < k; cPos++) {
                  uint16_t cIndex = partition_orders[cPos];
                  for (uint16_t vPos = (cPos == 0 ? 0 : seperator[cPos - 1]); vPos < seperator[cPos]; vPos++) {
                        uint16_t vIndex = arr[vPos];
                        newPartitions[cIndex].emplace_back(vIndex);
                  }
            }

            solution -> reconstruct(newPartitions);

            assert(std::abs(solution -> objective - dp[n]) < parameters.eps);

            return true;
      }

      return false;
}