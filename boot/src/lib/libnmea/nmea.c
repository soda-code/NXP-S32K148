/*
 * nmea.c
 *
 *  Created on: 2025��11��19��
 *      Author: 18750
 */

#include "nmea.h"

static GPS gps_all;
static char gps_src_backup[1024];

/**
 * @brief 将度分格式(DM)的纬度转换为十进制格式
 *
 * @param lat_dm 度分格式的纬度值，格式为DDMM.MMMM（度+分）
 * @param lat_dir 纬度方向，'N'表示北纬，'S'表示南纬
 * @return double 转换后的十进制纬度值，范围为[-90.0, 90.0]
 *
 * @note 转换公式：十进制纬度 = 度 + (分 + 分的小数部分)/60
 *       南纬(S)返回负值，北纬(N)返回正值
 */
double lat_dm_to_decimal(double lat_dm, char lat_dir)
{

    int lat_int = (int)lat_dm;
    int degrees = lat_int / 100;     
    int minutes_int = lat_int % 100; 
    double minutes_frac = lat_dm - lat_int;
    double total_minutes = minutes_int + minutes_frac;

    double decimal_lat = degrees + total_minutes / 60.0;

    if (lat_dir == 'S') {
        decimal_lat = -decimal_lat;
    }

    return decimal_lat;
}

/**
 * @brief 将经度从度分格式转换为十进制格式
 *
 * @param lon_dm 度分格式的经度值（例如1234.56表示123度45.6分）
 * @param lon_dir 经度方向（'E'表示东经，'W'表示西经）
 * @return double 十进制格式的经度值（例如123.76）
 *
 * @note 输入值范围限制：lon_dm应在0-18000之间
 */
double lon_dm_to_decimal(double lon_dm, char lon_dir)
{
    int lon_int = (int)lon_dm;
    int degrees = lon_int / 100;
    int minutes_int = lon_int % 100;  
    double minutes_frac = lon_dm - lon_int;
    double total_minutes = minutes_int + minutes_frac; 

    double decimal_lon = degrees + total_minutes / 60.0;

    if (lon_dir == 'W') {
        decimal_lon = -decimal_lon;
    }

    return decimal_lon;
}

/**
 * @brief 分割字符串为标记(token)
 *
 * @param[in,out] stringp 指向待分割字符串指针的指针，分割后会被更新为剩余部分
 * @param[in] delim 分隔符字符串，包含所有可能的分隔字符
 * @return char* 返回当前分割出的标记字符串，若没有更多标记则返回NULL
 *
 * @note 此函数会修改原始字符串内容，将分隔符替换为'\0'
 * @note 类似标准库的strtok_r函数，但线程安全且可重入
 */
static char *strsplit(char **stringp, const char *delim)
{
    char *start = *stringp;
    char* p;

    p = (start != NULL) ? strpbrk(start, delim) : NULL;

    if (p == NULL)
    {
        *stringp = NULL;
    }
    else
    {
        *p = '\0';
        *stringp = p + 1;
    }

    return start;
}

/**
 * @brief 计算子字符串在主字符串中出现的次数
 *
 * @param str 主字符串，在其中搜索子字符串
 * @param substr 要搜索的子字符串
 * @return int 子字符串出现的次数，若未找到返回0
 *
 * @note 该函数区分大小写，且会统计所有重叠出现的子字符串
 */
static int strstr_cnt(char *str, char *substr)
{
    char *srcStr = str;
    int count = 0;

    do
    {
        srcStr = strstr(srcStr, substr);
        if(srcStr != NULL)
        {
            count++;
            srcStr = srcStr + strlen(substr);
        }
        else
        {
            break;
        }
    }while (*srcStr != '\0');

    return count;
}

#if ENABLE_GGA
/**
 * @brief 解析NMEA GGA格式的GPS数据
 *
 * 该函数解析标准的NMEA 0183 GGA语句，提取其中的GPS定位信息。
 * GGA语句格式示例：$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
 *
 * @param gga_data 包含GGA语句的字符串，以逗号分隔各字段
 *
 * @return GGA 包含解析后的GPS数据的结构体，包含以下字段：
 *         - utc: UTC时间字符串(HHMMSS格式)
 *         - lat: 纬度值(度分格式)
 *         - lat_dir: 纬度方向(N/S)
 *         - lon: 经度值(度分格式)
 *         - lon_dir: 经度方向(E/W)
 *         - quality: GPS质量指示(0=无效,1=GPS,2=DGPS)
 *         - sats: 使用的卫星数量
 *         - hdop: 水平精度因子
 *         - alt: 海拔高度(米)
 *         - undulation: 大地水准面高度差(米)
 *         - age: DGPS数据年龄(秒)
 *         - stn_ID: DGPS基准站ID
 *
 * @note 该函数不验证输入数据的校验和，调用者应确保数据有效性
 * @warning 输入字符串必须包含完整的GGA语句，否则可能导致解析错误
 */
static GGA gga_data_parse(char *gga_data)
{
    GGA gga;
    unsigned char times = 0;
    char *p;
    char end[16];

    p = strsplit(&gga_data, ",");
    while (p)
    {
        switch (times)
        {
            case 1:   // UTC
                strcpy(gga.utc, p);
                break;
            case 2:   // lat
                gga.lat = strtod(p, NULL);
                break;
            case 3:   // lat dir
                gga.lat_dir = p[0];
                break;
            case 4:   // lon
                gga.lon = strtod(p, NULL);
                break;
            case 5:   // lon dir
                gga.lon_dir = p[0];
                break;
            case 6:   // quality
                gga.quality = (unsigned char)strtol(p, NULL, 10);
                break;
            case 7:   // sats
                gga.sats = (unsigned char)strtol(p, NULL, 10);
                break;
            case 8:   // hdop
                gga.hdop = strtod(p, NULL);;
                break;
            case 9:   // alt
                gga.alt = strtod(p, NULL);
                break;
            case 11:  // undulation
                gga.undulation = strtod(p, NULL);
                break;
            case 13:  // age
                gga.age = (unsigned char)strtol(p, NULL, 10);
                break;
            case 14:  // stn_ID
                strncpy(end, p, strlen(p)-3);
                end[strlen(p)-3] = '\0';
                gga.stn_ID = (unsigned short )strtol(end, NULL, 10);
                break;
            default:
                break;
        }
        p = strsplit(&gga_data, ",");
        times++;
    }
    return gga;
}
#endif

#if ENABLE_GLL
/**
 * @brief 解析NMEA GLL格式的GPS位置数据
 *
 * @param gll_data 包含GLL格式数据的字符串，格式为：$GPGLL,ddmm.mmmm,N,dddmm.mmmm,W,hhmmss.ss,A*xx
 *
 * @return GLL 解析后的GLL数据结构体，包含：
 *            - lat: 纬度值(度分格式)
 *            - lat_dir: 纬度方向(N/S)
 *            - lon: 经度值(度分格式)
 *            - lon_dir: 经度方向(E/W)
 *            - utc: UTC时间字符串
 *            - data_status: 数据状态(A=有效,V=无效)
 *
 * @note 输入字符串应为完整的GLL语句，函数会按逗号分隔解析各字段
 */
static GLL gll_data_parse(char *gll_data)
{
    GLL gll;
    unsigned char times = 0;
    char *p;
    char *s = gll_data;

    p = strsplit(&s, ",");
    while (p)
    {
        switch (times)
        {
            case 1:   // lat
                gll.lat = strtod(p, NULL);
                break;
            case 2:   // lat dir
                gll.lat_dir = p[0];
                break;
            case 3:   // lon
                gll.lon = strtod(p, NULL);
                break;
            case 4:   // lon dir
                gll.lon_dir = p[0];
                break;
            case 5:   // lon dir
                strcpy(gll.utc, p);
                break;
            case 6:  // data status
                gll.data_status = p[0];
                break;
            default:
                break;
        }
        p = strsplit(&s, ",");
        times++;
    }
    return gll;
}
#endif

#if ENABLE_GSA
/**
 * @brief 从GPS数据中提取GSA语句的PRN（伪随机噪声码）信息
 *
 * 该函数解析NMEA格式的GSA语句，提取卫星PRN编号信息并填充到GSA_PRN结构体数组中。
 *
 * @param[in] gps_data 包含GSA语句的原始GPS数据字符串
 * @param[out] gsa_prn 存储解析结果的GSA_PRN结构体数组
 *
 * @note 函数会处理多条GSA语句，每条语句最多提取12个PRN编号
 * @note 输入数据应包含完整的NMEA语句，以"\r\n"分隔
 *
 * @warning 调用者需确保gsa_prn缓冲区足够大(MAX_GSA_CHANNELS * sizeof(GSA_PRN))
 */
static void get_prn_data(char *gps_data, GSA_PRN *gsa_prn)
{
    unsigned char times = 0;
    unsigned char i;
    unsigned char sentences_index = 0;
    char *p;
    char gsa_data_buffer[128];
    char *s = gsa_data_buffer;
    char *sentences;
    int gsa_count;

    gsa_count = strstr_cnt(gps_data, PRE_GSA);

    (void)memset(gsa_prn, 0, sizeof(GSA_PRN) * MAX_GSA_CHANNELS);
    sentences = strtok(gps_data, "\r\n");
    while (sentences)
    {
        if (strstr(sentences, PRE_GSA))
        {
            sentences_index++;
            s = gsa_data_buffer;
            (void)memcpy(s, sentences, strlen(sentences));
            p = strsplit(&s, ",");
            while (p)
            {
                if (times == 2)
                {
                    for (i=0; i<12; i++)
                    {
                        p = strsplit(&s, ",");
                        (gsa_prn+i+(sentences_index-1)*12)->total = (unsigned char)gsa_count * 12;
                        (gsa_prn+i+(sentences_index-1)*12)->prn_ID = i + (sentences_index - 1) * 12;
                        (gsa_prn+i+(sentences_index-1)*12)->prn = (unsigned char)strtol(p, NULL, 10);
                    }
                }
                p = strsplit(&s, ",");
                times++;
            }
            times = 0;
        }
        sentences = strtok(NULL, "\r\n");
    }
}

/**
 * @brief 解析GSA格式的GPS数据
 *
 * 该函数解析NMEA协议中的GSA格式数据，提取卫星定位模式、精度因子等信息。
 *
 * @param gsa_data 待解析的GSA格式字符串数据
 * @param gpsdata 完整的GPS原始数据字符串
 * @return GSA 解析后的GSA数据结构，包含以下字段：
 *         - mode_MA: 定位模式(M=手动，A=自动)
 *         - mode_123: 定位类型(1=无定位，2=2D定位，3=3D定位)
 *         - gsa_prn: 参与定位的卫星PRN号数组
 *         - pdop: 位置精度因子
 *         - hdop: 水平精度因子
 *         - vdop: 垂直精度因子
 * @note 该函数为静态函数，仅在当前文件内部使用
 */
static GSA gsa_data_parse(char *gsa_data, char *gpsdata)
{
    GSA gsa;
    unsigned char times = 0;
    char *p;
    char end[16];
    char *s = gsa_data;
    char *alldata = gpsdata;

    p = strsplit(&s, ",");
    while (p)
    {
        switch (times)
        {
            case 1:   // mode_MA
                gsa.mode_MA = p[0];
                break;
            case 2:   // mode_123
                gsa.mode_123 = p[0];
                break;
            case 3:   // prn
                get_prn_data(alldata, gsa.gsa_prn);
                break;
            case 15:  // pdop
                gsa.pdop = strtod(p, NULL);
                break;
            case 16:  // hdop
                gsa.hdop = strtod(p, NULL);
                break;
            case 17:  // vdop
                strncpy(end, p, strlen(p)-3);
                end[strlen(p)-3] = '\0';
                gsa.vdop = strtod(end, NULL);
                break;
            default:
                break;
        }
        p = strsplit(&s, ",");
        times++;
    }

    return gsa;
}
#endif

#if ENABLE_RMC
/**
 * @brief 解析NMEA RMC语句数据
 *
 * 解析标准的NMEA 0183 RMC（Recommended Minimum Specific GNSS Data）语句，
 * 提取其中的定位、导航和时间信息。
 *
 * @param rmc_data 包含RMC语句的字符串，格式为：$GPRMC,hhmmss.sss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a,a*hh
 *
 * @return RMC 包含解析后的RMC数据结构，包含以下字段：
 *         - utc: UTC时间（hhmmss.sss格式）
 *         - pos_status: 定位状态（A=有效，V=无效）
 *         - lat: 纬度（度分格式）
 *         - lat_dir: 纬度方向（N/S）
 *         - lon: 经度（度分格式）
 *         - lon_dir: 经度方向（E/W）
 *         - speed_Kn: 对地速度（节）
 *         - track_true: 对地航向（度）
 *         - date: UTC日期（ddmmyy格式）
 *         - mag_var: 磁偏角（度）
 *         - var_dir: 磁偏角方向（E/W）
 *         - mode_ind: 模式指示（A=自主，D=差分，E=估算，N=无效）
 */
static RMC rmc_data_parse(char *rmc_data)
{
    RMC rmc;
    unsigned char times = 0;
    char *p;
    char *s = rmc_data;

    p = strsplit(&s, ",");
    while (p)
    {
        switch (times)
        {
            case 1:   // UTC
                strcpy(rmc.utc, p);
                break;
            case 2:   // pos status
                rmc.pos_status = p[0];
                break;
            case 3:   // lat
                rmc.lat = strtod(p, NULL);
                break;
            case 4:   // lat dir
                rmc.lat_dir = p[0];
                break;
            case 5:   // lon
                rmc.lon = strtod(p, NULL);
                break;
            case 6:   // lon dir
                rmc.lon_dir = p[0];
                break;
            case 7:   // speen Kn
                rmc.speed_Kn = strtod(p, NULL);
                break;
            case 8:   // track true
                rmc.track_true = strtod(p, NULL);
                break;
            case 9:   // date
                strcpy(rmc.date, p);
                break;
            case 10:  // mag var
                rmc.mag_var = strtod(p, NULL);
                break;
            case 11:  // var dir
                rmc.var_dir = p[0];
                break;
            case 12:  // mode ind
                rmc.mode_ind = p[0];
                break;
            default:
                break;
        }
        p = strsplit(&s, ",");
        times++;
    }

    return rmc;
}
#endif

#if ENABLE_VTG
/**
 * @brief 解析NMEA VTG语句数据
 *
 * 该函数解析NMEA 0183协议中的VTG（地面速度）语句，提取航向和速度信息。
 *
 * @param vtg_data 包含VTG语句的字符串，格式如"$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*xx"
 *
 * @return VTG 包含解析后的VTG数据结构体，包含以下字段：
 *         - track_true: 真北航向（度）
 *         - track_mag: 磁北航向（度）
 *         - speed_Kn: 速度（节）
 *         - speed_Km: 速度（公里/小时）
 *
 * @note 该函数为静态函数，仅在当前文件内部使用
 * @note 输入字符串应为有效的NMEA VTG语句，函数不做格式验证
 */
static VTG vtg_data_parse(char *vtg_data)
{
    VTG vtg;
    unsigned char times = 0;
    char *p;
    char *s = vtg_data;

    p = strsplit(&s, ",");
    while (p)
    {
        switch (times)
        {
            case 1:   // track true
                vtg.track_true = strtod(p, NULL);
                break;
            case 3:   // track mag
                vtg.track_mag = strtod(p, NULL);
                break;
            case 5:   // speed Kn
                vtg.speed_Kn = strtod(p, NULL);
                break;
            case 7:   // speed Km
                vtg.speed_Km = strtod(p, NULL);
                break;
            default:
                break;
        }
        p = strsplit(&s, ",");
        times++;
    }

    return vtg;
}
#endif

#if ENABLE_GSV
/**
 * @brief 解析GPS卫星信息并填充到SAT_INFO结构体数组
 *
 * @param gps_data 包含GSV语句的原始GPS数据字符串
 * @param sats 需要解析的卫星总数
 * @param prefix GSV语句前缀(如"$GPGSV")
 * @param sats_info 输出参数，用于存储解析后的卫星信息数组
 *
 * @note 该函数会解析多行GSV语句，每条语句包含最多4颗卫星的信息
 * @note 函数内部会先清空sats_info数组，然后填充有效数据
 *
 * @details 解析逻辑：
 * 1. 按行分割原始数据，查找包含指定前缀的GSV语句
 * 2. 解析每条GSV语句中的卫星数量、当前消息序号等信息
 * 3. 提取每颗卫星的PRN号、仰角、方位角和信噪比(SNR)
 * 4. 将解析结果存储到对应的SAT_INFO结构体中
 */
static void get_sats_info(char *gps_data, unsigned char sats, char *prefix, SAT_INFO *sats_info)
{
    unsigned char times = 0;
    unsigned char msgs = 0;
    unsigned char msg = 0;
    unsigned char for_times;
    unsigned char i;
    char *p;
    char gsv_data_buffer[128];
    char *s = gsv_data_buffer;
    char *sentences;

    (void)memset(sats_info, 0, sizeof(SAT_INFO) * (sats+1));
    sentences = strtok(gps_data, "\r\n");
    while (sentences)
    {
        if (strstr(sentences, prefix))
        {
            s = gsv_data_buffer;
            (void)memcpy(s, sentences, strlen(sentences));
            p = strsplit(&s, ",");
            while (p)
            {
                switch (times)
                {
                    case 1:   // msgs
                        msgs = (unsigned char) strtol(p, NULL, 10);
                        break;
                    case 2:   // msg
                        msg = (unsigned char) strtol(p, NULL, 10);
                        break;
                    case 3:   // sat info
                        for_times = (msgs == msg) ? ((sats % 4) ? sats % 4 : 4) : 4;
                        for (i = 0; i < for_times; i++)
                        {
                            p = strsplit(&s, ",");
                            (sats_info+(msg-1)*4+i)->prn = (unsigned char) strtol(p, NULL, 10);
                            p = strsplit(&s, ",");
                            (sats_info+(msg-1)*4+i)->elev = (unsigned char) strtol(p, NULL, 10);
                            p = strsplit(&s, ",");
                            (sats_info+(msg-1)*4+i)->azimuth = (unsigned short) strtol(p, NULL, 10);
                            p = strsplit(&s, ",");
                            (sats_info+(msg-1)*4+i)->SNR = (unsigned char) strtol(p, NULL, 10);
                        }
                        break;
                    default:
                        break;
                }
                p = strsplit(&s, ",");
                times++;
            }
            times = 0;
        }
        sentences = strtok(NULL, "\r\n");
    }
}

/**
 * @brief 解析GSV（GNSS Satellites in View）NMEA语句数据
 *
 * @param gsv_data 待解析的GSV原始数据字符串
 * @param gps_data 完整的GPS数据字符串，用于获取卫星详细信息
 * @param prefix 卫星信息前缀标识符
 * @return GSV 解析后的GSV数据结构，包含卫星数量和信息
 *
 * @note 该函数会解析GSV语句中的以下字段：
 *       - 总消息数（msgs）
 *       - 当前消息序号（msg）
 *       - 可见卫星数（sats）
 *       - 各卫星详细信息（通过get_sats_info获取）
 */
static GSV gsv_data_parse(char *gsv_data, char *gps_data, char *prefix)
{
    GSV gsv;
    unsigned char times = 0;
    char *p;
    char *s = gsv_data;
    char *src_data = gps_data;

    p = strsplit(&s, ",");
    while (p)
    {
        switch (times)
        {
            case 1:   // msgs
                gsv.msgs = (unsigned char)strtol(p, NULL, 10);
                break;
            case 2:   // msg
                gsv.msg = (unsigned char)strtol(p, NULL, 10);
                break;
            case 3:   // sats
                gsv.sats = (unsigned char)strtol(p, NULL, 10);
                get_sats_info(src_data, gsv.sats, prefix, gsv.sat_info);
                break;
            default:
                break;
        }
        p = strsplit(&s, ",");
        times++;
    }

    return gsv;
}
#endif

#if ENABLE_UTC
/**
 * @brief 解析UTC日期时间字符串为UTC结构体
 *
 * @param date 日期字符串，格式为DDMMYY
 * @param time 时间字符串，格式为hhmmss.ss
 * @return UTC 解析后的UTC时间结构体
 *
 * @note 日期格式示例："010122"表示2022年1月1日
 * @note 时间格式示例："123456.78"表示12点34分56秒78毫秒
 */
static UTC utc_parse(char *date, char *time)
{
    UTC utc_data;
    unsigned int date_int;
    double time_float;

    date_int = (unsigned int)strtol(date, NULL, 10);
    utc_data.DD = date_int / 10000;
    utc_data.MM = date_int % 10000 / 100;
    utc_data.YY = date_int % 100;
    time_float = strtod(time, NULL);
    utc_data.hh = (unsigned int)time_float / 10000;
    utc_data.mm = (unsigned int)time_float % 10000 / 100;
    utc_data.ss = (unsigned int)time_float % 100;
    utc_data.ds = (unsigned short)(time_float - (unsigned int)time_float);

    return utc_data;
}
#endif

/**
 * @brief 初始化全局GPS数据结构
 *
 * 该函数用于初始化全局GPS数据结构体gps_all，根据编译时定义的宏(ENABLE_GGA/ENABLE_RMC等)
 * 来初始化对应的NMEA语句解析数据结构。每个启用的NMEA语句类型都会被初始化为默认值。
 *
 * @note 该函数应在系统启动时调用一次，用于初始化GPS解析模块的全局数据结构
 * @note 当前实现不使用动态内存分配，所有缓冲区均为静态分配
 *
 * 支持的NMEA语句类型包括：
 * - GGA: 全球定位系统定位数据
 * - GLL: 地理定位信息
 * - GSA: 当前卫星信息
 * - RMC: 推荐最小定位信息
 * - VTG: 地面速度信息
 * - GSV: 可见卫星信息
 *
 * 每个语句类型的初始化值均为零或空字符串
 */
void gps_global_init(void)
{
    (void)memset(gps_all.gps_buffer_data, 0u, MAX_GPS_BUFFER_SIZE);
#if ENABLE_GGA
    GGA default_gga_data = {"\0",0.0,'N',0.0,'S',0,0,0,0,0,0,0};
    gps_all.gga_data = default_gga_data;
#endif

#if ENABLE_GLL
    GLL default_gll_data = {0.0,'\0',0.0,'\0',"\0",'\0'};
    gps_all.gll_data = default_gll_data;
#endif

#if ENABLE_GSA
    GSA_PRN default_gsa_prn_data = {0,0,0};
    GSA default_gsa_data = {'\0', '\0', 0.0, 0.0, 0.0, default_gsa_prn_data};
    gps_all.gsa_data = default_gsa_data;
#endif

#if ENABLE_RMC
    RMC default_rmc_data = {"\0",'\0',0.0,0.0,'\0',0.0,0.0,'\0',0.0,0.0,"\0",0.0,'\0','\0'};
    gps_all.rmc_data = default_rmc_data;
#endif

#if ENABLE_VTG
    VTG default_vtg_data = {0.0,0.0,0.0,0.0};
    gps_all.vtg_data = default_vtg_data;
#endif

#if ENABLE_GSV
    SAT_INFO default_sat_info_data = {0,0,0,0};
    GSV default_gsv_data = {0, 0, 0, default_sat_info_data};
    gps_all.gpgsv_data = default_gsv_data;
#endif
}

/**
 * @brief 解析NMEA格式的GPS原始数据，提取各数据段信息
 *
 * @param gps_src 输入的NMEA格式GPS原始数据字符串
 * @param gps_src_len 输入数据的长度
 * @return GPS 包含所有解析后GPS数据的结构体
 *
 * @note 根据编译时定义的宏(ENABLE_GGA/ENABLE_GLL等)决定解析哪些NMEA数据段
 *       未启用的数据段将返回默认值
 *       支持解析的NMEA数据段包括：
 *       - GGA(全球定位系统固定数据)
 *       - GLL(地理定位信息)
 *       - GSA(当前卫星信息)
 *       - RMC(推荐最小特定GPS/TRANSIT数据)
 *       - VTG(地面速度信息)
 *       - GSV(可见卫星信息)
 *       - UTC(时间信息)
 */
GPS gps_data_parse(const char *gps_src, const unsigned short gps_src_len)
{
#if ENABLE_GGA
    (void)memcpy(gps_all.gps_buffer_data, gps_src, gps_src_len);
    if(strstr(gps_src, PRE_GGA))
    {
        gps_all.gga_data = gga_data_parse(strtok(strstr(gps_all.gps_buffer_data, PRE_GGA), "\r\n"));
    }
#endif

#if ENABLE_GLL
    (void)memcpy(gps_all.gps_buffer_data, gps_src, gps_src_len);
    if(strstr(gps_src, PRE_GLL))
    {
        gps_all.gll_data = gll_data_parse(strtok(strstr(gps_all.gps_buffer_data, PRE_GLL), "\r\n"));
    }
#endif

#if ENABLE_GSA
    (void)memcpy(gps_src_backup, gps_src, gps_src_len);
    if(strstr(gps_src, PRE_GSA))
    {
        gps_all.gsa_data = gsa_data_parse(strtok(strstr(gps_all.gps_buffer_data, PRE_GSA), "\r\n"), gps_src_backup);
    }
#endif

#if ENABLE_RMC
    (void)memcpy(gps_all.gps_buffer_data, gps_src, gps_src_len);
    if(strstr(gps_src, PRE_RMC))
    {
        gps_all.rmc_data = rmc_data_parse(strtok(strstr(gps_all.gps_buffer_data, PRE_RMC), "\r\n"));
        gps_all.rmc_data.lat_dec = lat_dm_to_decimal(gps_all.rmc_data.lat, gps_all.rmc_data.lat_dir);
        gps_all.rmc_data.lon_dec = lon_dm_to_decimal(gps_all.rmc_data.lon, gps_all.rmc_data.lon_dir);
    }
#endif

#if ENABLE_VTG
    (void)memcpy(gps_all.gps_buffer_data, gps_src, gps_src_len);
    if(strstr(gps_src, PRE_VTG))
    {
        gps_all.vtg_data = vtg_data_parse(strtok(strstr(gps_all.gps_buffer_data, PRE_VTG), "\r\n"));
    }
#endif

#if ENABLE_GSV
    SAT_INFO default_sat_info_data = {0,0,0,0};
    GSV default_gsv_data = {0, 0, 0, default_sat_info_data};

    (void)memset(gps_all.gps_buffer_data, 0u, sizeof(gps_all.gps_buffer_data));
    (void)memcpy(gps_all.gps_buffer_data, gps_src, gps_src_len);
    (void)memcpy(gps_src_backup, gps_src, gps_src_len);

    gps_all.gpgsv_data = strstr(gps_src, PRE_GPGSV) ? gsv_data_parse(strtok(strstr(gps_all.gps_buffer_data, PRE_GPGSV), "\r\n"), gps_src_backup, PRE_GPGSV) : default_gsv_data;

    (void)memset(gps_all.gps_buffer_data, 0u, sizeof(gps_all.gps_buffer_data));
    (void)memcpy(gps_all.gps_buffer_data, gps_src, gps_src_len);
    (void)memcpy(gps_src_backup, gps_src, gps_src_len);
    gps_all.gngsv_data = strstr(gps_src, PRE_GNGSV) ? gsv_data_parse(strtok(strstr(gps_all.gps_buffer_data, PRE_GNGSV), "\r\n"), gps_src_backup, PRE_GNGSV) : default_gsv_data;

    (void)memset(gps_all.gps_buffer_data, 0u, sizeof(gps_all.gps_buffer_data));
    (void)memcpy(gps_all.gps_buffer_data, gps_src, gps_src_len);
    (void)memcpy(gps_src_backup, gps_src, gps_src_len);
    gps_all.glgsv_data = strstr(gps_src, PRE_GLGSV) ? gsv_data_parse(strtok(strstr(gps_all.gps_buffer_data, PRE_GLGSV), "\r\n"), gps_src_backup, PRE_GLGSV) : default_gsv_data;
#endif

#if ENABLE_UTC && ENABLE_RMC
    gps_all.utc = utc_parse(gps_all.rmc_data.date, gps_all.rmc_data.utc);
#endif

    return gps_all;
}
