from pickle import GLOBAL
import serial
import simpleaudio as sa
import time
import json
from concurrent.futures import ThreadPoolExecutor

SET_PREFIX = "sounds\\set-"
DATA = {}
DATA_LENGTH = 0
GLOBAL_LUT = {}
GLOBAL_LUT_LENGTH = 0
TO_HEX = {
    0: '0', 1: '1', 2: '2', 3: '3',
    4: '4', 5: '5', 6: '6', 7: '7',
    8: '8', 9: '9', 10: 'A', 11: 'B',
    12: 'C', 13: 'D', 14: 'E', 15: 'F'
}
TO_INT = {
    '0': 0,'1': 1,'2': 2,'3': 3,
    '4': 4,'5': 5,'6': 6,'7': 7,
    '8': 8,'9': 9,'A': 10,'B': 11,
    'C': 12,'D': 13,'E': 14,'F': 15
}

def load_json():
    f = open('./sounds.json')
    global DATA
    global DATA_LENGTH
    DATA = json.load(f)
    DATA_LENGTH = len(DATA)


def load_data(set_num):
    global SET_PREFIX
    global GLOBAL_LUT
    global GLOBAL_LUT_LENGTH
    set_name = SET_PREFIX + str(set_num)
    set_sounds = DATA[set_name]
    for x in range(len(set_sounds)):
        hex_char = TO_HEX[x]
        filename = set_sounds[x]
        wave_obj = sa.WaveObject.from_wave_file(filename)
        print(f"Preloading {filename}")
        GLOBAL_LUT[hex_char] = wave_obj
        GLOBAL_LUT_LENGTH = x + 1

def play_sound(key):
    global GLOBAL_LUT_LENGTH
    print(f"Received {key} from BT")
    key_i = TO_INT[key]
    if key_i < GLOBAL_LUT_LENGTH:
        GLOBAL_LUT[key].play()
        time.sleep(0.1)



def main():
    set_num = 0
    load_json()
    load_data(set_num)
    s = serial.Serial('COM3', 9600)
    executor = ThreadPoolExecutor(16)
    try:
        while 1:
            if s.in_waiting:
                res = s.read()
                res_c = res.decode("utf-8")
                if res_c is 'N':
                    set_num = (set_num+1) % (DATA_LENGTH)
                    load_data(set_num)
                elif res_c is 'M':
                    set_num = 0
                    load_data(set_num)
                else:
                    future = executor.submit(play_sound, (res_c))
    except:
        print("Serial Connection error")
    finally:
        s.close()


if __name__ == "__main__":
    main()
