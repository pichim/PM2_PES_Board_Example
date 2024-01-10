%%

clc, clear variables

u = [8637, 7840, 6346, 5153, 3966, 2810, 1650, 1089, 777, 660, 484, 371, 224, 176, 142, 91].' + 0.3;
y = [ 150,  135,  110,   90,   70,   50,   30,   20,  15,  13,  10,   8,   6,   5,   4,  3].';

theta = [u, ones(size(u))] \ y;
single(theta)

figure(1)
plot(u, y, '.-', u, [u, ones(size(u))] * theta, 'x-'), grid on

% 10000 <-> 164
% 14000 <-> 232.5

t = [10000 14000].';
d = [163.9 232.5].';
theta = [d, ones(size(d))] \ t;
200 * theta(1) + theta(2)

%%

clc, clear variables

data = readmatrix('putty_01.log');

time = data(:,1) * 1e-3;
time = time - time(1);
data = data(:,2:end);
median(diff(time))

figure(1)
% plot(time, data), grid on, hold on

ind = data == -1.0;
time(ind) = [];
data(ind) = [];

plot(time, data), hold off

