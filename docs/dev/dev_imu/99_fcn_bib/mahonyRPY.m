function [quat_, bias_] = mahonyRPY(gyro, acc, mag, para, Ts, quat0)

if nargin == 5
    quat = eye(4,1);
else
    quat = quat0;
end

kp = para.kp;
ki = para.ki;

N = size(gyro, 1);
bias = zeros(3,1);
quat_ = zeros(N, 4);
bias_ = zeros(N, 3);
for i = 1:N

    % h = (hx; hy;  0) - measured mag field vector in EF (assuming Z-component is zero)
    % b = (bx;  0;  0) - reference mag field vector heading due North in EF (assuming Z-component is zero)
    mag_n = mag(i,:).';
    mag_n = mag_n ./ norm(mag_n);

    CBE = quat2CBE(quat);
    CEB = CBE.';
    h = CEB * mag_n; % Cmu.' * CEB = (CEB.' * Cmu).' = = (CBE * Cmu).'
    h(3) = 0;

%     % by normalising here you adjust implicit the weigth
%     if i == 1
%         h_norm_init = norm(h);
%     end
%     b = [1; 0; 0];
%     e = CBE * cross(h * h_norm_init / norm(h), b);

%     b = [1; 0; 0];
%     e = CBE * cross(h/norm(h), b);

    % by normalising here you adjust implicit the weight
    b = [norm(h); 0; 0];
    %e = CBE * cross(h, b);
    [ang, vn] = calcAngleBetween2Vectors(h, b);
    e = norm(h) * CBE * ang * vn;

    g_n = CEB(3,:).';

    acc_n = acc(i,:).';
    acc_n = acc_n ./ norm(acc_n);
    %e = e + cross(acc_n, g_n);
    [ang, vn] = calcAngleBetween2Vectors(acc_n, g_n);
    e = e + ang * vn;
        
    bias = bias + ki .* e * Ts;

    Q = [[-quat(2), -quat(3), -quat(4)]; ...
         [ quat(1), -quat(4),  quat(3)]; ...
         [ quat(4),  quat(1), -quat(2)]; ...
         [-quat(3),  quat(2),  quat(1)]];

    dquat = Ts * 0.5 * Q * ( gyro(i,:).' + bias + kp .* e );
    quat = quat + dquat;
    quat = quat ./ norm(quat);

    quat_(i,:) = quat.';
    bias_(i,:) = bias.';
end


return