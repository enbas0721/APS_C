import wave
import numpy as np
import matplotlib.pyplot as plt
import csv
import sys
import matplotlib as mpl
mpl.rcParams['agg.path.chunksize'] = 100000

def readcsv(path):
    with open(path) as f:
        reader = csv.reader(f)
        data = []
        for row in reader:
            if row[0] != "amp":
                data.append(int(row[0]))

    return data

def set_graph(fig, ax):
    ax.set_xlabel("Time", fontweight=600, size=26, color='k')
    ax.set_ylabel("Amplitude",
                  fontweight=600, size=26, color='k')
    ax.tick_params(axis='x', labelsize=20)
    ax.tick_params(axis='y', labelsize=20)
    ax.grid(alpha=0.8, zorder=1)

path = sys.argv[1]
start = float(sys.argv[2])
end = float(sys.argv[3])

data = readcsv(path)

x = []
for i in range(len(data)):
    x.append(i)

fig = plt.figure(figsize=(14, 4))
ax = fig.add_subplot()

set_graph(fig,ax)

ax.set_xticks(np.arange(start,end,1000))
ax.set_xlim(xmin=start,xmax=end)
ax.plot(x, data)
plt.show()
# fig.savefig("../Record/" + path + "_wave.pdf")