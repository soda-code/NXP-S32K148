#include "./bsp/adc.h"
#include "Cpu.h"

uint16_t adcMax; // ADC最大值，根据配置的分辨率计算得出

void adc_init(void)
{
    //*根据配置选择量程*/
    if (adConv1_ConvConfig0.resolution == ADC_RESOLUTION_8BIT)
    {
        adcMax = (uint16_t) (1 << 8);
    }
    else if (adConv1_ConvConfig0.resolution == ADC_RESOLUTION_10BIT)
    {
        adcMax = (uint16_t) (1 << 10);
    }
    else
    {
        adcMax = (uint16_t) (1 << 12);
    }

    // ADC初始化代码    
	ADC_DRV_ConfigConverter(INST_ADCONV1, &adConv1_ConvConfig0);//初始化ADC
	ADC_DRV_AutoCalibration(INST_ADCONV1);//校准ADC
}

//*****************************************************************
// @fun     : adc_read
// @brief   : 读取ADC值并转换为电压
// @param   : 无
// @return  : 转换后的电压值
//*****************************************************************
uint16_t adc_read(void)
{
    uint16_t adcRawValue = 0; // 存储原始ADC值
    /* 设置ADC通道,并设置软件触发 */
    ADC_DRV_ConfigChan(INST_ADCONV1, 0U, &adConv1_ChnConfig0);
    /* 等待转换完成 */
    ADC_DRV_WaitConvDone(INST_ADCONV1);
    /* 获取ADC量 */
    ADC_DRV_GetChanResult(INST_ADCONV1, 0U, &adcRawValue);
    /* 转换数值为电压 */
    return adcRawValue;
}
