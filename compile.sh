#/bin/bash -ex

# Comment out executing JIT in Generator.cc before using
# TODO: have ingotc parse arguments and either execute JIT or output LLVM IR.

./build/src/ingotc ${1} > ${2}.ll
llc ${2}.ll -o ${2}.s
g++ -o ${2}_raw.o -c ${2}.s
ld -r -o ${2}.o ${2}_raw.o ./build/src/codegen/libingotruntime.a
g++ -o ${2} ${2}.o