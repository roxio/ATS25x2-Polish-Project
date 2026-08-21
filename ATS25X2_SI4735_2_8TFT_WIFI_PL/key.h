//types of button
#define B_BLUE    	0
#define B_GREEN   	1
#define B_GOLD    	2

//layers
#define L_FIRST   	0
#define L_THIRD   	1
#define L_HAM     	2
#define L_BAND    	3
#define L_FREQ    	4
#define L_BANDW_SSB	5
#define L_BANDW_AM 	6
#define L_BANDW_FM 	7
#define L_STEP_AM 	8
#define L_STEP_FM  	9
#define L_MODE    	10
#define L_SCAN    	11
#define L_RETRO   	12
#define L_MEMO    	13
#define L_SETUP   	14
#define L_CONFIRM   15

//buttons: {number, layer, type, name, x vert, y vert, x hori, y hori}
But but[] PROGMEM = {
  {0,   L_FIRST, 		  B_BLUE,  "HAM", 	  2,  215,    2,  164},
  {1,   L_FIRST, 		  B_BLUE,  "BFO", 	 82,  215,   82,  164},
  {2,   L_FIRST, 		  B_BLUE,  "FREQ",	162,  215,  162,  164},
  {3,   L_FIRST, 		  B_BLUE,  "AGC", 	 82,  267,  242,  190},
  {4,   L_FIRST, 		  B_BLUE,  "MUTE",	 82,  293,  162,  216},
  {5,   L_FIRST, 		  B_BLUE,  "VOL", 	  2,  293,   82,  216},
  {6,   L_FIRST, 		  B_BLUE,  "MODE",	 82,  241,    2,  190},
  {7,   L_FIRST, 		  B_BLUE,  "BANDW",	162,  241,   82,  190},
  {8,   L_FIRST, 		  B_BLUE,  "STEP", 	  2,  267,  162,  190},
  {9,   L_FIRST, 		  B_BLUE,  "BAND", 	  2,  241,  242,  164},
  {10,  L_FIRST, 		  B_BLUE,  "ATT", 	162,  267,    2,  216},
  {11,  L_FIRST, 		  B_GREEN, "NEXT", 	162,  293,  242,  216},

  {0,   L_THIRD, 		  B_BLUE,  ">> UP",	  2,  215,    2,  164},
  {1,   L_THIRD, 		  B_BLUE,  "<< DN",	 82,  215,   82,  164},
  {2,   L_THIRD, 		  B_BLUE,  "INFO",	162,  215,  162,  164},
  {3,   L_THIRD, 		  B_BLUE,  "RDS", 	162,  241,   82,  190},
  {4,   L_THIRD, 		  B_BLUE,  "FM",  	  2,  267,  162,  190},
  {5,   L_THIRD, 		  B_BLUE,  "MEMO",	 82,  267,  242,  190},
  {6,   L_THIRD, 		  B_BLUE,  "LIGHT",	162,  267,    2,  216},
  {7,   L_THIRD, 		  B_BLUE,  "SETUP",	 82,  293,  162,  216},
  {8,   L_THIRD, 		  B_BLUE,  "SQLCH", 	 82,  241,    2,  190},
  {9,   L_THIRD, 		  B_BLUE,  "SCAN", 	  2,  241,  242,  164},
  {10,  L_THIRD, 		  B_BLUE,  "RETRO",	  2,  293,   82,  216},
  {11,  L_THIRD, 		  B_GREEN, "BACK", 	162,  293,  242,  216},

  {3,   L_HAM, 		    B_BLUE,  "800M",	  0,   80,    0,   80},
  {4,   L_HAM,   		  B_BLUE,  "630M",	  0,  120,   80,   80},
  {5,   L_HAM, 	  	  B_BLUE,  "160M",	  0,  160,  160,   80},
  {8,   L_HAM, 		    B_BLUE,  "80M",		  0,  200,  240,   80},
  {10,  L_HAM,   		  B_BLUE,  "60M",		 80,   80,    0,  120},
  {12,  L_HAM, 	  	  B_BLUE,  "40M",		 80,  120,   80,  120},
  {15,  L_HAM, 		    B_BLUE,  "30M",		 80,  160,  160,  120},
  {18,  L_HAM,   		  B_BLUE,  "20M",		 80,  200,  240,  120},
  {21,  L_HAM, 	  	  B_BLUE,  "16M",		160,   80,    0,  160},
  {23,  L_HAM, 		    B_BLUE,  "14M",		160,  120,   80,  160},
  {25,  L_HAM,   		  B_BLUE,  "12M",		160,  160,  160,  160},
  {28,  L_HAM, 	  	  B_BLUE,  "10M",		160,  200,  240,  160},

  {0,   L_BAND, 		  B_GREEN, "FM", 		  0,   80,    0,   80},
  {1,   L_BAND, 		  B_GREEN, "LW", 		 80,   80,   80,   80},
  {2,   L_BAND, 		  B_GREEN, "MW", 		160,   80,  160,   80},
  {6,   L_BAND, 		  B_BLUE,  "120M",	  0,  120,  240,   80},
  {7,   L_BAND, 		  B_BLUE,  "90M",		 80,  120,    0,  120},
  {9,   L_BAND, 		  B_BLUE,  "75M",		160,  120,   80,  120},
  {11,  L_BAND, 		  B_BLUE,  "49M",		  0,  160,  160,  120},
  {13,  L_BAND, 		  B_BLUE,  "41M",		 80,  160,  240,  120},
  {14,  L_BAND, 		  B_BLUE,  "31M",		160,  160,    0,  160},
  {16,  L_BAND, 		  B_BLUE,  "25M",		  0,  200,   80,  160},
  {17,  L_BAND, 		  B_BLUE,  "22M",		 80,  200,  160,  160},
  {19,  L_BAND, 		  B_BLUE,  "19M",		160,  200,  240,  160},
  {20,  L_BAND, 		  B_BLUE,  "17M",		  0,  240,    0,  200},
  {22,  L_BAND, 		  B_BLUE,  "15M",		 80,  240,   80,  200},
  {24,  L_BAND, 		  B_BLUE,  "13M",		160,  240,  160,  200},
  {26,  L_BAND, 		  B_BLUE,  "11M",		  0,  280,  240,  200},
  {27,  L_BAND, 		  B_BLUE,  "CB", 		 80,  280,  240,    0},
  {29,  L_BAND, 		  B_BLUE,  "SW", 		160,  280,  240,   40},

  {0,   L_FREQ, 		  B_BLUE,  "1", 		  0,  160,    0,  120},
  {1,   L_FREQ, 		  B_BLUE,  "2", 		 80,  160,   80,  120},
  {2,   L_FREQ, 		  B_BLUE,  "3", 		160,  160,  160,  120},
  {3,   L_FREQ, 		  B_BLUE,  "4", 		  0,  200,    0,  160},
  {4,   L_FREQ, 		  B_BLUE,  "5", 		 80,  200,   80,  160},
  {5,   L_FREQ, 		  B_BLUE,  "6", 		160,  200,  160,  160},
  {6,   L_FREQ, 		  B_BLUE,  "7", 		  0,  240,    0,  200},
  {7,   L_FREQ, 		  B_BLUE,  "8", 		 80,  240,   80,  200},
  {8,   L_FREQ, 		  B_BLUE,  "9", 		160,  240,  160,  200},
  {9,   L_FREQ, 		  B_BLUE,  ".", 		 80,  280,  240,  160},
  {10,  L_FREQ, 		  B_BLUE,  "0", 		  0,  280,  240,  200},
  {11,  L_FREQ, 		  B_GREEN, "OK", 		160,  280,  240,  120},
  {12,  L_FREQ, 		  B_GREEN, "DEL",		160,  120,  240,   80},
  {13,  L_FREQ, 		  B_GREEN, "CLS",		 80,  120,  240,   40},
  {14,  L_FREQ, 		  B_GREEN, "X", 		  0,  120,  240,    0},

  {0,   L_BANDW_SSB,  B_BLUE,  "1.2", 	 20,  150,  220,  100},
  {1,   L_BANDW_SSB,  B_BLUE,  "2.2", 	140,  150,   20,  150},
  {2,   L_BANDW_SSB,  B_BLUE,  "3.0", 	 20,  200,  120,  150},
  {3,   L_BANDW_SSB,  B_BLUE,  "4.0", 	140,  200,  220,  150},
  {4,   L_BANDW_SSB,  B_BLUE,  "0.5",	   20,  100,   20,  100},
  {5,   L_BANDW_SSB,  B_BLUE,  "1.0", 	140,  100,  120,  100},

  {0,   L_BANDW_AM,   B_BLUE,  "6.0", 	 80,  250,  120,  200},
  {1,   L_BANDW_AM,   B_BLUE,  "4.0", 	140,  200,  220,  150},
  {2,   L_BANDW_AM,   B_BLUE,  "3.0", 	 20,  200,  120,  150},
  {3,   L_BANDW_AM,   B_BLUE,  "2.0", 	 20,  150,  220,  100},
  {4,   L_BANDW_AM,   B_BLUE,  "1.0",	   20,  100,   20,  100},
  {5,   L_BANDW_AM, 	B_BLUE,  "1.8", 	140,  100,  120,  100},
  {6,   L_BANDW_AM,   B_BLUE,  "2.5",  	140,  150,   20,  150},

  {0,   L_BANDW_FM, 	B_BLUE,  "AUTO", 	 80,  200,  120,  200},
  {1,   L_BANDW_FM, 	B_BLUE,  "110", 	 20,  100,   60,  100},
  {2,   L_BANDW_FM, 	B_BLUE,  "84", 		140,  100,  180,  100},
  {3,   L_BANDW_FM, 	B_BLUE,  "60", 		 20,  150,   60,  150},
  {4,   L_BANDW_FM, 	B_BLUE,  "40",	  140,  150,  180,  150},

  {1,   L_STEP_AM, 	  B_BLUE,  "1KHz", 	 20,  120,   60,  120},
  {5,   L_STEP_AM, 	  B_BLUE,  "5KHz", 	140,  120,  180,  120},
  {9,   L_STEP_AM, 	  B_BLUE,  "9KHz", 	 20,  180,   60,  180},
  {10,  L_STEP_AM, 	  B_BLUE,  "10KHz",	140,  180,  180,  180},

  {10,  L_STEP_FM, 	  B_BLUE,  "100KHz", 20,  120,   60,  120},
  {1,   L_STEP_FM, 	  B_BLUE,  "10KHZ",	140,  120,  180,  120},

  {1,   L_MODE,    	  B_BLUE,  "LSB",  	 20,  120,   60,  120},
  {2,   L_MODE,    	  B_BLUE,  "USB",  	140,  120,  180,  120},
  {3,   L_MODE,    	  B_BLUE,  "AM",   	 20,  180,   60,  180},
  {4,   L_MODE,    	  B_BLUE,  "CW",   	140,  180,  180,  180},

  {0,   L_SCAN,    	  B_BLUE,  "SCALE",	  0,  280,    0,  200},
  {1,   L_SCAN,    	  B_BLUE,  "PAUSE",	 80,  280,   80,  200},
  {2,   L_SCAN,    	  B_BLUE,  "STEP", 	  0,  240,  160,  200},
  {3,   L_SCAN,    	  B_GREEN, "BACK", 	160,  280,  240,  200},
  {4,   L_SCAN,       NULL,    "",       80,  240,  320,  240},
  {4,   L_SCAN,       NULL,    "",      160,  240,  320,  240},

  {0,   L_RETRO,   	  B_GOLD,  "CITY", 	  0,  240,    0,  200},
  {1,   L_RETRO,   	  B_GOLD,  "BAND", 	  0,  280,   80,  200},
  {2,   L_RETRO,   	  B_GOLD,  "VOL",  	 80,  280,  160,  200},
  {3,   L_RETRO,   	  B_GOLD,  "BACK", 	160,  280,  240,  200},
  {4,   L_RETRO,      B_GOLD,  " ",      80,  240,  320,  240},
  {4,   L_RETRO,      B_GOLD,  " ",     160,  240,  320,  240},

  {0,   L_MEMO,    	  B_BLUE,  "EDIT", 	  0,  280,    0,  200},
  {1,   L_MEMO,    	  B_BLUE,  "ADD",  	 80,  240,   80,  200},
  {2,   L_MEMO,    	  B_BLUE,  "DEL",  	 80,  280,  160,  200},
  {3,   L_MEMO,       B_GREEN, "BACK",  160,  280,  240,  200},
  {4,   L_MEMO,       NULL,    "",        0,  240,  320,  240},
  {4,   L_MEMO,       NULL,    "",      160,  240,  320,  240},

  {0,   L_SETUP,   	  B_BLUE,  "PREV", 	  0,  240,    0,  200},
  {1,   L_SETUP,   	  B_BLUE,  "NEXT", 	  0,  280,   80,  200},
  {2,   L_SETUP,     	B_BLUE,  "RESET",	160,  240,  160,  200},
  {3,   L_SETUP,     	B_GREEN, "EXIT", 	160,  280,  240,  200},
  {4,   L_SETUP,      NULL,    "",       80,  240,  320,  240},
  {4,   L_SETUP,      NULL,    "",       80,  280,  320,  240},

  {1,   L_CONFIRM,    B_GREEN, "TAK",    20,  100,   60,  100},
  {0,   L_CONFIRM,    B_BLUE,  "NIE",    140,  100,  180,  100}
};
