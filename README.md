# Customizable Sound Board

The purpose of this project is to give an amateur DJ a makeshift, cheap, and customizable soundboard.

Custom sound files can be added and wired to select keys
- Sound sets can be quickly toggled
- Soundboard is highly customizable, since additional modules can be added
- Functionality is easily extensible with user-friendly Python API’s

## HARDWARE REQUIRMENTS
- Basys 3 Artix-7 FPGA Trainer Board [Buy Here](https://digilent.com/shop/basys-3-artix-7-fpga-trainer-board-recommended-for-introductory-users/)
- Pmod KYPD [Buy Here](https://digilent.com/shop/pmod-kypd-16-button-keypad/)
- Pmod OLED [Buy Here](https://digilent.com/shop/pmod-oled-128-x-32-pixel-monochromatic-oled-display/)
- Pmod BT2 [Buy Here](https://digilent.com/shop/pmod-bt2-bluetooth-interface/)

## Project Description
![Alt text](.md/block_diagram.jpg)
* Microblaze Soft CPU reads keypad inputs
    - sends byte encodings to Python script over Bluetooth
    - sends text description of sound to OLED display
* Python uses byte encodings to toggle sound profiles or determine sound to play
    - spawns new thread to play each sound
    - loads new sound set from file system when sound profile is toggled


## Algorithms
* FPGA Board runs an infinite loop:
    - checks for inputs from keypad
    - sends byte encoding of key pressed over bluetooth
    - sends text description of sound to OLED
* key presses are communicated over bluetooth as single byte values that can be decoded to determine sound to play and sound set to select
* connected PC uses a Python script to listen to the byte signals and carry out the appropriate action. These can be:
    - change sound profile 
    - play sound

## Software Requirements
* Python3
* Configure PMODS on FPGA 
* Configure Microblaze Soft CPU on FPGA [Resource](https://digilent.com/reference/learn/programmable-logic/tutorials/basys-3-getting-started-with-microblaze/start)

## Software (Board Side) [./board]
* Written in C and runs on Microblaze
    - initializes PMOD modules and global variables
    - sends a synchronization signal to connected PC
        - enters infinite while loop
    - writes description of sound set and sound to PMOD based on last key pressed
    - checks keypad state
        - if key pressed, sends byte encoding to PC over bluetooth
    - checks button state
        - if button pressed, sends byte encoding that tells PC to switch sound set

## Software (Client Side) [./client]
- sound files are prefetched and are stored in a LUT, ready to be played (Main Thread)
- uses pySerial library to listen to Bluetooth COMM port (Main Thread)
- A pool of threads are spawned, ready to play sounds when an appropriate signal is received (Main Thread)
- If signal received is = b’N’, the sound set is changed and its contents are preloaded (Main Thread)
- If signal received is = b’M’, the sound set is changed to set 0 and its contents are preloaded (Main Thread)
- For any other signals, a thread from the pool is assigned a task to play a sound based on input. (Worker Thread)
- This task utilizes the LUT to fetch the appropriate file object and play it on the device’s speakers.

> Note: Since the program is multithreaded, sounds can overlap.

