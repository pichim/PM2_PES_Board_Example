function [time, step_resp] = get_step_resp_from_frd(G, Ts, f_max, df)
% function [time, step_resp] = get_step_resp_from_frd(G, Ts, f_max, df)
% function [time, step_resp] = get_step_resp_from_frd(G, Ts)
%   f_max <= f_nyq : (0, f_max) considered for step response (default: f_nyq)
%   df             : frequency resolution in hz (default: 1)

    % handle function call without f_max specified
    if (~exist('f_max', 'var') || isempty(f_max))
        f_max = max(G.Frequency);
    end

    % handle function call without df specified
    if (~exist('df', 'var') || isempty(df))
        df = 1.0; % hz
    end

    % constrain f_max <= f_nyq
    f_max = min([f_max, 1/(2*Ts)]);

    % interpolate frequency response data from (0, f_max)
    freq = (0:df:1/Ts).';
    g = zeros(size(freq));
    ind_ip = freq <= f_max;
    % g(ind_ip) = squeeze(freqresp(G, 2*pi*freq(ind_ip)));
    g(ind_ip) = interp1(G.Frequency, squeeze(G.ResponseData), ...
        freq(ind_ip), 'linear', 'extrap');
    g(1) = abs(g(2));
   
    % calculate step response data
    % - since we use ifft with the option 'symmetric' there is no need to
    %   fill g_ip with the compl. conj.
    step_resp = cumsum(real(ifft( g, 'symmetric' )));
    time = (0:length(step_resp)-1).' * Ts;

end

