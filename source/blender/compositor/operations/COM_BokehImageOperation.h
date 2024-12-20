/* SPDX-FileCopyrightText: 2011 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include "BLI_math_vector_types.hh"

#include "COM_MultiThreadedOperation.h"

namespace blender::compositor {

class BokehImageOperation : public MultiThreadedOperation {
 private:
  const NodeBokehImage *data_;

  int resolution_ = COM_BLUR_BOKEH_PIXELS;

  float exterior_angle_;
  float rotation_;
  float roundness_;
  float catadioptric_;
  float lens_shift_;

  /* See the delete_data_on_finish method. */
  bool delete_data_;

  /**
   * Get the 2D vertex position of the vertex with the given index in the regular polygon
   * representing this bokeh. The polygon is rotated by the rotation amount and have a unit
   * circumradius. The regular polygon is one whose vertices' exterior angles are given by
   * exterior_angle. See the bokeh function for more information.
   */
  float2 get_regular_polygon_vertex_position(int vertex_index);
  /**
   * Find the closest point to the given point on the given line. This assumes the length of the
   * given line is not zero.
   */
  float2 closest_point_on_line(float2 point, float2 line_start, float2 line_end);
  /**
   * Compute the value of the bokeh at the given point. The computed bokeh is essentially a regular
   * polygon centered in space having the given circumradius. The regular polygon is one whose
   * vertices' exterior angles are given by "exterior_angle", which relates to the number of
   * vertices n through the equation "exterior angle = 2 pi / n". The regular polygon may
   * additionally morph into a shape with the given properties:
   *
   * - The regular polygon may have a circular hole in its center whose radius is controlled by the
   *   "catadioptric" value.
   * - The regular polygon is rotated by the "rotation" value.
   * - The regular polygon can morph into a circle controlled by the "roundness" value,
   *   such that it becomes a full circle at unit roundness.
   *
   * The function returns 0 when the point lies inside the regular polygon and 1 otherwise.
   * However, at the edges, it returns a narrow band gradient as a form of anti-aliasing.
   */
  float bokeh(float2 point, float circumradius);

 public:
  BokehImageOperation();

  void init_execution() override;
  void deinit_execution() override;

  void determine_canvas(const rcti &preferred_area, rcti &r_area) override;

  void set_data(const NodeBokehImage *data)
  {
    data_ = data;
  }

  void set_resolution(int resolution)
  {
    resolution_ = resolution;
  }

  /**
   * \brief delete_data_on_finish
   *
   * There are cases that the compositor uses this operation on its own (see defocus node)
   * the delete_data_on_finish must only be called when the data has been created by the
   *compositor. It should not be called when the data has been created by the node-editor/user.
   */
  void delete_data_on_finish()
  {
    delete_data_ = true;
  }

  void update_memory_buffer_partial(MemoryBuffer *output,
                                    const rcti &area,
                                    Span<MemoryBuffer *> inputs) override;
};

}  // namespace blender::compositor
