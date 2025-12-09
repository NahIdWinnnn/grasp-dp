#!/bin/bash

rm -rf results/logs/legacy_logs
mkdir -p results/logs/legacy_logs/evolution
mkdir -p results/logs/legacy_logs/solutions
mkdir -p results/logs/legacy_logs/objectives

config_params='--schema 2P-R-GRASP --version rnd-grd --m 4 --block 38 --delta 1 --exploration first'
fixed_params='--termination_criteria iter --termination_value 1000 --logs 1 --move ext --efficient 1'

./MCGP --instance 'normalized_instances/pollster/muestra1_20_3.txt' --seed 1 $fixed_params $config_params