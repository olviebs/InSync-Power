import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import pandas as pd
import time

showGraph = False

df = pd.read_csv('Bus2_Competition_Data.csv', usecols=['BUS2_VA_ANG', 'BUS2_VA_MAG', 'BUS2_Freq']) # df.iat[0,0] = ANG, df.iat[0,1] = MAG, df.iat[0,2] = FREQ

T0 = 0 # start time
TE = 0.033 # time after 1 30hz cycle

PI = np.pi

TIMERES = 1000 # number of values per 2 cycles of 60hz output wave

ROW = 0

FREQ = df.iat[ROW,2] # initial frequency
ANG = df.iat[ROW,0] # initial angle
MAG = df.iat[ROW,1] # initial magnitute

fig = plt.figure()

ax1 = fig.add_subplot(1,1,1)

ts = np.linspace(T0, TE, 1000)
print(len(ts))

def waveConditioning (mag, freq, ang):
    global PI
    ys = np.sin((2*PI*freq*ts)+(ang*(PI/180))) # ys is a list with 1000 entries, # of entries per 2 periods can be edited with TIMERES constant
    ys = ys * mag *(128/229173.491)
    ys = ys + 128
    ys = ys.astype(int)
    return ys

if showGraph == True:
    def animate(i):
        global T0, TE, FREQ, MAG, ROW, ANG, TIMERES
        ys = waveConditioning(MAG, FREQ, ANG)
        ax1.clear()
        ax1.plot(ts,ys)
        for i in range(0,TIMERES):
            print(ys[i])
            time.sleep(TE/TIMERES)# when using raspberry pi, switch this to nanosleep functions
        ROW += 1
        MAG = df.iat[ROW,1]
        FREQ = df.iat[ROW,2]
        ANG = df.iat[ROW,0]
    # code to initialize visualizer. Not required for actual use, comment out when testing with device
    ani = animation.FuncAnimation(fig,animate, interval=33) # interval dictates how much time between graph updates in ms
    plt.show()
elif showGraph == False:
    while True:
        ys = waveConditioning(MAG, FREQ, ANG)
        timestart = time.time_ns()
        for i in range(0,TIMERES):
            print(f"{ys[i]}:{time.time_ns()}")
            time.sleep(TE/TIMERES) # when using raspberry pi, switch this to nanosleep functions
        ROW += 1
        MAG = df.iat[ROW,1]
        FREQ = df.iat[ROW,2]
        ANG = df.iat[ROW,0]
else:
    print("Please select True or False for showGraph in line 7")