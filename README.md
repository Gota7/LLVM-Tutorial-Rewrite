# LLVM Tutorial Rewrite
LLVM tutorial has been refactored to my code style, and a more sophisticated build system was added to implement this.

Since the parser and lexer are now object oriented, I implemented them reading from a generic input stream instead of standard in. All of the test cases read from test.k sample code, but this could manually be changed to read from std::cin if appropriate.

Requirements:
* LLVM-13
* Clang++
* llvm-dis
* Cmake build system.

Running a test:
1. cd to the appropriate folder.
2. Delete the build folder and replace it with an empty folder called build.
3. Run ./run.sh to build and run the code.
