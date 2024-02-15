clc, clear variables
%%

linewidth = 1.2;

% sprintf
% -------------------------------------------------------------------------
% data = readmatrix("putty_00.log");
% Ts = 1e-3;


% memcopy (binary data)
% -------------------------------------------------------------------------
% file_id = fopen('putty_01.log');
% Ts = 1e-3;
% n_col = 4;

% file_id = fopen('putty_02.log');  % not working
% Ts = 250e-6;
% n_col = 4;

% file_id = fopen('putty_03.log');
% Ts = 500e-6;
% n_col = 4;

% file_id = fopen('putty_04.log'); % not working
% Ts = 500e-6;
% n_col = 7;

% % file_id = fopen('putty_05.log'); % working but had large time jitter
% file_id = fopen('putty_06.log'); % not working
% Ts = 500e-6;
% n_col = 6;

% file_id = fopen('putty_07.log');
file_id = fopen('putty_08.log');
Ts = 500e-6;
n_col = 5;

data = fread(file_id, 'single');
fclose(file_id);
data = reshape(data, [n_col, length(data) / n_col]).';


time = data(:,1) * 1e-3;
time = time - time(1);
% time = (0:length(time)-1).' * Ts;

exc = data(:,2);
fchirp = data(:,3);
sinarg = data(:,4);

figure(99)
plot(time(1:end-1) * 1e3, diff(time * 1e6))
xlabel('Time (ms)'), ylabel('dTime (mus)')

figure(1)
subplot(311)
plot(time, exc), grid on, ylabel('exc')
subplot(312)
plot(time, sinarg, 'r'), grid on, ylabel('sinarg (rad)')
subplot(313)
plot(time, fchirp, 'color', [0, 0.5, 0]), grid on, ylabel('fchirp (Hz)')
set(findall(gcf, 'type', 'line'), 'linewidth', linewidth)


chirp = exc;

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

