% model = './subsys.slx';
% load_system(model)
% sim(model)
% 
load helidata
rpm=rpm';
t = 1:1:5001;
noise_a = rand(0.5,1,5001);
noise_b = -0.8 * noise_a;
noise_f = noise_a + noise_b;
noise_c = rand(0.8,1,5001);
noise_d = -0.7  * noise_c;
noise_e = noise_c+
rpm1 = 1./(1+exp(-t*(0.001)) .* rpm + 100./(1.+exp(-t.*0.001));
plot(t,rpm)
hold on
plot(t,rpm1,'o')
hold off