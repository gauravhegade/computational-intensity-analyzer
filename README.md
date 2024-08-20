# Computational Intensity Analyzer

This project provides a custom static analyzer built using the LLVM infrastructure. The tool is designed to analyze the computational intensity of code by examining the opcode distribution within LLVM IR.

## Installation and Setup

Follow these steps to build and run the tool:

### Build and Run the Tool

1. Generate the LLVM IR for your test file:

```bash
clang -S -emit-llvm test/test.c -o test/test.ll -O0 -Xclang -disable-O0-optnone
```

2. Compile the pass as a shared library:

```bash
clang++ -shared -fPIC -o AnalyzeWithCount.so `llvm-config --cxxflags --ldflags --system-libs --libs core` AnalyzeWithCount.cpp
```

3. Run the pass on the generated LLVM IR:

```bash
opt -load-pass-plugin ./AnalyzeWithCount.so -passes="analyze-computational-intensity-opcode" -disable-output test/test.ll
```
