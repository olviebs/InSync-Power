import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import pandas as pd
import time

df = pd.read_csv('Bus2_Competition_Data.csv', usecols=['BUS2_VA_ANG', 'BUS2_VA_MAG', 'BUS2_Freq']) # df.iat[0,0] = ANG, df.iat[0,1] = MAG, df.iat[0,2] = FREQ

T0 = 0 # start time

PI = np.pi

TIMERES = 30300

ROW = 0

FREQ = df.iat[ROW,2] # initial frequency
ANG = df.iat[ROW,0] # initial angle
MAG = df.iat[ROW,1] # initial magnitute

TE = 0.033 # corresponds to PMU sampling rate

fig = plt.figure()

ax1 = fig.add_subplot(1,1,1)

ts = np.arange(T0, TE, (1/TIMERES))

def animate(i):
    global T0, TE, FREQ, MAG, ROW, ANG
    ys = MAG *(3.3/217449.8633)* np.sin((2*PI*FREQ*ts)+(ANG*(PI/180))) # ys is a list with 1000 entries, # of entries per 2 periods can be edited with TIMERES constant
    ax1.clear()
    ax1.plot(ts,ys)
    for i in range(0,int(TE*TIMERES)+1):
        print(ys[i])
        time.sleep(1/TIMERES)
    ROW += 1
    MAG = df.iat[ROW,1]
    FREQ = df.iat[ROW,2]
    ANG = df.iat[ROW,0]
    
# code to initialize visualizer. Not required for actual use, comment out when testing with device
ani = animation.FuncAnimation(fig,animate, interval=33) # set to 33 interval for accurate readings
plt.show()