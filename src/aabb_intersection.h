#ifndef AABB_INTERSECTION_H_
#define AABB_INTERSECTION_H_

#include "glm/common.hpp"
#include <glm/glm.hpp>

// Based on https://tavianator.com/cgit/dimension.git/tree/libdimension/bvh/bvh.c#n196
// And https://tavianator.com/2011/ray_box.html


// Intersection using the SLAB algortihm
inline void ray_AABB_intersection(const glm::vec3 &ray_origin,
                                  const glm::vec3 &ray_dir,
                                  const glm::vec3 &box_origin,
                                  const glm::vec3 &box_size,
                                  glm::vec3 *near_intersection,
                                  glm::vec3 *far_intersection) {
    const glm::vec3 box_min = box_origin;
    const glm::vec3 box_max = box_origin + box_size;

    // Testing X axis slab
    float tx1 = (box_min.x - ray_origin.x) / ray_dir.x;
    float tx2 = (box_max.x - ray_origin.x) / ray_dir.x;

    float tmin = glm::min(tx1, tx2);
    float tmax = glm::max(tx1, tx2);

    // Testing Y axis slab
    float ty1 = (box_min.y - ray_origin.y) / ray_dir.y;
    float ty2 = (box_max.y - ray_origin.y) / ray_dir.y;

    tmin = glm::max(glm::min(ty1, ty2), tmin);
    tmax = glm::min(glm::max(ty1, ty2), tmax);

    // Testing Z axis slab
    float tz1 = (box_min.z - ray_origin.z) / ray_dir.z;
    float tz2 = (box_max.z - ray_origin.z) / ray_dir.z;

    tmin = glm::max(glm::min(tz1, tz2), tmin);
    tmax = glm::min(glm::max(tz1, tz2), tmax);

    *near_intersection = ray_dir * tmin + ray_origin;
    *far_intersection = ray_dir * tmax + ray_origin;
}


/**
 *
 *double tx1 = (box.min.X - optray.x0.X)*optray.n_inv.X;
  double tx2 = (box.max.X - optray.x0.X)*optray.n_inv.X;

  double tmin = dmnsn_min(tx1, tx2);
  double tmax = dmnsn_max(tx1, tx2);

  double ty1 = (box.min.Y - optray.x0.Y)*optray.n_inv.Y;
  double ty2 = (box.max.Y - optray.x0.Y)*optray.n_inv.Y;

  tmin = dmnsn_max(tmin, dmnsn_min(ty1, ty2));
  tmax = dmnsn_min(tmax, dmnsn_max(ty1, ty2));

  double tz1 = (box.min.Z - optray.x0.Z)*optray.n_inv.Z;
  double tz2 = (box.max.Z - optray.x0.Z)*optray.n_inv.Z;

  tmin = dmnsn_max(tmin, dmnsn_min(tz1, tz2));
  tmax = dmnsn_min(tmax, dmnsn_max(tz1, tz2));

  return tmax >= dmnsn_max(0.0, tmin) && tmin < t;
} */

#endif // AABB_INTERSECTION_H_
