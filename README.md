# simplePascalCompiler

A simple Pascal compiler implementation using C++ and LLVM.

Both lexer and parser were implemented from scratch.

## Supported features

- [x] if-else
- [x] integer
- [x] while
- [x] function
- [x] pass by value
- [ ] pass by reference
- [ ] procedure
- [ ] string
- [ ] float

## Build

``
cmake  -B build/ -S . && cmake --build build
``

## Usage

To compile file.pas run:

``
./build/simpascompiler file.pas > t.ll && llc t.ll -o t.s && clang t.s -o program
``
