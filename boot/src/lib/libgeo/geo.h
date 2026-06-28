/**
 * @file geo.h
 */

#ifndef LIB_LIBGEO_GEO_H_
#define LIB_LIBGEO_GEO_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief 重力加速度,单位m/s^2
 */
static float CONSTANTS_ONE_G = 9.80665f;
/**
 * @brief 地球半径,双精度,单位m
 */
static double CONSTANTS_RADIUS_OF_EARTH = 6371000;
/**
 * @brief 地球半径,单精度
 */
static float CONSTANTS_RADIUS_OF_EARTH_F = 6371000;
/**
 * @brief 地球自转速率,单位rad/s
 */
static float CONSTANTS_EARTH_SPIN_RATE = 7.2921150e-5f;

/**
 * @struct crosstrack_error_s
 * @brief 相对于线段或弧段的横向误差（crosstrack）信息。
 *
 * 包含评估横向偏差以及从参考位置（例如车辆位置）到线段或弧段最近点的方向所需的信息。
 *
 * @var crosstrack_error_s::past_end
 * 如果最近点位于线段/弧段的终点之外，则为真。
 *
 * @var crosstrack_error_s::distance
 * 与线段或弧段最近点的距离，单位为米。
 *
 * @var crosstrack_error_s::bearing
 * 从参考位置到最近点的方位角，单位为弧度，按导航系统的角度约定表示。
 */
struct crosstrack_error_s {
    bool past_end;   // 如果最近点超出线段/弧段末端则为真
    float distance;  // 与线/弧最近点的距离，单位：米
    float bearing;   // 指向最近点的方位角，单位：弧度
};

float get_bearing_to_next_waypoint(double lat_now, double lon_now, double lat_next, double lon_next);
float get_distance_to_next_waypoint(double lat_now, double lon_now, double lat_next, double lon_next);
void waypoint_from_heading_and_distance(double lat_start, double lon_start, float bearing, float dist, double *lat_target, double *lon_target);
void create_waypoint_from_line_and_dist(double lat_A, double lon_A, double lat_B, double lon_B, float dist, double *lat_target, double *lon_target);
float get_distance_to_point_global_wgs84(double lat_now, double lon_now, float alt_now, double lat_next, double lon_next, float alt_next,float *dist_xy, float *dist_z);
int get_distance_to_line(struct crosstrack_error_s *crosstrack_error, double lat_now, double lon_now, double lat_start, double lon_start, double lat_end, double lon_end);
int get_distance_to_arc(struct crosstrack_error_s *crosstrack_error, double lat_now, double lon_now,double lat_center, double lon_center,float radius, float arc_start_bearing, float arc_sweep);
void get_vector_to_next_waypoint(double lat_now, double lon_now, double lat_next, double lon_next, float *v_n, float *v_e);
void get_vector_to_next_waypoint_fast(double lat_now, double lon_now, double lat_next, double lon_next, float *v_n, float *v_e);
void add_vector_to_global_position(double lat_now, double lon_now, float v_n, float v_e, double *lat_res, double *lon_res);

#endif /* LIB_LIBGEO_GEO_H_ */
