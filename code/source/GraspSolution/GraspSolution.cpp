/*
      File: GraspSolution.cpp
      Author: Nanashi
*/

#include "GraspSolution.h"

GraspSolution::GraspSolution(double alpha) {
      if (alpha > 1) {
            objective = std::numeric_limits<double>::max();
      }
      construct(alpha);
}

void GraspSolution::printDetailedDebug() {
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

void GraspSolution::validate() {
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

bool GraspSolution::checkFeasibility() {
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

void GraspSolution::construct(double alpha) {

      // Data assignment
      partitions.assign(instance.nK, {});
      delta.assign(instance.nV, std::vector<double>(instance.nK));
      w.assign(instance.nK, std::vector<double>(instance.nT));
      sigma.assign(instance.nK, double(0));
      objective = 0;
      infeasibility = 0;

      // Assertion
      if (instance.nV < instance.nK) {
            errorTermination("Invalid input: Number of vertices(" + std::to_string(instance.nV) + ") must be greater than number of clusters(" + std::to_string(instance.nK) + ")!");
      }

      // Construction
      if (parameters.consModel == "greedy-grasp") {
            constructGreedy(alpha);
      }
      else if (parameters.consModel == "random-grasp") {
            constructRandomized();
      }
      else {
            errorTermination("Invalid configuration: Construction model \"" + parameters.consModel + "\" is not available!");
      }
}

bool GraspSolution::explore(bool objective) {
      if (objective) {
            if (!checkFeasibility()) {
                  std::cerr << "1\n";
                  errorTermination("Invalid solution detected: Infeasible solution BEFORE explore!");
            }
      }
      if (parameters.searMove == "insert") {
            auto [der, vPos, source, target] = exploreInsert(objective);
            if (der < 0) {
                  insertVertex(vPos, source, target);
                  if (objective) {
                        validate();
                        if (!checkFeasibility()) {
                              errorTermination("Invalid solution detected: Infeasible solution after insert move!");
                        }
                  }
                  return true;
            }
      }
      else if (parameters.searMove == "exchange") {
            auto [der, fPos, fClus, sPos, sClus] = exploreExchange(objective);
            if (der < 0) {
                  exchangeVertex(fPos, fClus, sPos, sClus);
                  if (objective) {
                        validate();
                        if (!checkFeasibility()) {
                              errorTermination("Invalid solution detected: Infeasible solution after exchange move!");
                        }
                  }
                  return true;
            }
      }
      else {
            if (objective) {
                  if (!checkFeasibility()) {
                        std::cerr << "2\n";
                        errorTermination("Invalid solution detected: Infeasible solution BEFORE extended move!");
                  }
            }
            assert(parameters.searMove == "extended");
            auto [derI, vPos, source, target] = exploreInsert(objective);
            auto [derE, fPos, fClus, sPos, sClus] = exploreExchange(objective);
            if (objective) {
                  if (!checkFeasibility()) {
                        std::cerr << "3\n";
                        errorTermination("Invalid solution detected: Infeasible solution after expIns, expExc!");
                  }
            }
            if (derI < 0 or derE < 0) {
                  if (derI < derE) {
                        // uint16_t vIndex = partitions[source][vPos];
                        // auto pre = partitions;
                        // validate();
                        // if (!checkFeasibility()) {
                        //       std::cerr << "Wtf\n";
                        //       errorTermination("Invalid solution detected: Infeasible solution BEFORE extended-insert move!");
                        // }
                        insertVertex(vPos, source, target);
                        // if (objective) {
                        //       validate();
                        // if (!checkFeasibility()) {
                        //       std::cerr << "inserted: " << vIndex << " from " << source << " to " << target << "\n\n";
                        //       for (int c = 0; c < instance.nK; c++) {
                        //             std::cerr << "cluster " << c << ":\n";
                        //             ///////
                        //             for (int i = 0; i < instance.nT; i++) {
                        //                   std::cerr << instance.Wl[c][i] << " ";
                        //             } std::cerr << "\n";
                        //             for (int i = 0; i < instance.nT; i++) {
                        //                   double W = 0;
                        //                   for (auto vId : pre[c]) {
                        //                         W += instance.W[vId][i];
                        //                   }
                        //                   std::cerr << W << " ";
                        //             } std::cerr << "\n";
                        //             for (int i = 0; i < instance.nT; i++) {
                        //                   std::cerr << instance.Wu[c][i] << " ";
                        //             } std::cerr << "\n";
                        //             //////
                        //             for (int i = 0; i < pre[c].size(); i++) {
                        //                   std::cerr << pre[c][i] << " ";
                        //             }
                        //             std::cerr << "\n";
                        //       }
                        //       std::cerr << "\nCurrent:\n";
                        //       for (int c = 0; c < instance.nK; c++) {
                        //             std::cerr << "cluster " << c << ":\n";
                        //             ///////
                        //             for (int i = 0; i < instance.nT; i++) {
                        //                   std::cerr << instance.Wl[c][i] << " ";
                        //             } std::cerr << "\n";
                        //             for (int i = 0; i < instance.nT; i++) {
                        //                   double W = 0;
                        //                   for (auto vId : partitions[c]) {
                        //                         W += instance.W[vId][i];
                        //                   }
                        //                   std::cerr << W << " ";
                        //             } std::cerr << "\n";
                        //             for (int i = 0; i < instance.nT; i++) {
                        //                   std::cerr << instance.Wu[c][i] << " ";
                        //             } std::cerr << "\n";
                        //             //////
                        //             for (int i = 0; i < partitions[c].size(); i++) {
                        //                   std::cerr << partitions[c][i] << " ";
                        //             }
                        //             std::cerr << "\n";
                        //       }
                        //       std::cerr << "\n";
                        //       errorTermination("Invalid solution detected: Infeasible solution after extended-insert move!");
                        //       }
                        // }
                  }
                  else {
                        exchangeVertex(fPos, fClus, sPos, sClus);
                        if (objective) {
                              validate();
                              if (!checkFeasibility()) {
                                    errorTermination("Invalid solution detected: Infeasible solution after extended-exchange move!");
                              }
                        }
                        
                  }
                  return true;
            }
      }

      return false;
}
