#!/bin/bash
g++ -w -o puzzle constraintSatisfactionEASearch.cpp
./puzzle
./puzzle ./configs/generatedPuzzleConfig.cfg
./puzzle ./configs/givenPuzzleConfig.cfg