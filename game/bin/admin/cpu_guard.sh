#!/bin/bash

THRESHOLD=15

echo " CPU Monitor started (threshold=${THRESHOLD}%)"

while true
do
    ps -eo user,pid,comm,%cpu --no-headers | awk -v t=$THRESHOLD '
    {
        if ($4 > t) {
            printf "[KILL] USER=%s PID=%s CMD=%s CPU=%s%%\n", $1, $2, $3, $4;
            system("kill -9 " $2);
        }
    }'

    sleep 2
done