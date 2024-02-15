clc, clear variables
%%

linewidth = 1.2;

% file_id = fopen('putty_16.log'); % 488:1
% file_id = fopen('putty_17.log'); % 78:1
file_id = fopen('putty_18.log'); % 31:1
Ts = 500e-6;
n_col = 5;

data = fread(file_id, 'single');
fclose(file_id);
data = reshape(data, [n_col, length(data) / n_col]).';

time = cumsum(data(:,1)) * 1e-3;
time = time - time(1);
% time = (0:length(time)-1).' * Ts;

voltage = data(:,2);
fchirp = data(:,3);
sinarg = data(:,4);
rotations = data(:,5);

figure(99)
plot(time(1:end-1) * 1e3, diff(time * 1e6))
xlabel('Time (ms)'), ylabel('dTime (mus)')

figure(1)
subplot(311)
plot(time, voltage), grid on, ylabel('exc')
subplot(312)
plot(time, sinarg, 'r'), grid on, ylabel('sinarg (rad)')
subplot(313)
plot(time, fchirp, 'color', [0, 0.5, 0]), grid on, ylabel('fchirp (Hz)')
set(findall(gcf, 'type', 'line'), 'linewidth', linewidth)


chirp = voltage;

Nest     = round(1.0 / Ts);
koverlap = 0.9;
Noverlap = round(koverlap * Nest);
window   = hann(Nest);
[pxx, freq] = estimate_spectras(chirp, window, Noverlap, Nest, Ts);
spectra = sqrt(pxx); % power -> amplitude (dc needs to be scaled differently)

figure(2)
plot(freq, spectra), grid on, ylabel('Signal Spectra'), xlabel('Frequency (Hz)')
set(gca, 'YScale', 'log'), set(gca, 'XScale', 'log')
xlim([0 1/2/Ts])
set(findall(gcf, 'type', 'line'), 'linewidth', linewidth)


Nest     = round(5 / Ts);
koverlap = 0.9;
Noverlap = round(koverlap * Nest);
window   = hann(Nest);

% % rotating filter
% Dlp = sqrt(3) / 2;
% wlp = 2 * pi * 10;
% Glp = c2d(tf(wlp^2, [1 2*Dlp*wlp wlp^2]), Ts, 'tustin');

% % 78:1
% P_mod = db2mag(-11.9) * ...
%     tf(1, [1/45 1]) * ...
%     tf(1, [1/800 1]) * ...
%     tf(1, [1 0]);
% R = 15; % 16;
% km = 48 * 0.0981 / 2.6;
% % dc = 0.2541
% L = 0.0183
% J = 0.0108;
% b = 0.2565;

% 31:1
P_mod = db2mag(-11.9+9.2) * ...
    tf(1, [1/45 1]) * ...
    tf(1, [1/800 1]) * ...
    tf(1, [1 0]);
R = 15; % 14;
km = 20.5 * 0.0981 / 2.6;
% dc = 0.7328
L = 0.0182
J = 0.0016;
b = 0.0305;

s = tf('s')
P_mod_sym = km/(km^2 + J*L*s^2 + (L*b + J*R)*s + R*b) * 1/s;

inp = voltage;
out = rotations;
% inp = apply_rotfiltfilt(Glp, sinarg, voltage);
% out = apply_rotfiltfilt(Glp, sinarg, rotations);
[P, C_P] = estimate_frequency_response(inp, out, window, Noverlap, Nest, Ts);

Gd = tf([1 -1], [Ts 0], Ts);


figure(3)
bode(P, P_mod, P_mod_sym, 2*pi*P.Frequency), grid on
set(findall(gcf, 'type', 'line'), 'linewidth', linewidth)

opt = bodeoptions('cstprefs');
opt.MagScale = 'linear';
opt.MagUnits = 'abs';
figure(4)
bodemag(C_P, 2*pi*C_P.Frequency, opt), grid on
set(findall(gcf, 'type', 'line'), 'linewidth', linewidth)

