function CBE = quat2CBE(q)
% q = [qw qx qy qz], ||q|| = 1

% CBE := R^T

% CBE = [[qw^2 + qx^2 - qy^2 - qz^2,         2*(qx*qy + qw*qz),         2*(qx*qz - qw*qy)]; ...
%        [        2*(qx*qy - qw*qz), qw^2 - qx^2 + qy^2 - qz^2,         2*(qy*qz + qw*qx)]; ...
%        [        2*(qx*qz + qw*qy),         2*(qy*qz - qw*qx), qw^2 - qx^2 - qy^2 + qz^2]];

CBE = [[q(1)^2 + q(2)^2 - q(3)^2 - q(4)^2,         2*(q(2)*q(3) + q(1)*q(4)),         2*(q(2)*q(4) - q(1)*q(3))]; ...
       [        2*(q(2)*q(3) - q(1)*q(4)), q(1)^2 - q(2)^2 + q(3)^2 - q(4)^2,         2*(q(3)*q(4) + q(1)*q(2))]; ...
       [        2*(q(2)*q(4) + q(1)*q(3)),         2*(q(3)*q(4) - q(1)*q(2)), q(1)^2 - q(2)^2 - q(3)^2 + q(4)^2]];

return