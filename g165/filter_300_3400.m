function Hd = filter_300_3400
%FILTER_300_3400 Returns a discrete-time filter object

%
% M-File generated by MATLAB(R) 7.0.1 and the Signal Processing Toolbox 6.2.1.
%
% Generated on: 17-Mar-2009 15:39:00
%

% FIR Window Bandpass filter designed using the FIR1 function.

% All frequency values are in Hz.
Fs = 8000;  % Sampling Frequency

Fstop1 = 200;             % First Stopband Frequency
Fpass1 = 300;             % First Passband Frequency
Fpass2 = 3400;            % Second Passband Frequency
Fstop2 = 3500;            % Second Stopband Frequency
Dstop1 = 0.001;           % First Stopband Attenuation
Dpass  = 0.057501127785;  % Passband Ripple
Dstop2 = 0.0001;          % Second Stopband Attenuation
flag   = 'noscale';       % Sampling Flag

% Calculate the order from the parameters using KAISERORD.
[N,Wn,BETA,TYPE] = kaiserord([Fstop1 Fpass1 Fpass2 Fstop2]/(Fs/2), [0 ...
                             1 0], [Dstop1 Dpass Dstop2]);

% Calculate the coefficients using the FIR1 function.
b  = fir1(N, Wn, TYPE, kaiser(N+1, BETA), flag);
Hd = dfilt.dffir(b);


% [EOF]