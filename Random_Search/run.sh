#!/bin/bash
g++ -w -o puzzle randomSearch.cpp
./puzzle
./puzzle ./configs/config.cfg
./puzzle ./configs/randomConfig.cfg