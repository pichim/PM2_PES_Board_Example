%% servo figures

clf

x = (0:0.01:1).';
N = size(x, 1);

servo_pos_ms = 4;

servo_period_mus = 20000;
servo_period_ms = servo_period_mus * 1e-3;

servo_ratio = servo_pos_ms / servo_period_ms;
Nhigh = floor(servo_ratio * N);
pwm_full_range = ones(size(x));

ang_gain = 40;
ang_max = 20;

ang_full_range = x*ang_gain;

ind_ang_max = find(ang_full_range <= ang_max, 1, 'last');
pwm_max_range = pwm_full_range;
pwm_max_range(ind_ang_max+1:end) = 0;

ind_ang_act = find(x <= servo_ratio, 1, 'last');
pwm_act_range = pwm_full_range;
pwm_act_range(ind_ang_act+1:end) = 0;

fontsize = 13;

figure(1)
subplot(121)
stairs(x*servo_period_ms, pwm_full_range, 'linewidth', 3), grid on, hold on
stairs(x*servo_period_ms, pwm_max_range , 'linewidth', 3)
stairs(x*servo_period_ms, pwm_act_range, 'linewidth', 3), hold off
xlabel('Servo Period (ms)', 'Fontsize', fontsize), ylabel('PWM Output', 'Fontsize', fontsize)
subplot(122)
plot(x*servo_period_ms, ang_full_range, 'linewidth', 3), grid on, hold on
plot([x(10)*servo_period_ms;x(10:ind_ang_max)*servo_period_ms;x(ind_ang_max)*servo_period_ms], ...
    [0;ang_full_range(10:ind_ang_max);0], 'linewidth', 3)
stem(x(ind_ang_act)*servo_period_ms, x(ind_ang_act)*ang_gain, 'r', 'linewidth', 3), hold off
legend('Full Range PWM', 'Angle Range', 'Angle Actual', 'location', 'best', 'Fontsize', fontsize-4)
xlabel('Servo Period (ms)', 'Fontsize', fontsize), ylabel('Angle (deg)', 'Fontsize', fontsize)
