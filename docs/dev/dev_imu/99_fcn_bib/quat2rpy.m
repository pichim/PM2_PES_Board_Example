function ang = quat2rpy(q)
% q = [qw qx qy qz] (= [q0, q1, q2, q3] = [qr, qi, qj, qk])
% transforms a quaternion parametrization to
% Z-Y-X, Roll-Pitch-Yaw, Tait–Bryan angles
% CB2I = Rz(psi)*Ry(theta)*Rx(phi) (transforms Body to Inertia)

[m,n] = size(q);
for i = 1:m % restore norm
    q(i,:) = q(i,:) / sqrt(q(i,1)^2 + q(i,2)^2 + q(i,3)^2 + q(i,4)^2);
end

ang = [quat2phi(q), quat2theta(q), quat2psi(q)];

end

% Version 1:
% https://math.stackexchange.com/questions/687964/getting-euler-tait-bryan-angles-from-quaternion-representation

% Note that the gimbal-lock situation occurs when 2(q1q3+q0q2)=±1 
% (which gives a theta of +/-pi/2), so it can be clearly identified before 
% you attempt to evaluate phi and theta

function phi = quat2phi(q)
% arctan2(q2q3+q0q1,1/2-(q1^2+q2^2))
    phi   = atan2( (q(:,3).*q(:,4) + q(:,1).*q(:,2) ), ...
                   1/2 - (q(:,2).^2 + q(:,3).^2) );
end

function theta = quat2theta(q)
% arcsin(-2(q1q3-q0q2))
    sinarg = -2*(q(:,2).*q(:,4) - q(:,1).*q(:,3));
    sinarg(sinarg > 1) = 1;
    sinarg(sinarg < -1) = -1;
    theta = asin(sinarg);
end

function psi = quat2psi(q)
% arctan2(q1q2+q0q3,1/2-(q2^2+q3^2))
    psi   = atan2( (q(:,2).*q(:,3) + q(:,1).*q(:,4) ), ...
                   1/2 - (q(:,3).^2 + q(:,4).^2) );
end

% Version 2 (it is the same):
% https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
%     phi   = atan2( 2*(q(:,1).*q(:,2) + q(:,3).*q(:,4) ), ...
%                    1 - 2*(q(:,2).^2 + q(:,3).^2) );
%     sinarg = 2*(q(:,1).*q(:,3) - q(:,2).*q(:,4));
%     psi   = atan2( 2*(q(:,1).*q(:,4) + q(:,2).*q(:,3) ), ...
%                    1 - 2*(q(:,3).^2 + q(:,4).^2) );
