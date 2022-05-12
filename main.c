/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "stdio.h"

#define TRANSMIT_BUFFER_SIZE 32
#define NUMBER_OF_SENSORS 6
#define NUMBER_OF_CAN_PARAM 3

uint16 TX_DATA[NUMBER_OF_CAN_PARAM];
//uint32 output;

uint16 max(uint16 arr[], int n)
{
    int i;
    int max = 0;
    
    for (i = 0; i < n; i++)
        if (arr[i] > max)
        max = arr[i];
        
     return max;
}

uint16 min(uint16 arr[], int n, int max)
{
    int i;
    int min = max; //Initialising
    
    for (i = 0; i < n; i++){
        if ((arr[i] < min) && (arr[i] != 0)){
            min = arr[i];}
    }
        
     return min;
}

uint16 avg(uint16 arr[], int n, int max)
{
    int i;
    int sum = max; //Initialising
    int conn_sensors = 0;
    
    for (i = 0; i < n; i++){
        if (arr[i] != 0){
            sum = sum + arr[i];
            conn_sensors = conn_sensors + 1;
        }
    }
     return ((sum-max)/conn_sensors);
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    char TransmitBuffer[TRANSMIT_BUFFER_SIZE];
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_1_Start();
    ADC_DelSig_1_Start();
    AMux_1_Start();
    CAN_1_Start();
    
    ADC_DelSig_1_StartConvert();
    UART_1_PutString("COM Port Open");
    
    for(;;)
    {   
        /* Place your application code here. */
        uint16 datapin[NUMBER_OF_SENSORS-1];
        int i;
        for (i = 0; i < NUMBER_OF_SENSORS; i++){
        AMux_1_Select(i);
        if(ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_RETURN_STATUS))
        {
        //ADC_DelSig_1_SetGain(51);
            datapin[i]=  ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult16());
            if (datapin[i] < 100){ //Some tolerance for Grounded Voltage (100mV) for 'Disconnected Pin'
                datapin[i] = 0;
                sprintf(TransmitBuffer, "Pin %d to GND\r\n", i);
                UART_1_PutString(TransmitBuffer);
            }
            else{   
        //output = ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult8());
            sprintf(TransmitBuffer, "Pin %d: %d mV\r\n", i, datapin[i]);
            UART_1_PutString(TransmitBuffer);
            }
        }
        
        CyDelay(100);
        }
        
        TX_DATA[0] = max(datapin, NUMBER_OF_SENSORS);
        TX_DATA[1] = min(datapin, NUMBER_OF_SENSORS, TX_DATA[0]);
        TX_DATA[2] = avg(datapin, NUMBER_OF_SENSORS, TX_DATA[0]);
        //TX_DATA8[0] = (int)min(datapin, NUMBER_OF_SENSORS)*10;
        //TX_DATA8[1] = (int)max(datapin, NUMBER_OF_SENSORS)*10;
        //TX_DATA8[2] = (int)avg(datapin, NUMBER_OF_SENSORS)*10;
        
        sprintf(TransmitBuffer, "\nMax: %d mV\r\n", TX_DATA[0]);
        UART_1_PutString(TransmitBuffer);
        CyDelay(100);
        sprintf(TransmitBuffer, "Min: %d mV\r\n", TX_DATA[1]);
        UART_1_PutString(TransmitBuffer);
        CyDelay(100);
        sprintf(TransmitBuffer, "Avg: %d mV\r\n\n", TX_DATA[2]);
        UART_1_PutString(TransmitBuffer);
        CyDelay(100);
        
        CAN_1_SendMsg0();
        CyDelay(1000);
    }
}

/* [] END OF FILE */
