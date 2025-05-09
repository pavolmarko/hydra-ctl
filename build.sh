#!/bin/bash
mkdir -p ./out
COMMON_SOURCES="src/hydra_lib/hydra_radio.cc src/hydra_lib/hydra_client.cc"
HYDRA_CTL_SOURCES="$COMMON_SOURCES src/hydra_ctl/hydra_ctl.cc"
HYDRA_SIM_SOURCES="$COMMON_SOURCES src/hydra_sim/hydra_sim.cc"

g++ -o out/hydra_ctl -std=c++2b -I ./src -I /usr/include/libnl3 $HYDRA_CTL_SOURCES
g++ -o out/hydra_sim -std=c++2b -I ./src -I /usr/include/libnl3 $HYDRA_SIM_SOURCES
