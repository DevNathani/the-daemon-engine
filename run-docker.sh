#!/bin/bash

sudo docker build -t the-daemon-engine:theDaemon -f docker/Dockerfile .
sudo docker run -d --name the-daemon-engine -p 2222:22 -v $(pwd)/var/game:/var/game -v /etc/localtime:/etc/localtime:ro -v /etc/timezone:/etc/timezone:ro --cpus="3.0" --memory="6g" --pids-limit=9000 --hostname=theDaemon the-daemon-engine:theDaemon
sudo docker exec -it the-daemon-engine bash

