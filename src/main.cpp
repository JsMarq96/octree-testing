#include <bits/types/clock_t.h>
#include <cstdio>
#include <iostream>
#include <ctime>

#include <GL/gl3w.h>
#include "gl3w.h"
#include <GLFW/glfw3.h>

#include "cube_renderer.h"
#include "generation.h"

#define WIN_WIDTH	740
#define WIN_HEIGHT	680
#define WIN_NAME	"Octree"


void glfw_error_callback(int error_code, const char* decscr) {
    std::cout << "GLTF ERROR: " << error_code << std::endl;
}

void render_frame(GLFWwindow *window) {
    glfwMakeContextCurrent(window);

    sCubeRenderer renderer;
    renderer.init();

    while(!glfwWindowShouldClose(window)) {
        int width, heigth;
        double temp_mouse_x, temp_mouse_y;

        glfwPollEvents();
        glfwGetFramebufferSize(window, &width, &heigth);
        glViewport(0,0, width, heigth);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        glfwSwapBuffers(window);
    }
}

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

    clock_t end_time, start_time = clock();
    OCTREE::generate_octree(vol);
    end_time = clock();

    std::cout << (end_time - start_time) / (CLOCKS_PER_SEC / 1000) << std::endl;

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
