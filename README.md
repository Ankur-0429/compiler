# Hydrogen Compiler
![example workflow](https://github.com/Ankur-0429/compiler/actions/workflows/test.yml/badge.svg)

## Get Started

### LLVM
You will need to install llvm. [instructions can be found here](https://www.youtube.com/watch?v=l0LI_7KeFtw)

You might need to modify the location of your llvm project in the root directory of the CMakeLists.txt file

```cmake
set(LLVM_DIR /opt/homebrew/opt/llvm/lib/cmake/llvm)
```

The one above works for macOS systems if you're installed it using homebrew. As long as you install llvm in a standard file location, this shouldn't be a problem.

### Build
```sh
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug -j 12
```

### Test
```sh
ctest --test-dir cmake-build-debug --output-on-failure
```