#!/bin/bash

make re

python3 ./servicio_web.py & WEB_PID=$!

./server.out -p 8080

kill $WEB_PID