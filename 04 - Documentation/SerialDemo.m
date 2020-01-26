%This code demonstrates the capabilities of the microactuator array driver,
%by updating the 108 channels of the driver with different frequencies. 

%© Fernando Zigunov, 2019.

%%
clear; clc; close all;

%%
%Initializes the serial port parameters
s = serial('COM10','BaudRate',57600,'DataBits',8,'StopBits',1,'Parity','none','Terminator','LF'); %Change COM port to whichever port 
ports={'A','B','C','D','E','F'};

fopen(s); %Opens the serial port
SendCommand(s,'ZERO'); %Sends a zero command to reset all channels' settings.
SendCommand(s,'STOP'); %Sends a stop command to stop all channels.

freq=2;
duty=50;
phase=0;
for p=1:6
    currPort=ports{p};
    for c=0:17
        cStr=num2str(c,'%02.f');
        fStr=num2str(freq,'%03.f');
        dStr=num2str(duty,'%03.f');
        pStr=num2str(phase,'%03.f');
        
        completeString=['CONF ',currPort, cStr,' ',fStr,' ',dStr,' ',pStr]; %Formats the string in the correct size
        
        SendCommand(s,completeString);
        freq=freq+1; %Increments the frequency (each channel will have a different frequency for demo purposes)
    end    
end

SendCommand(s,'INIT'); %Sends a init command to run all channels back again.
fclose(s); %Closes the serial port (otherwise Matlab will take over the serial port)




function SendCommand(serialPort, command)
    %Sends a command to the serial port. Assumes it is already open. Waits
    %for 'OK' or 'CHG' response from uC. 
    fprintf(serialPort,command);
    
    
    %Response takes time to be interpreted by Matlab, so for fast applications the commented
    %out code will be slow (2s per 108 channels). If not executing, the
    %refresh time is 200ms for all channels.
%     r=fscanf(serialPort);
%     
%     result=strcmpi(r(1:2),'OK');    
%     result=result | strcmpi(r(1:3),'CHG');
%     
%     if ~result
%         error(r)
%     end
end
