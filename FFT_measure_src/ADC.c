#include "ADC.h"
#include "stupid_delay.h"
#define ADC_INTR_NO 18 /*STM32F10x manual, p. 130*/
extern __IO uint16_t ADC_result[256];
void ADC_init(){

	/*PC3 - probe pin, ADC channel = 13 */

	/*enabling clock source for ADC and GPIOC*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPCEN;
	/*setting ADC preslacer to PLCK divided by 2 (24MHz/2)*/
	RCC->CFGR &= ~RCC_CFGR_ADCPRE;
	/*reseting PC3 to input - analog mode*/
	GPIOC->CRL &= ~GPIO_CRL_MODE3;
	/*enabling end of conv. interrupt*/
	ADC1->CR1 |= ADC_CR1_EOCIE;
	NVIC_EnableIRQ((IRQn_Type)ADC_INTR_NO);
	/*Discontinous mode channel count = 1*/
	ADC1->CR1 &= ~ADC_CR1_DISCNUM;
	/*Discontinous mode on regulary channel enabled*/
	ADC1->CR1 |= ADC_CR1_DISCEN;
	/*External trigger conversion mode for regular channels*/
	ADC1->CR2 |= ADC_CR2_EXTTRIG; //TODO ARE YOU SURE? TODO
	/*External event select for regular group: TIM3 TRGO event*/
	ADC1->CR2 &= ~ADC_CR2_EXTSEL;
	ADC1->CR2 |= ADC_CR2_EXTSEL_2;
	/*Alignment to right*/
	ADC1->CR2 &= ~(ADC_CR2_ALIGN); 
	/*sample time = 239.5 cycles*/
	ADC1->SMPR1 |= ADC_SMPR1_SMP13_2;
	ADC1->SMPR1 |= ADC_SMPR1_SMP13_1 | ADC_SMPR1_SMP13_0;
	ADC1->SQR1 = 0; //only 1 conversion
	ADC1->SQR3 = ADC_SQR3_SQ1_3 | ADC_SQR3_SQ1_2 | ADC_SQR3_SQ1_0;
	/*A/D converter ON*/
	ADC1->CR2 |= ADC_CR2_ADON;
	/*initalizing calibration registers, and wait to end*/
	ADC1->CR2 |= ADC_CR2_RSTCAL;
	while(ADC1->CR2 & ADC_CR2_RSTCAL);
	/*A/D calibration*/
	ADC1->CR2 |= ADC_CR2_CAL;
	while(ADC1->CR2 & ADC_CR2_CAL);
	/*^waiting till end of calib*/
	/*ADC start*/
	delay_ms(10);	/*TODO*/
	ADC1->CR2 |= ADC_CR2_ADON;
}

void ADC1_IRQHandler(void){
	static uint16_t it = 0;
	ADC_result[it] = ADC1->DR;
	it = (it + 1)%256;
}
