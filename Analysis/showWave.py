# wavファイルを読み込んで、波形を表示する。
# 第一引数にwavファイルのパス
# 第二、第三引数にグラフのx軸始端、終端を指定することで一部分を切り取ってグラフ表示可能
# 指定しない場合は、全体が表示される。

import wave
import numpy as np
import matplotlib.pyplot as plt
import csv
import sys
import matplotlib as mpl
mpl.rcParams['agg.path.chunksize'] = 100000

def set_graph(fig, ax):
    ax.set_xlabel("Time", fontweight=600, size=26, color='k')
    ax.set_ylabel("Amplitude",
                  fontweight=600, size=26, color='k')
    ax.tick_params(axis='x', labelsize=20)
    ax.tick_params(axis='y', labelsize=20)
    ax.grid(alpha=0.8, zorder=1)

path = sys.argv[1]
wf = wave.open(path, "r")
buf = wf.readframes(wf.getnframes())
fr = wf.getframerate()

data1 = np.frombuffer(buf,dtype="int16")

data = [0]*len(data1)
for i in range(len(data1)):
     data[i] = data1[i]

x = [i/44100 for i in range(len(data))]

print(len(sys.argv))
if len(sys.argv) == 4:
    start_time = sys.argv[2]
    end_time = sys.argv[3]
else:
    start_time = 0
    end_time = x[-1]

print(data1[44100:44200])

fig = plt.figure(figsize=(14, 4))
ax = fig.add_subplot()

set_graph(fig,ax)

# ax.set_xticks(np.arange(0,1,0.1))
ax.set_xlim(xmin=start_time,xmax=end_time)
# ax.set_yticks(np.arange(0,0.20,0.05))
# ax.set_ylim(ymin=0,ymax=5500/peak)
ax.plot(x, data)
plt.show()
# fig.savefig("../Record/" + path + "_wave.pdf")
