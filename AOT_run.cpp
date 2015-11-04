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
    
    // Use 16-bit input.
    buffer_t input_buf = {0};
    
    input_buf.host = (uint8_t *)rawProcessor.imgdata.rawdata.raw_image;
    input_buf.stride[0] = 1;
    input_buf.stride[1] = rawProcessor.imgdata.sizes.width;

    input_buf.extent[0] = rawProcessor.imgdata.sizes.width;
    input_buf.extent[1] = rawProcessor.imgdata.sizes.height;
    
    input_buf.elem_size = sizeof(*rawProcessor.imgdata.rawdata.raw_image);

    // Allocate WxH 16-bit, 3 channel output buffer.
    buffer_t output_buf = {0};
    uint8_t output_data [rawProcessor.imgdata.sizes.width * rawProcessor.imgdata.sizes.height * 2 * 3];
    
    output_buf.host = output_data;

    output_buf.stride[0] = 1;
    output_buf.stride[1] = rawProcessor.imgdata.sizes.width;
    output_buf.stride[2] = rawProcessor.imgdata.sizes.width * rawProcessor.imgdata.sizes.height;

    output_buf.extent[0] = rawProcessor.imgdata.sizes.width;
    output_buf.extent[1] = rawProcessor.imgdata.sizes.height;
    output_buf.extent[2] = 3;
    output_buf.elem_size = 2;
    
    halide_debayer(&input_buf, &output_buf);

    Image<uint16_t> output_image = Image<uint16_t>(&output_buf, "output");
    output_image.copy_to_host();

    Tools::save_image(output_image, "test.png");

    return 0;
}