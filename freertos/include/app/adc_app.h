#ifndef ADC_APP_H_
#define ADC_APP_H_


/* ADC 数据队列配置 */
#define ADC_QUEUE_LENGTH    16      /* 队列深度：最多缓存16个ADC值 */
#define ADC_QUEUE_ITEM_SIZE sizeof(uint16_t)  /* 每个元素大小 */




void adc_task_creat(void);

#endif /* RTOS_H_ */
