%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Computes the success probabilty with various parameters
%s is the expected number of right quartets 
%s1 is the threshold, keys whose counters are larger than s1 are keeped
%F is the cumulative distribution function of Poisson distribution
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

digits(20);

%32-round attack on SKINNY-128-384
s=10;
s1=[0,1,2,3,4,5,6,7,8,9];

%the expected number of right quartets lamda_R=s
lamda_R=s;

%the expected value of the counter for wrong key guessing,lamda_W=s/(2^{2n}*2^{-n}*p^2*q^2*r)
lamda_W=s*power(2,-12.91);

%the success rate, P_s=1-F(s1,lamda_R)
P_s=1-poisscdf(s1,lamda_R)

%the advantage h, 2^{-h}=1-F(s1,lamda_W)
h=vpa(sym(-log2(poisscdf(s1,lamda_W,'upper'))),6) 

%17-round attack on SKINNY-64-128
s=1;

s1=[0,1];

lamda_R=s;

lamda_W=s*power(2,-60);

P_s=1-poisscdf(s1,lamda_R)

h=vpa(sym(-log2(poisscdf(s1,lamda_W,'upper'))),6)

