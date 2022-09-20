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
#include "octree_1.h"


#define WIN_WIDTH	740
#define WIN_HEIGHT	680
#define WIN_NAME	"Octree"

glm::mat4 cubes_model[526 * 526];
uint32_t cubes_count = 0;
glm::vec3 octant_position_LUT[8] = {};


void glfw_error_callback(int error_code, const char* decscr) {
    std::cout << "GLTF ERROR: " << error_code << std::endl;
}

void iterate_ocrtree(const sVoxel* octree,
                     const uint32_t curr_index,
                     const uint32_t curr_level,
                     const uint32_t octant,
                     const glm::vec3 pos) {
    if (octree[curr_index].type != MIXED_VOXEL) {
        if (octree[curr_index].type == FULL_VOXEL) {
            float curr_scale = 1.0f / (1.0f * curr_level);
            cubes_model[cubes_count] = glm::scale(glm::translate(glm::mat4(1.0f), 20.0f * pos), {curr_scale, curr_scale, curr_scale});
            cubes_count++;
        }
        return;
    }

    for(uint8_t i = 0; i < 8 ;i++) {
        iterate_ocrtree(octree,
                        octree[curr_index].childs[i],
                        curr_level + 1,
                        i,
                        pos - (1.0f / (curr_level+1)) * octant_position_LUT[i]);
    }
}


void render_frame(GLFWwindow *window) {
    glfwMakeContextCurrent(window);

    sCamera camera = {};
    sCubeRenderer renderer;
    renderer.init();


    camera.position = {35.0f, 5.5, 35.0f};
    camera.look_at({0.0f, 5.5f, 0.0f});

    glm::mat4 vp_mat = {};

    cubes_model[0] = glm::mat4(1.0f);


    //std::cout << glm::to_string(position_LUT[1] / 7.0f) << std::endl;

    FILE *file;

    file = fopen("resources/volumes/bonsai_256x256x256_uint8.raw", "rb");

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    sRawVolume vol = {  .raw_volume = (uint8_t*) malloc(file_size),
                                .width = 256,
                                .heigth = 256,
                                .depth = 256, .density_threshold = 50};

    fread(vol.raw_volume, file_size, 1, file);

    for(uint32_t z_delta = 0; z_delta < 2; z_delta++) {
        for(uint32_t y_delta = 0; y_delta < 2; y_delta++) {
            for(uint32_t x_delta = 0; x_delta < 2; x_delta++) {
                octant_position_LUT[x_delta + y_delta * 2 + z_delta * 4] = {x_delta, y_delta, z_delta};
            }
        }
    }

    uint8_t values[] = {
         23,0,23,23,   0, 0, 32,40,   0,0,0,0,   23,0,0,0 ,
         0,32,23,23,    0,0,32,45,   0,0,0,0,   0,0,0,0,
         0,0,32,0,    0,0,0,0,   0,0,23,23,   0,0,23,23,
         0,0,55,32,    0,0,0,0,   0,0,23,23,   0,0,23,23,
    };

    sRawVolume test_volume = { .raw_volume = values, .width = 4, .heigth = 4, .depth = 4, .density_threshold = 19};
    sVoxel* octree = octree_generation(vol);

    uint32_t test_child[4] = {};
    fill_children_of_voxel(2, 1, 0, 0, 0,test_child);
    for(uint32_t i = 0; i< 4; i++) {
        std::cout << test_child[i] << " <-" << std::endl;
    }
    iterate_ocrtree(octree, 0, 1, 0, {1,1,1});

    std::cout << "Num of cubes extracted from octree: " <<  cubes_count << std::endl;

    while(!glfwWindowShouldClose(window)) {
        int width, heigth;

        glfwPollEvents();
        glfwGetFramebufferSize(window, &width, &heigth);
        glViewport(0,0, width, heigth);

        camera.get_perspective_viewprojection_matrix(45.0f,
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
                        true);

        glfwSwapBuffers(window);
    }
}

int main(int argc, char *argv[]) {
    // Morton code test

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
