clc, clear variables
%% parameter fit

% Distance Data
dist_cm = [4:1:15, 16:2:30]';

dist_mV = [2871.016, 2410.036, 1967.107, 1711.913, 1526.303, 1371.04, 1262.783, ...
    1190.79, 1096.26, 1037.14, 991.746, 940.973, 907.13, 861.463, 852.867, ...
    784.64, 744.343, 703.833, 669.937, 661.07]';

% Define a fit region: in the example we only want [7, 70]
ind_fit = dist_cm >= 7.0 & dist_cm <= 70;

% Model function: a / (x + b)
ft = fittype('a / (x + b)');

% Curve fitting
f1 = fit(dist_mV(ind_fit), dist_cm(ind_fit), ft);

fprintf("   Fitted parameters:\n")
fprintf("   a = %.4f,  b = %.4f\n", f1.a, f1.b);

figure(1)

% 1) dist_cm vs. dist_mV
subplot(131)
plot(dist_cm, dist_mV, 'x-'), grid on
axis([0 max(dist_cm) 0 max(dist_mV)])
xlabel('Distance (cm)'), ylabel('Voltage (mV)')

% 2) dist_mV vs. dist_cm with fitted model (over the fit region)
subplot(132)
plot(dist_mV, dist_cm, 'x-'), grid on, hold on
plot(dist_mV(ind_fit), f1(dist_mV(ind_fit)), 'x-'), hold off
ylabel('Distance (cm)'), xlabel('Voltage (mV)')
legend('Measured', 'Fitted Fcn.')
axis([0 max(dist_mV) 0 max(dist_cm)])

% 3) dist_cm vs. fitted_cm
subplot(133)
plot(dist_cm, dist_cm, 'x-'), grid on, hold on
plot(dist_cm, f1(dist_mV), 'x-'), grid on, hold off
axis([0  max(dist_cm) 0 max(dist_cm)])
xlabel('Distance (cm)'), ylabel('Fit (cm)')

% set(findall(gcf, 'type', 'line'), 'linewidth', 1.5)
