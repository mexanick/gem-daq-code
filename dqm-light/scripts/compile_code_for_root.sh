#!/bin/bash

if [ -r myexe ]; then
  /bin/rm -rf myDQMlight
fi

if [ -r $1 ]; then
  echo $1 "will compile soon"
  g++ -g -std=c++0x -I/usr/include/root -I${BUILD_HOME}/gemdaq-testing/gemreadout/include $1 `root-config --libs --glibs` -o myDQMlight
  ls -ltF myDQMlight
else
  echo "any file for compilation is missing"
fi

if [ -r myDQMlight ]; then
  ./myDQMlight
else 
  echo "executable is missing"
fi