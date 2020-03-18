#!/usr/bin/bash

cd ~/esp/accelerators/stratus_hls/sha/hls-work-virtex7
export ESP_ROOT=../../../..; export ACCELERATOR=sha; export TECH=virtex7
source ~/source_file.sh
make sim_BEHAV_DMA64 -j4
