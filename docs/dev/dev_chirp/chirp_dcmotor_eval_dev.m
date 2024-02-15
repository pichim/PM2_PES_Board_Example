clc, clear variables
%%

linewidth = 1.2;

% file_id = fopen('putty_09.log');
% file_id = fopen('putty_10.log');
% file_id = fopen('putty_11.log');
% file_id = fopen('putty_12.log');
% file_id = fopen('putty_13.log');
% file_id = fopen('putty_14.log');
file_id = fopen('putty_15.log');
Ts = 500e-6;
n_col = 5;

data = fread(file_id, 'single');
fclose(file_id);
data = reshape(data, [n_col, length(data) / n_col]).';


time = data(:,1) * 1e-3;
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

P_mod = db2mag(-11.9) * ...
    tf(1, [1/45 1]) * ...
    tf(1, [1/800 1]) * ...
    tf(1, [1 0]);

inp = voltage;
out = rotations;
% inp = apply_rotfiltfilt(Glp, sinarg, voltage);
% out = apply_rotfiltfilt(Glp, sinarg, rotations);
[P, C_P] = estimate_frequency_response(inp, out, window, Noverlap, Nest, Ts);

Gd = tf([1 -1], [Ts 0], Ts);

% load P_gpa_00.mat
load P_gpa_01.mat
w0 = 2.0 * pi * 15.0;
D = 1.0;
K = 1.0;
w0_d = w0;
D_d = D;
Ts_d = Ts;
K_d = K;
k0 = Ts_d * Ts_d * w0_d * w0_d;
k1 = 4.0 * D_d * Ts_d * w0_d;
k2 = k0 + k1 + 4.0;
B0 = K_d * k0 / k2;
B1 = 2.0 * K_d * k0 / k2;
B2 = B0;
A0 = (2.0 * k0 - 8.0) / k2;
A1 = (k0 - k1 + 4.0) / k2;
Gf = tf([B0 B1 B2], [1 A0 A1], Ts);
P_gpa = P_gpa / Gf / Gd

figure(3)
bode(P, P_gpa, P_mod, 2*pi*P.Frequency), grid on
set(findall(gcf, 'type', 'line'), 'linewidth', 1.5)

opt = bodeoptions('cstprefs');
opt.MagScale = 'linear';
opt.MagUnits = 'abs';
figure(4)
bodemag(C_P, 2*pi*C_P.Frequency, opt), grid on

