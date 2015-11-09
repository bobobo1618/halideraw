#include <libraw/libraw.h>
#include <iostream>
//#include <Halide.h>
#include <HalideRuntime.h>
#include "HalideRuntimeOpenCL.h"
#include "igd_demosaic.h"
#include "halide_image.h"
#include "halide_image_io.h"
#include <time.h>
#include <stdio.h>
#include <ctime>
//using namespace Halide;

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

    output_buf.stride[0] = 1;
    output_buf.stride[1] = rawProcessor.imgdata.sizes.width;
    output_buf.stride[2] = rawProcessor.imgdata.sizes.width * rawProcessor.imgdata.sizes.height;
    output_buf.stride[3] = rawProcessor.imgdata.sizes.width * rawProcessor.imgdata.sizes.height * 3;

    output_buf.extent[0] = rawProcessor.imgdata.sizes.width;
    output_buf.extent[1] = rawProcessor.imgdata.sizes.height;
    output_buf.extent[2] = 3;
    output_buf.elem_size = 2;

    uint8_t *output_data = new uint8_t[sizeof(uint16_t) * rawProcessor.imgdata.sizes.width * rawProcessor.imgdata.sizes.height * 3];
    output_buf.host = output_data;
    output_buf.host_dirty = false;
    output_buf.dev_dirty = false;
    output_buf.dev = 0;

    for (int i = 0; i<20; i++) {
        auto start = std::chrono::system_clock::now();
        input_buf.host_dirty = true;
        halide_copy_to_device(NULL, &input_buf, halide_opencl_device_interface());
        std::cout << "Input: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-start).count() << std::endl;

        buffer_t input_no_host = *(&input_buf);
        input_no_host.host = NULL;

        buffer_t output_no_host = *(&output_buf);
        output_no_host.host = (uint8_t *)1;

        igd_demosaic(&input_no_host, 0, &output_no_host);
        std::cout << "Halide: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-start).count() << std::endl;
        output_no_host.host = output_data;
        halide_copy_to_host(NULL, &output_no_host);
        std::cout << "Output: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-start).count() << std::endl << std::endl;
    }

    //Halide::Tools::Image<uint16_t> output_image = Image<uint16_t>(&output_buf, "output");
    //output_image.copy_to_host();

    //Tools::save_image(output_image, "test.png");

    return 0;
}
