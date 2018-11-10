#!/bin/bash
g++ -w -o puzzle MOEA.cpp
./puzzle
./puzzle ./configs/givenPuzzleConfig.cfg
./puzzle ./configs/generatedPuzzleConfig.cfg