clc, clear variables
%%

% nominal square length
L_square = 0.8;

% nominal values active during the measurement
r1_wheel = (1.275 / 1.3) * 0.0357 / 2.0; % 0.0175
r2_wheel = (1.275 / 1.3) * 0.0357 / 2.0; % 0.0175
b_wheel = 0.1518;                        % 0.1518

% 4 CW run points (x, y) in meters
cw  = [  -4.5,   1.0;...
        -11.0,  -1.0; ...
        -26.5,  -6.5; ...
        -34.0,  -7.5] * 0.01;

% 4 CCW run points (x, y) in meters
ccw = [  -6.0,  -1.5;...
        -11.0,  -2.0; ...
        -16.0,  -3.0; ...
        -21.0,  -4.0] * 0.01;


%%

% mean
cw_cog  = mean(cw);
ccw_cog = mean(ccw);

% distance of cog
r_cw_cog  = norm(cw_cog);
r_ccw_cog = norm(ccw_cog);

% alpha -> correct wheelbase
alpha = (cw_cog(1) + ccw_cog(1)) / (-4*L_square);
alpha * 180/pi

b_actual = (pi/2) / (pi/2 - alpha) * b_wheel;

% beta -> correct radius
beta = (cw_cog(1) - ccw_cog(1)) / (-4*L_square);
beta * 180/pi

R_actual   = (L_square/2) / sin(beta/2);

Ed = (R_actual + b_actual/2) / (R_actual - b_actual/2);
ra = 0.5 * (r1_wheel + r2_wheel);
r1_actual = 2 / (1/Ed + 1) * ra;
r2_actual = 2 / (Ed + 1) * ra;


%%

fprintf("   corrected values\n");
fprintf("    const float r1_wheel = %.5ff; // right wheel radius in meters\n", r1_actual);
fprintf("    const float r2_wheel = %.5ff; // left  wheel radius in meters\n", r2_actual);
fprintf("    const float b_wheel  = %.5ff; // wheelbase, distance from wheel to wheel in meters\n", b_actual);

