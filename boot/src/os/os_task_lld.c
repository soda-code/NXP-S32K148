#include "os_task_lld.h"
#include "interface.h"

void task_lld_asw_init(void)
{
   // demo_initialize();
}


void task_lld_asw_10ms(void *pvPara)
{
    (void)pvPara;
    TickType_t LastTick = xTaskGetTickCount();

    while(1)
    {
        //demo_step();

        vTaskDelayUntil(&LastTick, TASK_PERIOD_10MS);
    }
}

/**
 * @brief 100ms 鍛ㄦ湡搴旂敤灞傝皟搴︿换鍔�
 *
 * 浠� 100ms 涓哄懆鏈熸墽琛屽簲鐢ㄥ眰涓氬姟閫昏緫锛堝綋鍓嶄负棰勭暀浠诲姟锛屽緟鍚庣画鍔熻兘鎵╁睍锛夈��
 *
 * @param[in] pvPara 浠诲姟鍙傛暟锛堟湭浣跨敤锛�
 *
 * @note 浠诲姟浼樺厛绾э細闇�鍦� Task_Create() 涓‘璁�
 */
void task_lld_asw_100ms(void *pvPara)
{
    (void)pvPara;
    TickType_t LastTick = xTaskGetTickCount();

    while(1)
    {
        /* TODO: 娣诲姞 100ms 鍛ㄦ湡涓氬姟閫昏緫 */

        vTaskDelayUntil(&LastTick, TASK_PERIOD_100MS);
    }
}
/**********************************************************************************************************************
 *  END OF FILE: os_task_lld.c
 *********************************************************************************************************************/
