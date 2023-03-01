import playsound
# from pydub import AudioSegment
# from pydub.playback import play

# low_acoustic = AudioSegment.from_wav("sounds/1-snare/low-acoustic.wav")

while True:
    key = input('enter: ')
    key = int(key)

    if (key == 1):
        playsound.playsound('./sounds/1-snare/low-acoustic.mp3')
        # play(low_acoustic)
        
    elif (key == 2):
        playsound.playsound('./sounds/1-snare/snare.mp3')

    elif (key == 3):
        playsound.playsound('./sounds/1-snare/subtle-reverb.mp3')

    elif (key == 4):
        playsound.playsound('./sounds/1-snare/treble-heavy.mp3')

    elif (key == 5):
        playsound.playsound('./sounds/2-hi-hat/closed.mp3')

    elif (key == 6):
        playsound.playsound('sounds/2-hi-hat/foot.mp3')
    
    elif (key == 7):
        playsound.playsound('sounds/2-hi-hat/open.mp3')
    
    elif (key == 8):
        playsound.playsound('sounds/2-hi-hat/open2.mov')

    elif (key == 9):
        playsound.playsound('sounds/3-cymballs/china-cymball-crash.mov')

    elif (key == 10):
        playsound.playsound('sounds/3-cymballs/crash-cymball-hit.mov')

    elif (key == 11):
        playsound.playsound('sounds/3-cymballs/ride-cymball.mov')
    
    elif (key == 12):
        playsound.playsound('sounds/3-cymballs/spash-cymball.mov')

    elif (key == 13):
        playsound.playsound('sounds/4-misc/bass.mp3')

    elif (key == 14):
        playsound.playsound('sounds/4-misc/drum-sticks.mp3')

    elif (key == 15):
        playsound.playsound('sounds/4-misc/floor-tom.mp3')

    elif (key == 16):
        playsound.playsound('sounds/4-misc/small-tom.mp3')

# playsound.playsound('sounds/cymballs/china-cymball-crash.mov')
# playsound.playsound('sounds/cymballs/crash-cymball-hit.mp3')
# playsound.playsound('sounds/cymballs/ride-cymball.mp3')
# playsound.playsound('sounds/cymballs/splash-cymball-hit.mp3')