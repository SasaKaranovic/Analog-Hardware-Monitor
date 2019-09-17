import time, signal, serial, struct
import GPUtil

def gpu_usage():
    GPUs = GPUtil.getGPUs()
    print("GPU LOAD {}".format(GPUs[0].load*100))
    return int(GPUs[0].load*100)


while True:
	gpu_usage()
	time.sleep(0.5)
