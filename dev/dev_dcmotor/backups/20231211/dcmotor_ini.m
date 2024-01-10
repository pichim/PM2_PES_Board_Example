clc, clear variables
%%

clc

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
m_velocity_max = 0.8 * m_kn * voltage_max

% static constexpr float PWM_MIN = 0.01f;
% static constexpr float PWM_MAX = 0.99f;
% static float voltage_min = m_voltage_max * (2.0f * PWM_MIN - 1.0f);
% static float voltage_max = m_voltage_max * (2.0f * PWM_MAX - 1.0f);
PWM_MIN = 0.01;
PWM_MAX = 0.99;
voltage_min = m_voltage_max * (2.0 * PWM_MIN - 1.0)
voltage_max = m_voltage_max * (2.0 * PWM_MAX - 1.0)
PWM_MIN_ = 0.5 + 0.5 * voltage_min / m_voltage_max
PWM_MAX_ = 0.5 + 0.5 * voltage_max / m_voltage_max

% m_counts_per_turn: 1562.500000
% m_kn: 0.250000
% m_voltage_max: 12.000000
% m_velocity_max: 2.400000
% voltage_min: -11.760000
% voltage_max: 11.760000
% PWM_MIN_: 0.010000
% PWM_MAX_: 0.990000


%%

Ts = 1e-3;
w0 = 120;

a11 = (1.0+w0*Ts)*exp(-w0*Ts);
a12 = Ts*exp(-w0*Ts);
a21 = -w0*w0*Ts*exp(-w0*Ts);
a22 = (1.0-w0*Ts)*exp(-w0*Ts);
b1 = (1.0-(1.0+w0*Ts)*exp(-w0*Ts))/w0/w0;
b2 = Ts*exp(-w0*Ts);

A = [a11, a12; a21, a22];
B = [b1; b2];
C = [w0*w0 0];

Gf = tf(ss(A, B, C, 0, Ts)) * tf('z' , Ts);

% [a, b, c, d] = dlinmod('lowpassfilter_sim', Ts);
% sys_sim = ss(a, b, c, d, Ts);
% Gf_sim = tf(sys_sim);

figure(99)
% bode(Gf, Gf_sim), grid on
subplot(121)
bode(Gf), grid on
title('LowpassFilter')
subplot(122)
step(Gf), grid on
title('LowpassFilter')


%%

data = readmatrix('putty_22.log');

% 1.5 rps

time = data(:,1) * 1e-3;
time = time - time(1);
data = data(:,2:end);
median(diff(time))

robot_state = data(:,1);
data = data(:,2:end);

figure(1)
stairs(time, robot_state), grid on

figure(22)
ax(1) = subplot(221);
plot(time, data(:,1:3)), grid on
ylabel('Rotation')
ax(2) = subplot(223);
plot(time, data(:,4:6)), grid on
ylabel('Velocity (RPS)'), xlabel('Time (sec)')
ax(3) = subplot(222);
plot(time(1:end-1), diff(data(:,4:6)) ./ diff(time)), grid on
ylabel('Acceleration (RPS/sec)'), xlabel('Time (sec)')
ax(4) = subplot(224);
plot(time, data(:,7)), grid on
ylabel('Voltage (V)'), xlabel('Time (sec)')
linkaxes(ax, 'x'), clear ax
xlim([0 time(end)])

figure(33)
plot(time, data(:,8)), grid on
ylabel('PWM'), xlabel('Time (sec)')
xlim([0 time(end)])

