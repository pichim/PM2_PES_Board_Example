clc, clear variables
%%

syms L R km J b s

% Grl: uE -> i
Grl = 1 / (L*s + R);
% Grl = 1 / R; % dc-value

% Gm: uE -> w (dphi)
Gm = 1 / (J*s + b);

Gi = collect(Grl / (1 + Grl * km * Gm * km), s)
% (J*s + b)/(km^2 + J*L*s^2 + (L*b + J*R)*s + R*b)

Gidc = simplify( subs(Gi, s, 0) )
% b/(km^2 + R*b)

Gw = collect(Grl * km * Gm / (1 + Grl * km * Gm * km), s)
% km/(km^2 + J*L*s^2 + (L*b + J*R)*s + R*b)

Gwdc = simplify( subs(Gw, s, 0) )
% km/(km^2 + R*b)

p12 = simplify(solve((km^2 + J*L*s^2 + (L*b + J*R)*s + R*b) == 0, s))
% -(L*b - (J^2*R^2 - 2*J*L*R*b - 4*J*L*km^2 + L^2*b^2)^(1/2) + J*R)/(2*J*L)
% -(L*b + (J^2*R^2 - 2*J*L*R*b - 4*J*L*km^2 + L^2*b^2)^(1/2) + J*R)/(2*J*L)

syms dc p1 p2

eqns = [km/(km^2 + R*b) == dc, ...
    -(L*b - (J^2*R^2 - 2*J*L*R*b - 4*J*L*km^2 + L^2*b^2)^(1/2) + J*R)/(2*J*L) == p1, ...
    -(L*b + (J^2*R^2 - 2*J*L*R*b - 4*J*L*km^2 + L^2*b^2)^(1/2) + J*R)/(2*J*L) == p2];
vars = [L, J, b];

S = solve(eqns, vars)

%%

% 78:1
R = 15; % 16;
km = 48 * 0.0981 / 2.6;
dc = 0.2541
% L = 0.0183
% J = 0.0108;
% b = 0.2565;

% % 31:1
% R = 15; % 14;
% km = 20.5 * 0.0981 / 2.6;
% dc = 0.7328
% % L = 0.0182
% % J = 0.0016;
% % b = 0.0305;

p1 = -45
p2 = -800

% S.L
-(R*km*p1 - (R*(km*p1 + km*p2 + km*(p1^2 - 2*p1*p2 + p2^2 + 4*dc*km*p1*p2)^(1/2)))/2 + R*km*p2)/(- dc*p1*p2*km^2 + p1*p2*km)
-(R*km*p1 - (R*(km*p1 + km*p2 - km*(p1^2 - 2*p1*p2 + p2^2 + 4*dc*km*p1*p2)^(1/2)))/2 + R*km*p2)/(- dc*p1*p2*km^2 + p1*p2*km)
% S.J
-(km*p1 + km*p2 + km*(p1^2 - 2*p1*p2 + p2^2 + 4*dc*km*p1*p2)^(1/2))/(2*R*dc*p1*p2)
-(km*p1 + km*p2 - km*(p1^2 - 2*p1*p2 + p2^2 + 4*dc*km*p1*p2)^(1/2))/(2*R*dc*p1*p2)
% S.b
(- km^2 + km/dc)/R
(- km^2 + km/dc)/R

