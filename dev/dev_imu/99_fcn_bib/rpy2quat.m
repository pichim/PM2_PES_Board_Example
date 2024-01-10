function q = rpy2quat(rpy)
% rpy = [phi, theta, psi]
% q = [qw qx qy qz] (= [q0, q1, q2, q3] = [qr, qi, qj, qk])

N = size(rpy, 1);
q = zeros(N,4);
for i = 1:N
    c = cos(rpy(i,:)/2);
    s = sin(rpy(i,:)/2);
    q(i,:) = [c(3)*c(2)*c(1) + s(3)*s(2)*s(1), ...
             c(3)*c(2)*s(1) - s(3)*s(2)*c(1), ...
             c(3)*s(2)*c(1) + s(3)*c(2)*s(1), ...
             s(3)*c(2)*c(1) - c(3)*s(2)*s(1)];
    q(i,:) = q(i,:)./sqrt(sum(q(i,:).^2, 2));
end

return