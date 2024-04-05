#include "stm32f10x.h"                  // Device header

uint16_t AD_Value[4];

void AD_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // 使能ADC1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 使能DMA1时钟
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 设置ADC时钟分频系数为6，数据手册推荐
    
    GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 设置GPIO模式为模拟输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; // 设置GPIO引脚为PA0~PA3
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 设置GPIO速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIOA
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5); // 配置ADC1的常规通道0，对应上面GPIO_Pin_x，参考引脚定义
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5); // 配置ADC1的常规通道1，硬件请求dma映射上必须是adc1，软件均可，参考dma请求映像
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5); // 配置ADC1的常规通道2
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5); // 配置ADC1的常规通道3
        
    ADC_InitTypeDef ADC_InitStructure; // 定义ADC初始化结构体
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 设置ADC工作模式为独立模式
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 设置ADC数据对齐方式为右对齐
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 设置ADC外部触发转换方式为无触发，硬件触发
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 设置ADC连续转换模式为使能，对应dma循环模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; // 设置ADC扫描转换模式为使能
    ADC_InitStructure.ADC_NbrOfChannel = 4; // 设置ADC通道数量为4，上面开启4个
    ADC_Init(ADC1, &ADC_InitStructure); // 初始化ADC1
    
    DMA_InitTypeDef DMA_InitStructure; // 定义DMA初始化结构体
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // 设置DMA外设基地址为ADC1的数据寄存器地址
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 设置DMA外设数据大小为半字
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 设置DMA外设地址不递增
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value; // 设置DMA内存基地址为AD_Value数组的地址
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 设置DMA内存数据大小为半字
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 设置DMA内存地址递增，往下存储，否则全部是一样的数据
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 设置DMA数据传输方向为外设到内存
    DMA_InitStructure.DMA_BufferSize = 4; // 设置DMA缓冲区大小为4
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 设置DMA模式为循环模式，对应ADC连续转换模式
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 设置DMA为非存储器到存储器模式，非内存到内存的传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // 设置DMA优先级为中等
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); // 初始化DMA1通道1
    
    DMA_Cmd(DMA1_Channel1, ENABLE); // 使能DMA1通道1
    ADC_DMACmd(ADC1, ENABLE); // 使能ADC1的DMA功能，参考硬件图选择
    ADC_Cmd(ADC1, ENABLE); // 使能ADC1
    
    ADC_ResetCalibration(ADC1); // 复位ADC1校准寄存器
    while (ADC_GetResetCalibrationStatus(ADC1) == SET); // 等待复位校准完成
    ADC_StartCalibration(ADC1); // 开始ADC1校准
    while (ADC_GetCalibrationStatus(ADC1) == SET); // 等待校准完成
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 启动ADC1软件转换，自动化
}
