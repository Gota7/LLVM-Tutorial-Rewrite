cd build
cmake ../
make -j8
cd ..
bin/Kaleidoscope
clang++ driver.cpp test.o -o driver.o
./driver.o