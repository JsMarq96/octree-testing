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
glm::vec3 position_LUT[8] = {};


void glfw_error_callback(int error_code, const char* decscr) {
    std::cout << "GLTF ERROR: " << error_code << std::endl;
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


    //std::cout << glm::to_string(position_LUT[1] / 7.0f) << std::endl;

    /*FILE *file;

    file = fopen("resources/volumes/bonsai_256x256x256_uint8.raw", "rb");

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    OCTREE::sRawVolume vol = {  .raw_volume = (uint8_t*) malloc(file_size),
                                .width = 256,
                                .heigth = 256,
                                .depth = 256, .density_threshold = 50};

    fread(vol.raw_volume, file_size, 1, file);*/

    uint8_t values[] = { 0,0,0,23,   0,0,0,0,   0,0,0,0,   25,0,0,0  };

    sRawVolume test_volume = { .raw_volume = values, .width = 4, .heigth = 4, .depth = 4, .density_threshold = 19};
    octree_generation(test_volume);


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
