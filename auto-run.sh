#!/bin/bash

INPUT_DIR="normalized_instances/tsplib"
SUMMARY_DIR="results/summary"
MAX_PARALLEL_INSTANCES=1

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
NC='\033[0m'

set -u 

mkdir -p "$SUMMARY_DIR"

is_running() {
    kill -0 "$1" 2>/dev/null
}

wait_and_visualize() {
    local pids=("$@")
    local model_labels=("Legacy" "Pred  " "DP    " "Hybrid")
    local spin='-\|/'
    local i=0

    tput civis

    while true; do
        all_done=true

        if [ "$i" -gt 0 ]; then
            tput cuu 4
        fi

        for idx in {0..3}; do
            pid="${pids[$idx]}"
            model_name="${model_labels[$idx]}"
            
            if is_running "$pid"; then
                all_done=false
                printf "${CYAN}[ %c ]${NC} %s is ${YELLOW}RUNNING...${NC} (PID: %s)   \n" "${spin:i%4:1}" "$model_name" "$pid"
            else
                printf "${GREEN}[ ✔ ]${NC} %s is ${GREEN}DONE/STOPPED${NC}           \n" "$model_name"
            fi
        done

        if $all_done; then
            break
        fi

        i=$((i+1))
        sleep 0.2
    done

    tput cnorm
    echo ""
}

run_legacy() {
    ./MCGP --instance "$1" --seed "$2" \
    --termination_criteria iter --termination_value 5000 --logs 1 --move ext --efficient 1 \
    --schema 2P-R-GRASP --version rnd-grd --m 4 --block 38 --delta 1 --exploration first
}

run_pred() {
    ./DP_GRASP_MCGP --instance "$1" \
    --termination_criteria iter --termination_value 5000 --logs 1 --seed "$2" \
    --cons_model greedy-grasp --cons_move extended --cons_exploration first \
    --sear_model 2P-R-GRASP --sear_move extended --sear_exploration first \
    --version random-greedy --m 4 --block 40 --delta 1
}

run_dp() {
    ./DP_GRASP_MCGP --instance "$1" \
    --termination_criteria iter --termination_value 5000 --logs 1 --seed "$2" \
    --cons_model greedy-grasp --cons_move extended --cons_exploration first \
    --sear_model 2P-R-GRASP-DP --sear_move extended --sear_exploration first \
    --version random-greedy --m 4 --block 40 --delta 1
}

run_hybrid() {
    ./DP_GRASP_MCGP --instance "$1" \
    --termination_criteria iter --termination_value 5000 --logs 1 --seed "$2" \
    --cons_model greedy-grasp --cons_move extended --cons_exploration first \
    --sear_model 2P-R-HGRASP-DP --sear_move extended --sear_exploration first \
    --version random-greedy --m 4 --block 40 --delta 1
}

process_file() {
    inst="$1"
    name=$(basename "$inst" .txt)

    echo -e "${BLUE}============================================================${NC}"
    echo -e "${BLUE} PROCESSING: $name ${NC}"
    echo -e "${BLUE}============================================================${NC}"

    for seed in {1..10}; do
        echo -e "\n${YELLOW}>>> Running Seed $seed / 10 for $name${NC}"

        SEED_DIR="$SUMMARY_DIR/$name/seed_$seed"
        mkdir -p "$SEED_DIR"

        rm -rf results/logs/legacy_logs results/logs/pred_logs results/logs/dp_logs results/logs/hybrid_logs
        mkdir -p results/logs/legacy_logs/{evolution,solutions,objectives}
        mkdir -p results/logs/pred_logs/{evolution,solutions,objectives}
        mkdir -p results/logs/dp_logs/{evolution,solutions,objectives}
        mkdir -p results/logs/hybrid_logs/{evolution,solutions,objectives}

        run_legacy "$inst" "$seed" > legacy_out.log 2>&1 & pid0=$!
        run_pred   "$inst" "$seed" > pred_out.log   2>&1 & pid1=$!
        run_dp     "$inst" "$seed" > dp_out.log     2>&1 & pid2=$!
        run_hybrid "$inst" "$seed" > hybrid_out.log 2>&1 & pid3=$!

        wait_and_visualize $pid0 $pid1 $pid2 $pid3

        error_report=""
        has_error=0

        wait $pid0
        code=$?
        if [ $code -ne 0 ]; then
            error_report+="\n   - ${RED}LEGACY Model${NC} failed (PID $pid0). Exit Code: ${RED}$code${NC}. Check 'legacy_out.log'."
            has_error=1
        fi

        wait $pid1
        code=$?
        if [ $code -ne 0 ]; then
            error_report+="\n   - ${RED}PRED Model${NC} failed   (PID $pid1). Exit Code: ${RED}$code${NC}. Check 'pred_out.log'."
            has_error=1
        fi

        wait $pid2
        code=$?
        if [ $code -ne 0 ]; then
            error_report+="\n   - ${RED}DP Model${NC} failed     (PID $pid2). Exit Code: ${RED}$code${NC}. Check 'dp_out.log'."
            has_error=1
        fi

        wait $pid3
        code=$?
        if [ $code -ne 0 ]; then
            error_report+="\n   - ${RED}HYBRID Model${NC} failed (PID $pid3). Exit Code: ${RED}$code${NC}. Check 'hybrid_out.log'."
            has_error=1
        fi

        if [ $has_error -ne 0 ]; then
            echo -e "\n${RED}!!! CRITICAL EXECUTION ERROR AT SEED $seed !!!${NC}"
            echo -e "Details:$error_report"
            echo -e "${RED}Aborting script immediately to prevent data corruption.${NC}"

            tput cnorm 
            exit 1
        fi

        res_legacy=$(cat results/logs/legacy_logs/objectives/* 2>/dev/null | head -n 1 || echo "N/A")
        res_pred=$(cat results/logs/pred_logs/objectives/* 2>/dev/null | head -n 1 || echo "N/A")
        res_dp=$(cat results/logs/dp_logs/objectives/* 2>/dev/null | head -n 1 || echo "N/A")
        res_hybrid=$(cat results/logs/hybrid_logs/objectives/* 2>/dev/null | head -n 1 || echo "N/A")

        get_sort_val() { if [[ "$1" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then echo "$1"; else echo "9999999999"; fi; }

        v1=$(get_sort_val "$res_legacy")
        v2=$(get_sort_val "$res_pred")
        v3=$(get_sort_val "$res_dp")
        v4=$(get_sort_val "$res_hybrid")

        sorted_list=$(echo -e "$v1 LEGACY\n$v2 PRED\n$v3 DP\n$v4 HYBRID" | sort -n -k1)

        get_rank_str() {
            local target_model="$1"
            local rank=$(echo "$sorted_list" | grep -n "$target_model" | cut -d: -f1)
            local val_check=$(echo "$sorted_list" | grep "$target_model" | cut -d' ' -f1)
            
            if [ "$val_check" == "9999999999" ]; then echo "${RED}(Failed)${NC}";
            elif [ "$rank" == "1" ]; then echo "${YELLOW}(Rank: 1 *)${NC}";
            else echo "(Rank: $rank)"; fi
        }

        get_pct_str() {
            local val="$1"
            local base="$2"

            if [[ ! "$val" =~ ^[0-9]+(\.[0-9]+)?$ ]] || [[ ! "$base" =~ ^[0-9]+(\.[0-9]+)?$ ]] || [ "$base" == "0" ]; then
                echo ""
                return
            fi

            awk -v a="$val" -v b="$base" 'BEGIN { printf "(%.2f%%)", (a / b) * 100 }'
        }

        pct_legacy="(100.00%)"
        pct_pred=$(get_pct_str "$res_pred" "$res_legacy")
        pct_dp=$(get_pct_str "$res_dp" "$res_legacy")
        pct_hybrid=$(get_pct_str "$res_hybrid" "$res_legacy")

        echo -e "${PURPLE}   -> Scores / Comparison (Legacy = 100%) / Ranking:${NC}"
        echo -e "${CYAN}      • Legacy:  [$res_legacy] \t $pct_legacy \t $(get_rank_str "LEGACY")${NC}"
        echo -e "${CYAN}      • Pred:    [$res_pred] \t $pct_pred \t $(get_rank_str "PRED")${NC}"
        echo -e "${CYAN}      • DP:      [$res_dp] \t $pct_dp \t $(get_rank_str "DP")${NC}"
        echo -e "${CYAN}      • Hybrid:  [$res_hybrid] \t $pct_hybrid \t $(get_rank_str "HYBRID")${NC}"

        mv results/logs/legacy_logs results/logs/Legacy
        mv results/logs/pred_logs   results/logs/Pred
        mv results/logs/dp_logs     results/logs/DP
        mv results/logs/hybrid_logs results/logs/Hybrid

        rm -rf "$SEED_DIR"

        mv results/logs "$SEED_DIR"

        rm -f legacy_out.log pred_out.log dp_out.log hybrid_out.log
    done

    echo -e "${GREEN}Instance $name completed successfully.${NC}"
}

shopt -s nullglob
files=("$INPUT_DIR"/*.txt)

if [ ${#files[@]} -eq 0 ]; then
    echo "Error: No .txt files found in $INPUT_DIR"
    exit 1
fi

for inst in "${files[@]}"; do
    process_file "$inst"
done

echo -e "\n${GREEN}ALL JOBS FINISHED SUCCESSFULLY.${NC}"