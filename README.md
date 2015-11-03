This is just as basic example of how to process RAW files with Halide and LibRAW.

Build with something like `clang++ main.cpp KHC_YHC_demosaic_halide.cpp -std=c++1y -lraw -I/usr/local/include -I$HOME/Build/Halide/build/include -I$HOME/Build/Halide/tools -L$HOME/Build/Halide/build/lib -lHalide -lpng`
