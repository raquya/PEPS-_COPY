#! /bin/bash

for i in `seq 1 40000`
do
 ./DeltaCallTest i >> resultDelta.txt
done
