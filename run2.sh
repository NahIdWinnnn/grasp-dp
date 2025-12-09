#!/bin/bash

rm -rf results/logs/dp_logs
mkdir -p results/logs/dp_logs/evolution
mkdir -p results/logs/dp_logs/solutions
mkdir -p results/logs/dp_logs/objectives

configuration='--termination_criteria iter --termination_value 500 --logs 1 --seed 1'
cons_params='--cons_model greedy-grasp --cons_move extended --cons_exploration first'
sear_params='--sear_model 2P-R-GRASP-DP --sear_move extended --sear_exploration first'
grasp_params='--version random-greedy --m 4 --block 40 --delta 1'

./DP_GRASP_MCGP --instance 'normalized_instances/tsplib/a280_05_2.txt' $configuration $cons_params $sear_params $grasp_params
