#include "key.h"
#include "sys.h" 
#include "delay.h"

// 按键初始化函数
void KEY_Init(void)
{ 
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 配置KEY1~KEY4引脚为上拉输入模式
    GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN | KEY3_PIN | KEY4_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// 按键扫描函数
// 参数mode: 0-不支持连续按; 1-支持连续按
// 返回值: 
//   0-无按键按下
//   1-KEY1按下
//   2-KEY2按下  
//   3-KEY3按下
//   4-KEY4按下
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1; // 按键松开标志
    
    if(mode == 1)
    {
        // 支持连续按模式
        if(KEY1 == 0) // 检测KEY1按下
        {
            delay_ms(10); // 去抖动
            if(KEY1 == 0) // 确认按键按下
            {
                return KEY1_PRES;
            }
        }
        else if(KEY2 == 0) // 检测KEY2按下
        {
            delay_ms(10); // 去抖动
            if(KEY2 == 0) // 确认按键按下
            {
                return KEY2_PRES;
            }
        }
        else if(KEY3 == 0) // 检测KEY3按下
        {
            delay_ms(10); // 去抖动
            if(KEY3 == 0) // 确认按键按下
            {
                return KEY3_PRES;
            }
        }
        else if(KEY4 == 0) // 检测KEY4按下
        {
            delay_ms(10); // 去抖动
            if(KEY4 == 0) // 确认按键按下
            {
                return KEY4_PRES;
            }
        }
    }
    else
    {
        // 不支持连续按模式
        if((KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0) && key_up == 1)
        {
            delay_ms(10); // 去抖动
            key_up = 0;   // 标记按键已按下
            
            if(KEY1 == 0) // 确认KEY1按下
            {
                return KEY1_PRES;
            }
            else if(KEY2 == 0) // 确认KEY2按下
            {
                return KEY2_PRES;
            }
            else if(KEY3 == 0) // 确认KEY3按下
            {
                return KEY3_PRES;
            }
            else if(KEY4 == 0) // 确认KEY4按下
            {
                return KEY4_PRES;
            }
        }
        else if(KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1) // 所有按键都松开
        {
            key_up = 1; // 标记按键已松开
        }
    }
    
    return 0; // 无按键按下
}
