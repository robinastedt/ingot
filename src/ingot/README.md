# Ingot compiler
This project is a compiler for a programming language that I'm developing called Ingot.

## Goals
* Foremost for me to learn about compiler and language design
* To make a pure functional language that handles concurrency automatically and by default
  * I plan to accomlish this by designing the language in a way such that the vast majority of functions written by the user are pure (no side effects), and can therefore be trivially called in a deferred manner.
* It always bothered me that normal function definitions and lambda expressions have very different syntax, even in prominently functional languages such as haskell. I would like to see if I can find a syntax where there's no distinction between normal functions and lambda expressions in terms of their syntax.
* Since I'm using LLVM as a backend, arbitrarily sized integers should be a part of the language itself. i8 = char, i64 = int64_t, etc. But also i6, i512, should "just work".
