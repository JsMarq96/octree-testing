#include <cstdio>
#include <iostream>
#include "generation.h"

#include <libmorton/morton.h>

int main(int argc, char *argv[]) {
    FILE *file;

    file = fopen("resources/volumes/bonsai_256x256x256_uint8.raw", "rb");

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    OCTREE::sRawVolume vol = {  .raw_volume = (uint8_t*) malloc(file_size),
                                .width = 256,
                                .heigth = 256,
                                .depth = 256
    };

    fread(vol.raw_volume, file_size, 1, file);

    OCTREE::generate_octree(vol);

    return 0;
}
