## GL Compiler

A single pass, recursive descendant LL(1) compiler writren for academic purpose.


## Compiling
* GCC (Linux)
```shell
g++ main.cpp -std=c++11 -o QGLC
```
* CLANG (MacOS)
```shell
clang++ main.cpp -std=c++11 -o QGLC
```
* MINGW (Windows)
```shell
g++ main.cpp -o QGLC.exe -std=c++11 -static-libgcc -static-libstdc++
```

## Usage
`QGLC.exe  [file] -o[0:1:2:3] -d[0:1:2:3] [-v] [-h]`

Parameter | Description |
----------:|:-----------|
  file | Input File
   -o0 | Does not optimize. Default
   -o1 | Peephole
   -o2 | Redundant Branchs and DeadCode(subsequent jumps)
   -o3 | `-o1` e `-o2`
   -d0 | No debug. Default
   -d1 | Comments on assembly without optimization.
   -d2 | Save optimized assembly files separately.
   -d3 | `-d1` e `-d2`
   -v  | About
   -h  | Help
