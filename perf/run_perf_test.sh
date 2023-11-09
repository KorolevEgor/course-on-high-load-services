#!/bin/bash

for i in 1 2 5 10
    do
        echo "Params: threads: $i, connections: $i"
        wrk -d 10 -t $i -c $i --latency -s './cache_perf.lua' http://localhost:8080/
    done
