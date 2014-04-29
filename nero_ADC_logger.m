%% Temperature sensor liveplotting & logging.
%
% Read sensor data from the M2 microcontroller
% Plots the data realtime in a subplot.
% Logged into ttvals[time, temp1, temp2, temp3, temp4]
% Hit Ctrl-C to quit the program
%
% By Nick McGill [nmcgill.com]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Initialize program and USB port
% % close any existing open port connections
% % % for the first time running, comment this.
if(exist('M2USB'))
    fclose(M2USB);
else
    fclose(instrfindall);
end

clear all
close all

%----> for ***WINDOZE***
% M2USB = serial('COM4','Baudrate', 9600); %where port is either 'COM#' in Windows or '/dev/tty.usbmodem#' in OS X.
%----> for ***MAC***
M2USB = serial('/dev/tty.usbmodem411','Baudrate',9600);

fopen(M2USB);
flushinput(M2USB);

%% Send initial packet to get first set of data from microcontroller
fwrite(M2USB,1);
time = 0;
i=0;
tic

maxPoints = 20; % max number of data points displayed at a time
ADC1pos = zeros(maxPoints,1);%1-100
ADC2pos = zeros(maxPoints,1);%1-100
ADC3pos = zeros(maxPoints,1);%1-100
ADC4pos = zeros(maxPoints,1);%1-100
ADC5pos = zeros(maxPoints,1);%1-100
ADC6pos = zeros(maxPoints,1);%1-100
ADC7pos = zeros(maxPoints,1);%1-100
ADC8pos = zeros(maxPoints,1);%1-100
t = 1:1:maxPoints;

ADCData = zeros(1,7);
hallEffectData = zeros(1,1);
ttvals = zeros(1,8);


%Run program for specified amount of time
try
while 1
    
    %% Read in the data and send a confirmation packet
    a2 = fgetl(M2USB);
    fwrite(M2USB,1); 

    
    
    %% Parse the data sent by the microcontroller
	% Expecting data in the form: [uint ADC1, uint ADC2, uint ADC3, uint ADC4]
    ADC1 = hex2dec(a2(1:4))
    
    
%     b0_switch = hex2dec(a2(29:32));
%     
%     if(b0_switch == 0)
%         disp('poo');
%     end
   
    %% Retieve and store data in array at next index
    % ADC values for each ADC input
    
%     ADCData(i,1) = ADC1;
%     ADCData(i,2) = ADC2;
%     ADCData(i,3) = ADC3;
%     ADCData(i,4) = ADC4;
%     ADCData(i,5) = ADC5;
%     ADCData(i,6) = ADC6;
%     ADCData(i,7) = ADC7;
%     

    hallEffectData = [hallEffectData; ADC1];
     

    ADC1pos = circshift(ADC1pos,-1);%film...
     ADC1pos(maxPoints,:) = ADC1;

    i=i+1;
    

    
    %% plotting
    figure(1);
    clf
    hold on
    grid on
    
    plot(t,ADC1pos(:,1),':or');
    title('Hall Effect (f0)');
    axis([0 maxPoints 0 1023]);
    
    grid on
    pause(.01);
    hold off

    
    time = toc;
end 
catch
    %Close serial object
    fclose(M2USB);
end