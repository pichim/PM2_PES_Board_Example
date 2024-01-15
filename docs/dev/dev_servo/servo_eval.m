clc, clear variables
%%

% notes:
% - introducing max velocity does sound weird when used in ServoController
%   class, so i just constrain it to 1e6 (inf) -> unconstrained

pulse_min = 0.035;
pulse_max = 0.115;
(pulse_max - pulse_min) / 2.0

% // set up motion
% const float pulse_min = 0.035f;
% const float pulse_max = 0.115f;
% m_Motion.setProfileVelocity((pulse_max - pulse_min) / 1.0f);
% float max_acceleration = 0.1f;
% m_Motion.setProfileAcceleration(max_acceleration);
% m_Motion.setProfileDeceleration(max_acceleration);
data = readmatrix("putty_00.log");

% // set up motion
% const float pulse_min = 0.035f;
% const float pulse_max = 0.115f;
% m_Motion.setProfileVelocity((pulse_max - pulse_min) / 0.5f);
% float max_acceleration = 0.2f;
% m_Motion.setProfileAcceleration(max_acceleration);
% m_Motion.setProfileDeceleration(max_acceleration);
data = readmatrix("putty_01.log");

% // set up motion
% m_Motion.setProfileVelocity(1.0e6f); // 1.0e6f instead of inf
% float max_acceleration = 0.3f;
% m_Motion.setProfileAcceleration(max_acceleration);
% m_Motion.setProfileDeceleration(max_acceleration);
data = readmatrix("putty_02.log");

% % // set up motion
% % m_Motion.setProfileVelocity(1.0e6f); // 1.0e6f instead of inf
% % float max_acceleration = 0.35f;
% % m_Motion.setProfileAcceleration(max_acceleration);
% % m_Motion.setProfileDeceleration(max_acceleration);
% data = readmatrix("putty_03.log");

time = data(:,1) * 1e-6;
time = time - time(1);
data = data(:,2:end);

figure(1)
ax(1) = subplot(211);
plot(time - 1.5, data(:,1:2)), grid on, ylabel('Normalised Pulswidth')
ax(2) = subplot(212);
plot(time - 1.5, data(:,3), 'r'), grid on, ylabel('Normalised Pulswidth Velocity')
xlabel('Time (sec)')
linkaxes(ax, 'x'), clear ax
xlim([0 4])
set(findall(gcf, 'type', 'line'), 'linewidth', 2.0)
