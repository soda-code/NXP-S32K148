#ifndef OTA_FLAG_H
#define OTA_FLAG_H

#include <stdint.h>


/* * APP 向量表起始地址：
 * 按照 Bootloader 占用 128 KB（0x20000 字节）规划，
 * APP A 区的起始地址必须挪到 0x00020000
 */
#define APP_START_ADDRESS	0x00012000 //app开始地址


typedef struct
{
  uint32_t active_app;      // 0:app_a 1:app_b
  uint32_t upgrade_request; // 0:upgrade not request 1:upgrade request
  uint32_t app_a_crc;       // app_a crc 鏍￠獙
  uint32_t app_b_crc;       // app_b crc 鏍￠獙
  uint8_t  app_a_version[12];   // app_a 鐗堟湰鍙�
  uint8_t  app_b_version[12];   // app_b 鐗堟湰鍙�
  uint32_t reserve[10];
}ota_flag_t;

void ota_flag_init(void);
void ota_flag_update(void);
void ota_flag_reset(void);
ota_flag_t* ota_flag_get(void);
void ota_task_creat(void);
void JumpToUserApplication(unsigned int userSP, unsigned int userStartup);
#endif
