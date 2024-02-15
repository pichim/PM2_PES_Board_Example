%% basic testing and debugging

clc, clear variables

% const float gear_ratio = 78.125;                  // 78.125:1 gear box
% const float voltage_max = 12.0f;                  // define maximum voltage of battery packs, adjust this to 6.0f V if you only use one batterypack
% const float counts_per_turn = 20.0f * gear_ratio; // define counts per turn at gearbox end: counts/turn * gearratio
% const float kn = 180.0f / 12.0f;                  // define motor constant in RPM / V

gear_ratio = 78.125;
voltage_max = 12.0;
counts_per_turn = 20.0 * gear_ratio;
kn = 180.0 / 12.0;

m_counts_per_turn = counts_per_turn
m_kn = kn / 60.0
m_voltage_max = voltage_max
m_velocity_max = m_kn * voltage_max

% static constexpr float PWM_MIN = 0.01f;
% static constexpr float PWM_MAX = 0.99f;
% static float voltage_min = m_voltage_max * (2.0f * PWM_MIN - 1.0f);
% static float voltage_max = m_voltage_max * (2.0f * PWM_MAX - 1.0f);
PWM_MIN = 0.01;
PWM_MAX = 0.99;
voltage_min_ = m_voltage_max * (2.0 * PWM_MIN - 1.0)
voltage_max_ = m_voltage_max * (2.0 * PWM_MAX - 1.0)
PWM_MIN_ = 0.5 + 0.5 * voltage_min_ / m_voltage_max
PWM_MAX_ = 0.5 + 0.5 * voltage_max_ / m_voltage_max

% m_counts_per_turn: 1562.500000
% m_kn: 0.250000
% m_voltage_max: 12.000000
% m_velocity_max: 2.400000
% voltage_min: -11.760000
% voltage_max: 11.760000
% PWM_MIN_: 0.010000
% PWM_MAX_: 0.990000

%%

clc, clear variables

% const float gear_ratio_M1 = 31.25f;                     // 31.25:1 gear box
% const float counts_per_turn_M1 = 20.0f * gear_ratio_M1; // counts per turn at gear box end
% const float kn_M1 = 450.0f / 12.0f;                     // motor constant in RPM / V
% 
% const float gear_ratio_M2 = 488.28125f;                 // 488.28125:1 gear box
% const float counts_per_turn_M2 = 20.0f * gear_ratio_M2; // counts per turn at gear box end
% const float kn_M2 = 28.0f / 12.0f;                      // motor constant in RPM / V
% 
% const float gear_ratio_M3 = 78.125f;                     // 78.125:1 gear box
% const float counts_per_turn_M3 = 20.0f * gear_ratio_M3; // counts per turn at gear box end
% const float kn_M3 = 180.0f / 12.0f;                     // motor constant in RPM / V

gear_ratio_M1 = 31.25
counts_per_turn_M1 = 20.0 * gear_ratio_M1;
kn_M1 = 450.0 / 12.0;

gear_ratio_M2 = 488.28125;
counts_per_turn_M2 = 20.0 * gear_ratio_M2;
kn_M2 = 28.0 / 12.0;

gear_ratio_M3 = 78.125;
counts_per_turn_M3 = 20.0 * gear_ratio_M3;
kn_M3 = 180.0 / 12.0;

kn_M1 / gear_ratio_M1 * gear_ratio_M1
kn_M2 / gear_ratio_M1 * gear_ratio_M2
kn_M3 / gear_ratio_M1 * gear_ratio_M3

kn_M1 / gear_ratio_M2 * gear_ratio_M1
kn_M2 / gear_ratio_M2 * gear_ratio_M2
kn_M3 / gear_ratio_M2 * gear_ratio_M3 

kn_M1 / gear_ratio_M3 * gear_ratio_M1
kn_M2 / gear_ratio_M3 * gear_ratio_M2
kn_M3 / gear_ratio_M3 * gear_ratio_M3 


%% tunig pid controller, eval step responses

clc, clear variables

% data = readmatrix('putty_00.log'); % 78:1, vel, p-d-f
% data = readmatrix('putty_01.log'); % 78:1, vel, p-i-d-f
% data = readmatrix('putty_03.log'); % 78:1, pos, p-i-d-f, p
% data = readmatrix('putty_04.log'); % 78:1, pos, p-i-d-f, p, acc = 5.0
% data = readmatrix('putty_05.log'); % 78:1, pos, p-i-d-f, p, acc = 5.0, ki = 0

% data = readmatrix('putty_06.log'); % 78:1, pos, p-i-d-f, p

% data = readmatrix('putty_07.log'); % 31:1, vel, p-i-d-f
% data = readmatrix('putty_08.log'); % 31:1, pos, p-i-d-f, p
% data = readmatrix('putty_09.log'); % 31:1, pos, p-i-d-f, p, acc = 5.0

% data = readmatrix('putty_10.log'); % 488:1, vel, p-i-d-f
% data = readmatrix('putty_11.log'); % 488:1, pos, p-i-d-f, p
% data = readmatrix('putty_12.log'); % 488:1, pos, p-i-d-f, p, acc = 5.0

% data = readmatrix('putty_13.log'); % 31:1, pos, p-i-d-f, p, acc = 5.0
% data = readmatrix('putty_14.log'); % 488:1, pos, p-i-d-f, p, acc = 5.0
% data = readmatrix('putty_15.log'); % 78:1, pos, p-i-d-f, p, acc = 5.0

% data = readmatrix('putty_16.log'); % 78:1, pos, p-i-d-f, p, acc = 5.0
% data = readmatrix('putty_17.log'); % 78:1, pos, p-i-d-f, p, acc = 5.0

% data = readmatrix('putty_18.log'); % 78:1, pos, p-i-d-f, p, acc = 5.0
% data = readmatrix('putty_19.log'); % 488:1, pos, p-i-d-f, p, acc = 5.0
% data = readmatrix('putty_20.log'); % 31:1, pos, p-i-d-f, p, acc = 5.0

% data = readmatrix('putty_21.log'); % 31:1, pos, p-i-d-f, p, acc = 5.0
% data = readmatrix('putty_22.log'); % 31:1, pos, p-i-d-f, p

% data = readmatrix('putty_23.log'); % 31:1, pos, p-i-d-f, p
% data = readmatrix('putty_24.log'); % 488:1, pos, p-i-d-f, p
% data = readmatrix('putty_25.log'); % 488:1, pos, p-i-d-f, p

% data = readmatrix('putty_26.log'); % 31:1, pos, p-i-d-f, p
% data = readmatrix('putty_27.log'); % 488:1, pos, p-i-d-f, p
% data = readmatrix('putty_28.log'); % 78:1, pos, p-i-d-f, p

% data = readmatrix('putty_29.log'); % 31:1, pos, p-i-d-f, p, ff
% data = readmatrix('putty_30.log'); % 488:1, pos, p-i-d-f, p, ff
% data = readmatrix('putty_31.log'); % 78:1, pos, p-i-d-f, p, ff

% data = readmatrix('putty_32.log'); % 78:1, pos, p-i-d-f, p, ff, motion planner disabled
% data = readmatrix('putty_33.log'); % 78:1, pos, p-i-d-f, p, ff, motion planner enabled

% data = readmatrix('putty_34.log'); % 78:1, pos, p-i-d-f, p, ff, motion planner enabled
% data = readmatrix('putty_35.log'); % 78:1, pos, p-i-d-f, p, ff, motion planner enabled
% data = readmatrix('putty_36.log'); % 78:1, pos, p-i-d-f, p, ff, motion planner enabled

% data = readmatrix('putty_37.log'); % 78:1, pos, p-i-d-f, p, ff, motion planner enabled
% data = readmatrix('putty_38.log'); % 31:1, pos, p-i-d-f, p, ff, motion planner enabled
% data = readmatrix('putty_39.log'); % 488:1, pos, p-i-d-f, p, ff, motion planner enabled

% data = readmatrix('putty_40.log'); % 31:1
% data = readmatrix('putty_41.log'); % 488:1
data = readmatrix('putty_42.log'); % 78:1

time = data(:,1) * 1e-3;
time = time - time(1);
data = data(:,2:end);
median(diff(time))

robot_state = data(:,1);
data = data(:,2:end);

% figure(1)
% stairs(time, robot_state), grid on

figure(2)
ax(1) = subplot(221);
% plot(time, [data(:,1:3), data(:,2)-data(:,3)]), grid on
plot(time - 1, [data(:,1:3)]), grid on
ylabel('Rotation'), xlabel('Time (sec)')
ax(2) = subplot(223);
plot(time - 1, data(:,4:6)), grid on
ylabel('Velocity (RPS)'), xlabel('Time (sec)')
ax(3) = subplot(222);
% plot(time(1:end-1), diff(data(:,4:6)) ./ diff(time)), grid on
plot(time(1:end-1) - 1, diff(data(:,4:6)) ./ diff(time)), grid on
ylabel('Acceleration (RPS/sec)'), xlabel('Time (sec)')
ax(4) = subplot(224);
% plot(time, data(:,7)), grid on
plot(time - 1, data(:,7)), grid on
ylabel('Voltage (V)'), xlabel('Time (sec)')
linkaxes(ax, 'x'), clear ax
xlim([0 time(end)])
% xlim([0.0 12.0])

figure(3)
plot(time, data(:,8)), grid on
ylabel('PWM'), xlabel('Time (sec)')
xlim([0 time(end)])


%%

clc, clear variables

voltage_max = 12.0;

% // https://www.pololu.com/product/3475/specs
gear_ratio_M1 = 31.25;                        %// 31.25:1 gear box
kn_M1 = 450.0 / 12.0;                         %// motor constant in RPM / V
velocity_max_M1 = kn_M1 / 60.0 * voltage_max  %// maximum velocity in rotations per second
velocity_max_M1 * 2/5
velocity_max_M1 * 2/5 * 2/3
    % 7.5
    % 3.0
    % 2.0

% // https://www.pololu.com/product/3485/specs
gear_ratio_M2 = 488.28125;                   %// 488.28125:1 gear box
kn_M2 = 28.0 / 12.0;                         %// motor constant in RPM / V
velocity_max_M2 = kn_M2 / 60.0 * voltage_max %// maximum velocity in rotations per second
velocity_max_M2 * 2/5
velocity_max_M2 * 2/5 * 2/3
    % 0.4667
    % 0.1867
    % 0.1244

% // https://www.pololu.com/product/3477/specs
gear_ratio_M3 = 78.125;                      %// 78.125:1 gear box
kn_M3 = 180.0 / 12.0;                        %// motor constant in RPM / V
velocity_max_M3 = kn_M3 / 60.0 * voltage_max %// maximum velocity in rotations per second
velocity_max_M3 * 2/5
velocity_max_M3 * 2/5 * 2/3
    % 3
    % 1.2000
    % 0.8000

% kn_M1 / 60.0 * gear_ratio_M1 * 2 * pi
% kn_M2 / 60.0 * gear_ratio_M2 * 2 * pi
% kn_M3 / 60.0 * gear_ratio_M3 * 2 * pi

s = 390.6250
s / gear_ratio_M1
s / gear_ratio_M2
s / gear_ratio_M3



%%

clc, clear variables

% Ts = 1e-3;

% load gpa_data_02.mat % 78:1
% % 3.500000, 175.000000, 0.120000, 0.015915, 0.000354, 0.001000
% Kp = 3.5;
% Ki = 175.0;
% Kd = 0.12;

% load gpa_data_03.mat % 488:1
% % 21.875000, 1093.750000, 0.750000, 0.015915, 0.000354, 0.001000
% Kp = 21.875;
% Ki = 1093.75;
% Kd = 0.75;

% load gpa_data_04.mat % 31:1
% % load gpa_data_05.mat % 31:1
% % load gpa_data_06.mat % 31:1
% % load gpa_data_07.mat % 31:1
% % 1.400000, 70.000000, 0.048000, 0.015915, 0.000354, 0.001000
% Kp = 1.4;
% Ki = 70.0;
% Kd = 0.048;
% 
% tau_f = 1.0 / (2.0 * pi * 10.0);
% tau_ro = 1.0 / (2.0 * pi * 0.9 / (2.0 * Ts));

% load gpa_data_08.mat % 31:1
% % 2.100000, 70.000000, 0.009600, 0.005305, 0.000637, 0.001000
% Kp = 2.1;
% Ki = 70.0;
% Kd = 0.0096;
% 
% tau_f = 1.0 / (2.0 * pi * 30.0);
% tau_ro = 1.0 / (2.0 * pi * 0.5 / (2.0 * Ts));
% w0 = 2.0 * pi * 20.0;
% D = 1.0;
% K = 1;

Ts = 5e-4;

% load gpa_data_09.mat % 78:1
% load gpa_data_10.mat % 78:1
load gpa_data_11.mat % 78:1
% load gpa_data_12.mat % 78:1, no roll-off filter
Kp = 4.2;
Ki = 140.0;
Kd = 0.0192;

tau_f = 1.0 / (2.0 * pi * 30.0);
tau_ro = 1.0 / (2.0 * pi * 0.5 / (2.0 * Ts));

Cm = pid(Kp, Ki, Kd, tau_f, Ts, 'IFormula', 'BackwardEuler', ...
    'DFormula', 'Trapezoidal') * ...
    c2d(tf(1, [tau_ro 1]), Ts, 'tustin')
% Cm = pid(Kp, Ki, Kd, tau_f, Ts, 'IFormula', 'BackwardEuler', ...
    % 'DFormula', 'Trapezoidal')


w0 = 2.0 * pi * 15.0;
D = 1.0;
K = 1;

k0 = Ts * Ts * w0 * w0;
k1 = 4.0 * D * Ts * w0;
k2 = k0 + k1 + 4.0;
b0 = K * k0 / k2;
b1 = 2.0 * K * k0 / k2;
b2 = b0;
a1 = (2.0 * k0 - 8.0) / k2;
a2 = (k0 - k1 + 4.0) / k2;
Gfm = tf([b0 b1 b2], [1 a1 a2], Ts);


U = data(:,2) + 1i*data(:,3);
Y = data(:,4) + 1i*data(:,5);
R = data(:,6) + 1i*data(:,7);
P = frd(Y./U, data(:,1), Ts, 'Units', 'Hz');
T = frd(Y./R, data(:,1), Ts, 'Units', 'Hz');
S = 1 - T;
C = T/S/P;
SC = S*C;
SP = S*P;
freq = data(:,1);
spec = abs([U Y R]);


% Kp = 0.8 * Kp;
% Ki = 0.8 * Ki;
% Kd = 0.8 * Kd;

Kp = Kp;
Ki = Ki;
Kd = Kd;

tau_f = 1.0 / (2.0 * pi * 30.0);
tau_ro = 1.0 / (2.0 * pi * 0.5 / (2.0 * Ts));

w0 = 2.0 * pi * 15.0;
D = 1.0;
K = 1;

k0 = Ts * Ts * w0 * w0;
k1 = 4.0 * D * Ts * w0;
k2 = k0 + k1 + 4.0;
b0 = K * k0 / k2;
b1 = 2.0 * K * k0 / k2;
b2 = b0;
a1 = (2.0 * k0 - 8.0) / k2;
a2 = (k0 - k1 + 4.0) / k2;
Gfmn = tf([b0 b1 b2], [1 a1 a2], Ts);

Cmn = pid(Kp, Ki, Kd, tau_f, Ts, 'IFormula', 'BackwardEuler', ...
    'DFormula', 'Trapezoidal') * ...
    c2d(tf(1, [tau_ro 1]), Ts, 'tustin')
% Cmn = pid(Kp, Ki, Kd, tau_f, Ts, 'IFormula', 'BackwardEuler', ...
    % 'DFormula', 'Trapezoidal')
Sn = feedback(1, Cmn*P/Gfm*Gfmn);
Tn = 1 - Sn;

figure(4)
ax(1) = subplot(311);
semilogx(freq, spec(:,1), 'b.-'), grid on
title('Signal Spectras')
ax(2) = subplot(312);
semilogx(freq, spec(:,2), '.-', 'color', [0 0.5 0]), grid on
ax(3) = subplot(313);
semilogx(freq, spec(:,3), 'r.-'), grid on
linkaxes(ax, 'x'), clear ax
xlim([min(freq) 1/2/Ts])

opt = bodeoptions('cstprefs');

figure(5)
subplot(121)
bode(P, 'b.-', 2*pi*P.Frequency), grid on
title('Plant')
subplot(122)
opt.YLim = {[1e-5 3], [-180 180]};
bodemag(S, 'b.-', T, 'g.-', Sn, 'r.-', Tn, 'c.-', 2*pi*P.Frequency, opt), grid on
title('S, T')

figure(6)
bode(C, 'b.-', Cm, 'g.-', Cmn, 'r.-', 2*pi*P.Frequency), grid on
title('Controller')

Gstep = Tn;

[time, step_resp]   = get_step_resp_from_frd(Gstep, Ts, 100);

z = zero(pid(Kp, Ki, Kd, tau_f) * tf(1, [tau_ro 1]));
Gv = c2d(tf(1, [1 -z(2)]), Ts, 'tustin');
% Gv = c2d(tf(1, conv([1 -z(1)], [1 -z(2)])), Ts, 'tustin');
Gv = Gv / dcgain(Gv);
[~   , step_resp_v] = get_step_resp_from_frd(Gstep * Gv, Ts);

figure(7)
subplot(121)
bode(Gstep), grid on
subplot(122)
stairs(time, step_resp  ), grid on, hold on
stairs(time, step_resp_v), hold off
xlim([0 0.3])


%% ol-measurement with filter

clc, clear variables

Ts = 1e-3;

load gpa_data_00.mat


w0 = 2.0 * pi * 20.0;
D = 1.0;
K = 1;

k0 = Ts * Ts * w0 * w0;
k1 = 4.0 * D * Ts * w0;
k2 = k0 + k1 + 4.0;
b0 = K * k0 / k2;
b1 = 2.0 * K * k0 / k2;
b2 = b0;
a1 = (2.0 * k0 - 8.0) / k2;
a2 = (k0 - k1 + 4.0) / k2;
Gfm = tf([b0 b1 b2], [1 a1 a2], Ts);


U = data(:,2) + 1i*data(:,3);
Y = data(:,4) + 1i*data(:,5);
R = data(:,6) + 1i*data(:,7);
P1 = frd(Y./U, data(:,1), Ts, 'Units', 'Hz');
P2 = frd(Y./R, data(:,1), Ts, 'Units', 'Hz');
freq = data(:,1);
spec = abs([U Y R]);


figure(4)
ax(1) = subplot(311);
semilogx(freq, spec(:,1), 'b.-'), grid on
title('Signal Spectras')
ax(2) = subplot(312);
semilogx(freq, spec(:,2), '.-', 'color', [0 0.5 0]), grid on
ax(3) = subplot(313);
semilogx(freq, spec(:,3), 'r.-'), grid on
linkaxes(ax, 'x'), clear ax
xlim([min(freq) 1/2/Ts])

figure(5)
bode(P1, P2, Gfm, 2*pi*P1.Frequency), grid on
title('Filter')


%% ol-measurement with controller

clc, clear variables

Ts = 1e-3;

load gpa_data_01.mat
% 3.500000, 175.000000, 0.120000, 0.015915, 0.001592, 0.001000


Kp = 3.5;
Ki = 175.0;
Kd = 0.12;
tau_f = 1.0 / (2.0 * pi * 10.0);
tau_ro = 1.0 / (2.0 * pi * 100.0);

Cm = pid(Kp, Ki, Kd, tau_f, Ts, 'IFormula', 'BackwardEuler', 'DFormula', 'Trapezoidal') * c2d(tf(1, [tau_ro 1]), Ts, 'tustin')


w0 = 2.0 * pi * 20.0;
D = 1.0;
K = 1;

k0 = Ts * Ts * w0 * w0;
k1 = 4.0 * D * Ts * w0;
k2 = k0 + k1 + 4.0;
b0 = K * k0 / k2;
b1 = 2.0 * K * k0 / k2;
b2 = b0;
a1 = (2.0 * k0 - 8.0) / k2;
a2 = (k0 - k1 + 4.0) / k2;
Gfm = tf([b0 b1 b2], [1 a1 a2], Ts);


U = data(:,2) + 1i*data(:,3);
Y = data(:,4) + 1i*data(:,5);
R = data(:,6) + 1i*data(:,7);
P1 = frd(Y./U, data(:,1), Ts, 'Units', 'Hz');
P2 = frd(Y./R, data(:,1), Ts, 'Units', 'Hz');
freq = data(:,1);
spec = abs([U Y R]);


figure(4)
ax(1) = subplot(311);
semilogx(freq, spec(:,1), 'b.-'), grid on
title('Signal Spectras')
ax(2) = subplot(312);
semilogx(freq, spec(:,2), '.-', 'color', [0 0.5 0]), grid on
ax(3) = subplot(313);
semilogx(freq, spec(:,3), 'r.-'), grid on
linkaxes(ax, 'x'), clear ax
xlim([min(freq) 1/2/Ts])

figure(5)
bode(P1, Cm, 2*pi*P1.Frequency), grid on
title('Controller')

