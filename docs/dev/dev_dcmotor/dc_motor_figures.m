clc, clear variables

% https://www.pololu.com/category/213/12v-carbon-brush-cb-20d-gearmotors

u_max = 12;
gearratio = 78;
switch gearratio
    case 78
        % https://www.pololu.com/product/3489
        % gear length: 17.5 mm -> gear ratio 63:1 or 78.125:1
        % measured gear ratio: 77.4 = 4644/3/(20) -> gear ratio 78.125
        w_max_rpm = 180;
        M_max_oz = 75;
    case 100
        % https://www.pololu.com/product/3490
        w_max_rpm = 140;
        M_max_oz = 85;
    case 195
        % https://www.pololu.com/product/3493
        w_max_rpm = 72;
        M_max_oz = 140;
    otherwise
end

kn = w_max_rpm/60*2*pi / u_max;
k_ozin2Nm = 0.0070615518333333;

w_max = kn*u_max;
M_max = M_max_oz * k_ozin2Nm;
i_max = M_max*kn;

a = -M_max/w_max;
b = M_max;

N = 1001;
w = (-w_max:2*w_max/(N-1):w_max).';
M(1:(N-1)/2,1) = a*w(1:(N-1)/2) - b;
M((N-1)/2:N,1) = a*w((N-1)/2:N) + b;

i = (-i_max:2*i_max/(N-1):i_max).';
u = (-u_max:2*u_max/(N-1):u_max).';

figure(1)
subplot(221)
plot(i, i/kn, 'r.'), grid on
axis([0 max(i) 0 max(i/kn)])
xlabel('Current (A)')
ylabel('Torque (Nm)')
set(gca,'FontSize',11)
subplot(222)
plot(u, u*kn/2/pi, 'b.'), grid on
axis([0 max(u) 0 max(u*kn/2/pi)])
xlabel('Voltage (V)')
ylabel('Speed (rps)')
set(gca,'FontSize',11)
subplot(223)
plot(w/2/pi, M, 'k.'), grid on
axis([0 max(w/2/pi) 0 max(M)])
xlabel('Speed (rps)')
ylabel('Torque (Nm)')
set(gca,'FontSize',11)
subplot(224)
plot(w/2/pi, M.*w, '.', 'color', [0 0.5 0]), grid on
axis([0 max(w/2/pi) 0 max(M.*w)])
xlabel('Speed (rps)')
ylabel('Power (W)')
set(gca,'FontSize',11)

% %% this was before motion was introduced
% 
% clc, clear all
% 
% data = readmatrix('putty_02.log');
% 
% Ts = 5e-3;
% N = size(data, 1);
% time = (0:N-1).'*Ts;
% 
% ind = time > 4 & time < 10;
% time = time(ind); time = time - time(1);
% data = data(ind,:);
% 
% fs = 11;
% 
% figure(1)
% stairs(time, data(:,1), 'color', [0 0.5 0], 'linewidt', 2), grid on
% title('Speed-Cntrl', 'Fontsize', fs)
% ylabel('Velocity (RPS)', 'Fontsize', fs)
% xlabel('Time (sec)', 'Fontsize', fs)
% xlim([0 max(time)])
% 
% figure(2)
% ax(1) = subplot(211);
% stairs(time, data(:,3), 'r', 'linewidt', 2), grid on
% title('Position-Cntrl', 'Fontsize', fs)
% ylabel('Position (R)', 'Fontsize', fs)
% ax(2) = subplot(212);
% stairs(time, data(:,2), 'r', 'linewidt', 2), grid on
% ylabel('Velocity (RPS)', 'Fontsize', fs)
% xlabel('Time (sec)', 'Fontsize', fs)
% linkaxes(ax, 'x'), clear ax
% xlim([0 max(time)])
% 
% %% this is after motion is introduced
% 
% clc, clear all
% 
% data = readmatrix('putty_10.log'); % no acc limitation active, no vel. feedforward
% data = readmatrix('putty_11.log'); % default acc limitation active (40%), no vel. feedforward
% data = readmatrix('putty_12.log'); % default acc limitation active (40%) -> overshoot
% % data = readmatrix('putty_13.log'); % acc limitation active (10%), no vel. feedforward
% 
% Ts = 0.01;
% N = size(data, 1);
% time = (0:N-1).'*Ts;
% 
% % ind = time > 0.5 & time < 7;
% % time = time(ind); time = time - time(1);
% % data = data(ind,:);
% 
% fs = 11;
% 
% figure(3)
% ax(1) = subplot(211);
% stairs(time, data(:,1), 'color', [0 0.5 0], 'linewidt', 2), grid on
% title('Speed-Cntrl', 'Fontsize', fs)
% ylabel('Velocity (RPS)', 'Fontsize', fs)
% ax(2) = subplot(212);
% stairs(time, [0; diff(data(:,1))/Ts], 'r', 'linewidt', 2), grid on
% ylabel('Acceleration (RPS/sec)', 'Fontsize', fs)
% xlabel('Time (sec)', 'Fontsize', fs)
% linkaxes(ax, 'x'), clear ax
% xlim([0 max(time)])
% 
% figure(4)
% ax(1) = subplot(311);
% stairs(time, data(:,2), 'b', 'linewidt', 2), grid on
% title('Position-Cntrl', 'Fontsize', fs)
% ylabel('Position (R)', 'Fontsize', fs)
% ax(2) = subplot(312);
% stairs(time, data(:,3), 'color', [0 0.5 0], 'linewidt', 2), grid on
% ylabel('Velocity (RPS)', 'Fontsize', fs)
% ax(2) = subplot(313);
% stairs(time, [0; diff(data(:,3))/Ts], 'r', 'linewidt', 2), grid on
% ylabel('Acceleration (RPS/sec)', 'Fontsize', fs)
% xlabel('Time (sec)', 'Fontsize', fs)
% linkaxes(ax, 'x'), clear ax
% xlim([0 max(time)])
% 
% figure(5)
% ax(1) = subplot(311);
% stairs(time, data(:,4), 'b', 'linewidt', 2), grid on
% title('Position-Cntrl', 'Fontsize', fs)
% ylabel('Position (R)', 'Fontsize', fs)
% ax(2) = subplot(312);
% stairs(time, data(:,5), 'color', [0 0.5 0], 'linewidt', 2), grid on
% ylabel('Velocity (RPS)', 'Fontsize', fs)
% ax(2) = subplot(313);
% stairs(time, [0; diff(data(:,5))/Ts], 'r', 'linewidt', 2), grid on
% ylabel('Acceleration (RPS/sec)', 'Fontsize', fs)
% xlabel('Time (sec)', 'Fontsize', fs)
% linkaxes(ax, 'x'), clear ax
% xlim([0 max(time)])
% 
% %% ROME2 robot tuning
% 
% clc, clear all
% 
% % data = readmatrix('putty_14.log'); % default kp = 0.1, default acc limitation active (40%)
% % data = readmatrix('putty_15.log'); % kp = 0.05, default acc limitation active (40%)
% % data = readmatrix('putty_16.log'); % kp = 0.02, default acc limitation active (40%)
% % data = readmatrix('putty_17.log'); % kp = 0.02, acc limitaion off
% data = readmatrix('putty_18.log'); % kp = 0.02, acc limitaion at 10 RPS/sec
% 
% Ts = 0.01;
% N = size(data, 1);
% time = (0:N-1).'*Ts;
% 
% % ind = time > 0.5 & time < 7;
% % time = time(ind); time = time - time(1);
% % data = data(ind,:);
% 
% fs = 11;
% 
% figure(2)
% ax(1) = subplot(211);
% stairs(time, data(:,1:2), 'color', [0 0.5 0], 'linewidt', 2), grid on
% title('Speed-Cntrl', 'Fontsize', fs)
% ylabel('Velocity (RPS)', 'Fontsize', fs)
% ax(2) = subplot(212);
% stairs(time, [zeros(1,2); diff(data(:,1:2))/Ts], 'r', 'linewidt', 2), grid on
% ylabel('Acceleration (RPS/sec)', 'Fontsize', fs)
% xlabel('Time (sec)', 'Fontsize', fs)
% linkaxes(ax, 'x'), clear ax
% xlim([0 max(time)])
