import numpy as np
import wave
import sys

class SoundMaker:
    def __init__(self, amp=0.3, f0=1700, f1=1800, duration=0.1, rate=48000, total_time = 30, send_cycle=1, fname='test_sound'):
        self.amp = amp
        self.f0 = f0
        self.f1 = f1
        self.duration = duration
        self.rate = rate
        self.send_cycle = send_cycle
        self.total_time = total_time
        self.samples = self.makeWave()

        self.fname = fname + ".wav"
        self.wavefile = self.prepareFile()

    def prepareFile(self):
        wavefile = wave.open(self.fname, 'wb')
        wavefile.setnchannels(1)
        wavefile.setsampwidth(2)
        wavefile.setframerate(self.rate)

        return wavefile

    def makeWave(self):
        signal = np.array([])
        samples = np.array([0]*(self.total_time*self.rate+self.rate))
        for i in range(int(self.rate * self.duration)):
            n = i/self.rate
            signal =  np.append(signal, self.amp * 10000 * np.sin(2*np.pi * n * (1700+((1800-1700)/(2*0.1))*n)))
        
        for i in range(self.total_time+1):        
            for j in range(self.send_cycle*self.rate):
                if i >= 1:
                    if len(signal) > j:
                        samples[(i*self.rate) + j] = signal[j]
                    else:
                        samples[(i*self.rate) + j] = 0
                else:
                    samples[(i*self.rate) + j] = 0

        return samples
    
    def writeWave(self, source):
        source = source.astype(np.int16)
        self.wavefile.writeframes(source)
        self.wavefile.close()

if __name__ == '__main__':
    amp = float(sys.argv[1])
    total_time = int(sys.argv[2])    
    maker = SoundMaker(amp=amp,total_time=total_time)
    sound_source = maker.makeWave()
    maker.writeWave(sound_source)

