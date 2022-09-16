#ifndef OCTREE_H_
#define OCTREE_H_

#include "generation.h"
#include <cstdint>
#include <cmath>
#include <cstring>

#include <libmorton/morton.h>
#include <type_traits>

namespace OCTREE {

    struct sOctreeNode {
        // Strucutre:
        //  - First texel:
        //    type
        //    child 0
        //    child 1
        //    child 2
        //  - Second texel:
        //    child 3
        //    child 4
        //    child 5
        //    child 6
        //  - Third Texel
        //    child 7
        //    padding * 3
        uint32_t type = 0;
        union {
            struct {
                uint32_t child_0 = 0;
                uint32_t child_1 = 0;
                uint32_t child_2 = 0;
                // Second texel
                uint32_t child_3 = 0;
                uint32_t child_4 = 0;
                uint32_t child_5 = 0;
                uint32_t child_6 = 0;
                // Third texel :(
                uint32_t child_7 = 0;
            };

            uint32_t children[8];
        };
        uint32_t padding[3];

        // TODO packing
        /*// Packed the octree ids
        union {
            uint32_t raw_1 = 0;
            struct {
                uint16_t child_0;
                uint16_t child_1;
            };
        };

        union {
            uint32_t raw_2 = 0;
            struct {
                uint16_t child_2;
                uint16_t child_3;
            };
        };

        union {
            uint32_t raw_3 = 0;
            struct {
                uint16_t child_4;
                uint16_t child_5;
            };
        };

        union {
            uint32_t raw_4 = 0;
            struct {
                uint16_t child_6;
                uint16_t child_7;
            };
        };*/

    };



    struct sVoxel {
        eVoxelState state = TBD_VOXEL;
        uint32_t children[8] = {};
    };

    struct sTreeLayer {
        uint16_t level;
        uint32_t width = 0;

        sVoxel *voxels  = NULL;

        sTreeLayer *next_layer = NULL;
        sTreeLayer *prev_layer = NULL;
    };

    inline void generate(const sRawVolume &volume) {
        assert(volume.width == volume.depth &&
               volume.width == volume.heigth &&
               IS_POWER_OF_TWO(volume.width) &&
               "Error while generating octree, non square and power of two text");


        const uint32_t base_voxel_count = volume.width * volume.width * volume.width;
        const uint32_t base_width = volume.width;
        const uint32_t base_squared = base_width * base_width;

        uint32_t num_blocks = 0;
        uint32_t it_blocks = volume.width;

        while(it_blocks > 1) {
            num_blocks += it_blocks * it_blocks * it_blocks;
            it_blocks = it_blocks / 2;
        }

        sOctreeNode *octree = (sOctreeNode*) malloc(sizeof(sOctreeNode) * num_blocks);

        uint32_t base_layer_start = num_blocks - base_voxel_count;
        // Store the children in each base with morton ordering
        for(uint32_t i = 0; i < base_voxel_count; i--) {
            uint16_t x = i % base_width;
            uint16_t y = (i / base_width) % base_width;
            uint16_t z = i / (base_squared);

            uint32_t morton_index = libmorton::morton3D_32_encode(x, y, z) + base_layer_start;
            octree[morton_index].type = volume.get_pixel_state(x,
                                                               y,
                                                               z);
        }

        uint32_t prev_layer_start_index = base_layer_start;
        uint32_t it_base = 256;

        // Child LUT
        uint32_t child_pos_LUT[8] = {};
        child_pos_LUT[0 + (0 * 2) + (0 * 4)] = libmorton::morton3D_32_encode(0, 0, 0);
        child_pos_LUT[0 + (0 * 2) + (1 * 4)] = libmorton::morton3D_32_encode(0, 0, 1);
        child_pos_LUT[0 + (1 * 2) + (0 * 4)] = libmorton::morton3D_32_encode(0, 1, 0);
        child_pos_LUT[0 + (1 * 2) + (1 * 4)] = libmorton::morton3D_32_encode(0, 1, 1);
        child_pos_LUT[1 + (0 * 2) + (0 * 4)] = libmorton::morton3D_32_encode(1, 0, 0);
        child_pos_LUT[1 + (0 * 2) + (1 * 4)] = libmorton::morton3D_32_encode(1, 0, 1);
        child_pos_LUT[1 + (1 * 2) + (0 * 4)] = libmorton::morton3D_32_encode(1, 1, 0);
        child_pos_LUT[1 + (1 * 2) + (1 * 4)] = libmorton::morton3D_32_encode(1, 1, 1);



        octree[0].type = TBD_VOXEL;
        for(uint32_t layer_index = base_layer_start; layer_index > 0;) {
            // Iterate to the next layer
            uint32_t prev_base_width = it_base;
            it_base /= 2;
            uint32_t curr_base_size = it_base * it_base * it_base;
            uint32_t curr_base_start = layer_index - curr_base_size;

            // For each new voxel on this level, we check the previus value
            for(uint32_t i = 0; i < curr_base_size; i++) {
                 uint16_t x = i % it_base;
                 uint16_t y = (i / it_base) % it_base;
                 uint16_t z = i / (it_base * it_base);

                 uint16_t child_x = (x / it_base) * prev_base_width;
                 uint16_t child_y = (y / it_base) * prev_base_width;
                 uint16_t child_z = (z / it_base) * prev_base_width;

                 uint32_t base_voxel_index = curr_base_start + libmorton::morton3D_32_encode(x,
                                                                                             y,
                                                                                             z);
                 eVoxelState base_state = TBD_VOXEL;

                 // Check te current voxel's children to see the type
                 for(uint16_t z = 0; z < 2; z++) {
                     for(uint16_t y = 0; y < 2; y++) {
                         for(uint16_t x = 0; x < 2; x++) {
                             uint32_t child_index = libmorton::morton3D_32_encode(child_x + x,
                                                                                  child_y + y,
                                                                                  child_z + z) + prev_layer_start_index;

                             // TODO this kinde of morton test can be encoded on a LUT
                             octree[base_voxel_index].children[child_pos_LUT[x + y * 2 + y * 4]] = child_index;

                             eVoxelState new_state = (eVoxelState) octree[child_index].type;
                             if (base_state == TBD_VOXEL) {
                                 base_state = new_state;
                             } else if (base_state != HALF_VOXEL) {
                                 if (base_state != new_state) {
                                     // Is mixed
                                     base_state = HALF_VOXEL;
                                 }
                             }
                         }
                     }
                 }

                 octree[base_voxel_index].type = (eVoxelState) base_state;
            }

            layer_index -= curr_base_size;
        }

        std::cout << octree[0].type << std::endl;

        uint32_t voxels_inside = 0;

    }
};


#endif // OCTREE_H_
