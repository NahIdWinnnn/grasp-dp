/*
      File: main.cpp
      Author: Nanashi
*/

#include "includes.h"
#include "algorithm/Algorithm.h"

Instance    instance;
Parameters  parameters;
std::vector<std::mt19937> rng;

std::string LoadInput(int argc, const char *argv[]);
void        LoadInstance(const std::string& pathInstance);



int main(int argc, const char *argv[]) {

      // Input arguments
      std::string instancePath = LoadInput(argc, argv);
      LoadInstance(instancePath);

      // Initialization
      std::random_device randomDevice;
      for(uint32_t p = 0; p < omp_get_max_threads(); p++) {
            rng.emplace_back(randomDevice());
            if (parameters.seed) {
                  rng[p].seed(static_cast<uint32_t>(uint64_t(parameters.seed) * (p + 1)));
            }
      }

      // Termination criteria
      uint32_t iter = 0;
      uint32_t iter_max = std::round(parameters.terminationValue);
      double time_used = 0;
      double time_max = parameters.terminationValue;
      bool finished = false;
      auto start_time = high_clock_t();

      Algorithm algorithm(instancePath);
}



std::string LoadInput(int argc, const char *argv[]) {

      std::string pathInstance;

      for(int i = 1; i < argc; i += 2) {
            bool input = false;
            if (argv[i][0] == '-' and i + 1 < argc) {
                  std::string key   = argv[i];
                  std::string value = argv[i + 1];

                  if (key == "--instance")                  { input = true, pathInstance                    = value;                }

                  // Configuration
                  if (key == "--termination_criteria")      { input = true, parameters.terminationCriteria  = value;                }
                  if (key == "--termination_value")         { input = true, parameters.terminationValue     = std::stod(value);     }
                  if (key == "--seed")                      { input = true, parameters.seed                 = std::stoi(value);     }
                  if (key == "--logs")                      { input = true, parameters.logs                 = std::stoi(value);     }

                  // Phase 1: Construction
                  if (key == "--cons_model")                { input = true, parameters.consModel            = value;                }
                  if (key == "--cons_move")                 { input = true, parameters.consMove             = value;                }
                  if (key == "--cons_exploration")          { input = true, parameters.consMoveStrat        = value;                }

                  // Phase 2: Local search
                  if (key == "--sear_model")                { input = true, parameters.searModel            = value;                }
                  if (key == "--sear_move")                 { input = true, parameters.searMove             = value;                }
                  if (key == "--sear_exploration")          { input = true, parameters.searMoveStrat        = value;                }

                  // GRASP
                  if (key == "--version")                   { input = true, parameters.GRASPver             = value;                }
                  if (key == "--m")                         { input = true, parameters.GRASPalphaDiv        = std::stoi(value);     }
                  if (key == "--block")                     { input = true, parameters.GRASPblock           = std::stoi(value);     }
                  if (key == "--delta")                     { input = true, parameters.GRASPdelta           = std::stoi(value);     }
            }

            if (!input) {
                  std::cout << "\n Invalid input: " << argv[i] << "\n";
                  std::cin.get();
                  std::exit(1);
            }
      }

      return pathInstance;
}

void LoadInstance(const std::string& pathInstance) {

      // Opens the instance file 
      std::ifstream file;
      file.open(pathInstance);

      if (!file) {
            errorTermination("The instance \"" + pathInstance + "\" can not be opened.");
      }

      // Loads the instance parameters
      std::string temp;
      file >> temp, instance.type = temp[0];

      file >> instance.nV >> instance.nK >> instance.nT;
      instance.D.assign(instance.nV, std::vector<double>(instance.nV));
      instance.W.assign(instance.nV, std::vector<double>(instance.nT));
      instance.Wl.assign(instance.nV, std::vector<double>(instance.nT));
      instance.Wu.assign(instance.nV, std::vector<double>(instance.nT));

      if (instance.type == 'p') {
            for (uint32_t i = 0; i < instance.nV; i++) {
                  file >> temp;

                  for (uint32_t t = 0; t < instance.nT; t++) {
                        file >> instance.W[i][t];
                  }

                  for (uint32_t j = 0; j < instance.nV; j++) {
                        file >> instance.D[i][j];
                        instance.D[i][j] /= 2;
                  }
            }
      }
      else if (instance.type == 't') {
            std::vector<std::vector<double>> coord(instance.nV, std::vector<double>(2));
            for (int i = 0; i < instance.nV; i++) {
                  file >> temp;
                  for (uint32_t t = 0; t < instance.nT; t++) {
                        file >> instance.W[i][t];
                  }
                  file >> coord[i][0] >> coord[i][1];
            }
            uint32_t decimals = 6;
            double   factor   = std::pow(10, decimals);
            for (int i = 0; i < instance.nV; i++) {
                  for (int j = i + 1; j < instance.nV; j++) {
                        instance.D[i][j] = std::round(std::sqrt(
                              (coord[i][0] - coord[j][0]) * (coord[i][0] - coord[j][0]) + (coord[i][1] - coord[j][1]) * (coord[i][1] - coord[j][1])
                        ) / 2.0 * factor) / factor;
                        instance.D[j][i] = instance.D[i][j];
                  }
            }
      }
      else {
            errorTermination("Wrong instance file.");
      }

      // Generates the weight limits for each pair (cluster,attribute)
      if (instance.type == 'p' or instance.type == 't') {
            for (uint32_t k = 0; k < instance.nK; k++) {
                  for (uint32_t t = 0; t < instance.nT; t++) {
                        file >> instance.Wl[k][t];
                  }
                  for (uint32_t t = 0; t < instance.nT; t++) {
                        file >> instance.Wu[k][t];
                  }
            }
      }

      // Closes the instance file
      file.close();
}