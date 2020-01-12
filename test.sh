#!/bin/bash

FILE=$1
EP=$2
OBJ=obj
LIB=lib
 
if [ -e $FILE ]; then
  echo "'$FILE'が存在します"
  if [ -e $OBJ ]; then
    echo "'$OBJ'が存在します"
  else
    echo "'$OBJ'を作成します"
    mkdir $OBJ
  fi
  if [ -e $LIB ]; then
    echo "'$LIB'が存在します"
  else
    echo "'$LIB'を作成します"
    mkdir $LIB
  fi
  make test
  make library
  ./a.out $FILE
  rm -rf a.out *.o
fi
