#! /bin/bash

for i in `seq 1 60000`
do
 ./HedgingCallTest i >> resultHedging.txt
done
