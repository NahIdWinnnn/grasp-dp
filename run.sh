#!/bin/bash

rm -r results/logs
mkdir results/logs
mkdir results/logs/evolution
mkdir results/logs/solutions
mkdir results/logs/objectives

config_params='--schema 2P-GRASP-DP --version rnd-grd --m 8 --block 38 --delta 5 --exploration first --debug 0'
fixed_params='--termination_criteria iter --termination_value 100 --logs 1 --move ext --efficient 1'

# ./MCGP --instance 'instances/tsplib/a280_05_2.txt' --seed 998244353 $fixed_params $config_params
./MCGP --instance 'normalized_instances/tsplib/a280_05_2.txt' --seed 998244353 $fixed_params $config_params
