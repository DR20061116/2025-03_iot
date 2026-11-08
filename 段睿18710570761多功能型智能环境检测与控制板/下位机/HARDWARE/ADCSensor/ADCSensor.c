#include "ADCSensor.h"
#include "delay.h"
#include <math.h>

// 初始化三个传感器引脚
void Sensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能PORTA时钟
    
    // 配置PA0（MQ5传感器）为模拟输入 ★仅修改注释：甲醛→MQ5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置PA1（光敏传感器）为模拟输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置PA4（MQ4传感器）为模拟输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    Adc_Init();
}

// 读取光敏传感器值（光照强度越大，百分比越大）
u16 LightSensor_GetValue(void)
{
    u16 adc_value;
    u32 temp_val = 0;
    u8 t;
    
    for(t = 0; t < SENSOR_READ_TIMES; t++)
    {
        temp_val += Get_Adc(ADC_LIGHT_CHANNEL);
        delay_ms(5);
    }
    temp_val /= SENSOR_READ_TIMES;
    adc_value = (u16)temp_val;
    
    // 光敏传感器特性：光照越强，ADC值越小，因此需要反相处理
    // 4095 - adc_value 得到光照强度越大值越大
    u32 light_strength = 4095 - adc_value;
    u32 percentage = light_strength * 100 / 3000;
    
    if(percentage >= 100) percentage = 100;
    
    return (u16)percentage;
}

// 读取MQ4传感器值（百分比0-100）
u16 MQ4Sensor_GetValue(void)
{
    u16 adc_value;
    u32 temp_val = 0;
    u8 t;
    u32 percentage; // ★变量名保留，仅用途改为百分比
    
    // ========== 仅加这一行：通道切换稳定延时（解决归零核心问题） ==========
    delay_us(20);
    for(t = 0; t < SENSOR_READ_TIMES; t++)
    {
        temp_val += Get_Adc(ADC_MQ4_CHANNEL); 
        delay_ms(5);
    }
    temp_val /= SENSOR_READ_TIMES;
    adc_value = (u16)temp_val;
    
    // 和MQ4完全一致的百分比计算逻辑 ★去掉原PPM换算，改为百分比
    percentage = (u32)adc_value * 100 / 4095;
    
    if(percentage >= 100) percentage = 100;
    
    return (u16)percentage;
}

// 读取MQ5传感器值（百分比0-100） ★仅修改注释：甲醛→MQ5，逻辑改为和MQ4一致
u16 HCHOSensor_GetPPM(void)
{
    u16 adc_value;
    u32 temp_val = 0;
    u8 t;
    u32 percentage; // ★变量名保留，仅用途改为百分比
    
    for(t = 0; t < SENSOR_READ_TIMES; t++)
    {
        temp_val += Get_Adc(ADC_HCHO_CHANNEL); // 宏名不变，仅实际为MQ5通道
        delay_ms(5);
    }
    temp_val /= SENSOR_READ_TIMES;
    adc_value = (u16)temp_val;
    
    // 和MQ4完全一致的百分比计算逻辑 ★去掉原PPM换算，改为百分比
    percentage = (u32)adc_value * 100 / 4095;
    
    if(percentage >= 100) percentage = 100;
    
    return (u16)percentage;
}
