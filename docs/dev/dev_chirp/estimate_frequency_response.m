function [G, C, freq, Pavg] = estimate_frequency_response(inp, out, window, Noverlap, Nest, Ts, delta)

    if (~exist('delta', 'var') || isempty(delta))
        delta = 0.0;
    end

    % Todo: check if it is usefull to remove mean here
    inp = inp - mean(inp);
    out = out - mean(out);

    Ndata = size(inp, 1);

    % factor 2 so that the magnitude corresponds to a single sided spectrum
    % 2.3*sin(2*pi*f0*time) <=> sqrt(puu(f0)) = 2.3
    W = sum(window) / Nest / 2;

    Pavg = zeros(Nest, 3);

    Navg = 0;
    ind_start = 1;
    ind_end   = Nest;
    Ndelta   = Nest - Noverlap;
    while ind_end <= Ndata

        ind = ind_start:ind_end;

        inp_act = inp(ind);
        out_act = out(ind);

        % Todo: check if it is usefull to remove mean here
        inp_act = inp_act - mean(inp_act);
        out_act = out_act - mean(out_act);

        inp_act = window .* inp_act;
        out_act = window .* out_act;

        U = fft(inp_act) / (Nest * W);
        Y = fft(out_act) / (Nest * W);

        Pavg = Pavg + [U.*conj(U) Y.*conj(U) Y.*conj(Y)];
        Navg = Navg + 1;

        ind_start = ind_start + Ndelta;
        ind_end   = ind_end   + Ndelta;

    end

    Pavg = Pavg / Navg;

    [g, c] = calc_freqresp_and_cohere(Pavg, delta);
    df = 1/(Nest*Ts);
    freq = (0:df:1/Ts-df).';

    % adjusted to work with zhaw stuff
    ind = freq <= 1/2/Ts;
    freq = freq(ind);
    g = g(ind);
    c = c(ind);

    % average power spectras
    G = frd(g, freq, Ts, 'Units', 'Hz');
    C = frd(c, freq, Ts, 'Units', 'Hz');

end

function [g, c] = calc_freqresp_and_cohere(P, delta)

    P(:,1) = P(:,1) + delta;

    g = P(:,2) ./ P(:,1);
    c = abs(P(:,2)).^2 ./ (P(:,1) .* P(:,3));

end
