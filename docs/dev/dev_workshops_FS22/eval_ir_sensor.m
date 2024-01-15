clc, clear variables
%% example

% dist_cm = [2.5 5 7.5 10:5:100].';
% dist_mV = [2900 3100 2700 2200 1500 1200 1000 800 750 650 610 580 540 510 450 430 400 370 320 340 300 340];

dist_cm = [4:2:50].';
dist_mV = [2360 1874 1444 1167 1000 850 738 644 568 524 482 450 414 393 357 334 322 314 301 297 291 275 277 276].';

% remove unwanted datepoints if needed
ft = fittype('c/x + b*x + a')
f1 = fit(dist_mV, dist_cm, ft, 'Robust', 'Bisquare')

figure(1)
subplot(121)
plot(dist_cm, dist_mV, 'bx-'), grid on
axis([0 max(dist_cm) 0 max(dist_mV)])
xlabel('Distanz (cm)')
ylabel('Spannung (mV)')
subplot(122)
plot(dist_mV, [dist_cm], 'bx-'), grid on, hold on
plot(f1, 'r'), hold off
axis([0 max(dist_mV) 0 max(dist_cm)])
xlabel('Spannung (mV)')
ylabel('Distanz (cm)')
