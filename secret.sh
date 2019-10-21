#!/bin/bash
[ -d secret ] || mkdir secret
openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout secret/server.key -out secret/server.crt -subj /CN=yzlab3.chem.nyu.edu
openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout secret/rover.key  -out secret/rover.crt -subj /CN=rover
openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout secret/driver.key -out secret/driver.crt -subj /CN=driver 
cat secret/rover.crt secret/driver.crt > secret/clients.crt
echo -e "Hal?ou\nWode*%^!()" > secret/basicAuth.conf
