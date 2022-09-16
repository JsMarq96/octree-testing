#ifndef OCTREE_H_
#define OCTREE_H_

#include "generation.h"
#include <cstdint>
#include <cmath>
#include <cstring>

namespace OCTREE {

    struct sOtreeNode {
        // First texel
        uint32_t type = 0;
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


        sTreeLayer *base_level = (sTreeLayer*) malloc(sizeof(sTreeLayer));
        base_level->voxels = (sVoxel*) malloc(sizeof(sVoxel) * base_voxel_count);
        base_level->level = sqrt(base_width) / 2;
        base_level->width = base_width;

        // Fill all the voxels from the volume
        for(uint32_t i = 0; i < base_voxel_count; i++) {
           uint16_t x = i % base_width;
           uint16_t y = (i / base_width) % base_width;
           uint16_t z = i / (base_squared);

           base_level->voxels[i].state = volume.get_pixel_state(x,
                                                                y,
                                                                z);
        }

        uint16_t base_it = base_width;

        uint16_t level = sqrt(base_width) / 2;
        level--;

        sTreeLayer *it_layer = base_level;

        uint32_t voxels_inside = base_voxel_count;

        // Iterate thorugh the levels, bottom-up direction,
        // merging the voxels that area uniform, and marking
        // the non uniform ones
        while(level > 1) {
            const uint32_t level_width = pow(2, level);
            const uint32_t level_width_sq = level_width * level_width;
            const uint32_t level_area = level_width * level_width * level_width;

            // Create new layer
            sTreeLayer *new_layer = (sTreeLayer*) malloc(sizeof(sTreeLayer));
            new_layer->level = level;
            new_layer->prev_layer = it_layer;
            new_layer->width = level_width;
            new_layer->voxels = (sVoxel*) malloc(sizeof(sVoxel) * level_area);

            it_layer = new_layer;

            uint32_t rescalling_indices = (1.0f / level_width) * it_layer->prev_layer->width;
            for(uint32_t i = 0; i < level_area; i++) {
                uint32_t x = i % level_width;
                uint32_t y = (i / level_width) % level_width;
                uint32_t z = i / level_width_sq;

                // Calculate the corresponding indexes to the prev level
                uint16_t old_x = x * rescalling_indices;
                uint16_t old_y = y * rescalling_indices;
                uint16_t old_z = z * rescalling_indices;

                eVoxelState voxel_state = TBD_VOXEL;

                const uint32_t prev_layer_width = it_layer->prev_layer->width;
                const uint32_t prev_layer_width_sq = prev_layer_width * prev_layer_width;

                uint32_t children_indices[8] = {};
                uint8_t child_indices_count = 0;
                for(uint16_t it_z = 0; it_z < 2; it_z++) {
                    for(uint16_t it_y = 0; it_y < 2; it_y++) {
                        for(uint16_t it_x = 0; it_x < 2; it_x++) {
                            const uint32_t index = (old_x + it_x) + (it_x + old_y) * prev_layer_width + (it_z + old_z) * prev_layer_width_sq;
                            eVoxelState state = it_layer->prev_layer->voxels[index].state;

                            // Store the indices for filling the father
                            children_indices[child_indices_count++] = index;
                            if (state == HALF_VOXEL) {
                                continue;
                                //goto exit_octant_check;
                            } else if (voxel_state == TBD_VOXEL) {
                                voxel_state = state;
                            } else if (state != voxel_state) {
                                voxel_state = HALF_VOXEL;
                                continue;
                                //goto exit_octant_check;
                            }
                        }
                    }
                }

                //exit_octant_check:

                // Store reference to children
                if (voxel_state == HALF_VOXEL) {
                    //memcpy(it_layer->voxels[i].children, children_indices, sizeof(uint32_t) * child_indices_count);
                    voxels_inside++;
                } else {
                    voxels_inside -= 7; // Reduce the number of blokc on the total end
                }

                 it_layer->voxels[i].state = voxel_state;
                 //std::cout << "level " << i<< std::endl;


            }

            level--;
        }

        std::cout << it_layer->width << "  -> " << voxels_inside << " vs " << (256*256*256) <<  std::endl;

        // Transverse the resulting tree and generate the compressed sparse tree
    }
};


#endif // OCTREE_H_
