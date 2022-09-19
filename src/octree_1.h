#ifndef OCTREE_1_H_
#define OCTREE_1_H_

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cassert>

enum eVoxelType : uint32_t {
    FULL_VOXEL = 0,
    EMPTY_VOXEL,
    MIXED_VOXEL,
    TBD_VOXEL
};

struct sVoxel {
    eVoxelType type;
    uint32_t  childs[8];
    // TODO: pad for the pixel textre aligment
};

struct sRawVolume {
    uint8_t *raw_volume;
    uint16_t width = 0;
    uint16_t heigth = 0;
    uint16_t depth = 0;

    uint8_t density_threshold = 20;

    inline eVoxelType get_pixel_state(const uint16_t x,
                                      const uint16_t y,
                                      const uint16_t z) const {
        uint8_t value = raw_volume[(x + y * width + z * heigth * width)];

        return (value > density_threshold) ? FULL_VOXEL : EMPTY_VOXEL;
    }

    inline uint8_t get_pixel(const uint16_t x,
                             const uint16_t y,
                             const uint16_t z) const {
        return raw_volume[(x + y * width + z * heigth * width)];
    }

};

inline uint32_t get_octree_index(const uint32_t curr_level,
                                 const uint32_t x,
                                 const uint32_t y,
                                 const uint32_t z) {
    // Early out
    // TODO: speedup via LUT
    if (curr_level == 0) {
        return 0;
    }
    uint32_t index = 0;
    for(uint32_t i = 0; i < curr_level; i++) {
        uint32_t size = pow(2, i);
        index += size*size*size;
    }

    return index;
}

inline void fill_children_of_voxel(const uint32_t curr_level,
                                   const uint32_t x,
                                   const uint32_t y,
                                   const uint32_t z,
                                   uint32_t *children) {
    const uint32_t prev_level_start = get_octree_index(curr_level+1,
                                                       0,
                                                       0,
                                                       0);
    const uint32_t prev_level_size = pow(2,
                                         curr_level+1);

    // Using some napkin math, to map the index to the previus index
    const uint32_t x_prev = (x == 0) ? 0 : pow(2, x);
    const uint32_t y_prev = (y == 0) ? 0 : pow(2, y);
    const uint32_t z_prev = (z == 0)? 0 :pow(2, z);

    for(uint32_t z_delta = 0; z_delta < 2; z_delta++) {
        for(uint32_t y_delta = 0; y_delta < 2; y_delta++) {
            for(uint32_t x_delta = 0; x_delta < 2; x_delta++) {
                children[x_delta + y_delta * 2 + z_delta * 4] = x_prev + x_delta +
                                                                (y_prev + y_delta) * prev_level_size +
                                                                (z_prev + z_delta) * (prev_level_size * prev_level_size) + prev_level_start;
            }
        }
    }
}


#define IS_POWER_OF_TWO(x) ((x & (x - 1)) == 0)


inline sVoxel* octree_generation(const sRawVolume &raw_vol) {
    assert(raw_vol.width == raw_vol.depth &&
               raw_vol.width == raw_vol.heigth &&
               IS_POWER_OF_TWO(raw_vol.width) &&
               "Error while generating octree, non square and power of two text");


    // Calculate the memmry chunk of the whle tree
    uint32_t octree_size = 0;
    uint32_t size = raw_vol.width;
    uint32_t level_count = 0;
    while(size >= 1) {
        octree_size += size*size*size;
        size /= 2;
        level_count++;
    }
    sVoxel *octree = (sVoxel*) malloc(sizeof(sVoxel) * octree_size);

    // Populate the base of the octree
    const uint32_t base_size = raw_vol.width;
    const uint32_t base_area_size = raw_vol.width * raw_vol.width * raw_vol.width;
    const uint32_t base_index = get_octree_index(level_count-1, 0, 0, 0);
    for(uint32_t i = 0; i < base_area_size; i++) {
        const uint32_t x = i % base_size;
        const uint32_t y = (i / base_size) % base_size;
        const uint32_t z = i / (base_size * base_size);

        octree[base_index + i].type = raw_vol.get_pixel_state(x,
                                                              y,
                                                              z);
    }

    // Generate all the layers octree bottom-to-top
    uint32_t current_level = level_count -2;
    uint32_t layer_start = get_octree_index(level_count-2,
                                            0,
                                            0,
                                            0);

    uint32_t layer_size = base_size / 2;
    uint32_t layer_area = layer_size * layer_size * layer_size;
    uint32_t prev_layer_size = base_size;
    uint32_t prev_layer_start = base_index;
    for(uint32_t i = 0; i < layer_area; i++) {
        const uint32_t x = i % layer_size;
        const uint32_t y = (i / layer_size) % layer_size;
        const uint32_t z = i / (layer_size * layer_size);

        uint32_t index = i + layer_start;

        fill_children_of_voxel(current_level,
                               x,
                               y,
                               z,
                               octree[layer_start + i].childs);

        eVoxelType curr_voxel = TBD_VOXEL;

        for(uint8_t child_id = 0; child_id < 8; child_id++) {
            eVoxelType child_state = octree[octree[index].childs[child_id]].type;

            if (curr_voxel == TBD_VOXEL) {
                curr_voxel = child_state;
            }

            if (child_state == MIXED_VOXEL || child_state != curr_voxel) {
                curr_voxel = MIXED_VOXEL;
                break;
            }
        }

        octree[index].type = curr_voxel;
    }


    return octree;
}

#endif // OCTREE_1_H_
