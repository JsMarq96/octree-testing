#ifndef CAMERA_H_
#define CAMERA_H_

#include <cstring>
#include <cmath>
#include <cassert>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

/**
 * Camera functions
 * TODO: Implement zoom as a view matrix
 * */

#define to_radians(FOV) (FOV * 0.0174533f)

struct sCamera {
    glm::vec3     position {0.0f, 0.0f, 0.0f};
    float        zoom      = 0.059f;

    // TODO: initialize this and the view matrix code
    glm::vec3    u = {};
    glm::vec3    s = {};
    glm::vec3    f = {};

    glm::mat4 view_mat = {};

    // View Port size
    int        vp_height  = 0.0f;
    int        vp_width   = 0.0f;


    // Orthografic projection is the only one by default
    void
    get_ortho_projection_matrix(glm::mat4     &result) const {
        assert(false && "No ortho projection fucntion defined yet");
    };

    void
    get_perspective_projection_matrix(const float FOV,
                                      const float far_plane,
                                      const float near_plane,
                                      const float aspect_ratio,
                                      glm::mat4  *result) const {
      float tan_half_FOV = tan(to_radians(FOV) / 2.0f);

      *result = glm::perspective(FOV,
                                 aspect_ratio,
                                 near_plane,
                                 far_plane);
    }

    void
    look_at(const glm::vec3 &center) {
      f = glm::normalize(glm::vec3{center.x - position.x, center.y - position.y, center.z - position.z});
      s = glm::normalize(glm::cross(f, glm::vec3{0.f, 1.0f, 0.0f}));
      u = glm::cross(s, f);

      view_mat = glm::lookAt(position,
                             center,
                             u);
    }

  void compute_view_matrix() {
    /*s = cross_prod(f, sVector3{0.f, 1.0f, 0.0f}).normalize();
      u = cross_prod(s, f);
      view_mat.set_identity();
      view_mat.mat_values[0][0] = s.x;
      view_mat.mat_values[1][0] = s.y;
      view_mat.mat_values[2][0] = s.z;
      view_mat.mat_values[0][1] = u.x;
      view_mat.mat_values[1][1] = u.y;
      view_mat.mat_values[2][1] = u.z;
      view_mat.mat_values[0][2] = -f.x;
      view_mat.mat_values[1][2] = -f.y;
      view_mat.mat_values[2][2] = -f.z;
      view_mat.mat_values[3][0] = -dot_prod(s, position);
      view_mat.mat_values[3][1] = -dot_prod(u, position);
      view_mat.mat_values[3][2] = dot_prod(f, position);*/
  }

    void get_perspective_viewprojection_matrix(const float FOV,
                                               const float far_plane,
                                               const float near_plane,
                                               const float aspect_ratio,
                                               glm::mat4  *result) const {
        glm::mat4 persp = {};
        get_perspective_projection_matrix(FOV, far_plane, near_plane, aspect_ratio, &persp);

        *result = persp * view_mat;
    }

  void set_rotation(const float pitch,
                    const float yaw) {
    /*sVector3 dir = {};
    dir.x = cos(radians(yaw)) * cos(radians(pitch));
    dir.y = radians(pitch);
    dir.z = sin(radians(yaw)) * cos(radians(pitch));
    f = dir.normalize();*/
  }
};

#endif // CAMERA_H_
