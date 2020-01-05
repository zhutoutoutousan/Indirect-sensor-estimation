#  Author: Tian Shao
#  Email: amazingostian@gmail.com
#  Function: Create a class that contains these functions:


import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import odeint
from scipy import signal



class signalgeneration:

#--------------------------DRAFT--------------------------
    # def __init__(self, Fs=10e3, N=10e5, SignalType="cos", \
    #     phase0=2.5, CosOst=4000, CosAmp=200):

    #     self.Fs = Fs
    #     self.N  = N
    #     self.SignalType = SignalType
    #     self.phase0 = phase0
    #     self.CosOst = CosOst
    #     self.CosAmp = CosAmp
    # def __init__(self,)
#--------------------------DRAFT--------------------------

#-----------------Worked version for *argv in class-------------
# class foo:
#     def __init__(self,*argv):
#         self.arg1 = argv[0]
#         self.arg2 = argv[1]
#         self.arg3 = argv[2]
#     def myFun(self): 
#         print("arg1:", self.arg1) 
#         print("arg2:", self.arg2) 
#         print("arg3:", self.arg3) 
      
# # Now we can use *args or **kwargs to 
# # pass arguments to this function :  
# args = ("Geeks", "for", "Geeks") 
# f = foo(*args) 
# f.myFun()
# # kwargs = {"arg1" : "Geeks", "arg2" : "for", "arg3" : "Geeks"} 
# # foo(**kwargs) 
#-----------------Worked version for *argv in class-------------

# Recommendations for this code: Try to re-write it with
# **kwargs, __dict__ and for loops to make it look wiser.


    def __init__(self,*argv):

        if "cos" in argv:
            self.Fs = argv[0]
            self.N  = argv[1]
            self.SignalType = argv[2]
            self.phase0 = argv[3]
            self.CosOst = argv[4]
            self.CosAmp = argv[5]

        elif "lin" in argv:
            self.Fs = argv[0]
            self.N  = argv[1]
            self.SignalType = argv[2]
            self.Start = argv[3]
            self.End = argv[4]

        elif "motorsim" in argv:
            self.Fs = argv[0]
            self.N  = argv[1]
            self.SignalType = argv[2]
            self.StartDuration = argv[3]
            self.OperateDuration = argv[4]
            self.StopDuration = argv[5] 
            self.Kp = argv[6]
            self.taup = argv[7]
            self.max = argv[8]

        else:
            print("Error: No signal type requested available"\
                +"or not enough arguments")


    def GenerateRpm(self):
        TypeBuff = self.SignalType
        t = np.asarray(np.arange(self.N))/self.Fs
        if TypeBuff=="cos":
            rpm = self.CosOst + self.CosAmp * np.cos(np.array(t))

        elif TypeBuff=="lin":
            rpm = np.asarray(np.linspace(self.Start,self.End,\
                num=self.N))
        elif TypeBuff=="motorsim":
            # Duration is not yet defined.
            #if self.StartDuration+self.OperateDuration+\
            #    self.StopDuration == self.N/self.Fs:
            def model3(y,t):
                u=1
                return(-y + self.Kp * u)/self.taup
            y1 = odeint(model3,0,t[0:t.size//2])
            y2 = np.flip(y1)
            rpm = np.r_[y1,y2]
        else:
            print("Input error: Duration and sample don't \
                match")
        return rpm


    def rpm2vib(self,rpm,ord):
        t = np.asarray(np.arange(self.N))/self.Fs
        phase = 2 * np.pi * np.true_divide(rpm,10) 
        amp = rpm
        out = amp * np.cos(2 * np.pi * self.Fs * t + 2.5\
             + ord * rpm )
        return out
    
    def STFFT(self,out,window=1000,mmin=0,mmax):
        f, t, Zxx = signal.stft(out,self.Fs,nperseg=window)
        Z = np.abs(Zxx)
        plt.pcolormesh(t,f,Z,vmin=mmin,vmax=mmax)
        plt.title("STFT magnitude")
        plt.ylabel("Frequency[Hz]")
        plt.xlabel("Time[sec]")
        plt.show()
        
        return Z
    
    def peakextract(Zin,f,t):
        # Zin is a m \times n matrix, and f is column and t is row
        

    def datacleaning():
    

