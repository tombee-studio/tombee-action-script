#!/bin/bash

FILE=$1
EP=$2
OBJ=obj
LIB=lib
 
if [ -n "$FILE" ] && [ -e "$FILE" ]; then
  echo "'$FILE'が存在します"
fi

if [ -e "$OBJ" ]; then
  echo "'$OBJ'が存在します"
else
  echo "'$OBJ'を作成します"
  mkdir -p "$OBJ"
fi

if [ -e "$LIB" ]; then
  echo "'$LIB'が存在します"
else
  echo "'$LIB'を作成します"
  mkdir -p "$LIB"
fi

make test
make library
echo "Test & Library build completed successfully."
