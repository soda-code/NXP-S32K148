#ifndef ADC_H_
#define ADC_H_

#include "app.h"

typedef enum
{
	ADC_CH_1 = 0,
	ADC_CH_2,
	ADC_CH_MAX=255,
} Adc_Enum;

void adc_init(void);
uint16_t adc_read(void);

#endif /* RTOS_H_ */
