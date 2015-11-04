#include <libraw/libraw.h>
#include <iostream>
#include <Halide.h>
#include <HalideRuntime.h>
#include "KHC_YHC_demosaic_halide.h"
#include "halide_image_io.h"

using namespace Halide;

int main(int argc, char const *argv[])
{
    ImageParam param = ImageParam(UInt(16), 2);
    Func output = make_demosaic_func(param, UInt(16));
    output.compile_to_file("halide_debayer", std::vector<Argument>{param});
    return 0;
}