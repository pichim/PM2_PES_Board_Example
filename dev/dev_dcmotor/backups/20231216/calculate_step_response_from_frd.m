function step_resp = calculate_step_response_from_frd(G, f_max)

    g = squeeze(G.ResponseData);
    if isnan(abs(g(1))) % Todo: interpolate based on point 2 and 3
        g(1) = g(2);
    end
    
    freq = G.Frequency;
    g(freq >= f_max & freq <= freq(end) - f_max + freq(2)) = 0;
    
    step_resp = cumsum(real(ifft(g)));

end