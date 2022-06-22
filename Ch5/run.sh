cd build
cmake ../
make -j8
cd ..
bin/Kaleidoscope
llvm-dis test.bc -o test.ll