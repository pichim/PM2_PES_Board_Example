




dist_m = [2.5 5 7.5 10:5:100]*0.01;
dist_V = [2900 3100 2700 2200 1500 1200 1000 800 750 650 610 580 540 510 450 430 400 370 320 340 300 340]*0.001;

figure(2)
plot(dist_m, dist_V, 'bx-'), grid on
xlabel('Distanz (m)')
ylabel('Spannung (V)')