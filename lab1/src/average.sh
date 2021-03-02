#!/bin/sh


echo "There are ${#} numbers"

ans=0

for param in $@
do
ans=$(($ans+$param))
done
mean=$(($ans/$#))
echo "Arefmetic mean: ${mean}"