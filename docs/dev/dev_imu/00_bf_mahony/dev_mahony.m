clc, clear variables
addpath ..\99_fcn_bib\
%%

% data = readmatrix('putty_02.log');
% data = readmatrix('putty_04.log');
data = readmatrix('putty_06.log');
% bessel (D = sqrt(3)/2)
w0 = 3;
kp = w0 / ( sqrt(3)/3 )
ki = kp^2 / 3

% data = readmatrix('putty_03.log');
% % real pole, no integrator, use this if you dont use the mag
% w0 = 3;
% kp = w0;
% ki = 0;


ind_tilt = 18;

time = data(:,10) * 1e-3;
time = time - time(1);
Ts = median(diff(time));
ind_gyro = 1:3;
ind_acc  = 4:6;
ind_mag  = 7:9;
ind_quat = 11:14;
ind_rpy  = 15:17;

% ind_imu = [ind_gyro, ind_acc, ind_mag];
% Gf = c2d(tf(1, [1 1]), Ts, 'tustin');
% data(:,ind_imu) = filter(Gf.num{1}, Gf.den{1}, data(:,ind_imu));

figure(99)
plot(time, -atan2(data(:,ind_mag(2)), data(:,ind_mag(1))) * 180/pi), grid on

%%

figure(1)
plot(diff(time)), grid on

figure(2)
tiledlayout_ = tiledlayout(3,1); tiledlayout_.TileSpacing = 'compact';
ax(1) = nexttile;
plot(ax(1), time, data(:,ind_gyro) * 180/pi), grid on, ylabel('Gyro (deg/sec)')
ax(2) = nexttile;
plot(ax(2), time, data(:,ind_acc)), grid on, ylabel('Acc (m/s^2)')
ax(3) = nexttile;
plot(ax(3), time, data(:,ind_mag)), grid on, ylabel('Mag'), xlabel('Time (sec)')
linkaxes(ax, 'x'), clear ax, xlim([0, max(time)])

figure(3)
plot(time, cumtrapz(time, data(:,ind_gyro)) * 180/pi), grid on
ylabel('Gyro Integral (deg)'), xlabel('Time (sec)')
xlim([0, max(time)])

para.kp = kp;
para.ki = ki;

rpy0 = 0 * [60, 60, 60] * pi/180;
quat0 = rpy2quat(rpy0).';

[quatRP , biasRP ] = mahonyRP (data(:,ind_gyro), data(:,ind_acc), para, Ts, quat0);
[quatRPY, biasRPY] = mahonyRPY(data(:,ind_gyro), data(:,ind_acc), data(:,ind_mag), para, Ts, quat0);

getTiltAngleFromQuaternion = @(quat) acos( quat(:,1).^2 - quat(:,2).^2 - quat(:,3).^2 + quat(:,4).^2 );

rpyRP  = quat2rpy(quatRP );
rpyRPY = quat2rpy(quatRPY);

angleFun = @wrapToPi;

figure(44)
tiledlayout_ = tiledlayout(3,1); tiledlayout_.TileSpacing = 'compact';
ax(1) = nexttile;
ang_ind = 1;
plot(time, angleFun(cumtrapz(time, data(:,ind_gyro(ang_ind)))) * 180/pi), grid on, hold on
plot(time, angleFun(rpyRP (:,ang_ind)) * 180/pi)
plot(time, angleFun(rpyRPY(:,ang_ind)) * 180/pi)
plot(time, angleFun(data(:,ind_rpy(ang_ind))) * 180/pi), hold off
ylabel('Roll (deg)'), xlabel('Time (sec)')
ax(2) = nexttile;
ang_ind = 2;
plot(time, angleFun(cumtrapz(time, data(:,ind_gyro(ang_ind)))) * 180/pi), grid on, hold on
plot(time, angleFun(rpyRP (:,ang_ind)) * 180/pi)
plot(time, angleFun(rpyRPY(:,ang_ind)) * 180/pi)
plot(time, angleFun(data(:,ind_rpy(ang_ind))) * 180/pi), hold off
ylabel('Pitch (deg)'), xlabel('Time (sec)')
ax(3) = nexttile;
ang_ind = 3;
plot(time, angleFun(cumtrapz(time, data(:,ind_gyro(ang_ind)))) * 180/pi), grid on, hold on
plot(time, angleFun(rpyRP (:,ang_ind)) * 180/pi)
plot(time, angleFun(rpyRPY(:,ang_ind)) * 180/pi)
plot(time, angleFun(data(:,ind_rpy(ang_ind))) * 180/pi), hold off
ylabel('Yaw (deg)'), xlabel('Time (sec)')
linkaxes(ax, 'x'), clear ax, xlim([0, max(time)])

figure(5)
plot(time, getTiltAngleFromQuaternion(quatRP ) * 180/pi), grid on, hold on
plot(time, getTiltAngleFromQuaternion(quatRPY) * 180/pi)
plot(time, data(:, ind_tilt) * 180/pi), hold off

figure(6)
tiledlayout_ = tiledlayout(2,1); tiledlayout_.TileSpacing = 'compact';
ax(1) = nexttile;
plot(time, biasRP  * 180/pi), grid on
ylabel('Gyro Bias (deg/s)')
ax(2) = nexttile;
plot(time, biasRPY * 180/pi), grid on
ylabel('Gyro Bias (deg/s)'), xlabel('Time (sec)')
linkaxes(ax, 'x'), clear ax, xlim([0, max(time)])

% figure(6)
% subplot(211)
% plot(time, data(:,ind_quat)), grid on
% ylabel('Quaternion')
% subplot(212)
% plot(time, quat_), grid on
% ylabel('Quaternion'), xlabel('Time (sec)')
