#include <libraw/libraw.h>
#include <iostream>
#include <Halide.h>
#include <HalideRuntime.h>
#include "halide_debayer.h"
#include "halide_image_io.h"

using namespace Halide;

int main(int argc, char const *argv[])
{
    LibRaw rawProcessor;
    rawProcessor.open_file(argv[argc-1]);
    rawProcessor.unpack();
    
    buffer_t input_buf = {0}, output_buf = {0};
    input_buf.host = (uint8_t *)rawProcessor.imgdata.rawdata.raw_image;

    uint8_t output_data [rawProcessor.imgdata.sizes.width * rawProcessor.imgdata.sizes.height * 2 * 3];
    output_buf.host = output_data;

    input_buf.stride[0] = output_buf.stride[0] = 1;
    input_buf.stride[1] = output_buf.stride[1] = rawProcessor.imgdata.sizes.width;
    output_buf.stride[2] = rawProcessor.imgdata.sizes.width * rawProcessor.imgdata.sizes.height;
    
    input_buf.extent[0] = output_buf.extent[0] = rawProcessor.imgdata.sizes.width;
    input_buf.extent[1] = output_buf.extent[1] = rawProcessor.imgdata.sizes.height;
    output_buf.extent[2] = 3;
    
    input_buf.elem_size = sizeof(*rawProcessor.imgdata.rawdata.raw_image);
    output_buf.elem_size = sizeof(uint8_t);
    
    halide_debayer(&input_buf, &output_buf);

    //param.set(in_image);
    //Image<uint8_t> output_image = output.realize(in_image.width(), in_image.height(), 3);
    Image<uint16_t> output_image = Image<uint16_t>(&output_buf, "output");
    output_image.copy_to_host();
    Tools::save_image(output_image, "test.png");
    return 0;
}