#!/bin/bash

rm -r results/logs
mkdir results/logs
mkdir results/logs/evolution
mkdir results/logs/solutions
mkdir results/logs/objectives

configuration='--termination_criteria iter --termination_value 1000 --logs 1 --seed 1 --move ext --efficient 1'
cons_params='--cons_model greedy-grasp --cons_move extended --cons_exploration first'
sear_params='--sear_model greedy-grasp --sear_move extended --sear_exploration first'
grasp_params='--version random-greedy --m 4 --block 40 --delta 1'

./MCGP --instance 'normalized_instances/tsplib/a280_05_2.txt' $configuration $cons_params $sear_params $grasp_params
