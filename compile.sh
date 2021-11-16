#/bin/bash

set -ex

# Comment out executing JIT in Generator.cc before using
# TODO: have ingotc parse arguments and either execute JIT or output LLVM IR.

cmake --build build -j16
./build/ingot/ingotc ${1} > ${2}.ll
llc ${2}.ll -o ${2}.s
g++ -o ${2}.o -c ${2}.s
g++ -o ${2} ${2}.o