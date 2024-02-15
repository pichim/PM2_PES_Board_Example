function xf = apply_rotfiltfilt(G, sinarg, x)

    % signal size
    [Nx, nx] = size(x);
    xf = zeros(Nx, nx);
    p = exp(1j*(sinarg));
    
    for i = 1:nx
        % eliminate mean
        y = x(:,i) - mean(x(:,i));
        yR = y .* p;
        yQ = y .* conj(p);
        % filtering in transformed coordinates
        yR = filtfilt(G.num{1}, G.den{1}, yR);
        yQ = filtfilt(G.num{1}, G.den{1}, yQ);
        % back transformation
        % xf(:,i) = real((yR.*conj(p) + yQ.*p)*0.5); % scaling does not
        % matter if we build the raio anyways
        xf(:,i) = real((yR.*conj(p) + yQ.*p));
    end

end