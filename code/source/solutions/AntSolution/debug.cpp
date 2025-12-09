/*
      Header: AntSolution.h
      File: Debug.cpp
      Author: Nanashi
*/

#include "AntSolution.h"


void AntSolution::printDetailedDebug() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  🔍 DEBUG: SOLUTION INTEGRITY CHECK & COMPARISON\n";
    std::cout << std::string(70, '=') << "\n";

    double manual_objective = 0.0;
    bool is_objective_match = true;
    bool is_weight_match = true;

    // --- DUYỆT QUA TỪNG CLUSTER ---
    for (uint16_t c = 0; c < instance.nK; c++) {
        std::cout << "Cluster " << std::setw(2) << (c + 1) 
                  << " (Size: " << partitions[c].size() << "):\n";
        
        // 1. Tính Manual Objective cho cluster này
        double cluster_obj = 0.0;
        for (auto u : partitions[c]) {
            for (auto v : partitions[c]) {
                cluster_obj += instance.D[u][v];
            }
        }
        manual_objective += cluster_obj;

        // 2. Tính Manual Weights cho cluster này
        std::vector<double> cluster_w(instance.nT, 0.0);
        for (auto v : partitions[c]) {
            for (uint16_t t = 0; t < instance.nT; t++) {
                cluster_w[t] += instance.W[v][t];
            }
        }

        // 3. In và So sánh Weights
        std::cout << "  Bounds & Weights Check:\n";
        std::cout << "    " << std::left << std::setw(6) << "Attr"
                  << std::right << std::setw(12) << "Min(L)"
                  << std::setw(12) << "Tracked(w)"
                  << std::setw(12) << "Real(Calc)"
                  << std::setw(12) << "Max(U)"
                  << "   Status\n";
        std::cout << "    " << std::string(60, '-') << "\n";

        for (uint16_t t = 0; t < instance.nT; t++) {
            // So sánh giá trị tracked (w) và calculated
            double diff = std::abs(w[c][t] - cluster_w[t]);
            std::string sync_status = (diff < parameters.eps) ? "" : " [SYNC ERR!]";
            if (diff >= parameters.eps) is_weight_match = false;

            // Check bound vi phạm (dựa trên giá trị thực tế)
            std::string bound_status = "OK";
            if (cluster_w[t] + parameters.eps < instance.Wl[c][t]) bound_status = "< VIOLATION";
            else if (instance.Wu[c][t] < cluster_w[t] - parameters.eps) bound_status = "> VIOLATION";

            std::cout << "    #" << std::left << std::setw(5) << t
                      << std::right << std::fixed << std::setprecision(2)
                      << std::setw(12) << instance.Wl[c][t]
                      << std::setw(12) << w[c][t]          // Giá trị trong bộ nhớ
                      << std::setw(12) << cluster_w[t]     // Giá trị tính lại
                      << std::setw(12) << instance.Wu[c][t]
                      << "   " << bound_status << sync_status << "\n";
        }
        std::cout << "\n";
    }

    // --- TỔNG KẾT OBJECTIVE ---
    double obj_diff = std::abs(objective - manual_objective);
    if (obj_diff > parameters.eps) is_objective_match = false;

    std::cout << std::string(70, '-') << "\n";
    std::cout << "  📊 OBJECTIVE COMPARISON:\n";
    std::cout << "     Tracked Objective (Incremental): " << std::fixed << std::setprecision(6) << objective << "\n";
    std::cout << "     Manual Objective (Re-calculated): " << std::fixed << std::setprecision(6) << manual_objective << "\n";
    std::cout << "     Difference:                       " << std::scientific << obj_diff << "\n";
    std::cout << std::string(70, '-') << "\n";
    
    // --- KẾT LUẬN ---
    if (is_objective_match && is_weight_match) {
        std::cout << "  ✅ SUCCESS: Data consistency verified.\n";
    } else {
        std::cout << "  ❌ FAILURE: Data inconsistency detected!\n";
        if (!is_objective_match) std::cout << "     -> Objective does not match.\n";
        if (!is_weight_match)    std::cout << "     -> Weights (w) do not match.\n";
        
        // Dừng chương trình ngay nếu muốn
        // errorTermination("Debug check failed."); 
    }
    std::cout << std::string(70, '=') << "\n";
}

void AntSolution::validate() {
      double aux_infeasibility = 0;
      for (uint16_t i = 0; i < instance.nK; i++) {
            if (partitions[i].empty()) {
                  errorTermination("Invalid solution detected: Empty cluster!");
            }
            double accumulated_attributes[instance.nT] = {};
            for (uint16_t &index : partitions[i]) {
                  if (instance.nV <= index) {
                        errorTermination("Invalid solution detected: Vertex does not exists!");
                  }
                  for (uint16_t t = 0; t < instance.nT; t++) {
                        accumulated_attributes[t] += instance.W[index][t];
                  }
            }
            // for (uint16_t t = 0; t < instance.nT; t++) {
            //       if (accumulated_attributes[t] + parameters.eps < instance.Wl[i][t] or instance.Wu[i][t] < accumulated_attributes[t] - parameters.eps) {
            //             errorTermination("Invalid solution detected: Out of bound!");
            //       }
            // }
            for (uint16_t t = 0; t < instance.nT; t++) {
                  if (accumulated_attributes[t] + parameters.eps < instance.Wl[i][t]) {
                        aux_infeasibility += instance.Wl[i][t] - accumulated_attributes[t];
                  }
                  if (instance.Wu[i][t] < accumulated_attributes[t] - parameters.eps) {
                        aux_infeasibility += accumulated_attributes[t] - instance.Wu[i][t];
                  }
            }
      }

      std::vector<uint16_t> used(instance.nV);
      for (uint16_t i = 0; i < instance.nK; i++) {
            for (uint16_t &index : partitions[i]) {
                  used[index] += 1;
                  if (used[index] > 1) {
                        errorTermination("Invalid solution detected: Duplicate vertices!");
                  }
            }
      }

      if (int count = std::accumulate(used.begin(), used.end(), 0); count != instance.nV) {
            assert(count < instance.nV);
            errorTermination("Invalid solution detected: Insufficient vertex count in solution!");
      }

      if (std::abs(aux_infeasibility - infeasibility) > parameters.eps) {
            printDetailedDebug();
            errorTermination("Logic error detected: Wrong infeasibility!");
      }

      double aux_objective = 0;
      for (uint16_t c = 0; c < instance.nK; c++) {
            for (auto v1 : partitions[c]) {
                  for (auto v2 : partitions[c]) {
                        aux_objective += instance.D[v1][v2];
                  }
            }
      }

      if (std::abs(aux_objective - objective) > parameters.eps) {
            printDetailedDebug();
            errorTermination("Logic error detected: Wrong objective!");
      }
}

bool AntSolution::checkFeasibility() {
      for (uint16_t i = 0; i < instance.nK; i++) {
            double accumulated_attributes[instance.nT] = {};
            for (uint16_t &index : partitions[i]) {
                  for (uint16_t t = 0; t < instance.nT; t++) {
                        accumulated_attributes[t] += instance.W[index][t];
                  }
            }
            for (uint16_t t = 0; t < instance.nT; t++) {
                  if (accumulated_attributes[t] + parameters.eps < instance.Wl[i][t] or instance.Wu[i][t] < accumulated_attributes[t] - parameters.eps) {
                        printDetailedDebug();
                        return false;
                  }
            }
      }
      return true;
}