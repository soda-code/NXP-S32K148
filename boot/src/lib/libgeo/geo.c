/*
 * geo.c
 *
 *  Created on: 2025年11月24日
 *      Author: 18750
 */

#include "geo.h"
#include <math.h>
#include <float.h>
#include "AMMCLib1.h"

#define M_PI 3.14159265358979323846
#define M_PI_F			3.14159265f
#define M_PI_2_F		1.57079632f
#define M_PI_4_F		0.78539816f

/**
 * @brief 将角度转换为弧度
 * @param degrees 角度值
 * @return 弧度值
 */
static double radians(double degrees)
{
    return degrees * M_PI / 180.0;
}
static double degrees(double radians)
{
    return radians * 180 / M_PI;
}
static float wrapf(float x, float low, float high) {
    const float range = high - low;
    if (range <= 0.0f) return low;
    return x - range * floorf((x - low) / range);
}

static double wrapd(double x, double low, double high) {
    const double range = high - low;
    if (range <= 0.0) return low;
    return x - range * floor((x - low) / range);
}

static float wrap_pif(float x) {
    return wrapf(x, -(float)M_PI, (float)M_PI);
}

static float wrap_2pif(float x) {
    return wrapf(x, 0.0f, 2.0f * (float)M_PI);
}
static double wrap_2pid(double x) {
    return wrapd(x, 0.0, 2.0 * M_PI);
}

/**
 * @brief 计算到下一个航点的方位角（初始航向）
 * 
 * 该函数使用球面余弦定理计算从当前地理位置到下一个航点的初始方位角（弧度）。
 * 方位角是相对于北向的顺时针角度，范围归一化至 [-π, π]。
 * 
 * @param lat_now    当前纬度，单位：度
 * @param lon_now    当前经度，单位：度
 * @param lat_next   下一航点纬度，单位：度
 * @param lon_next   下一航点经度，单位：度
 * 
 * @return 方位角，单位：弧度，范围 [-π, π]，其中：
 *         - 0 弧度表示北方
 *         - π/2 弧度表示东方
 *         - π 或 -π 表示南方
 *         - -π/2 弧度表示西方
 * 
 * @note 所有输入坐标应以度为单位。该函数采用球形地球模型，
 *       适用于中小距离的计算。
 */
float get_bearing_to_next_waypoint(double lat_now, double lon_now, double lat_next, double lon_next)
{
    const double lat_now_rad = radians(lat_now);
    const double lat_next_rad = radians(lat_next);

    const double cos_lat_next = cos(lat_next_rad);
    const double d_lon = radians(lon_next - lon_now);

    const float y = (float)(sin(d_lon) * cos_lat_next);
    const float x = (float)(cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));

    return wrap_pif(atan2f(y, x));
}
/**
 * @brief 计算两个地理航点之间的大圆距离
 * 
 * 使用Haversine公式计算地球表面上两点之间的最短距离，
 * 这两点由它们的纬度和经度坐标指定。
 * 
 * @param lat_now    当前纬度，单位：十进制度
 * @param lon_now    当前经度，单位：十进制度
 * @param lat_next   下一航点纬度，单位：十进制度
 * @param lon_next   下一航点经度，单位：十进制度
 * 
 * @return 到下一航点的距离，单位：米（或CONSTANTS_RADIUS_OF_EARTH使用的单位）
 * 
 * @note 假设radians()宏/函数将度转换为弧度
 * @note CONSTANTS_RADIUS_OF_EARTH应定义为地球平均半径（约6371000米）
 * @note 输入坐标应为有效的纬度[-90, 90]和经度[-180, 180]值
 */
float get_distance_to_next_waypoint(double lat_now, double lon_now, double lat_next, double lon_next)
{
    const double lat_now_rad = radians(lat_now);
    const double lat_next_rad = radians(lat_next);

    const double d_lat = lat_next_rad - lat_now_rad;
    const double d_lon = radians(lon_next) - radians(lon_now);

    const double a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(lat_next_rad);
    const double c = atan2(sqrt(a), sqrt(1.0 - a));
    return CONSTANTS_RADIUS_OF_EARTH * 2.0 * c;
}


/**
 * @brief 根据起始点、方位角和距离计算目标航点坐标
 * 
 * 该函数基于起始地理位置、方位角和行进距离，计算目标航点的纬度和经度。
 * 计算使用球面三角学来考虑地球的曲率。
 * 
 * @param lat_start 起始纬度，单位：度
 * @param lon_start 起始经度，单位：度
 * @param bearing 方位角，单位：弧度（将被归一化到 [0, 2π]）
 * @param dist 行进距离，单位：米
 * @param lat_target 指向存储计算出的目标纬度的指针，单位：度
 * @param lon_target 指向存储计算出的目标经度的指针，单位：度
 * 
 * @note 方位角使用 wrap_2pif() 归一化，确保其在 [0, 2π] 范围内。
 * @note CONSTANTS_RADIUS_OF_EARTH 应定义为地球平均半径（米）。
 * @note 坐标使用 WGS84 基准（纬度/经度以度为单位）。
 * 
 * @see wrap_2pif(), radians(), degrees()
 */
void waypoint_from_heading_and_distance(double lat_start, double lon_start, float bearing, float dist,
                    double *lat_target, double *lon_target)
{
    bearing = wrap_2pif(bearing);

    double radius_ratio = (double)dist / CONSTANTS_RADIUS_OF_EARTH;

    double lat_start_rad = radians(lat_start);
    double lon_start_rad = radians(lon_start);

    *lat_target = asin(sin(lat_start_rad) * cos(radius_ratio) + cos(lat_start_rad) * sin(radius_ratio) * cos((double)bearing));
    *lon_target = lon_start_rad + atan2(sin((double)bearing) * sin(radius_ratio) * cos(lat_start_rad),
        cos(radius_ratio) - sin(lat_start_rad) * sin(*lat_target));
    
    *lat_target = degrees(*lat_target);
    *lon_target = degrees(*lon_target);
}

/**
 * @brief 从起始点沿着指向第二个点的方向创建指定距离的航点
 * 
 * @details 该函数通过以下步骤计算目标航点：
 *          1. 计算从点A到点B的方位角
 *          2. 沿该方位角从点A出发指定距离处创建新航点
 *          如果距离实际上为零（小于FLT_EPSILON），目标点设置为点A
 * 
 * @param[in] lat_A     起始点A的纬度，单位：度
 * @param[in] lon_A     起始点A的经度，单位：度
 * @param[in] lat_B     参考点B的纬度，单位：度
 * @param[in] lon_B     参考点B的经度，单位：度
 * @param[in] dist      从点A沿指向点B方向行进的距离，单位：米
 * @param[out] lat_target 指向存储计算的目标航点纬度的指针
 * @param[out] lon_target 指向存储计算的目标航点经度的指针
 * 
 * @return 无返回值
 * 
 * @note 该函数假设输入参数有效。确保指针不为NULL。
 * @note 距离可以是正值或负值；负值将在相反方向创建航点。
 */
void create_waypoint_from_line_and_dist(double lat_A, double lon_A, double lat_B, double lon_B, float dist,
                    double *lat_target, double *lon_target)
{
    if(fabsf(dist) < FLT_EPSILON)
    {
        *lat_target = lat_A;
        *lon_target = lon_A;
    }
    else
    {
        float heading = get_bearing_to_next_waypoint(lat_A, lon_A, lat_B, lon_B);
        waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist, lat_target, lon_target);
    }
}

/**
 * @brief 使用Haversine公式计算两个地理点之间的距离
 * 
 * 计算以WGS84坐标（纬度/经度/高度）指定的两点之间的3D距离。
 * 该函数返回总3D距离，并提供水平和竖直分量。
 * 
 * @param lat_now 当前纬度，单位：度
 * @param lon_now 当前经度，单位：度
 * @param alt_now 当前高度，单位：米
 * @param lat_next 目标纬度，单位：度
 * @param lon_next 目标经度，单位：度
 * @param alt_next 目标高度，单位：米
 * @param dist_xy 指向存储水平距离的指针，单位：米
 * @param dist_z 指向存储竖直距离的指针，单位：米
 * 
 * @return float 两点之间的总3D距离，单位：米
 * 
 * @note 使用Haversine公式计算地球表面的大圆距离
 * @note 所有返回的距离值为绝对值（正数）
 * @note 需要定义CONSTANTS_RADIUS_OF_EARTH（通常约为6371000米）
 */
float get_distance_to_point_global_wgs84(double lat_now, double lon_now, float alt_now,
        double lat_next, double lon_next, float alt_next,
        float *dist_xy, float *dist_z)
{
    double lat_now_rad = radians(lat_now);
    double lon_now_rad = radians(lon_now);
    double lat_next_rad = radians(lat_next);
    double lon_next_rad = radians(lon_next);

    double d_lat = lat_next_rad - lat_now_rad;
    double d_lon = lon_next_rad - lon_now_rad;

    double a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(lat_next_rad);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    const float dxy = (float)(CONSTANTS_RADIUS_OF_EARTH * c);
    const float dz = (float)(alt_next - alt_now);

    *dist_xy = fabsf(dxy);
    *dist_z = fabsf(dz);

    return sqrtf(dxy * dxy + dz * dz);
}

/**
 * @brief 计算当前位置到线段的横向误差（cross-track error）。
 *
 * 计算从当前位置 (lat_now, lon_now) 到由 (lat_start, lon_start) 与 (lat_end, lon_end)
 * 定义的大圆航线段的有符号最短距离（横向误差）以及指向该航线的制导航向。
 * 同时检测当前位置是否已越过航段终点。
 *
 * @note crosstrack_error 结构体字段：
 *  - crosstrack_error->distance : 有符号横向距离（与 get_distance_to_next_waypoint 返回相同的线性单位）。
 *      符号表示相对于由起点到终点方向的航线在左/右侧（正值表示在航线左侧）。
 *  - crosstrack_error->bearing  : 指向航线的航向（弧度），已归一化到 [-pi, pi]。
 *      值基于航线航向旋转 ±pi/2，取决于当前位置在哪一侧。
 *  - crosstrack_error->past_end : 当当前位置沿航线方向越过终点时置为 true
 *      （即航线航向与指向终点的航向之间的角度差超过 ±pi/2）。
 *
 * @note
 *  - 航向以弧度表示，使用 wrap_pif/wrap_pi 等函数归一化。
 *  - 距离使用与 get_distance_to_next_waypoint 相同的单位（通常为米）。
 *  - 函数在内部使用浮点运算（中间计算为 float 精度）。
 *
 * @param[out] crosstrack_error : 指向要填充的 struct crosstrack_error_s 的指针。
 * @param[in] lat_now, lon_now : 当前点纬度/经度（度）。
 * @param[in] lat_start, lon_start : 航段起点纬度/经度（度）。
 * @param[in] lat_end, lon_end : 航段终点纬度/经度（度）。
 *
 * @return
 *  -  0 成功（crosstrack_error 已填充）。
 *  - -1 错误（参数无效或当前位置与终点距离过小）。此时 crosstrack_error 保持默认/重置状态
 *       （past_end = false, distance = 0.0, bearing = 0.0）。
 *
 * @note
 *  - 若当前位置到终点距离非常小（dist_to_end < 0.1，与 get_distance_to_next_waypoint 单位一致），
 *    为避免不可靠的横向误差计算，函数返回 -1。
 */
int get_distance_to_line(struct crosstrack_error_s *crosstrack_error, double lat_now, double lon_now,
             double lat_start, double lon_start, double lat_end, double lon_end)
{
    int return_value = -1;	// 设为错误标志，成功计算后置为 0
    crosstrack_error->past_end = false;
    crosstrack_error->distance = 0.0f;
    crosstrack_error->bearing = 0.0f;

    float dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

    // 参数校验：若到终点距离太小则返回错误
    if (dist_to_end < 0.1f) {
        return -1;
    }

    float bearing_end = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
    float bearing_track = get_bearing_to_next_waypoint(lat_start, lon_start, lat_end, lon_end);
    float bearing_diff = wrap_pif(bearing_track - bearing_end);

    // 若已越过终点则标记 past_end 并返回 0
    if (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
        crosstrack_error->past_end = true;
        return_value = 0;
        return return_value;
    }

    // 横向距离：使用到终点距离乘以角差的正弦（带符号）
    crosstrack_error->distance = (dist_to_end) * sinf(bearing_diff);

    // 根据横向符号选择指向航线的航向（航线航向 ± π/2）
    if (sinf(bearing_diff) >= 0) {
        crosstrack_error->bearing = wrap_pi(bearing_track - M_PI_2_F);

    } else {
        crosstrack_error->bearing = wrap_pi(bearing_track + M_PI_2_F);
    }

    return_value = 0;

    return return_value;
}

/**
 * @brief 计算当前位置到指定圆弧（arc）的最近距离和方位，并填充横向误差结构体
 *
 * 该函数在给定圆心和半径以及圆弧起始方位与扫过角的情况下，
 * 计算当前位置到圆弧的最短距离与相应的方位，并通过 crosstrack_error_s 结构返回结果。
 * 函数同时标记是否到达圆弧的终点（past_end）。
 *
 * @param[out] crosstrack_error 指向用于返回结果的结构体指针。函数会设置以下字段：
 *                              - crosstrack_error->distance : 最近点到当前位置的距离（单位：米，非负）。
 *                              - crosstrack_error->bearing  : 从当前位置指向最近点的航向（单位：弧度），
 *                                                            最终用 wrap_pif 包裹到典型区间（例如 -π 到 π）。
 *                              - crosstrack_error->past_end : 布尔值，若最近点为圆弧终点则为 true，否则为 false。
 * @param[in] lat_now     当前纬度（与项目中 get_distance_to_next_waypoint()/get_bearing_to_next_waypoint() 使用的纬度单位一致）。
 * @param[in] lon_now     当前经度（与项目中 get_distance_to_next_waypoint()/get_bearing_to_next_waypoint() 使用的经度单位一致）。
 * @param[in] lat_center  圆心纬度（与上述经纬度单位一致）。
 * @param[in] lon_center  圆心经度（与上述经纬度单位一致）。
 * @param[in] radius      圆弧半径（单位：米）。若 radius < 0.1，则函数视为无效并返回错误。
 * @param[in] arc_start_bearing 圆弧起始方位（单位：弧度）。与 arc_sweep 一起定义圆弧的区间（按模 2π 处理）。
 * @param[in] arc_sweep   圆弧扫过角（单位：弧度），可为正或负：
 *                        - 当 arc_sweep >= 0 时，圆弧区间为 [arc_start_bearing, arc_start_bearing + arc_sweep]（模 2π）。
 *                        - 当 arc_sweep < 0  时，圆弧区间为 [arc_start_bearing + arc_sweep, arc_start_bearing]（模 2π）。
 *
 * @return 返回值说明：
 *         - 0  表示成功计算并已填充 crosstrack_error。
 *         - -1 表示输入无效（当前仅在 radius < 0.1 m 时返回错误）；在出错时 crosstrack_error 不被保证为有效值。
 * @see get_distance_to_next_waypoint, get_bearing_to_next_waypoint, wrap_pif
 */
int get_distance_to_arc(struct crosstrack_error_s *crosstrack_error, double lat_now, double lon_now,
			double lat_center, double lon_center,
			float radius, float arc_start_bearing, float arc_sweep)
{
	float bearing_sector_start = 0.0f;
	float bearing_sector_end = 0.0f;
	float bearing_now = get_bearing_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);
	
    int return_value = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_end = false;
	crosstrack_error->distance = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	if (radius < 0.1f)
	{
		return return_value;
	}

	if (arc_sweep >= 0.0f)
	{
		bearing_sector_start = arc_start_bearing;
		bearing_sector_end = arc_start_bearing + arc_sweep;

		if (bearing_sector_end > 2.0f * M_PI_F)
		{
			bearing_sector_end -= (2 * M_PI_F);
		}

	} else {
		bearing_sector_end = arc_start_bearing;
		bearing_sector_start = arc_start_bearing - arc_sweep;

		if (bearing_sector_start < 0.0f)
		{
			bearing_sector_start += (2 * M_PI_F);
		}
	}

	bool in_sector = false;

    // Case where sector does not span zero
	if (bearing_sector_end >= bearing_sector_start && bearing_now >= bearing_sector_start
	    && bearing_now <= bearing_sector_end) {

		in_sector = true;
	}

	// Case where sector does span zero
	if (bearing_sector_end < bearing_sector_start && (bearing_now > bearing_sector_start
			|| bearing_now < bearing_sector_end)) {

		in_sector = true;
	}
	// If in the sector then calculate distance and bearing to closest point
	if (in_sector) {
		crosstrack_error->past_end = false;
		float dist_to_center = get_distance_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

		if (dist_to_center <= radius) {
			crosstrack_error->distance = radius - dist_to_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} else {
			crosstrack_error->distance = dist_to_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} else {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		double start_disp_x = (double)radius * sin((double)arc_start_bearing);
		double start_disp_y = (double)radius * cos((double)arc_start_bearing);
		double end_disp_x = (double)radius * sin((double)wrap_2pid(arc_start_bearing + arc_sweep));
		double end_disp_y = (double)radius * cos((double)wrap_2pid(arc_start_bearing + arc_sweep));
		double lon_start = lon_now + start_disp_x / 111111.0;
		double lat_start = lat_now + start_disp_y * cos(lat_now) / 111111.0;
		double lon_end = lon_now + end_disp_x / 111111.0;
		double lat_end = lat_now + end_disp_y * cos(lat_now) / 111111.0;
		float dist_to_start = get_distance_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);
		float dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

		if (dist_to_start < dist_to_end) {
			crosstrack_error->distance = dist_to_start;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);

		} else {
			crosstrack_error->past_end = true;
			crosstrack_error->distance = dist_to_end;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
		}
	}

	crosstrack_error->bearing = wrap_pif(crosstrack_error->bearing);
	return_value = 0;

	return return_value;
}

/**
 * @brief 计算当前位置到下一个航点的向量在局部东北坐标系（North, East）上的分量（单位：米）。
 *
 * @details
 * 给定当前点和目标航点的纬度/经度，函数基于球面地球模型（使用 CONSTANTS_RADIUS_OF_EARTH）
 * 计算从当前点指向目标航点的向量在北向和东向的分量。输入角度以十进制度（degrees）给出，
 * 函数会内部将其转换为弧度并使用三角函数进行计算。实现中为兼顾性能与精度在部分计算中混合
 * 使用了 double 与 float 类型的三角运算与转换。
 *
 * @param[in]  lat_now   当前纬度（degrees）。
 * @param[in]  lon_now   当前经度（degrees）。
 * @param[in]  lat_next  目标航点纬度（degrees）。
 * @param[in]  lon_next  目标航点经度（degrees）。
 * @param[out] v_n       指向 float 的指针，用于返回北向分量（单位：米）。调用者需保证非 NULL。
 * @param[out] v_e       指向 float 的指针，用于返回东向分量（单位：米）。调用者需保证非 NULL。
 *
 * @note
 * - 结果基于简化的球面近似（不考虑高度差或地球椭球体修正），适用于短至中距离导航向量计算。
 * - 输入角度必须为度值；函数内部使用 radians() 进行转换。
 * - 函数不对 v_n/v_e 进行空指针检查，调用者需负责传入有效指针。
 * - 该函数无全局可变状态（假定 CONSTANTS_RADIUS_OF_EARTH 为常量），因此在多线程环境下为可重入的。
 *
 * @see radians()
 */
void get_vector_to_next_waypoint(double lat_now, double lon_now, double lat_next, double lon_next, float *v_n, float *v_e)
{
    const double lat_now_rad = radians(lat_now);
	const double lat_next_rad = radians(lat_next);
	const double d_lon = radians(lon_next) - radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = (float)(CONSTANTS_RADIUS_OF_EARTH * (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(
					  lat_next_rad) * cos(d_lon)));
	*v_e = (float)(CONSTANTS_RADIUS_OF_EARTH * sin(d_lon) * cos(lat_next_rad));

}

/**
 * @brief 计算从当前位置到下一个航路点的向量（快速版本）
 * 
 * @details 该函数使用快速近似方法计算从当前地理位置到下一个航路点的北向和东向速度向量。
 *          混合使用双精度和单精度三角函数以最大化速度和效率。
 * 
 * @param[in] lat_now 当前位置纬度（度）
 * @param[in] lon_now 当前位置经度（度）
 * @param[in] lat_next 下一个航路点纬度（度）
 * @param[in] lon_next 下一个航路点经度（度）
 * @param[out] v_n 指向北向向量分量的指针（米）
 * @param[out] v_e 指向东向向量分量的指针（米）
 * 
 * @return void
 * 
 * @note 此函数使用简化的地球模型，假设地球半径恒定，适用于近距离航路点计算
 * @note 输出向量分量单位为米，基于地球半径常数计算
 * 
 * @see CONSTANTS_RADIUS_OF_EARTH
 * @see radians()
 */
void get_vector_to_next_waypoint_fast(double lat_now, double lon_now, double lat_next, double lon_next, float *v_n,
				 float *v_e)
{
	double lat_now_rad = radians(lat_now);
	double lon_now_rad = radians(lon_now);
	double lat_next_rad = radians(lat_next);
	double lon_next_rad = radians(lon_next);

	double d_lat = lat_next_rad - lat_now_rad;
	double d_lon = lon_next_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = (float)(CONSTANTS_RADIUS_OF_EARTH * d_lat);
	*v_e = (float)(CONSTANTS_RADIUS_OF_EARTH * d_lon * cos(lat_now_rad));
}

/**
 * @brief 将速度矢量添加到全球位置坐标
 * 
 * @details 该函数根据北向和东向速度分量，计算相对于当前位置的新的地理坐标。
 *          使用地球半径作为转换因子，将笛卡尔坐标系中的速度增量转换为地理坐标增量。
 * 
 * @param[in] lat_now 当前纬度，单位：度（°）
 * @param[in] lon_now 当前经度，单位：度（°）
 * @param[in] v_n 北向速度分量，单位：米/秒（m/s）
 * @param[in] v_e 东向速度分量，单位：米/秒（m/s）
 * @param[out] lat_res 计算得到的结果纬度指针，单位：度（°）
 * @param[out] lon_res 计算得到的结果经度指针，单位：度（°）
 * 
 * @return void
 * 
 * @note 该函数假设地球为球体，使用常数 CONSTANTS_RADIUS_OF_EARTH 表示地球半径。
 *       函数内部使用弧度制进行计算，输入输出均为十进制度数。
 * 
 * @see CONSTANTS_RADIUS_OF_EARTH
 */
void add_vector_to_global_position(double lat_now, double lon_now, float v_n, float v_e, double *lat_res,
				   double *lon_res)
{
	double lat_now_rad = radians(lat_now);
	double lon_now_rad = radians(lon_now);

	*lat_res = degrees(lat_now_rad + (double)v_n / CONSTANTS_RADIUS_OF_EARTH);
	*lon_res = degrees(lon_now_rad + (double)v_e / (CONSTANTS_RADIUS_OF_EARTH * cos(lat_now_rad)));
}