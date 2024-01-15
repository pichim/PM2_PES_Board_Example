function [quat_, bias_] = mahonyRP(gyro, acc, para, Ts, quat0)

if nargin == 4
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
    g_n = [         2*(quat(2)*quat(4) - quat(1)*quat(3)); ...
                    2*(quat(3)*quat(4) + quat(1)*quat(2)); ...
            quat(1)^2 - quat(2)^2 - quat(3)^2 + quat(4)^2];

    acc_n = acc(i,:).';
    acc_n = acc_n ./ norm(acc_n);
    %e = cross(acc_n, g_n);
    [ang, vn] = calcAngleBetween2Vectors(acc_n, g_n);
    e = ang * vn;
    
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