#!/bin/bash
g++ -w -o puzzle eaSearch.cpp
./puzzle
./puzzle ./configs/generatedPuzzleConfig.cfg
./puzzle ./configs/givenPuzzleConfig.cfg