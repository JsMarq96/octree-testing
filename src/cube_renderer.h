#ifndef CUBE_RENDERER_H_
#define CUBE_RENDERER_H_

#include "gl3w.h"
#include "glcorearb.h"
#include "shader.h"
#include "raw_shaders.h"
#include "raw_meshes.h"

#include <cstdint>
#include <iostream>

struct sCubeRenderer {
    unsigned int  VAO = 0;
    unsigned int  VBO = 0;
    unsigned int  EBO = 0;

    uint16_t indices_count = 0;

    sShader  shader;


    void init() {
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindBuffer(GL_ARRAY_BUFFER,
                     VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(RawMesh::cube_geometry),
                     RawMesh::cube_geometry,
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(RawMesh::cube_indices),
                     RawMesh::cube_indices,
                     GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Load vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Vertex position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);

        // UV coords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (sizeof(float) * 3));

        // Vertex normal
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (sizeof(float) * 5));

        // Load vertex indexing
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        shader = sShader(borring_vertex_shader, borring_frag_shader);
    }

    void render(const glm::mat4 *models,
                const glm::vec4 *colors,
                const int count,
                const glm::mat4 &view_proj,
                const bool show_wireframe) const {
        glBindVertexArray(VAO);

        if (show_wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        shader.activate();

        for(int i = 0; i < count; i++) {
            shader.set_uniform_matrix4("u_model_mat", models[i]);
            shader.set_uniform_matrix4("u_view_proj", view_proj);
            shader.set_uniform_vector("u_color", colors[i]);

            glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, 0);
        }

        shader.deactivate();
        glBindVertexArray(0);
    }

    void clean() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};

#endif // CUBE_RENDERER_H_
