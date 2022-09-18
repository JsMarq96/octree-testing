#ifndef GENERATION_H_
#define GENERATION_H_

#include <cstdint>
#include <cassert>
#include <cstdlib>
#include <cmath>

#include <iostream>
namespace OCTREE {

    enum eVoxelState : uint32_t {
        EMPTY_VOXEL = 0, // Leaf
        FULL_VOXEL,      // Leaf
        HALF_VOXEL,       // branch
        TBD_VOXEL
    };

    struct sMacroVoxel {
        eVoxelState state = EMPTY_VOXEL;

        uint16_t size = 0;

        sMacroVoxel *children[4] = {};
    };

    struct sTempMacroVoxel {

    };


    struct sRawVolume {
        uint8_t *raw_volume;
        uint16_t width = 0;
        uint16_t heigth = 0;
        uint16_t depth = 0;

        uint8_t density_threshold = 9;

        inline eVoxelState get_pixel_state(const uint16_t x,
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


    struct sVolumeOctree {
        sMacroVoxel *raw_data;

        uint16_t level_count = 0;
        uint32_t last_index = 0;

        inline bool is_full_at(const uint16_t level,
                               const uint16_t octant) {
            uint32_t level_padding = 0;
            uint32_t level_it = level;
            while(level_it > 0) {
                level_padding += powf(powf(2.0f, level_it+2.0f), 3.0);
                level_it--;
            }
             return false;
        }
    };

    inline void generate_octree_on_volume(const sRawVolume &volume) {
        assert("Ofr now this method only works for square, poer of 2 textures");
        uint32_t base_it = volume.width / 2;

        uint32_t macrovoxel_count = base_it * base_it * base_it;


        sMacroVoxel *macrovoxel_pool = (sMacroVoxel*) malloc(macrovoxel_count * sizeof(sMacroVoxel));

        uint32_t voxel_count = 0;
        // `https://coderwall.com/p/fzni3g/bidirectional-translation-between-1d-and-3d-arrays`
        for(uint32_t i = 0; i < macrovoxel_count; i++) {
            uint16_t x_n = i % base_it;
            uint16_t y_n = (i / base_it) % base_it;
            uint16_t z_n = i / (base_it * base_it);

            uint16_t old_x = (x_n / base_it) * volume.width;
            uint16_t old_y = (y_n / base_it) * volume.width;
            uint16_t old_z = (z_n / base_it) * volume.width;


           eVoxelState base_state = TBD_VOXEL;

           sMacroVoxel *children = (sMacroVoxel*) malloc(sizeof(sMacroVoxel) * 8);
           uint32_t vox_count = 0;
           for(uint16_t z = 0; z < 2; z++) {
               for(uint16_t y = 0; y < 2; y++) {
                   for(uint16_t x = 0; x < 2; x++) {
                       eVoxelState new_state = volume.get_pixel_state(old_x + x,
                                                                      old_y + y,
                                                                      old_z + z);
                       children[(x + y * 2 + z * 2 * 2)] = { .state = new_state };
                       if (base_state == TBD_VOXEL) {
                           base_state = new_state;
                       } else if (base_state != HALF_VOXEL) {
                           if (base_state != new_state) {
                               // Is mixed
                               base_state = HALF_VOXEL;
                           }
                       }

                       if (new_state == FULL_VOXEL) {
                           vox_count++;
                       }
                   }
               }
           }

           macrovoxel_pool[i] = { .state = base_state };

           if (base_state == HALF_VOXEL) {
               // Append to base states the children
               voxel_count += vox_count;
               macrovoxel_pool[i].children[0] = children;
           } else if (base_state == FULL_VOXEL){
               // Append the MacroVoxel as a non mix voxel
               voxel_count++;
           }
           //std::cout << voxel_count << std::endl;

        }

        std::cout << "Pool % first pass ======" << std::endl;
        std::cout << "From " << 256 * 256 * 256 << " to " <<(int) voxel_count << std::endl;
        std::cout << "Reduccion de " << 1.0f - (voxel_count / (256.0f*256.0f*256.0f)) << std::endl;

        base_it = base_it / 2;


        uint32_t old_base = base_it;
        sMacroVoxel *second_pool = (sMacroVoxel*) malloc(sizeof(sMacroVoxel) * macrovoxel_count);

        while(base_it > 4) {
            macrovoxel_count = base_it * base_it * base_it;

            for(uint32_t i = 0; i < macrovoxel_count; i++) {
                uint16_t x_n = i % base_it;
                uint16_t y_n = (i / base_it) % base_it;
                uint16_t z_n = i / (base_it * base_it);

                uint16_t old_x = (x_n / base_it) * old_base;
                uint16_t old_y = (y_n / base_it) * old_base;
                uint16_t old_z = (z_n / base_it) * old_base;


                eVoxelState base_state = TBD_VOXEL;

                uint32_t vox_count = 0;

                for(uint16_t z = 0; z < 2; z++) {
                    for(uint16_t y = 0; y < 2; y++) {
                        for(uint16_t x = 0; x < 2; x++) {
                            eVoxelState new_state; // Todo load state

                            // if new_state == half_full:
                            //    quit
                            // if new_state != base_state

                        }
                    }
                }

                // if hybrid
                //   push macro
                // if full or ampty:
                //   remove "children" from list
                //   push new macroblock
            }

            // Swap & clean buffers
        }
    }

    #define IS_POWER_OF_TWO(x) ((x & (x - 1)) == 0)

    inline sVolumeOctree generate_octree(const sRawVolume &volume) {
        assert(volume.width == volume.depth &&
               volume.width == volume.heigth &&
               IS_POWER_OF_TWO(volume.width) &&
               "Error while generating octree, non square and power of two text");
        // Generate size of the stack
        uint32_t num_blocks = 0;
        uint32_t it_blocks = volume.width;

        while(it_blocks > 1) {
            num_blocks += it_blocks * it_blocks * it_blocks;
            it_blocks = it_blocks / 2;
        }

        sMacroVoxel *octree_storage = (sMacroVoxel*) malloc(sizeof(sMacroVoxel) * num_blocks);

        // Load base macroblocks
        uint32_t base_size = volume.width;
        uint32_t base_size_squared = base_size * base_size;
        for(uint32_t z = 0; z < base_size; z++) {
            for(uint32_t y = 0; y < base_size; y++) {
                for(uint32_t x = 0; x < base_size; x++) {
                    uint32_t index = x + y * base_size + z * base_size_squared;
                    octree_storage[index].state = (volume.raw_volume[index] > volume.density_threshold) ? FULL_VOXEL : EMPTY_VOXEL;
                }
            }
        }


        // Start octree generation
        it_blocks = base_size;
        uint16_t level_count = 0;
        uint32_t prev_level_state = 0;
        uint32_t last_level_padding = base_size * base_size_squared;
        while(it_blocks > 1) {
            level_count++;
            uint32_t last_block_size = it_blocks;
            uint32_t last_block_size_squared = last_block_size * last_block_size;
            it_blocks = it_blocks / 2;

            for(uint32_t i = 0; i < it_blocks; i++) {
                uint16_t x_n = i % it_blocks;
                uint16_t y_n = (i / it_blocks) % it_blocks;
                uint16_t z_n = i / (it_blocks * it_blocks);

                // Calculate the corresponding indexes to the prev iteration
                uint16_t old_x = (x_n / it_blocks) * last_block_size;
                uint16_t old_y = (y_n / it_blocks) * last_block_size;
                uint16_t old_z = (z_n / it_blocks) * last_block_size;

                // Check the prev. iteration's octants
                eVoxelState macro_voxel_state = TBD_VOXEL;
                for(uint16_t z = 0; z < 2; z++) {
                    for(uint16_t y = 0; y < 2; y++) {
                        for(uint16_t x = 0; x < 2; x++) {
                            const uint32_t index = (x + old_x) + (y + old_y) * last_block_size + (z + old_z) * last_block_size_squared;
                            eVoxelState octant_state = octree_storage[prev_level_state + index].state;

                            if (octant_state == HALF_VOXEL) {
                                goto exit_octant_check;
                            }

                            if (macro_voxel_state == TBD_VOXEL) {
                                macro_voxel_state = octant_state;
                            } else if (macro_voxel_state != octant_state) {
                                macro_voxel_state = HALF_VOXEL;
                                goto exit_octant_check;
                            }
                        }
                    }
                }

                exit_octant_check:

                octree_storage[last_level_padding + i] = {.state = macro_voxel_state};
            }
        }

        return {.raw_data = octree_storage, .level_count = level_count, .last_index = num_blocks};
    }

};

#endif // GENERATION_H_
