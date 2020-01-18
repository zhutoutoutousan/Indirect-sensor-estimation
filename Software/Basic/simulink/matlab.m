% model = './subsys.slx';
% load_system(model)
% sim(model)
% 

a = 1:1:5000;
b = ones(1,3000).*5000;
c = fliplr(a);
d = [a b c];
x = 1:1:13000;
s = zeros(1,13000)
for t=1:1:13000
    s(t)=1/(1+exp(t/100));
end
noise1 = s(t).* randi(1000000,1,13000);
noise2 = s(t).* -randi(1000000,1,13000);
noise = noise1 + noise2
wave = noise + d;
p = polyfit(x,d,7)
x1 = linspace(1,13000);
y1 = polyval(p,x1);
plot(x,d)
hold on
plot(x,wave,'o')
hold off
load helidata
rpm=rpm';
t = 1:1:5001;
noise_a = 0.005 * rand(1,5001);
noise_b = -0.8 * noise_a;
noise_f = noise_a + noise_b;
noise_c = 0.0001 * rand(1,5001);
noise_d = -0.7  * noise_c;
noise_e = noise_c + noise_d;
rpm1 = 1./(1+exp(-t.*(1+noise_f))) .* rpm + 100./(1.+exp(-t.*(1+noise_e)));
plot(t,100*(rpm-rpm1)./rpm)
% plot(t,rpm)
% hold on
% plot(t,rpm1,'r')
% hold off
