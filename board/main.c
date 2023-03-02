
#include "PmodBT2.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "PmodKYPD.h"
#include "PmodOLED.h"
#include "sleep.h"
#include "xgpio.h"

// Required definitions for sending & receiving data over host board's UART port
#ifdef __MICROBLAZE__
#include "xuartlite.h"
typedef XUartLite SysUart;
#define SysUart_Send            XUartLite_Send
#define SysUart_Recv            XUartLite_Recv
#define SYS_UART_DEVICE_ID      XPAR_AXI_UARTLITE_0_DEVICE_ID
#define BT2_UART_AXI_CLOCK_FREQ XPAR_CPU_M_AXI_DP_FREQ_HZ
#else
#include "xuartps.h"
typedef XUartPs SysUart;
#define SysUart_Send            XUartPs_Send
#define SysUart_Recv            XUartPs_Recv
#define SYS_UART_DEVICE_ID      XPAR_PS7_UART_1_DEVICE_ID
#define BT2_UART_AXI_CLOCK_FREQ 100000000
#endif

// button stuff
#define BTN_ID XPAR_AXI_GPIO_0_DEVICE_ID
#define BTN_CHANNEL 1

PmodBT2 myDevice_bt;
PmodKYPD myDevice_kypd;
PmodOLED OLEDdevice;
const u8 orientation = 0x0;
const u8 invert = 0x0;
SysUart myUart;
XGpio btn_N;

void Initialize();
void Run();
void SysUartInit();
void EnableCaches();
void DisableCaches();
int char_to_index(u8 last_key);

// keytable is determined as follows (indices shown in Keypad position below)
// 12 13 14 15
// 8  9  10 11
// 4  5  6  7
// 0  1  2  3
#define DEFAULT_KEYTABLE "0FED789C456B123A"

// SOUND NAMES
char sounds[6][16][20] = {
	{"bass", "cymball-crash", "closed", "crash-cymball-hit", "drum-sticks", "floor-tom", "foot", "low-acoustic", "open", "open2", "ride-cymball", "small-tom", "snare", "splash-cymball", "subtle-reverb", "treble-heavy"},
	{"crotch-slam", "fast-jab", "fight-scene-1", "fight-scene-2", "head-crunch", "left-hook", "lofi-karate-kick", "right-cross", "right-hook", "stomach-punch", "temple-bell", "triple-punch", "empty", "empty", "empty", "empty"},
	{"A4", "Ab4", "B4", "Bb4", "C4", "C5", "D4", "D5", "Db4", "Db5", "E4", "Eb4", "Eb5", "F4", "G4", "Gb4"},
	{"electro-bass", "electro-cymbalcrash", "electro-cymbalshort", "electro-laser", "electro-snareclosed", "electro-snareopen", "electro-tomlong", "electro-tomshort", "electroperc-011", "electroperc-017", "electroperc-026", "electroperc-027", "electroperc-028", "electroperc-029", "electroperc-031", "electroperc-038"},
	{"analog_spindown", "badtekno", "biglaser", "bleep_3", "outtacontrol", "phaser-echo-hit", "ping_tsss", "ratatat", "schumacher1", "silly-echozap", "sillymetaltom", "snappy-clappy", "sonar", "starwars", "tricorder", "triplelaser"},
	{"disco-guitar-notes-1", "disco-guitar-notes-2", "disco-guitar-notes-3", "electric_guitarpluck", "fret-me-down", "gtrbend", "gtrchord", "guitar-prettynote-C", "hawaii_guitar", "one-perfect-retro-guitar-chord", "plucking-guitar", "string-blink", "stringslip", "twangybendy", "wah-flange", "wah-hit"}
};

char sets[6][20] = {"rock", "fight", "piano", "electro", "futuristic", "strings"};

int main() {
   Initialize();
   Run();
   DisableCaches();
   return XST_SUCCESS;
}

void Initialize() {
   EnableCaches();
   SysUartInit();
   BT2_Begin (
      &myDevice_bt,
      XPAR_PMODBT2_0_AXI_LITE_GPIO_BASEADDR,
      XPAR_PMODBT2_0_AXI_LITE_UART_BASEADDR,
      BT2_UART_AXI_CLOCK_FREQ,
      115200
   );

   // initialize keypad
   KYPD_begin(&myDevice_kypd, XPAR_PMODKYPD_0_AXI_LITE_GPIO_BASEADDR);
   KYPD_loadKeyTable(&myDevice_kypd, (u8*) DEFAULT_KEYTABLE);

   // initialize buttons
   XGpio_Initialize(&btn_N, BTN_ID);
   XGpio_SetDataDirection (&btn_N, BTN_CHANNEL, 0xffffffff);

   OLED_Begin(&OLEDdevice, XPAR_PMODOLED_0_AXI_LITE_GPIO_BASEADDR,
         XPAR_PMODOLED_0_AXI_LITE_SPI_BASEADDR, orientation, invert);
}

void Run() {
	// kypd stuff
	u16 keystate;
	XStatus status, last_status = KYPD_NO_KEY;
	u8 key, last_key = 'x';
	// Initial value of last_key cannot be contained in loaded KEYTABLE string

	// button value
	u8 btn_val = 0;
	u8 last_btn_val = 0;

	// OLED value
	u8 *pat;
	int set = 0;
	int sound = 0;

	Xil_Out32(myDevice_kypd.GPIO_addr, 0xF);

	// start at set 0
	u8 btn_data = 'M';
    SysUart_Send(&myUart, &btn_data, 1);
    BT2_SendData(&myDevice_bt, &btn_data, 1);

    pat = OLED_GetStdPattern(0);
    OLED_SetFillPattern(&OLEDdevice, pat);
    OLED_SetCharUpdate(&OLEDdevice, 0);

    while (1) {
	   OLED_ClearBuffer(&OLEDdevice);
		OLED_SetCursor(&OLEDdevice, 0, 0);
		OLED_PutString(&OLEDdevice, "Profile: ");
		OLED_PutString(&OLEDdevice, sets[set]);
		OLED_SetCursor(&OLEDdevice, 0, 2);
		OLED_PutString(&OLEDdevice, "Sound: ");
		OLED_PutString(&OLEDdevice, sounds[set][sound]);
		OLED_Update(&OLEDdevice);

	   // Capture state of each key
	   keystate = KYPD_getKeyStates(&myDevice_kypd);

	   // Determine which single key is pressed, if any
	   status = KYPD_getKeyPressed(&myDevice_kypd, keystate, &key);

	   // button press detection
	   last_btn_val = btn_val;
	   btn_val = XGpio_DiscreteRead(&btn_N, 1);
	   if (btn_val == 1 && last_btn_val == 0) {
		   btn_data = 'N';
		   SysUart_Send(&myUart, &btn_data, 1);
		   BT2_SendData(&myDevice_bt, &btn_data, 1);
		   set += 1;
		   if (set > 5) {
			   set = 0;
		   }
	   }

	   if (status == KYPD_SINGLE_KEY && (status != last_status || key != last_key)) {
	      xil_printf("Key Pressed: %c\r\n", (char) key);
		  last_key = key;

		  sound = char_to_index(last_key);

		  SysUart_Send(&myUart, &key, 1);
		  BT2_SendData(&myDevice_bt, &key, 1);
	   }

      last_status = status;
      usleep(1000);
   }
}


int char_to_index(u8 last_key) {
	 int sound = 0;

	 if ((char) last_key == '0') {
		sound = 0;
	  }
	  else if ((char) last_key == '1') {
		  sound = 1;
	  }
	  else if ((char) last_key == '2') {
		  sound = 2;
	  }
	  else if ((char) last_key == '3') {
		  sound = 3;
	  }
	  else if ((char) last_key == '4') {
		  sound = 4;
	  }
	  else if ((char) last_key == '5') {
		  sound = 5;
	  }
	  else if ((char) last_key == '6') {
		  sound = 6;
	  }
	  else if ((char) last_key == '7') {
		  sound = 7;
	  }
	  else if ((char) last_key == '8') {
		  sound = 8;
	  }
	  else if ((char) last_key == '9') {
		  sound = 9;
	  }
	  else if ((char) last_key == 'A') {
		  sound = 10;
	  }
	  else if ((char) last_key == 'B') {
		  sound = 11;
	  }
	  else if ((char) last_key == 'C') {
		  sound = 12;
	  }
	  else if ((char) last_key == 'D') {
		  sound = 13;
	  }
	  else if ((char) last_key == 'E') {
		  sound = 14;
	  }
	  else if ((char) last_key == 'F') {
		  sound = 15;
	  }

	  return sound;
}

// Initialize the system UART device
void SysUartInit() {
#ifdef __MICROBLAZE__
   // AXI Uartlite for MicroBlaze
   XUartLite_Initialize(&myUart, SYS_UART_DEVICE_ID);
#else
   // Uartps for Zynq
   XUartPs_Config *myUartCfgPtr;
   myUartCfgPtr = XUartPs_LookupConfig(SYS_UART_DEVICE_ID);
   XUartPs_CfgInitialize(&myUart, myUartCfgPtr, myUartCfgPtr->BaseAddress);
#endif
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheDisable();
#endif
#endif
}
