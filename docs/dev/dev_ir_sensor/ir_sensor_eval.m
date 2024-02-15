clc, clear variables
%% parameter fit

% measurements
% dist_cm = [1:1:15, ...
%            17.5:2.50:30, ...
%            35.0:5.00:75.0].';
% dist_mV = [1871, 1805, 2923, 2949, 3105, 3106, 3080, 2849, 2590, 2374, 2199, 2021, 1885,  1768,  1647, ...
%            1442, 1313, 1162, 1063, 987, 902 ...
%            815,  736,  737,  733,  734,  659, 813, 961, 693].';

dist_cm = [1:1:20, ...
    22:2:40, ...
    42:3:81].';

dist_mV = [1620, 2250, 2350, 2680, 3100, 3107, 3108, 2900, 2710, 2485, ...
    2205, 2060, 1930, 1803, 1700, 1595, 1520, 1410, 1350, 1310, ...
    1204, 1100, 1050, 965, 905, 845, 808, 770, 733, 697, ...
    678, 628, 610, 570, 560, 532, 500, 480, 470, 467, 450, 440, 414, 411].';

% use only relevant data for the fit
ind_fit = dist_cm >= 7.0 & dist_cm <= 70;

% define a symbolic Ansatz
ft = fittype('a / (x + b)')
% ft = fittype('c*x^2 + b*x + a') % you can also try another ansatz
f1 = fit(dist_mV(ind_fit), dist_cm(ind_fit), ft)

figure(1)
subplot(131)
plot(dist_cm, dist_mV, 'x-'), grid on
axis([0 max(dist_cm) 0 max(dist_mV)])
xlabel('Distance (cm)'), ylabel('Voltage (mV)')
subplot(132)
plot(dist_mV, dist_cm, 'x-'), grid on, hold on
plot(dist_mV(ind_fit), f1(dist_mV(ind_fit)), 'x-'), hold off
ylabel('Distance (cm)'), xlabel('Voltage (mV)')
legend('Measured', 'Fitted Fcn.')
axis([0 max(dist_mV) 0 max(dist_cm)])
subplot(133)
plot(dist_cm, dist_cm, 'bx-'), grid on, hold on
plot(dist_cm, f1(dist_mV), 'rx-'), grid on, hold off
axis([0  max(dist_cm) 0 max(dist_cm)])
xlabel('Distance (cm)'), ylabel('Fit (cm)')
set(findall(gcf, 'type', 'line'), 'linewidth', 1.5)
