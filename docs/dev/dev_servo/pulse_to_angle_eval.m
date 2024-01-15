clc, clear variables
%%
pulse = ["YOUR DATA POINTS PULSE"];
angle = ["YOUR DATA POINTS ANGLE"];

figure(1)
grid on
plot(pulse, angle, '-*r');
axis("tight");
title('Pulse to angle plot')
xlabel('Pulse width [-]')
ylabel('Angle [°]')
