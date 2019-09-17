from pyBusPirateLite.I2C import *
import time, signal
import psutil

SLEEP_SECONDS = 0.2

# I2C related "constants"
I2C_ADDRESS = 0xC0
CONTINUOUS_WRITE = 0x50
BIT_VREF = (1 << 7)


# Network usage calc
NET_MBps = 250  # MegaBITS per second, use something like https://speedtest.net to find out
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

# Calculate value to be transmitted over I2C
def percentToDAC(percent):
    value = int((percent)*4095/100)
    upper = (((value>>8) & 0xFF) | BIT_VREF)
    lower = (value & 0xFF)
    return [upper, lower]

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
    
    sendArray = [I2C_ADDRESS, CONTINUOUS_WRITE]
    sendArray.extend(percentToDAC(dial1))
    sendArray.extend(percentToDAC(dial2))
    sendArray.extend(percentToDAC(dial3))
    sendArray.extend(percentToDAC(dial4))

    print("CPU=%3d MEM=%3d NET=%3d" % (dial1, dial2, dial3))

    # print(sendArray)
    # print(''.join('0x{:02x} '.format(x) for x in sendArray))
    # exit()

    i2c.start()
    i2c.transfer(sendArray)
    i2c.stop()

def CycleSKDial(step=1, delay=0.5):
    # Increment
    for i in range(0,101,step):
        UpdateGauges(i, i, i, i)
        time.sleep(delay)

    # Decrement
    for i in range(0,101,step):
        UpdateGauges(100-i, 100-i, 100-i, 100-i)
        time.sleep(delay)

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
    return psutil.cpu_percent()

def memory_usage():
    return psutil.virtual_memory().percent

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


def main():
    global i2c
    i2c = I2C()
    i2c.speed = '50kHz'
    i2c.configure(power=True, pullup=True, aux=False, cs=False)

    while True:
        UpdateGauges(cpu_usage(), memory_usage(), network_usage(), 0)
        time.sleep(SLEEP_SECONDS)
    exit();

    # UpdateGauges(0, 0, 0, 0)

    # CycleDial(20, 1)
    # CycleDial(10, 0.5)
    # CycleDial(10, 0.1)
    # Reset()

if __name__ == '__main__':
    signal.signal(signal.SIGINT, ctrl_c_handler)
    main()

