#!/bin/bash

#Colours
declare -r greenColour="\e[0;32m\033[1m"
declare -r redColour="\e[0;31m\033[1m"
declare -r blueColour="\e[0;34m\033[1m"
declare -r yellowColour="\e[0;33m\033[1m"
declare -r endColour="\033[0m\e[0m"

UNO="$(./run-cliente1.sh &)"
DOS="$(./run-cliente2.sh &)"
TRES="$(./run-cliente3.sh &)"
CUATRO="$(./run-cliente4.sh &)"
wait

echo -e "${redColour}${UNO}${endColour}\n"
echo -e "${greenColour}${DOS}${endColour}\n"
echo -e "${blueColour}${TRES}${endColour}\n"
echo -e "${yellowColour}${CUATRO}${endColour}\n"
