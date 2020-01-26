#include "app.h"

APP_DATA appData;

bool sigGenEnabled; //Enables signal generation
const float fSampling=49800.; //Sampling Frequency (49800 when PR2 = 1000)

unsigned long PA_COUNTER[18]; //Counter variable for timing
unsigned long PB_COUNTER[18];
unsigned long PC_COUNTER[18];
unsigned long PD_COUNTER[18];
unsigned long PE_COUNTER[18];
unsigned long PF_COUNTER[18];

unsigned long PA_COUNTER_DEFAULT[18]; //Default counter value gives phase angle for reset
unsigned long PB_COUNTER_DEFAULT[18];
unsigned long PC_COUNTER_DEFAULT[18];
unsigned long PD_COUNTER_DEFAULT[18];
unsigned long PE_COUNTER_DEFAULT[18];
unsigned long PF_COUNTER_DEFAULT[18];

unsigned long PA_ON[18]; //Counter position where it toggles port on
unsigned long PB_ON[18];
unsigned long PC_ON[18];
unsigned long PD_ON[18];
unsigned long PE_ON[18];
unsigned long PF_ON[18];

unsigned long PA_OFF[18]; //Counter position where it toggles port off
unsigned long PB_OFF[18];
unsigned long PC_OFF[18];
unsigned long PD_OFF[18];
unsigned long PE_OFF[18];
unsigned long PF_OFF[18];

unsigned int PORT_SHADOWREGISTERS[10]; //Register contains the values to be loaded into the ports at once after update

//Constants indicating which uC port corresponds to the physical output ports
const int PA_PORT[18]={9,9,9,1,1,1,7,7,7,7,0,0,1,8,8,8,8,1};
const int PB_PORT[18]={1,1,4,0,6,9,6,6,6,2,2,2,8,8,8,8,2,4};
const int PC_PORT[18]={4,4,0,6,4,4,4,6,6,6,4,4,8,8,8,8,0,0};
const int PD_PORT[18]={6,6,9,5,5,3,3,3,3,8,8,8,8,3,3,3,3,3};
const int PE_PORT[18]={2,2,3,7,7,7,7,3,3,3,0,0,9,9,9,5,5,0};
const int PF_PORT[18]={0,0,7,7,7,7,5,5,5,3,3,7,7,7,7,1,1,1};

//Constants indicating which uC port pin corresponds to the physical output port pins
const int PA_PIN[18]={3,2,1,11,10,8,3,2,1,0,10,9,2,15,14,13,11,4};
const int PB_PIN[18]={5,9,8,0,9,0,8,7,6,4,3,2,10,12,9,8,1,7};
const int PC_PIN[18]={6,5,5,15,4,3,2,13,12,14,1,0,7,6,5,4,7,6};
const int PD_PIN[18]={0,1,7,1,0,7,6,5,4,3,2,1,0,13,12,3,2,1};
const int PE_PIN[18]={14,13,0,15,14,13,12,11,10,9,15,14,6,5,4,5,4,4};
const int PF_PIN[18]={3,2,11,10,9,8,8,2,3,15,14,7,6,5,4,15,14,13};

//const unsigned int BITMASK_PORTB=0b0000001011001000; //NOT NEEDED

//Serial USB strings
char* outputString;
char* inputString;
char strCommand[4];
char PortStr[1];
char ChanStr[2];
char FreqStr[3];
char DutyStr[3];
char PhaseStr[3];

unsigned int txByte;
unsigned int rxByte;
const unsigned int rxBufferLimit=50;


void ResetCounters(){    
        //Resets all signal generator units to default OFF state                    
        int i;
        for (i=0;i<18;i++){
            //Reset counters to default phase angles
            PA_COUNTER[i]=PA_COUNTER_DEFAULT[i];
            PB_COUNTER[i]=PB_COUNTER_DEFAULT[i];
            PC_COUNTER[i]=PC_COUNTER_DEFAULT[i];
            PD_COUNTER[i]=PD_COUNTER_DEFAULT[i];
            PE_COUNTER[i]=PE_COUNTER_DEFAULT[i];
            PF_COUNTER[i]=PF_COUNTER_DEFAULT[i];
        }  
        
        for (i=0;i<10;i++){
            PORT_SHADOWREGISTERS[i]=0;
        }
            
        LATA=PORT_SHADOWREGISTERS[0];
        LATB=PORT_SHADOWREGISTERS[1];
        LATC=PORT_SHADOWREGISTERS[2];
        LATD=PORT_SHADOWREGISTERS[3];
        LATE=PORT_SHADOWREGISTERS[4];
        LATF=PORT_SHADOWREGISTERS[5];
        LATG=PORT_SHADOWREGISTERS[6];
        LATH=PORT_SHADOWREGISTERS[7];
        LATJ=PORT_SHADOWREGISTERS[8]; 
        LATK=PORT_SHADOWREGISTERS[9];   
            
}

void ZeroSigGen(){    
        //Resets all signal generator units to default OFF state                    
        int i;
        for (i=0;i<18;i++){
            //All counters will have zero duty cycle
            PA_ON[i]=PA_OFF[i];
            PB_ON[i]=PB_OFF[i];
            PC_ON[i]=PC_OFF[i];
            PD_ON[i]=PD_OFF[i];
            PE_ON[i]=PE_OFF[i];
            PF_ON[i]=PF_OFF[i];
        }  
        
        for (i=0;i<10;i++){
            PORT_SHADOWREGISTERS[i]=0;
        }
            
        LATA=PORT_SHADOWREGISTERS[0];
        LATB=PORT_SHADOWREGISTERS[1];
        LATC=PORT_SHADOWREGISTERS[2];
        LATD=PORT_SHADOWREGISTERS[3];
        LATE=PORT_SHADOWREGISTERS[4];
        LATF=PORT_SHADOWREGISTERS[5];
        LATG=PORT_SHADOWREGISTERS[6];
        LATH=PORT_SHADOWREGISTERS[7];
        LATJ=PORT_SHADOWREGISTERS[8]; 
        LATK=PORT_SHADOWREGISTERS[9];   
            
}

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    
}

void APP_Tasks ( void )
{
    
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            //Initializes serial port commands
            rxByte=0;
            txByte=0;
            inputString="                                                  "; //Initializes with empty string
            outputString="                                                  "; //Initializes with empty string
            
            sigGenEnabled=1; //Enables signal generation
            
            //Initializes the parameters of the signal generator units                       
            int i;
            for (i=0;i<18;i++){
                PA_COUNTER[i]=0;
                PB_COUNTER[i]=0;
                PC_COUNTER[i]=0;
                PD_COUNTER[i]=0;
                PE_COUNTER[i]=0;
                PF_COUNTER[i]=0;
                
                PA_COUNTER_DEFAULT[i]=0;
                PB_COUNTER_DEFAULT[i]=0;
                PC_COUNTER_DEFAULT[i]=0;
                PD_COUNTER_DEFAULT[i]=0;
                PE_COUNTER_DEFAULT[i]=0;
                PF_COUNTER_DEFAULT[i]=0;
                
                PA_ON[i]=125;
                PB_ON[i]=125;
                PC_ON[i]=125;
                PD_ON[i]=125;
                PE_ON[i]=125;
                PF_ON[i]=125;
                
                PA_OFF[i]=500;
                PB_OFF[i]=500;
                PC_OFF[i]=500;
                PD_OFF[i]=500;
                PE_OFF[i]=500;
                PF_OFF[i]=500;
            }  
            
            for (i=0;i<10;i++){
                PORT_SHADOWREGISTERS[i]=0;
            }
            
            bool appInitialized = true;
//      
 
            if (appInitialized)
            { 
//                appData.state = APP_STATE_SERVICE_TASKS;
                appData.state = APP_STATE_RX;   // change state to USART receive
                
                //Disables signal generation
                sigGenEnabled=0;
                ResetCounters();
                outputString="INITIALIZED.\n";   
            }
            break;
        }
 
        /* TODO: implement your application state machine.*/
        case APP_STATE_RX:                              // USART receive state
        {   
            // if byte received in USART instance 0 (USART1 in this case)
            if (!DRV_USART0_ReceiverBufferIsEmpty())
            {
               appData.rx_byte = DRV_USART0_ReadByte(); // read received byte
               
               switch (appData.rx_byte){
                   case 0x0D:
                   {
                        //CR character 
                        //CR does nothing. Only LF! 
                        break;         
                   }
                   case 0x0A:
                   {
                       //LF character 
                        appData.state = APP_STATE_INTERPRET;            // change state to TX
                        txByte=0; //Resets TX byte counter
                        rxByte=0; //Resets RX byte counter 
                        break;
                   }
                   default:
                   {
                   //Other string
                       inputString[rxByte]=appData.rx_byte;
                       rxByte++;
                       if (rxByte>rxBufferLimit){
                           //Loops around when limit exceeded
                           rxByte=0;
                       }
                       break;
                   }
               }
            }
            break;
        }
 
        case APP_STATE_TX:                              // USART transmit state
        {
            // make sure the transmit buffer is not full before trying to write byte 
            if(!(DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL & DRV_USART0_TransferStatus()) )
            {
                char currChar;
                currChar=outputString[txByte];   
                
                if (currChar!=0){
                    DRV_USART0_WriteByte(currChar);  // send modified byte                
                    txByte++;
                }else{
                    appData.state = APP_STATE_RX; 
                }                
            }
            break;
        }                
 
        case APP_STATE_INTERPRET:                              // Interpret command sent
        {
            //Reads command from buffer
            
            memcpy(&strCommand, inputString, 4);
            
            memcpy(&PortStr, inputString+5, 1);
            memcpy(&ChanStr, inputString+6, 2);
            memcpy(&FreqStr, inputString+9, 3);
            memcpy(&DutyStr, inputString+13, 3);
            memcpy(&PhaseStr, inputString+17, 3);
            
            
            //Interprets the command
            if (!strcmp(strCommand, "STOP")){
                //Disables signal generation
                sigGenEnabled=0;
                ResetCounters();
                outputString="OK.\n";                  
            }else if(!strcmp(strCommand, "INIT")){
                //Re-enables signal generation
                sigGenEnabled=1;
                ResetCounters();
                outputString="OK.\n";    
            }else if(!strcmp(strCommand, "ZERO")){
                //Makes all duty cycles zero (effectively turning all ports off)
                sigGenEnabled=1;
                ZeroSigGen();                
                outputString="OK.\n";   
            }else if(!strcmp(strCommand, "CONF")){
                //Changes configuration of a channel
                int chVal;
                long freqVal;
                long dutyVal;
                long phaseVal;
                chVal=atoi(ChanStr);
                freqVal=atol(FreqStr);
                dutyVal=atol(DutyStr);
                phaseVal=atol(PhaseStr);
                
                //Makes sure values are valid
                bool InvalidFrequency;
                bool InvalidDuty;
                bool InvalidPhase;
                bool InvalidChannel;
                InvalidFrequency=0;
                InvalidPhase=0;
                InvalidDuty=0;
                InvalidChannel=0;
                if (freqVal<=0){
                    InvalidFrequency=1;
                }else if(freqVal>200){
                    InvalidFrequency=1;
                }       
                if (dutyVal<0){
                    InvalidDuty=1;
                }else if(dutyVal>100){
                    InvalidDuty=1;
                }                
                if (phaseVal<0){
                    InvalidPhase=1;
                }else if(phaseVal>360){
                    InvalidPhase=1;
                }                
                if (chVal<0){
                    InvalidChannel=1;
                }else if(chVal>17){
                    InvalidChannel=1;
                }
                
                //Changes frequency if input is valid
                if (InvalidFrequency){                    
                    outputString="INVALID FREQUENCY! MUST BE A 3-DIGIT VALUE BETWEEN 0 AND 200.\n"; 
                }else if(InvalidDuty){
                    outputString="INVALID DUTY CYCLE! MUST BE A 3-DIGIT VALUE BETWEEN 0 AND 100.\n"; 
                }else if(InvalidPhase){
                    outputString="INVALID PHASE ANGLE! MUST BE A 3-DIGIT VALUE BETWEEN 0 AND 360.\n"; 
                }else if(InvalidChannel){
                    outputString="INVALID CHANNEL! MUST BE A 2-DIGIT VALUE BETWEEN 0 AND 17.\n"; 
                }else{
                    float nON;
                    float nOFF;
                    float nDELTA;
                    long nSamplesON;  
                    long nSamplesOFF;  
                    long nSamplesDELTA;  
                    
                    nOFF=fSampling/((float)freqVal); //full cycle period
                    nSamplesOFF=(long)(nOFF+0.5);
                    
                    nON=nOFF*(((100.-(float)dutyVal))/100.); //on time 
                    nSamplesON=(long)(nON+0.5);
                    
                    nDELTA=nOFF*(((float)phaseVal)/360.); //shift time
                    nSamplesDELTA=(long)(nDELTA+0.5);
                    
                    
                    if (!strcmp(PortStr, "A")){
                        PA_OFF[chVal]=nSamplesOFF;
                        PA_ON[chVal]=nSamplesON;
                        PA_COUNTER_DEFAULT[chVal]=nSamplesDELTA;
                        outputString="CHG CONF PORT A\n"; 
                    }else if(!strcmp(PortStr, "B")){
                        PB_OFF[chVal]=nSamplesOFF;
                        PB_ON[chVal]=nSamplesON;
                        PB_COUNTER_DEFAULT[chVal]=nSamplesDELTA;
                        outputString="CHG CONF PORT B\n"; 
                    }else if(!strcmp(PortStr, "C")){
                        PC_OFF[chVal]=nSamplesOFF;
                        PC_ON[chVal]=nSamplesON;
                        PC_COUNTER_DEFAULT[chVal]=nSamplesDELTA;
                        outputString="CHG CONF PORT C\n"; 
                    }else if(!strcmp(PortStr, "D")){
                        PD_OFF[chVal]=nSamplesOFF;
                        PD_ON[chVal]=nSamplesON;
                        PD_COUNTER_DEFAULT[chVal]=nSamplesDELTA;
                        outputString="CHG CONF PORT D\n"; 
                    }else if(!strcmp(PortStr, "E")){
                        PE_OFF[chVal]=nSamplesOFF;
                        PE_ON[chVal]=nSamplesON;
                        PE_COUNTER_DEFAULT[chVal]=nSamplesDELTA;
                        outputString="CHG CONF PORT E\n"; 
                    }else if(!strcmp(PortStr, "F")){
                        PF_OFF[chVal]=nSamplesOFF;
                        PF_ON[chVal]=nSamplesON;
                        PF_COUNTER_DEFAULT[chVal]=nSamplesDELTA;
                        outputString="CHG CONF PORT F\n"; 
                    }else{
                        outputString="ERROR! INVALID PORT #! MUST BE BETWEEN A AND F.\n";  
                    }
                }
                      
            }else{                
                outputString="UNKNOWN COMMAND!\n";        
            }
            
            
            appData.state = APP_STATE_TX; 
            txByte=0; //Resets TX byte counter
            rxByte=0; //Resets RX byte counter
            
            //Clears the input buffer
            int k;
            for (k=0;k<rxBufferLimit;k++){
                inputString[k]=0;
            }
                       
            break;
        }
        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
    
    
}

void __ISR_AT_VECTOR (_TIMER_2_VECTOR, IPL4SRS) T2Interrupt(void)
{
    
    if(sigGenEnabled){
        //Updates counters
        int i;
        for (i=0;i<18;i++){
            PA_COUNTER[i]++;
            PB_COUNTER[i]++;
            PC_COUNTER[i]++;
            PD_COUNTER[i]++;
            PE_COUNTER[i]++;
            PF_COUNTER[i]++;
        }



        for (i=0;i<18;i++){
            //UPDATE PORT A IN BOARD
            if(PA_COUNTER[i]>PA_OFF[i]){ 
                PORT_SHADOWREGISTERS[PA_PORT[i]]&=(~(1<<PA_PIN[i])); //Atomic operation to clear the bit       

                PA_COUNTER[i]=0;
            }else if(PA_COUNTER[i]>PA_ON[i]){ 
                PORT_SHADOWREGISTERS[PA_PORT[i]]|=(1<<PA_PIN[i]); //Atomic operation to set the bit
            }

            //UPDATE PORT B IN BOARD
            if(PB_COUNTER[i]>PB_OFF[i]){ 
                PORT_SHADOWREGISTERS[PB_PORT[i]]&=(~(1<<PB_PIN[i])); //Atomic operation to clear the bit

                PB_COUNTER[i]=0;
            }else if(PB_COUNTER[i]>PB_ON[i]){ 
                PORT_SHADOWREGISTERS[PB_PORT[i]]|=(1<<PB_PIN[i]); //Atomic operation to set the bit
            }

            //UPDATE PORT C IN BOARD
            if(PC_COUNTER[i]>PC_OFF[i]){ 
                PORT_SHADOWREGISTERS[PC_PORT[i]]&=(~(1<<PC_PIN[i])); //Atomic operation to clear the bit

                PC_COUNTER[i]=0;
            }else if(PC_COUNTER[i]>PC_ON[i]){ 
                PORT_SHADOWREGISTERS[PC_PORT[i]]|=(1<<PC_PIN[i]); //Atomic operation to set the bit
            }

            //UPDATE PORT D IN BOARD
            if(PD_COUNTER[i]>PD_OFF[i]){ 
                PORT_SHADOWREGISTERS[PD_PORT[i]]&=(~(1<<PD_PIN[i])); //Atomic operation to clear the bit

                PD_COUNTER[i]=0;
            }else if(PD_COUNTER[i]>PD_ON[i]){ 
                PORT_SHADOWREGISTERS[PD_PORT[i]]|=(1<<PD_PIN[i]); //Atomic operation to set the bit
            }

            //UPDATE PORT E IN BOARD
            if(PE_COUNTER[i]>PE_OFF[i]){ 
                PORT_SHADOWREGISTERS[PE_PORT[i]]&=(~(1<<PE_PIN[i])); //Atomic operation to clear the bit

                PE_COUNTER[i]=0;
            }else if(PE_COUNTER[i]>PE_ON[i]){ 
                PORT_SHADOWREGISTERS[PE_PORT[i]]|=(1<<PE_PIN[i]); //Atomic operation to set the bit
            }

            //UPDATE PORT F IN BOARD
            if(PF_COUNTER[i]>PF_OFF[i]){ 
                PORT_SHADOWREGISTERS[PF_PORT[i]]&=(~(1<<PF_PIN[i])); //Atomic operation to clear the bit

                PF_COUNTER[i]=0;
            }else if(PF_COUNTER[i]>PF_ON[i]){ 
                PORT_SHADOWREGISTERS[PF_PORT[i]]|=(1<<PF_PIN[i]); //Atomic operation to set the bit
            }
        }

        //PORTB has UART interface, don't change those bits (NOT NEEDED, HERE JUST IN CASE)
        //unsigned int TEMP1; //Temporary registers
        //unsigned int TEMP2; 
        //TEMP1=LATB & BITMASK_PORTB;
        //TEMP2=PORT_SHADOWREGISTERS[1] & ~BITMASK_PORTB;
        //PORT_SHADOWREGISTERS[1]=TEMP1 | TEMP2;

        //Transfers shadow registers to ports
        LATA=PORT_SHADOWREGISTERS[0];
        LATB=PORT_SHADOWREGISTERS[1];
        LATC=PORT_SHADOWREGISTERS[2];
        LATD=PORT_SHADOWREGISTERS[3];
        LATE=PORT_SHADOWREGISTERS[4];
        LATF=PORT_SHADOWREGISTERS[5];
        LATG=PORT_SHADOWREGISTERS[6];
        LATH=PORT_SHADOWREGISTERS[7];
        LATJ=PORT_SHADOWREGISTERS[8]; 
        LATK=PORT_SHADOWREGISTERS[9];   


        // Reset interrupt flag
    }
    IFS0bits.T2IF = 0;
}