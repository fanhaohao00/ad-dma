#include "stm32f10x.h"                  // Device header

uint16_t AD_Value[4];

void AD_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // ʹ��ADC1ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��GPIOAʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ʹ��DMA1ʱ��
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ����ADCʱ�ӷ�Ƶϵ��Ϊ6�������ֲ��Ƽ�
    
    GPIO_InitTypeDef GPIO_InitStructure; // ����GPIO��ʼ���ṹ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // ����GPIOģʽΪģ������ģʽ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; // ����GPIO����ΪPA0~PA3
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // ����GPIO�ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure); // ��ʼ��GPIOA
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5); // ����ADC1�ĳ���ͨ��0����Ӧ����GPIO_Pin_x���ο����Ŷ���
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5); // ����ADC1�ĳ���ͨ��1��Ӳ������dmaӳ���ϱ�����adc1��������ɣ��ο�dma����ӳ��
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5); // ����ADC1�ĳ���ͨ��2
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5); // ����ADC1�ĳ���ͨ��3
        
    ADC_InitTypeDef ADC_InitStructure; // ����ADC��ʼ���ṹ��
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // ����ADC����ģʽΪ����ģʽ
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // ����ADC���ݶ��뷽ʽΪ�Ҷ���
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // ����ADC�ⲿ����ת����ʽΪ�޴�����Ӳ������
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // ����ADC����ת��ģʽΪʹ�ܣ���Ӧdmaѭ��ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; // ����ADCɨ��ת��ģʽΪʹ��
    ADC_InitStructure.ADC_NbrOfChannel = 4; // ����ADCͨ������Ϊ4�����濪��4��
    ADC_Init(ADC1, &ADC_InitStructure); // ��ʼ��ADC1
    
    DMA_InitTypeDef DMA_InitStructure; // ����DMA��ʼ���ṹ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // ����DMA�������ַΪADC1�����ݼĴ�����ַ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // ����DMA�������ݴ�СΪ����
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // ����DMA�����ַ������
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value; // ����DMA�ڴ����ַΪAD_Value����ĵ�ַ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // ����DMA�ڴ����ݴ�СΪ����
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // ����DMA�ڴ��ַ���������´洢������ȫ����һ��������
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // ����DMA���ݴ��䷽��Ϊ���赽�ڴ�
    DMA_InitStructure.DMA_BufferSize = 4; // ����DMA��������СΪ4
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // ����DMAģʽΪѭ��ģʽ����ӦADC����ת��ģʽ
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // ����DMAΪ�Ǵ洢�����洢��ģʽ�����ڴ浽�ڴ�Ĵ���
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // ����DMA���ȼ�Ϊ�е�
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); // ��ʼ��DMA1ͨ��1
    
    DMA_Cmd(DMA1_Channel1, ENABLE); // ʹ��DMA1ͨ��1
    ADC_DMACmd(ADC1, ENABLE); // ʹ��ADC1��DMA���ܣ��ο�Ӳ��ͼѡ��
    ADC_Cmd(ADC1, ENABLE); // ʹ��ADC1
    
    ADC_ResetCalibration(ADC1); // ��λADC1У׼�Ĵ���
    while (ADC_GetResetCalibrationStatus(ADC1) == SET); // �ȴ���λУ׼���
    ADC_StartCalibration(ADC1); // ��ʼADC1У׼
    while (ADC_GetCalibrationStatus(ADC1) == SET); // �ȴ�У׼���
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // ����ADC1���ת�����Զ���
}
