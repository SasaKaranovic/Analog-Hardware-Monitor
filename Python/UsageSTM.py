import time, signal, serial, struct
import psutil
import GPUtil

SLEEP_SECONDS = 0.5

# Network usage calc
NET_MBps = 300  # MegaBITS per second, use something like https://speedtest.net to find out
NET_MAX_BPS = NET_MBps * 125000  # bytes per second
g_networkBytes = 0

# catch ctrl-c
def ctrl_c_handler(signal, frame):
    print('Goodbye!')
    UpdateGauges(0, 0, 0, 0)
    exit()

def invalidRange(dialRange):
    if (dialRange < 0) or (dialRange > 100):
        print("Invalid dial range ! [ 0 <= dialRange <= 100 ]!")
        return True
    else:
        return False


# Update all four gauges
def UpdateGauges(dial1, dial2, dial3, dial4):
    if invalidRange(dial1):
        return
    elif invalidRange(dial2):
        return
    elif invalidRange(dial3):
        return
    elif invalidRange(dial4):
        return

    print("CPU=%3d MEM=%3d NET=%3d GPU=%3d" % (dial1, dial2, dial3, dial4))

    sendString = "[s:%d:%d:%d:%d]\r\n" % (dial1, dial2, dial3, dial4)
    print(sendString)
    ser.write(sendString.encode())
    print("\r\n")


def CycleDial(step=1, delay=0.5):
    # Increment
    for i in range(0,101,step):
        UpdateGauges(i, i, i, i)
        time.sleep(delay)

    # Decrement
    for i in range(0,101,step):
        UpdateGauges(100-i, 100-i, 100-i, 100-i)
        time.sleep(delay)

def cpu_usage():
    return int(psutil.cpu_percent())

def memory_usage():
    return int(psutil.virtual_memory().percent)

def network_usage():
    global g_networkBytes
    net_total = (psutil.net_io_counters().bytes_sent + psutil.net_io_counters().bytes_recv)

    # Edge case, on first start capture net status 
    # so we can use it in the next iteration of the loop
    if g_networkBytes == 0:
        g_networkBytes = net_total
        percent = 0
    else:
        bytesPerSec = (net_total - g_networkBytes) / SLEEP_SECONDS
        percent = bytesPerSec / NET_MAX_BPS * 100
        if percent > 100:
            percent = 100

    g_networkBytes = net_total
    return int(percent)

def gpu_usage():
    GPUs = GPUtil.getGPUs()
    return int(GPUs[0].load*100)


def main():

    global ser
    ser = serial.Serial('COM16', 115200, timeout=0, parity=serial.PARITY_EVEN, rtscts=0)

    # while True:
    #     CycleDial(5, 0.1)
    # exit()

    CycleDial(5, 0.1)
    while True:
        UpdateGauges(cpu_usage(), memory_usage(), network_usage(), gpu_usage())
        time.sleep(SLEEP_SECONDS)
    exit();


if __name__ == '__main__':
    signal.signal(signal.SIGINT, ctrl_c_handler)
    main()

