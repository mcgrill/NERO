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

offset = 12000;
maxY = 500;
maxPoints = 20; % max number of data points displayed at a time
ADC1pos = zeros(maxPoints,1);%1-100
ADC2pos = zeros(maxPoints,1);%1-100
ADC3pos = zeros(maxPoints,1);%1-100

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
    ADC1 = hex2dec(a2(1:4))-offset;
    ADC2 = hex2dec(a2(5:8))-offset;
    ADC3 = hex2dec(a2(9:12))-offset;
    
    ADC1pos = circshift(ADC1pos,-1);%film...
     ADC1pos(maxPoints,:) = ADC1;
    ADC2pos = circshift(ADC2pos,-1);%film...
     ADC2pos(maxPoints,:) = ADC2;
    ADC3pos = circshift(ADC3pos,-1);%film...
     ADC3pos(maxPoints,:) = ADC3;
     
    i=i+1;
    

    
    %% plotting
    figure(1);
    clf
    hold on
    grid on
    
    subplot(2,2,1);
    plot(t,ADC1pos(:,1),':or');
    title('Ch3');
    axis([0 maxPoints 0 maxY]);
    
    subplot(2,2,2);
    plot(t,ADC2pos(:,1),':or');
    title('Ch4');
    axis([0 maxPoints 0 maxY]);
    
    subplot(2,2,3);
    plot(t,ADC3pos(:,1),':or');
    title('Ch5');
    axis([0 maxPoints 0 maxY]);
    
    grid on
    pause(.01);
    hold off

    
    time = toc;
end 
catch
    %Close serial object
    fclose(M2USB);
end