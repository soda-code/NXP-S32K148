#ifndef ADC_H_
#define ADC_H_

#include "app.h"

typedef enum
{
    ADC_CH_POT      = 0,    /* 电位器通道 */
    ADC_CH_TEMP     = 1,    /* 温度传感器 */
    ADC_CH_BATTERY  = 2,    /* 电池电压检测 */
    ADC_CH_LIGHT    = 3,    /* 光照传感器 */

    ADC_CH_NUM      = 4,    /* ADC通道总数（放在最后！）*/
    ADC_CH_INVALID  = 0xFF  /* 无效通道（用于错误检测）*/
} Adc_Enum;

void adc_init(void);
void adc_get_value(Adc_Enum adc_ch, uint16_t *adc_value);
#endif /* RTOS_H_ */
