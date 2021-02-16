#!bin/bash

echo "There are ${#} numbers"

ans=0

for param in $@
do
ans=$(($ans+$param))
done
ans=$((ans/$#))
echo "Arefmetic mean: ${$sum/#}"