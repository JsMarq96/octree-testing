#include <bits/types/clock_t.h>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <ctime>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>

#include "cube_renderer.h"
#include "generation.h"
#include "camera.h"
#include "glm/ext/matrix_transform.hpp"
#include "libmorton/morton.h"
#include "octree.h"


#define WIN_WIDTH	740
#define WIN_HEIGHT	680
#define WIN_NAME	"Octree"

glm::mat4 cubes_model[526 * 526];
uint32_t cubes_count = 0;
glm::vec3 position_LUT[8] = {};


void glfw_error_callback(int error_code, const char* decscr) {
    std::cout << "GLTF ERROR: " << error_code << std::endl;
}



// TODO: limit the number of levels
void iterate_quadtree(const OCTREE::sOctreeNode *octree,
                      const uint32_t curr_index,
                      const uint32_t curr_level,
                      const glm::vec3 vect) {
    std::cout << octree[curr_index].type << std::endl;
    if (octree[curr_index].type == OCTREE::FULL_VOXEL) {
       //std::cout << curr_level << " <- " << p++ << " " << glm::to_string(vect) << std::endl;
        //cubes_model[cubes_count] = glm::scale(glm::mat4(1.0f), glm::vec3{1.0f, 1.0f, 1.0f} / ((float) curr_level * 2.0f));
        cubes_model[cubes_count] = glm::translate(glm::mat4(1.0f), vect * 10.0f);
        cubes_model[cubes_count] = glm::scale(cubes_model[cubes_count], glm::vec3{1.0f, 1.0f, 1.0f} / (float) curr_level);
        cubes_count++;
        return;
    }

    if (octree[curr_index].type == OCTREE::HALF_VOXEL ) {
        // If its mixed, we continue
        for(uint32_t i = 0; i < 8; i++) {
            iterate_quadtree(octree,
                             octree[curr_index].children[i],
                             curr_level+1,
                             vect - (position_LUT[i] / (float) curr_level));
        }
    }
}

void render_frame(GLFWwindow *window) {
    glfwMakeContextCurrent(window);

    sCamera camera = {};
    sCubeRenderer renderer;
    renderer.init();


    camera.position = {10.0f, 10.5, 10.0f};
    camera.look_at({0.0f, 5.5f, 0.0f});

    glm::mat4 vp_mat = {};

    cubes_model[0] = glm::mat4(1.0f);

    for(uint32_t i = 0; i < 8; i++) {
        uint_fast16_t x,y,z;

        libmorton::morton3D_32_decode(i, x, y, z);
        position_LUT[i] = {x, y, z };

        //std::cout << i << ": " << x << ", " << y << ", " << z << std::endl;
    }

    //std::cout << glm::to_string(position_LUT[1] / 7.0f) << std::endl;

    FILE *file;

    file = fopen("resources/volumes/bonsai_256x256x256_uint8.raw", "rb");

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    OCTREE::sRawVolume vol = {  .raw_volume = (uint8_t*) malloc(file_size),
                                .width = 256,
                                .heigth = 256,
                                .depth = 256, .density_threshold = 50};

    fread(vol.raw_volume, file_size, 1, file);

    uint32_t p = 0;
    for(uint32_t i = 0; i < (256*256*256);i++) {
        if (vol.raw_volume[i] < 50) {
            p++;
        }
    }
    std::cout << "Raw values out thresh: " <<  p << std::endl;

    //return;
    clock_t end_time, start_time = clock();
    //OCTREE::sVolumeOctree octr = OCTREE::generate_octree(vol);
    //OCTREE::ssssss

    uint8_t test [] = { 0, 0, 60, 20, 0, 0, 60, 20};
    OCTREE::sRawVolume test_vol = { .raw_volume = test, .width = 2, .heigth = 2, .depth = 2, .density_threshold = 40 };

    OCTREE::sOctreeNode *octree = OCTREE::generate(test_vol);
    end_time = clock();

    std::cout << (end_time - start_time) / (CLOCKS_PER_SEC / 1000) << std::endl;

    iterate_quadtree(octree, 0, 1, {0.0f, 0.0f, 0.0f});

    //return;
    // Test bitpaclong
    //
    std::cout << "Num of cubes extracted from octree: " <<  cubes_count << std::endl;

    while(!glfwWindowShouldClose(window)) {
        int width, heigth;

        glfwPollEvents();
        glfwGetFramebufferSize(window, &width, &heigth);
        glViewport(0,0, width, heigth);

        camera.get_perspective_viewprojection_matrix(90.0f,
                                                     0.01f,
                                                     1000.0f,
                                                     (float) width / (float) heigth,
                                                     &vp_mat);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        uint32_t prev_level_padding = 0;
         /*for(uint16_t i = octr.level_count; i > 0; i--) {
            uint32_t voxel_per_level = powf(powf(2.0f, i+2.0f), 3.0);

            for (uint32_t voxel = 0; voxel < voxel_per_level; voxel++) {
                if (octr.raw_data[octr.last_index - voxel + prev_level_padding].state == OCTREE::FULL_VOXEL) {

                }
            }

            prev_level_padding += voxel_per_level;
        }*/

        renderer.render(cubes_model,
                        cubes_count,
                        vp_mat,
                        false);

        glfwSwapBuffers(window);
    }
}

int main(int argc, char *argv[]) {
    // Morton code test

    std::cout << libmorton::morton3D_32_encode(1, 1, 1) << " + " << libmorton::morton3D_64_encode(10, 20, 10) << " = " << libmorton::morton3D_32_encode(11, 21, 11) << std::endl;


    // Init GL3W and GLFW
    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    // GLFW config
    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_NAME, NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!window) {
        std::cout << "Error, could not create window" << std::endl;
    } else {
        if (!gl3wInit()) {
            render_frame(window);
            //test_loop(window);
        } else {
            std::cout << "Cannot init gl3w" << std::endl;
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
