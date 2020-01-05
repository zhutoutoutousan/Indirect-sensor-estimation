#!/usr/bin/env python
# coding: utf-8

# In[43]:


import numpy as np
from matplotlib import pyplot as plt
from scipy import signal


# In[202]:


fs = 10e3  # Hz
N = 10e5
t = np.asarray(np.arange(N))/fs

sampling_num = 15000
sampling_rat = 15000

phase0 = 2.5
rpm = 4000 + 200 * np.cos(np.array(t))  # t = (np.asarray(np.linspace(1,45,num=5000)))
amp = rpm / 10

out = 100 * amp * np.cos(2 * np.pi * 1e3 * t  + phase0 + rpm)

plt.plot(out)
plt.show()



# In[203]:


2 * np.pi * fs * t


# In[204]:


rpm


# In[205]:


amp


# In[206]:


# t = np.asarray(np.arange(5000))
# fs = 500  # Hz

# phase0 = 2.5

# phasec = 2 * np.pi * fs * t

# rpm = 125 * np.cos(np.asarray(phasec))  # t = (np.asarray(np.linspace(1,45,num=5000)))

# amp = rpm / 10

# phase = amp

# out = 100 * amp * np.cos(2 * np.pi * phase * t  + phase0)

# plt.plot(out)
# plt.show()

plt.plot(rpm)
plt.show()


# In[207]:


ampm = np.amax(amp)
ampm


# In[210]:


# t = np.asarray(np.arange(5000))
# fs = 500  # Hz

# phase0 = 2.5
# rpm = 1000 + 125 * np.cos(2 * np.pi * fs * (np.asarray(np.linspace(1,36,num=5000))))  # t = (np.asarray(np.linspace(1,45,num=5000)))

# amp = rpm / 10

# phase = phase0 + rpm / 10

# out = amp * np.cos(phase * (np.asarray(np.linspace(1,36,num=5000))))

f, t, Zxx = signal.stft(out, fs, nperseg=2000)
plt.pcolormesh(t, f,  np.abs(Zxx),  vmin=0, vmax=10 * ampm)
plt.title('IF')
plt.ylim(0,2000)
plt.ylabel('Frequency[Hz]')
plt.xlabel('Time[sec]')
plt.show()


# In[4]:


out.size


# In[5]:


s = np.random.standard_cauchy(1000000)
s = s[(s>-25) & (s<25)]  # truncate distribution so it plots well
plt.hist(s, bins=100)
plt.show()

