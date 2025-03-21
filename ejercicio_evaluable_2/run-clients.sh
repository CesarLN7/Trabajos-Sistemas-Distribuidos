#!/bin/bash

#Colours
declare -r greenColour="\e[0;32m\033[1m"
declare -r endColour="\033[0m\e[0m"
declare -r redColour="\e[0;31m\033[1m"

UNO="$(./run-cliente1.sh &)"
DOS="$(./run-cliente2.sh &)"
wait

echo -e "${redColour}${UNO}${endColour}\n"
echo -e "${greenColour}${DOS}${endColour}\n"