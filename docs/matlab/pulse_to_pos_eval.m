clc, clear variables
%% parameters for mapping 

% Servo 1 - Futaba S3001
% Servo 2 - Reely S-0090

% Increment of pulse width
increment = 0.0025;

% parameters for servo 1
servo1_pulse_start = 0.0150;
servo1_pulse_end = 0.1150;

servo1_pulse_number = (servo1_pulse_end - servo1_pulse_start) / increment;

% parameters for servo 2
servo2_pulse_start = 0.0325;
servo2_pulse_end = 0.1200;

servo2_pulse_number = (servo2_pulse_end - servo2_pulse_start) / increment;

% servos data points calculation
pulse1 = zeros(1, servo1_pulse_number);
pulse1(1) = servo1_pulse_start;

pulse2 = zeros(1, servo2_pulse_number);
pulse2(1) = servo2_pulse_start;

for i = 2:servo1_pulse_number + 1
    pulse1(i) = pulse1(i-1) + increment;
end

for i = 2:servo2_pulse_number + 1
    pulse2(i) = pulse2(i-1) + increment; 
end

% pulse width is mapped to position from range 0 to 1.
% there is a linear relationship between these values.

% calculation of parameters of linear functions
a1 = 1/(pulse1(end) - pulse1(1));
b1 = - pulse1(1) * 1 / (pulse1(end) - pulse1(1));
 
a2 = 1/(pulse2(end) - pulse2(1));
b2 = - pulse2(1) * 1 / (pulse2(end) - pulse2(1));

position1 = a1.*pulse1 + b1;
position2 = a2.*pulse2 + b2; 

% pulse to position plot creation
figure(1)
plot(pulse1, position1, '-*r'), grid on, hold on
plot(pulse2, position2, '-*b'), hold off
ylim([0, 1])
xlabel('Position before Calibration')
ylabel('Position after Calibration')
legend('Servo1', 'Servo2', 'location', 'best')