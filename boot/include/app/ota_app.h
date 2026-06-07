#ifndef OTA_FLAG_H
#define OTA_FLAG_H

#include <stdint.h>

#define OTA_FLAG_ADDR    (0x0017E000u)

#define APP_A_START       (0x00020000u)
#define APP_B_START       (0x000CF000u)

#define APP_A_ID          (0u)
#define APP_B_ID          (1u)

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
#endif
