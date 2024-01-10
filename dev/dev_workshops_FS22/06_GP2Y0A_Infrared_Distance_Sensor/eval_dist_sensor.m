% pmic, 21.06.2019
clear all

% /*  Notes pmic 07.08.2019 zu GP2Y0A02YK0F:
%   - Stark nichtlineare Kennlinie
%   - Mapping (Invertierung) der Kennlinie nur mit Abstand > 0.15 m möglich
%    (mehrdeutig), siehe auch Datenblatt
%   - Ansonsten sieht Signalqualität eher mittel aus
%   - 50 Hz Abtastung mit max. 20+5 ms Totzeit (nach Datenblatt)
% */

% /*  Notes pmic 07.08.2019 zu GP2Y0A21YK:
%   - Eindruck der Signale in etwa wie GP2Y0A02YK0F
%   - Kennlinie lediglich ab 10 cm gemessen (noch keine Mehrdeutigkeit)
%   - Ansonsten sieht Signalqualität eher mittel aus
%   - 50 Hz Abtastung mit max. 20+5 ms Totzeit (nach Datenblatt)
% */

%% static analysis

% GP2Y0A02YK0F
distTrue1 = [0.05 0.1:0.1:1.0 0.125 0.15 0.175].' + 0.0012;
distS1 = [0.237 0.615 0.751 0.615 0.490 0.371 0.321 0.274 0.244 0.221 0.200 0.767 0.829 0.787].';

% GP2Y0A21YK
distTrue2 = [0.05 0.1:0.1:0.8].' + 0.0012;
distS2 = [0.876 0.836 0.394 0.259 0.196 0.167 0.151 0.120 0.111].';

[distTrue1, ind] = sort(distTrue1);
distS1 = distS1(ind);
% nn=4:14;
nn=1:length(distTrue1);
figure(1)
plot(distTrue1(nn), distS1(nn)*3.3, '-x', distTrue2, distS2*3.3, '-x'), grid on
xlabel('Distance (m)')
ylabel('Sensor Output without Scaling')
legend('Sensor GP2Y0A02YK0F', 'Sensor GP2Y0A21YK', 'location', 'best')
axis([0 max(distTrue1) 0 0.9*3.3])

figure(2)
plot(distS1*3.3, distTrue1, '-x', distS2*3.3, distTrue2, '-x'), grid on
ylabel('Distance (m)')
xlabel('Sensor Output without Scaling')
legend('Sensor GP2Y0A02YK0F', 'Sensor GP2Y0A21YK', 'location', 'best')

%% extract static characteristics where the actual distance is > 0.175 m

distS1 = distS1(4:end);
distTrue1 = distTrue1(4:end);

%% dynamic analysis

% GP2Y0A02YK0F
load meas_00
Ts = 5e-2;
time1 = (data(:,1)-1)*Ts;
dist1 = data(:,2);
ind = time1 > 3.65;
time1 = time1(ind);
time1 = time1 - time1(1);
dist1 = dist1(ind);
% GP2Y0A21YK
load meas_01
Ts = 5e-2;
time2 = (data(:,1)-1)*Ts;
dist2 = data(:,2);
ind = time2 > 4.6;
time2 = time2(ind);
time2 = time2 - time2(1);
dist2 = dist2(ind);

% inversion via measured characteristics
dist1 = interp1(distS1, distTrue1, dist1, 'linear', 'extrap');
dist2 = interp1(distS2, distTrue2, dist2, 'linear', 'extrap');

figure(3)
plot(time1, dist1, time2, dist2), grid on
xlabel('Time (s)')
ylabel('Distance (m)')
xlim([0 max([max(time1), max(time1)])])

[Dist1, freq1] = myfft(dist1, Ts);
[Dist2, freq2] = myfft(dist2, Ts);
figure(4)
plot(freq1, Dist1*1e3, freq2, Dist2*1e3), grid on
set(gca, 'YScale', 'log')
set(gca, 'XScale', 'log')
xlim([0 1/2/Ts])
xlabel('Frequency (Hz)')
ylabel('Distance (mm)')
