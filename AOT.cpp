#include <libraw/libraw.h>
#include <iostream>
#include <Halide.h>
#include <HalideRuntime.h>
#include "KHC_YHC_demosaic_halide.h"
#include "halide_image_io.h"

using namespace Halide;

int main(int argc, char const *argv[])
{
    LibRaw rawProcessor;
    rawProcessor.open_file(argv[argc-1]);
    rawProcessor.unpack();
    
    buffer_t input_buf = {0};
    input_buf.host = (uint8_t *)rawProcessor.imgdata.rawdata.raw_image;
    input_buf.stride[0] = 1;
    input_buf.stride[1] = rawProcessor.imgdata.sizes.width;
    input_buf.extent[0] = rawProcessor.imgdata.sizes.width;
    input_buf.extent[1] = rawProcessor.imgdata.sizes.height;
    input_buf.elem_size = sizeof(*rawProcessor.imgdata.rawdata.raw_image);
    
    Image<uint16_t> in_image = Image<uint16_t>(&input_buf, "image");
    ImageParam param = ImageParam(UInt(16), 2);
    Func output = make_demosaic_func(param, UInt(16));
    output.compile_to_file("halide_debayer", std::vector<Argument>{param});

    //param.set(in_image);
    //Image<uint8_t> output_image = output.realize(in_image.width(), in_image.height(), 3);
    //output_image.copy_to_host();
    //Tools::save_image(output_image, "test.png");
    return 0;
}