%% position controller with speed limitation

data = table2array(readtable('putty_04.txt'));

Ts = 2e-3;
N = length(data)
time = (0:N-1).'*Ts;

figure(1)
ax(1) = subplot(211);
stairs(time, data), grid on
ylabel('Position (Rotations)')
ax(2) = subplot(212);
stairs(time(1:end-1), diff(data)/Ts), grid on
ylabel('Velocity (Rotations/sec)')
xlabel('Time (sec)')
linkaxes(ax, 'x'), clear ax
