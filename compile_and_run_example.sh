#!/bin/bash
set -ex

mkdir -p build/compile
./compile.sh example.ingot build/compile/example
./build/compile/example