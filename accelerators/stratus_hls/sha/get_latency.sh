#!/usr/bin/bash
LATENCY_TRACE="./hls-work-virtex7/bdw_work/trace/sim.BEHAV_DMA64.trace"
LIST_INFO_DRIVER=$(cat $LATENCY_TRACE | grep "@")
ARR_INFO_DRIVER=($LIST_INFO_DRIVER)
TIME_BEG=${ARR_INFO_DRIVER[2]//@}
TIME_END_SCIENCE=${ARR_INFO_DRIVER[9]//@}
TIME_END=$(printf "%.f" "$TIME_END_SCIENCE")
TOT_TIME=$(echo "$TIME_END - $TIME_BEG" | bc)
echo "$TOT_TIME"