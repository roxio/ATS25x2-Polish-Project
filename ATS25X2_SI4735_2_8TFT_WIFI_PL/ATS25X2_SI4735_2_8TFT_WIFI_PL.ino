
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiManager.h>
#include "time.h"
struct KbdKeyDef;

WiFiManager wifiManager;
bool wifiConnected = false;
uint16_t touchCalData[5] = {387, 3530, 246, 3555, 7};

bool webServerOn = true;
bool webServerRunning = false;
WebServer webServer(80);
bool drawProgressScreenReady = false;

const char* ntpServer = "0.it.pool.ntp.org";
const long  gmtOffset_sec =3600 * 2;
const int   daylightOffset_sec = 0;

#define NTP_MIN_VALID_EPOCH 1533081600

#define ESP32_I2C_SDA    21
#define ESP32_I2C_SCL    22
#define RESET_PIN        12
#define ENCODER_PIN_A    17
#define ENCODER_PIN_B    16
#define ENCODER_SWITCH   33
#define BAT_INFO         35
#define BEEPER           32
#define DISPLAY_LED      14
#define AUDIO_MUTE       27
#define DECODER_PIN      39

#define SCREEN_V      0
#define SCREEN_H      1
uint16_t calDataV[5] = { 258, 3566, 413, 3512, 4 };
uint16_t calDataH[5] = { 387, 3530, 246, 3555, 7 };

#define IhaveCrystal

#include "Tahoma_10x12_pl.h"
#include "Tahoma_15x16_pl.h"

#define TFT_TRANS   -1

#define BL_T      -1
#define BC_T       0
#define BR_T       1
#define TL_T     -11
#define TC_T      10
#define TR_T      11

#define REG_T      0
#define BOL_T      1
#define NRG_T      2
#define NBL_T      3
#define CUR_T      4
#define CUB_T      5
#define NCR_T      6
#define NCB_T      7

#define T1012_T    0
#define T1516_T    1

float tftPlSize          = 1;
uint16_t tftPlColor      = 0xFFFF;
int32_t tftPlBack        = 0x0000;
int tftPlDatum           = 0;
int tftPlStyle           = 0;
int tftPlFont            = 0;
int tftPlBeginChar       = 0;
int tftPlContChar        = 0;

int tftPlWidth           = 10;
bool tftPlBottomCut      = false;
int tftPlCursiveLevel    = 4;

#include <TFT_eSPI.h>
#include <SPI.h>
#include <SI4735.h>
#include "EEPROM.h"
#include "Rotary.h"

#ifdef IhaveSI5351

#endif

#include "DSEG7_Classic_Mini_Regular_34.h"
#include "TFT_Colors.h"
#include "Button.h"
#include "logo.h"

#define B_NORMAL  0
#define B_JAM     1
#define B_SELECT  2
#define B_BLOCK   3

struct But {
  const uint8_t     num;
  const uint8_t  layout;
  const uint8_t    type;
  const char      *Name;
  const uint16_t  xPosV;
  const uint16_t  yPosV;
  const uint16_t  xPosH;
  const uint16_t  yPosH;
};

#include "key.h"

const int lastBut = (sizeof but / sizeof(But)) - 1;
bool butBlock[lastBut + 1];

typedef struct
{
  const char      *BandName;
  const char      *BandNamePL;
  const char      *RetroBandTime;
  const char      *RetroBandTimePL;
  const uint16_t  xPosV;
  const uint16_t  yPosV;
  const uint16_t  xPosH;
  const uint16_t  yPosH;
  const int       band;
  const float     minimumFreq;
  const float     maximumFreq;
  float           currentFreq;
  const float     scale;
  const float     mark;
  const float     hardStep;
  const float     softStep;
} RetroBand ;

RetroBand bandRetro[] {
  "FM",   "FM",   "",             "",             10, 40,   240,200,    0,  87.50, 108.00,  87.50,   50,    1, 10,   10,
  "VHF",  "UKF",  "",             "",             10, 80,   240,150,    0,  64.00,  87.00,  64.00,   50,    1, 10,    1,
  "LW",   "DL",   "",             "",            150, 40,   240, 40,    1,    153,    279,    153,   10,    9,  1,    1,
  "MW",   "SR",   "",             "",            150, 80,   240, 90,    2,    522,   1701,    522,    1,   90,  9,    1,
  "SW1",  "KR1",  "NIGHT WINTER", "NOC ZIMA",     10,120,     0, 40,   29,   1800,   5060,   2300,    1,  100,  5,    1,
  "SW2",  "KR2",  "NIGHT",        "NOC",          10,160,     0, 80,   29,   5300,   7600,   5900,    1,  100,  5,    1,
  "SW3",  "KR3",  "MOSTLY NIGHT", "GL. NOC",      10,200,     0,120,   29,   9400,  12160,   9400,    1,  100,  5,    1,
  "SW4",  "KR4",  "MOSTLY DAY",   "GL. DZIEN",    10,240,     0,160,   29,  13570,  18168,  13570,    1,  100,  5,    1,
  "SW5",  "KR5",  "DAY",          "DZIEN",        10,280,     0,200,   29,  18900,  26100,  18900,    1,  100,  5,    1,
  };
const int lastBandRetro  = (sizeof bandRetro / sizeof (RetroBand)) - 1;

typedef struct
{
  const uint16_t groupIdx;
  const char    *PresetName;
} Group ;

typedef struct
{
  const float    memoryIdx;
  char          *MemoryName;
  char          *memoryGroup;
} Memory ;

#include "Preset.h"

const int lastGroup  = (sizeof group / sizeof (Group)) - 1;
const int lastMemory = (sizeof memory / sizeof (Memory)) - 1;

uint16_t PresetId;
uint16_t prevPresetId;
int lastPreset;

typedef struct
{
  float      presetIdx;
  char      *PresetName;
  int        presetPos;
} Preset ;

int textScroll;
long elapsedScroll;
int directScroll = 0;

#include "patch_full.h"

const uint16_t size_content = sizeof ssb_patch_content;

#define FM_BAND_TYPE 0
#define MW_BAND_TYPE 1
#define SW_BAND_TYPE 2
#define LW_BAND_TYPE 3

#define MIN_ELAPSED_TIME             100
#define MIN_ELAPSED_RSSI_TIME        150
#define MIN_ELAPSED_AudMut_TIME        0
#define MIN_ELAPSED_RDS_TIME           5
#define DEFAULT_VOLUME                15
#define MIN_ELAPSED_VOLbut_TIME     1000
#define CLK_Xtal                    SI5351wire_CLK0

#define FM          0
#define LSB         1
#define USB         2
#define AM          3
#define CW          4

#define TFT_GREY 0x4A8B
#define TFT_LIGTHYELLOW 0xFF10

bool bfoTr          = false;
bool bfoOn          = false;
bool ssbLoaded      = false;
bool FirstLayer     = true;
bool FirstTime      = true;
bool SecondLayer    = false;
bool ThirdLayer     = false;
bool ForthLayer     = false;
bool HamBand        = false;
bool Modebut        = false;
bool FREQbut        = false;
bool Decipoint      = false;
bool STEPbut        = false;
bool encsw          = false;
bool BroadBand;
bool BandWidth;
bool MISCbut        = false;
bool PRESbut        = false;
bool VOLbut         = false;
bool AudioMut       = false;
bool Mutestat       = false;
bool AGCgainbut     = false;
bool writingEeprom  = false;

bool SquelchUsesRSSI = true;
bool SQUELCHbut = false;
long SQUELCHbutOnTime       = millis();
int previousSquelch;
int currentSquelch;
int SignalQuality = 0;
long squelchDecay            = 0;
#define squelchDecayTime     500
bool squelch = false;
uint8_t currentSQUELCHStep     =  1;
uint8_t MaxSQUELCH             = 50;
uint8_t MinSQUELCH             =  0;
uint8_t encoderBtnState        =  0;
#define MIN_ELAPSED_SQUELCHbut_TIME     1000

bool batVolt        = true;
long elapsedBat     = 0;

bool  SCANbut             = false;
int   currentScanFreq;
int   posScanFreq;
int   posScan;
int   posScanLast;
float SCANstep;
bool  SCANpause = true;
float currentScanLine;
int   ScanValueRSSI[320];
int   ScanPeakRSSI[320];

#define WF_ROWS 38
uint8_t waterfallBuf[WF_ROWS][320];
uint8_t waterfallHead = 0;
bool scanWaterfallOn = false;
unsigned long lastWaterfallCommit = 0;
#define WF_COMMIT_MS 2000
bool prevscanWaterfallOn;
bool prevcwDecoderOn;
bool prevwebServerOn;
bool prevanalogMeterOn;
int   ScanValueSNR[320];
bool  ScanMark[320];
uint8_t ScanScaleLine[320];
uint8_t ScanMarkSNR       = 3;
bool    scanStopOnSignal  = false;
uint8_t scanStopSeconds   = 5;
unsigned long scanStopUntil = 0;
int   ScanBeginBand;
int   ScanEndBand;
uint8_t ScanAGC;
bool  ScanEmpty           = true;
float deltaScanLine       = 0;
float currentMinScanStep;
float currentMaxScanStep;
int   countScanSignal     = 3;

struct HamSegment { uint16_t f1; uint16_t f2; uint8_t type; };
const HamSegment hamBandPlan[] = {
  {1810, 1838, 0}, {1838, 1843, 1}, {1843, 2000, 2},
  {3500, 3570, 0}, {3570, 3600, 1}, {3600, 3800, 2},
  {5351, 5367, 2},
  {7000, 7040, 0}, {7040, 7050, 1}, {7050, 7200, 2},
  {10100, 10150, 1},
  {14000, 14070, 0}, {14070, 14099, 1}, {14101, 14350, 2},
  {18068, 18095, 0}, {18095, 18109, 1}, {18111, 18168, 2},
  {21000, 21070, 0}, {21070, 21149, 1}, {21151, 21450, 2},
  {24890, 24915, 0}, {24915, 24929, 1}, {24931, 24990, 2},
  {28000, 28070, 0}, {28070, 28190, 1}, {28191, 29700, 2},
};
const int hamBandPlanCount = sizeof(hamBandPlan) / sizeof(HamSegment);

int8_t hamSegmentType(long freqKHz) {
  for (int i = 0; i < hamBandPlanCount; i++) {
    if (freqKHz >= hamBandPlan[i].f1 and freqKHz < hamBandPlan[i].f2) return hamBandPlan[i].type;
  }
  return -1;
}
bool showHamSegments = true;
float signalScale;
bool  prevScaleLine       = false;

bool  RETRObut            = false;
float currentRetroFreq;
float currentRetroScale;
const uint8_t RetroStationPos[] = {43, 55, 67, 79, 91, 103, 131, 143, 155, 167, 179, 191};
uint8_t RETROband         = 0;
bool  bandRETRObut        = false;
bool  cityRETRObut        = false;
int   cityRetroRotary     = 0;
int   scrollRetro         = 0;
int   bandHamRetro;

bool  MEMObut             = false;
int   currentMemo         = 0;
bool  MEMOadd             = false;
bool  MEMOdel             = false;
char  addMemoName[21];
uint16_t addMemoFreq;
uint8_t addMemoBand;
uint8_t addMemoMode;
uint8_t posMemoName;
uint8_t charMemoName;
long elapsedCursor        = millis();
bool  presetBank          = false;

bool    SETUPbut          = false;
int     pageSetup         = 0;
uint8_t maxPageSetup      = 7;

bool      PREtap          = false;
bool      PRE             = false;
uint16_t  PREfreq;
uint8_t   PREband;
uint8_t   PREmode;
int       PREbfo;
uint8_t   PREstep;
uint8_t   PREbw;
long      elapsedPRE      = millis();

bool VHFon;
bool langRetroEN;
bool digitLigth;
bool beeperOn;
bool memoPreset;
bool batShow;
bool loadMemory;
bool loadDefault;
bool saverOn;
uint16_t saverTime;
bool displayOff;
float minSCANstep;
float maxSCANstep;
bool  autoSCANstep;
int  SCANscale;
bool SCANaccuracy;
bool screenV;
bool displayPower;
uint16_t boolOpt;
bool RDSalways;
bool seekAccuracy;
bool saverDisableOnScan = true;
uint16_t batMinV = 270;
uint16_t batMaxV = 405;

bool wifiEnable          = true;
bool wifiConfigureNow    = false;
bool resetWifiConfig     = false;

bool prevdigitLigth;
bool prevlangRetroEN;
bool prevVHFon;
bool prevbeeperOn;
bool prevloadMemory;
bool prevbatShow;
bool prevmemoPreset;
bool prevloadDefault;
bool prevsaverOn;
uint16_t prevsaverTime;
bool prevdisplayOff;
float prevminSCANstep;
float prevmaxSCANstep;
bool prevautoSCANstep;
bool prevSCANaccuracy;
bool prevscreenV;
bool prevdisplayPower;
bool prevRDSalways;
bool prevseekAccuracy;
bool prevsaverDisableOnScan;
uint16_t prevbatMinV;
uint16_t prevbatMaxV;
uint8_t prevScanMarkSNR;
bool    prevscanStopOnSignal;
uint8_t prevscanStopSeconds;
bool prevwifiEnable;
bool prevwifiConfigureNow;
bool prevresetWifiConfig;
bool prevnightModeOn;

const uint16_t saverTimePresets[] = {30, 60, 90, 120, 180, 300, 600, 900, 1200, 1800};
const uint8_t saverTimePresetsCount = sizeof(saverTimePresets) / sizeof(saverTimePresets[0]);

uint16_t nextSaverTime(uint16_t current) {
  for (uint8_t i = 0; i < saverTimePresetsCount; i++) {
    if (current == saverTimePresets[i]) {
      return saverTimePresets[(i + 1) % saverTimePresetsCount];
    }
  }
  return saverTimePresets[0];
}

String saverTimeText(uint16_t seconds) {
  if (seconds < 60) return String(seconds) + "s";
  if (seconds % 60 == 0) return String(seconds / 60) + "min";
  return String(seconds / 60) + "min " + String(seconds % 60) + "s";
}

const uint16_t batMinVPresets[] = {250, 260, 270, 280, 290, 300, 320};
const uint8_t  batMinVPresetsCount = sizeof(batMinVPresets) / sizeof(batMinVPresets[0]);
const uint16_t batMaxVPresets[] = {390, 395, 400, 405, 410, 415, 420};
const uint8_t  batMaxVPresetsCount = sizeof(batMaxVPresets) / sizeof(batMaxVPresets[0]);

uint16_t nextBatMinV(uint16_t current) {
  for (uint8_t i = 0; i < batMinVPresetsCount; i++)
    if (current == batMinVPresets[i]) return batMinVPresets[(i + 1) % batMinVPresetsCount];
  return batMinVPresets[0];
}
uint16_t nextBatMaxV(uint16_t current) {
  for (uint8_t i = 0; i < batMaxVPresetsCount; i++)
    if (current == batMaxVPresets[i]) return batMaxVPresets[(i + 1) % batMaxVPresetsCount];
  return batMaxVPresets[0];
}
String batVText(uint16_t centivolts) {
  return String(centivolts / 100) + "." + String(centivolts % 100 < 10 ? "0" : "") + String(centivolts % 100) + "V";
}

const uint8_t scanSnrPresets[] = {1,2,3,4,5,6,8,10,12,15,20};
const uint8_t scanSnrPresetsCount = sizeof(scanSnrPresets)/sizeof(scanSnrPresets[0]);
uint8_t nextScanSnr(uint8_t current) {
  for (uint8_t i=0;i<scanSnrPresetsCount;i++) if (current==scanSnrPresets[i]) return scanSnrPresets[(i+1)%scanSnrPresetsCount];
  return scanSnrPresets[0];
}
const uint8_t scanStopSecPresets[] = {2,3,5,8,10,15,20,30};
const uint8_t scanStopSecPresetsCount = sizeof(scanStopSecPresets)/sizeof(scanStopSecPresets[0]);
uint8_t nextScanStopSec(uint8_t current) {
  for (uint8_t i=0;i<scanStopSecPresetsCount;i++) if (current==scanStopSecPresets[i]) return scanStopSecPresets[(i+1)%scanStopSecPresetsCount];
  return scanStopSecPresets[0];
}

long elapsedSaver = millis();
bool Saver = false;
int saverX;
int saverY;
int posSaver = 0;
#define SAVER_BLOCK_W 140
#define SAVER_BLOCK_H 100
int saverVX = 2;
int saverVY = 2;
long elapsedSaverMove = millis();
uint16_t saverColor = TFT_DARKCYAN;
uint8_t saverColorIdx = 0;
const uint16_t saverColors[] = {TFT_DARKCYAN, TFT_ORANGE, TFT_MAGENTA, TFT_GREENYELLOW, TFT_YELLOW, TFT_PINK, TFT_SKYBLUE};
const uint8_t saverColorsCount = sizeof(saverColors) / sizeof(saverColors[0]);

bool pressed;
bool presStat;
bool audioMuteOn  = true;
bool audioMuteOff = false;
bool RDS          = true;
bool SEEK         = false;
bool bright       = false;
bool CWShift      = false;
bool fstShift     = false;
bool calibratSI5351 = false;

int currentBFO;
int currentBFOmanu;
int previousBFO     = 0;
int previousBFOmanu = 0;
int OldRSSI;
int NewRSSI;
int NewSNR;
int encBut;
uint8_t AGCgain;
int PrevRSSI        = 0;
int strongup        = 0;

long elapsedRSSI        = millis();
long elapsedAudMut      = millis();
long stationNameElapsed = millis();
long VOLbutOnTime       = millis();

volatile int encoderCount  = 0;
volatile int encoderButton = 0;

bool volDisp               = false;

uint16_t previousFrequency;
uint8_t currentBFOStep     = 25;
uint8_t currentPRES        =  0;
int     previousPRES;
uint8_t currentPRESStep    =  1;

uint8_t currentAGCgain     =  1;
uint8_t previousAGCgain    =  1;
uint8_t currentAGCgainStep =  1;
uint8_t MaxAGCgain;
uint8_t MaxAGCgainFM       = 26;
uint8_t MaxAGCgainAM       = 37;
uint8_t MinAGCgain         =  1;

int     currentVOL         =  0;
int     previousVOL        =  0;
uint8_t currentVOLStep     =  1;
uint8_t MaxVOL             = 63;
uint8_t MinVOL             =  0;

uint8_t bwIdxSSB;
uint8_t bwIdxAM;
uint8_t bwIdxFM;
uint8_t ssIdxMW;
uint8_t ssIdxAM;
uint8_t ssIdxFM;
uint8_t bandIdx;
uint8_t currentMode        = FM;
uint8_t previousMode       =  0;
uint16_t x = 0, y = 0;
uint8_t encoderStatus;
uint16_t freqstep;
uint8_t freqstepnr         = 0;
int freqDec                = 0;

const int LedFreq          = 5000;
const int LedResol         = 8;
const int LedChannelforTFT = 0;
uint16_t currentBrightness;
uint16_t previousBrightness = 65535;
uint16_t MaxBrightness     = 16;
uint16_t MinBrightness     = 256;
uint8_t stepsizesynth      = 10;

float DisplayfreqNew       = 0;
float Displayfreq          = 0;
float currentFrequency     = 0;
float dpfrq                = 0;
float fact                 = 1;
float RSSIfact             = 3;

String BWtext;
String Modtext;
String AGCgainbuttext;

struct tm timeinfo;

const char *bandwidthSSB[] = {"1.2", "2.2", "3.0", "4.0", "0.5", "1.0"};
const char *bandwidthAM[]  = {"6.0", "4.0", "3.0", "2.0", "1.0", "1.8", "2.5"};
const char *bandwidthFM[]  = {"AUTO", "110", "84", "60", "40"};
const char *stepsize[]     = {"1", "5", "9", "10"};
const char *stepsizeFM[]   = {"100", "10"};

const char *Keypathtext[]  = {"1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "0", "OK", "DEL", "CLS", "X"};
const char *bandModeDesc[] = {"FM", "LSB", "USB", "AM", "CW"};

char buffer[64];
char buffer1[64];

char *stationName;
char bufferStatioName[50];

char *rdsMsg;
char bufferRdsMsg[100];

char *rdsTime;
char bufferRdsTime[32];

unsigned long FreqSI5351 = 3276800;
unsigned long calibratvalSI5351;

int Xsmtr   =   0;
int Ysmtr   =  80;

bool rssiHistoryOn = false;
bool prevrssiHistoryOn;
#define RSSI_HIST_LEN 103
uint8_t rssiHist[RSSI_HIST_LEN] = {0};
long elapsedRssiHist = 0;
#define RSSI_HIST_INTERVAL 700

bool screenLocked = false;

struct VFOState {
  uint8_t bandIdx;
  uint8_t mode;
  float   freq;
  uint8_t step;
  int     bfo;
  bool    valid;
};
VFOState vfoA = {0, 0, 0, 0, 0, false};
VFOState vfoB = {0, 0, 0, 0, 0, false};
bool vfoActiveIsB = false;

#define FREQ_HISTORY_LEN 10
struct FreqHistEntry { uint8_t bandIdx; uint8_t mode; float freq; };
FreqHistEntry freqHistory[FREQ_HISTORY_LEN];
uint8_t freqHistoryCount = 0;
float freqHistoryLastSeen = -1;
unsigned long freqHistoryStableSince = 0;
bool freqHistoryRecorded = false;
#define FREQ_HISTORY_STABLE_MS 4000

bool stopwatchRunning = false;
unsigned long stopwatchStart = 0;
unsigned long stopwatchAccum = 0;

bool closeCallOn = false;

#define SKIP_LIST_MAX 30
uint16_t skipList[SKIP_LIST_MAX];
uint8_t skipListCount = 0;

bool isFreqSkipped(long freqKHz) {
  for (int i = 0; i < skipListCount; i++) if (skipList[i] == freqKHz) return true;
  return false;
}

#define DISCOVERY_LOG_MAX 20
struct DiscoveryEntry { long freq; uint8_t snr; char timeStr[9]; };
DiscoveryEntry discoveryLog[DISCOVERY_LOG_MAX];
uint8_t discoveryLogCount = 0;
uint8_t discoveryLogHead = 0;

void logDiscovery(long freqKHz, uint8_t snr) {
  struct tm t;
  char buf[9] = "--:--:--";
  if (getLocalTime(&t, 5)) sprintf(buf, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
  discoveryLog[discoveryLogHead].freq = freqKHz;
  discoveryLog[discoveryLogHead].snr = snr;
  strcpy(discoveryLog[discoveryLogHead].timeStr, buf);
  discoveryLogHead = (discoveryLogHead + 1) % DISCOVERY_LOG_MAX;
  if (discoveryLogCount < DISCOVERY_LOG_MAX) discoveryLogCount++;
}

bool priorityOn = false;
bool priorityHasTarget = false;
VFOState priorityTarget = {0, 0, 0, 0, 0, false};
VFOState priorityReturnState = {0, 0, 0, 0, 0, false};
bool priorityActive = false;
unsigned long priorityActiveSince = 0;
unsigned long lastPriorityCheck = 0;
#define PRIORITY_CHECK_INTERVAL_MS 8000
#define PRIORITY_RETURN_AFTER_MS  15000
#define PRIORITY_SNR_THRESHOLD    3

bool cwDecoderOn = false;
bool analogMeterOn = false;

bool nightModeOn = false;
bool nightModeActive = false;
uint16_t nightModeSavedBrightness = 0;
unsigned long lastNightModeCheck = 0;
#define NIGHT_MODE_CHECK_MS 60000
#define NIGHT_MODE_START_HOUR 22
#define NIGHT_MODE_END_HOUR   6

unsigned long cwUnitTime = 80;
String cwSymbolBuffer = "";
String cwDecodedText = "";
unsigned long cwLastEdgeTime = 0;
bool cwToneActive = false;
#define CW_MAX_TEXT_LEN 120

unsigned long cwLastPollTime = 0;
#define CW_POLL_INTERVAL_MS 8
unsigned long cwStateChangeCount = 0;
int cwLastRawAvg = 0;
int cwLastRawRange = 0;

#define CW_ADC_SAMPLES        10
#define CW_ADC_RANGE_THRESH   80
#define CW_ADC_LEVEL_THRESH   500

struct MorseEntry { const char* code; char ch; };
const MorseEntry morseTable[] = {
  {".-",'A'},{"-...",'B'},{"-.-.",'C'},{"-..",'D'},{".",'E'},{"..-.",'F'},
  {"--.",'G'},{"....",'H'},{"..",'I'},{".---",'J'},{"-.-",'K'},{".-..",'L'},
  {"--",'M'},{"-.",'N'},{"---",'O'},{".--.",'P'},{"--.-",'Q'},{".-.",'R'},
  {"...",'S'},{"-",'T'},{"..-",'U'},{"...-",'V'},{".--",'W'},{"-..-",'X'},
  {"-.--",'Y'},{"--..",'Z'},
  {"-----",'0'},{".----",'1'},{"..---",'2'},{"...--",'3'},{"....-",'4'},
  {".....",'5'},{"-....",'6'},{"--...",'7'},{"---..",'8'},{"----.",'9'},
  {".-.-.-",'.'},{"--..--",','},{"..--..",'?'},{"-.-.--",'!'},{"-...-",'='}
};
const int morseTableCount = sizeof(morseTable) / sizeof(morseTable[0]);

void trimCWText() {
  if (cwDecodedText.length() > CW_MAX_TEXT_LEN) {
    cwDecodedText = cwDecodedText.substring(cwDecodedText.length() - CW_MAX_TEXT_LEN);
  }
}

void decodeCWSymbol() {
  bool found = false;
  for (int i = 0; i < morseTableCount; i++) {
    if (cwSymbolBuffer == morseTable[i].code) {
      cwDecodedText += morseTable[i].ch;
      found = true;
      break;
    }
  }
  if (!found) cwDecodedText += '#';
  trimCWText();
  cwSymbolBuffer = "";
}

bool readCWToneAnalog() {
  int minV = 4095, maxV = 0;
  long sum = 0;
  for (int i = 0; i < CW_ADC_SAMPLES; i++) {
    int v = analogRead(DECODER_PIN);
    if (v < minV) minV = v;
    if (v > maxV) maxV = v;
    sum += v;
  }
  cwLastRawRange = maxV - minV;
  cwLastRawAvg = sum / CW_ADC_SAMPLES;
  return (cwLastRawRange < CW_ADC_RANGE_THRESH and cwLastRawAvg < CW_ADC_LEVEL_THRESH);
}

void processCWEvents() {
  if (millis() - cwLastPollTime < CW_POLL_INTERVAL_MS) return;
  cwLastPollTime = millis();

  bool toneNow = readCWToneAnalog();
  unsigned long now = micros();

  if (toneNow != cwToneActive) {
    unsigned long durationUs = now - cwLastEdgeTime;
    cwLastEdgeTime = now;
    unsigned long durMs = durationUs / 1000;

    if (durMs >= 5) {
      if (toneNow) {
        if (durMs > cwUnitTime * 5) {
          if (cwSymbolBuffer.length() > 0) decodeCWSymbol();
          cwDecodedText += " ";
          trimCWText();
        } else if (durMs > cwUnitTime * 2) {
          if (cwSymbolBuffer.length() > 0) decodeCWSymbol();
        }
      } else {
        if (durMs < cwUnitTime * 2) {
          cwSymbolBuffer += ".";
          cwUnitTime = (cwUnitTime * 3 + durMs) / 4;
        } else {
          cwSymbolBuffer += "-";
        }
      }
      cwStateChangeCount++;
    }
    cwToneActive = toneNow;
  }
  if (cwSymbolBuffer.length() > 0 and (micros() - cwLastEdgeTime) > (unsigned long)(cwUnitTime * 6 * 1000)) {
    decodeCWSymbol();
  }
}

void updateCWDecoderPin() {
  if (cwDecoderOn) {
    pinMode(DECODER_PIN, INPUT);
    cwLastEdgeTime = micros();
    cwLastPollTime = 0;
  }

}

int XVolInd =   0;
int YVolInd = 130;

int XFreqDispl  =   0;
int YFreqDispl  =   0;

#define B_HAM       0
#define B_BFO       1
#define B_FREQ      2
#define B_AGC       3
#define B_MUTE      4
#define B_VOL       5
#define B_MODE      6
#define B_BANDW     7
#define B_STEP      8
#define B_BAND      9
#define B_ATT      10
#define B_NEXT     11

#define B_SEEKUP    0
#define B_SEEKDN    1
#define B_INFO      2
#define B_RDS       3
#define B_FM        4
#define B_MEMO      5
#define B_LIGHT     6
#define B_SETUP     7
#define B_SQUELCH   8
#define B_SCAN      9
#define B_RETRO    10
#define B_BACK     11

typedef struct
{
  const char *bandName;
  uint8_t  bandType;
  uint16_t prefmod;
  uint16_t minimumFreq;
  uint16_t maximumFreq;
  uint16_t currentFreq;
  uint8_t  currentStep;
  int          lastBFO;
  int      lastmanuBFO;

} Band;

Band band[] = {
  {   "FM", FM_BAND_TYPE,  FM,  6400, 10800,  9920, 10, 0, 0},
  {   "LW", LW_BAND_TYPE,  AM,   100,   514,   198,  9, 0, 0},
  {   "MW", MW_BAND_TYPE,  AM,   514,  1800,  1395,  9, 0, 0},
  { "800M", LW_BAND_TYPE,  AM,  280,   470,   284,  1, 0, 0},
  { "630M", SW_BAND_TYPE, LSB,   470,   480,   475,  1, 0, 0},
  { "160M", SW_BAND_TYPE, LSB,  1800,  2000,  1850,  1, 0, 0},
  { "120M", SW_BAND_TYPE,  AM,  2000,  3200,  2400,  5, 0, 0},
  {  "90M", SW_BAND_TYPE,  AM,  3200,  3500,  3300,  5, 0, 0},
  {  "80M", SW_BAND_TYPE, LSB,  3500,  3900,  3630,  1, 0, 0},
  {  "75M", SW_BAND_TYPE,  AM,  3900,  5300,  3950,  5, 0, 0},
  {  "60M", SW_BAND_TYPE, USB,  5300,  5900,  5375,  1, 0, 0},
  {  "49M", SW_BAND_TYPE,  AM,  5900,  7000,  6000,  5, 0, 0},
  {  "40M", SW_BAND_TYPE, LSB,  7000,  7500,  7074,  1, 0, 0},
  {  "41M", SW_BAND_TYPE,  AM,  7200,  9000,  7210,  5, 0, 0},
  {  "31M", SW_BAND_TYPE,  AM,  9000, 10000,  9600,  5, 0, 0},
  {  "30M", SW_BAND_TYPE, USB, 10000, 10200, 10099,  1, 0, 0},
  {  "25M", SW_BAND_TYPE,  AM, 10200, 13500, 11700,  5, 0, 0},
  {  "22M", SW_BAND_TYPE,  AM, 13500, 14000, 13700,  5, 0, 0},
  {  "20M", SW_BAND_TYPE, USB, 14000, 14500, 14074,  1, 0, 0},
  {  "19M", SW_BAND_TYPE,  AM, 14500, 17500, 15700,  5, 0, 0},
  {  "17M", SW_BAND_TYPE,  AM, 17500, 18000, 17600,  5, 0, 0},
  {  "16M", SW_BAND_TYPE, USB, 18000, 18500, 18100,  1, 0, 0},
  {  "15M", SW_BAND_TYPE,  AM, 18500, 21000, 18950,  5, 0, 0},
  {  "14M", SW_BAND_TYPE, USB, 21000, 21500, 21074,  1, 0, 0},
  {  "13M", SW_BAND_TYPE,  AM, 21500, 24000, 21500,  5, 0, 0},
  {  "12M", SW_BAND_TYPE, USB, 24000, 25500, 24940,  1, 0, 0},
  {  "11M", SW_BAND_TYPE,  AM, 25500, 26100, 25800,  5, 0, 0},
  {   "CB", SW_BAND_TYPE,  AM, 26100, 28000, 27200,  1, 0, 0},
  {  "10M", SW_BAND_TYPE, USB, 28000, 30000, 28500,  1, 0, 0},
  {   "SW", SW_BAND_TYPE,  AM,   100, 30000, 15500,  5, 0, 0}
};

#define BAND_FM     0
#define BAND_LW     1
#define BAND_MW     2
#define BAND_800M   3
#define BAND_630M   4
#define BAND_160M   5
#define BAND_120M   6
#define BAND_90M    7
#define BAND_80M    8
#define BAND_75M    9
#define BAND_60M    10
#define BAND_49M    11
#define BAND_40M    12
#define BAND_41M    13
#define BAND_31M    14
#define BAND_30M    15
#define BAND_25M    16
#define BAND_22M    17
#define BAND_20M    18
#define BAND_19M    19
#define BAND_17M    20
#define BAND_16M    21
#define BAND_15M    22
#define BAND_14M    23
#define BAND_13M    24
#define BAND_12M    25
#define BAND_11M    26
#define BAND_CB     27
#define BAND_10M    28
#define BAND_SW     29

typedef struct
{
  uint8_t  digit;
  uint16_t Xdignumos;
  uint16_t Xdignumsr;
  uint16_t Ydignumos;
  uint16_t Ydignumsr;
  uint16_t Xdignumnr;
} DigNum;

uint8_t Xdignum = 139;
uint8_t Ydignum = 25;

DigNum dn[] = {
  { 0 , Xdignum, 21, Ydignum, 35,  0},
  { 1 , Xdignum, 21, Ydignum, 35, 30},
  { 2 , Xdignum, 21, Ydignum, 35, 59}

};

const int lastBand      = (sizeof band / sizeof(Band)) - 1;
const int lastdignum    = (sizeof dn / sizeof(DigNum)) - 1;

uint16_t bandMode[(lastBand + 1)];

#define offsetEEPROM       32
#define EEPROM_SIZE        2304

struct StoreStruct {
  byte     chkDigit;
  byte     bandIdx;
  uint16_t Freq;
  uint8_t  currentMode;
  uint8_t  bwIdxSSB;
  uint8_t  bwIdxAM;
  uint8_t  bwIdxFM;
  uint8_t  ssIdxMW;
  uint8_t  ssIdxAM;
  uint8_t  ssIdxFM;
  int      currentBFO;
  int      currentBFOmanu;
  uint8_t  AGCgain;
  uint8_t  currentVOL;
  uint8_t  currentBFOStep;
  uint8_t  RDS;
  unsigned long FreqSI5351;
  uint16_t currentBrightness;
  uint8_t  currentAGCgain;
  unsigned long calibratvalSI5351;
  int  BFOLW;
  int  BFOMW;
  int  BFO600M;
  int  BFO630M;
  int  BFO160M;
  int  BFO120M;
  int  BFO90M;
  int  BFO80M;
  int  BFO75M;
  int  BFO60M;
  int  BFO49M;
  int  BFO40M;
  int  BFO41M;
  int  BFO31M;
  int  BFO30M;
  int  BFO25M;
  int  BFO22M;
  int  BFO20M;
  int  BFO19M;
  int  BFO17M;
  int  BFO16M;
  int  BFO15M;
  int  BFO15H;
  int  BFO13M;
  int  BFO12M;
  int  BFO11M;
  int  BFOCB;
  int  BFO10M;
  int  BFOSW;

  byte     chk4;
  uint16_t PresetId;
  uint8_t  currentPRES;
  uint16_t currentFreqRetro0;
  uint16_t currentFreqRetro1;
  uint16_t currentFreqRetro2;
  uint16_t currentFreqRetro3;
  uint16_t currentFreqRetro4;
  uint16_t currentFreqRetro5;
  uint16_t currentFreqRetro6;
  uint16_t currentFreqRetro7;
  uint16_t currentFreqRetro8;
  uint16_t saverTime;
  uint8_t  RETROband;
  uint8_t  SCANscale;
  uint16_t boolOpt;

  byte    chk5;
  int     SquelchVal;

  byte    chk6;
  uint8_t wifiEnableAtBoot;
  char    wifiSSID[33];
  char    wifiPassword[65];
  byte    chk7;
  uint16_t batMinV;
  uint16_t batMaxV;
  byte    chk8;
  uint8_t ScanMarkSNR;
  uint8_t scanStopOnSignal;
  uint8_t scanStopSeconds;
  uint8_t scanWaterfallOn;
  byte    chk9;
  uint8_t cwDecoderOn;
  byte    chk10;
  uint16_t touchCalData[5];
  byte    chk11;
};

StoreStruct storage = {
  '@',
  0,
  8930,
  0,
  1,
  1,
  0,
  9,
  5,
  10,
  0,
  0,
  0,
  45,
  25,
  1,
  3276800,
  0,
  1,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,

  '@',
  777,
  0,
  8750,
  6400,
  153,
  522,
  2300,
  5900,
  9400,
  13570,
  18900,
  600,
  0,
  193,
  1181,

  '@',
  0,

  '@',
  1,
  "",
  "",
  '@',
  270,
  405,
  '@',
  3,
  0,
  5,
  0,
  '@',
  0,
  '@',
  {387, 3530, 246, 3555, 7},
  '@',
};

#define offsetMemoEEPROM       360

typedef struct
{
  uint16_t        freq;
  uint8_t         band;
  char            Name[21];
} MemoryBank;
MemoryBank MemoBank[75];

bool memoFavorite[75];
const int lastMemoBank = (sizeof MemoBank / sizeof(MemoryBank)) - 1;
Preset preset [lastMemory + lastMemoBank + 1];

typedef struct
{
  uint16_t        freq;
  uint8_t         band;
  uint8_t         mode;
  char           *Name;
} MemoryBankFile;
#include "Memory.h"
const int lastMemoBankFile = (sizeof MemoBankFile / sizeof(MemoryBankFile)) - 1;

uint8_t rssi = 0;
uint8_t stereo = 1;
uint8_t volume = DEFAULT_VOLUME;

Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);

TFT_eSPI tft    = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
SI4735 si4735;

void drawCWDecoderView() {
  tft.fillRect(Xsmtr + 2, Ysmtr + 6, 236, 46, TFT_BLACK);
  tft.drawRect(Xsmtr + 2, Ysmtr + 6, 236, 46, TFT_DARKGREY);

  tftPlSetFont(T1012_T);
  tftPlSetSize(1);
  tftPlSetStyle(NRG_T);
  tftPlSetDatum(BL_T);
  tftPlSetColor(TFT_CYAN, TFT_TRANS);
  tftPlPrint("DEKODER CW", Xsmtr + 8, Ysmtr + 17);

  tftPlSetColor(TFT_SILVER, TFT_TRANS);
  tftPlSetDatum(BR_T);
  tftPlPrint("avg:" + String(cwLastRawAvg) + " rng:" + String(cwLastRawRange) + " n:" + String(cwStateChangeCount), Xsmtr + 231, Ysmtr + 17);

  uint16_t dotColor = cwToneActive ? TFT_YELLOW : TFT_DARKGREY;
  tft.fillCircle(Xsmtr + 222, Ysmtr + 30, 4, dotColor);

  String displayText = cwDecodedText;
  if (displayText.length() > 24) displayText = displayText.substring(displayText.length() - 24);
  tftPlSetDatum(BL_T);
  tftPlSetColor(TFT_YELLOW, TFT_TRANS);
  tftPlPrint(displayText, Xsmtr + 8, Ysmtr + 37);
}

void drawLockIndicator() {
  tft.fillRect(0, 58, 42, 12, TFT_BLACK);
  if (screenLocked) {
    tftPlSetFont(T1012_T);
    tftPlSetSize(1);
    tftPlSetStyle(NRG_T);
    tftPlSetDatum(BL_T);
    tftPlSetColor(TFT_RED, TFT_TRANS);
    tftPlPrint("LOCK", 0, 68);
  }
}

void cycleStep() {
  int n;
  if (band[bandIdx].bandType == MW_BAND_TYPE or band[bandIdx].bandType == LW_BAND_TYPE) {
    const int vals[] = {1, 5, 9, 10};
    int idx = 0;
    for (int i = 0; i < 4; i++) if (vals[i] == ssIdxMW) idx = i;
    n = vals[(idx + 1) % 4];
    ssIdxMW = n;
  } else if (currentMode == FM) {
    n = (ssIdxFM == 1) ? 10 : 1;
    ssIdxFM = n;
  } else {
    const int vals[] = {1, 5, 9, 10};
    int idx = 0;
    for (int i = 0; i < 4; i++) if (vals[i] == ssIdxAM) idx = i;
    n = vals[(idx + 1) % 4];
    ssIdxAM = n;
  }
  si4735.setFrequencyStep(n);
  band[bandIdx].currentStep = n;
  setStep();
  Beep(1, 0);
}

void drawVFOIndicator() {
  tft.fillRect(46, 58, 24, 12, TFT_BLACK);
  tftPlSetFont(T1012_T);
  tftPlSetSize(1);
  tftPlSetStyle(NRG_T);
  tftPlSetDatum(BL_T);
  tftPlSetColor(TFT_CYAN, TFT_TRANS);
  tftPlPrint(vfoActiveIsB ? "VFO B" : "VFO A", 46, 68);
}

void swapVFO() {
  VFOState current;
  current.bandIdx = bandIdx;
  current.mode = currentMode;
  current.freq = si4735.getFrequency();
  current.step = band[bandIdx].currentStep;
  current.bfo = currentBFO;
  current.valid = true;
  if (vfoActiveIsB) vfoB = current; else vfoA = current;

  vfoActiveIsB = !vfoActiveIsB;
  VFOState *target = vfoActiveIsB ? &vfoB : &vfoA;

  if (!target->valid) {

    *target = current;
    target->valid = true;
  } else {
    bandIdx = target->bandIdx;
    currentMode = target->mode;
    currentBFO = target->bfo;
    band[bandIdx].currentFreq = (uint16_t) target->freq;
    band[bandIdx].currentStep = target->step;
    BandSet();
    currentFrequency = si4735.getFrequency();
  }
  FreqDispl();
  drawVFOIndicator();
}

void drawPriorityIndicator() {
  tft.fillRect(100, 58, 60, 12, TFT_BLACK);
  if (priorityOn) {
    tftPlSetFont(T1012_T);
    tftPlSetSize(1);
    tftPlSetStyle(NRG_T);
    tftPlSetDatum(BL_T);
    tftPlSetColor(priorityActive ? TFT_YELLOW : TFT_GREEN, TFT_TRANS);
    tftPlPrint(priorityActive ? "PRIO!" : "PRIO", 100, 68);
  }
}

unsigned long stopwatchElapsedMs() {
  unsigned long total = stopwatchAccum;
  if (stopwatchRunning) total += millis() - stopwatchStart;
  return total;
}

void drawStopwatch() {
  tft.fillRect(163, 58, 62, 12, TFT_BLACK);
  if (stopwatchAccum == 0 and !stopwatchRunning) return;
  unsigned long s = stopwatchElapsedMs() / 1000;
  char buf[8];
  sprintf(buf, "%02lu:%02lu", s / 60, s % 60);
  tftPlSetFont(T1012_T);
  tftPlSetSize(1);
  tftPlSetStyle(NRG_T);
  tftPlSetDatum(BL_T);
  tftPlSetColor(stopwatchRunning ? TFT_GREEN : TFT_SILVER, TFT_TRANS);
  tftPlPrint(String(buf), 163, 68);
}

void toggleStopwatch() {
  if (stopwatchRunning) {
    stopwatchAccum += millis() - stopwatchStart;
    stopwatchRunning = false;
  } else {
    stopwatchStart = millis();
    stopwatchRunning = true;
  }
  drawStopwatch();
}

void resetStopwatch() {
  stopwatchRunning = false;
  stopwatchAccum = 0;
  drawStopwatch();
}

void savePriorityTarget() {
  priorityTarget.bandIdx = bandIdx;
  priorityTarget.mode = currentMode;
  priorityTarget.freq = si4735.getFrequency();
  priorityTarget.step = band[bandIdx].currentStep;
  priorityTarget.bfo = currentBFO;
  priorityTarget.valid = true;
  priorityHasTarget = true;
  priorityOn = true;
  drawPriorityIndicator();
}

void returnFromPriority() {
  si4735.setAudioMute(audioMuteOn);
  bandIdx = priorityReturnState.bandIdx;
  currentMode = priorityReturnState.mode;
  currentBFO = priorityReturnState.bfo;
  band[bandIdx].currentFreq = (uint16_t) priorityReturnState.freq;
  band[bandIdx].currentStep = priorityReturnState.step;
  BandSet();
  currentFrequency = si4735.getFrequency();
  si4735.setAudioMute(audioMuteOff);
  priorityActive = false;
  FreqDispl();
  drawPriorityIndicator();
}

void checkPriorityChannel() {
  if (!priorityOn or !priorityHasTarget) return;

  if (priorityActive) {

    si4735.getCurrentReceivedSignalQuality();
    if (si4735.getCurrentSNR() >= PRIORITY_SNR_THRESHOLD) {
      priorityActiveSince = millis();
    } else if (millis() - priorityActiveSince > PRIORITY_RETURN_AFTER_MS) {
      returnFromPriority();
    }
    return;
  }

  if (millis() - lastPriorityCheck < PRIORITY_CHECK_INTERVAL_MS) return;
  lastPriorityCheck = millis();

  priorityReturnState.bandIdx = bandIdx;
  priorityReturnState.mode = currentMode;
  priorityReturnState.freq = si4735.getFrequency();
  priorityReturnState.step = band[bandIdx].currentStep;
  priorityReturnState.bfo = currentBFO;
  priorityReturnState.valid = true;

  si4735.setAudioMute(audioMuteOn);
  bandIdx = priorityTarget.bandIdx;
  currentMode = priorityTarget.mode;
  currentBFO = priorityTarget.bfo;
  band[bandIdx].currentFreq = (uint16_t) priorityTarget.freq;
  band[bandIdx].currentStep = priorityTarget.step;
  BandSet();
  delay(150);
  si4735.getCurrentReceivedSignalQuality();

  if (si4735.getCurrentSNR() >= PRIORITY_SNR_THRESHOLD) {
    si4735.setAudioMute(audioMuteOff);
    priorityActive = true;
    priorityActiveSince = millis();
    currentFrequency = si4735.getFrequency();
    FreqDispl();
  } else {

    bandIdx = priorityReturnState.bandIdx;
    currentMode = priorityReturnState.mode;
    currentBFO = priorityReturnState.bfo;
    band[bandIdx].currentFreq = (uint16_t) priorityReturnState.freq;
    band[bandIdx].currentStep = priorityReturnState.step;
    BandSet();
    currentFrequency = si4735.getFrequency();
    si4735.setAudioMute(audioMuteOff);
  }
  drawPriorityIndicator();
}

void updateFreqHistory() {
  float f = si4735.getFrequency();
  if (f != freqHistoryLastSeen) {
    freqHistoryLastSeen = f;
    freqHistoryStableSince = millis();
    freqHistoryRecorded = false;
    return;
  }
  if (freqHistoryRecorded) return;
  if (millis() - freqHistoryStableSince < FREQ_HISTORY_STABLE_MS) return;
  freqHistoryRecorded = true;
  if (freqHistoryCount > 0 and freqHistory[0].freq == f and freqHistory[0].bandIdx == bandIdx) return;
  for (int i = FREQ_HISTORY_LEN - 1; i > 0; i--) freqHistory[i] = freqHistory[i - 1];
  freqHistory[0].bandIdx = bandIdx;
  freqHistory[0].mode = currentMode;
  freqHistory[0].freq = f;
  if (freqHistoryCount < FREQ_HISTORY_LEN) freqHistoryCount++;
}

void showFreqHistory() {
  if (freqHistoryCount == 0) { ErrorBeep(); return; }
  int d = !screenV * 40;
  if (!screenV) tftTransRect(0, 0, 320, 240, TFT_MAROON); else tftTransRect(0, 0, 240, 320, TFT_MAROON);
  tft.fillRect(d, 15, 240, 210, TFT_BLACK);
  tft.drawRect(d, 15, 240, 210, TFT_DARKGREY);

  tftPlSetSize(1);
  tftPlSetStyle(BOL_T);
  tftPlSetFont(T1012_T);
  tftPlSetDatum(BC_T);
  tftPlSetColor(TFT_CYAN, TFT_TRANS);
  tftPlPrint("OSTATNIE CZĘSTOTLIWOŚCI", 120 + d, 30);

  const int rowH = 18;
  const int startY = 48;
  for (int i = 0; i < freqHistoryCount; i++) {
    String freqStr;
    if (freqHistory[i].mode == FM) freqStr = String(freqHistory[i].freq / 100.0, 2) + " MHz";
    else freqStr = String((long)freqHistory[i].freq) + " kHz";
    String label = freqStr + "  " + String(band[freqHistory[i].bandIdx].bandName);
    tftPlSetStyle(REG_T);
    tftPlSetColor(TFT_WHITE, TFT_TRANS);
    tftPlPrint(label, d + 10, startY + i * rowH);
  }

  tftPlSetColor(TFT_SILVER, TFT_TRANS);
  tftPlPrint("Dotknij pozycję aby wrócić, gdziekolwiek indziej aby zamknąć", d + 10, 218);

  bool done = false;
  unsigned long startTime = millis();
  while (!done and (millis() - startTime) < 15000) {
    bool touched = tft.getTouch(&x, &y);
    if (touched) {
      int idx = (y - (startY - rowH + 4)) / rowH;
      if (x > d and x < d + 240 and idx >= 0 and idx < freqHistoryCount) {
        bandIdx = freqHistory[idx].bandIdx;
        currentMode = freqHistory[idx].mode;
        band[bandIdx].currentFreq = (uint16_t) freqHistory[idx].freq;
        BandSet();
        currentFrequency = si4735.getFrequency();
      }
      done = true;
      delay(200);
    }
  }
  x = y = 0; pressed = false;
  returnLayer();
  FreqDispl();
}

String webJsonEscape(String s) {
  s.replace("\\", "\\\\");
  s.replace("\"", "\\\"");
  s.replace("\n", " ");
  s.replace("\r", "");
  return s;
}

String webJsonStatus() {
  const char* modeNames[] = {"FM", "LSB", "USB", "AM", "CW"};
  String freqStr;
  if (currentMode == FM) freqStr = String(si4735.getFrequency() / 100.0, 2);
  else freqStr = String((long)si4735.getFrequency());

  unsigned long sw = stopwatchElapsedMs() / 1000;
  char swbuf[8];
  sprintf(swbuf, "%02lu:%02lu", sw / 60, sw % 60);

  String rdsStation = (currentMode == FM and stationName) ? String(stationName) : "";
  String rdsText = (currentMode == FM and rdsMsg) ? String(rdsMsg) : "";
  rdsStation = webJsonEscape(rdsStation);
  rdsText = webJsonEscape(rdsText);

  String j = "{";
  j += "\"freq\":\"" + freqStr + "\",";
  j += "\"unit\":\"" + String(currentMode == FM ? "MHz" : "kHz") + "\",";
  j += "\"band\":\"" + String(band[bandIdx].bandName) + "\",";
  j += "\"bandIdx\":" + String(bandIdx) + ",";
  j += "\"mode\":\"" + String(modeNames[currentMode]) + "\",";
  j += "\"modeIdx\":" + String(currentMode) + ",";
  j += "\"rssi\":" + String(rssi) + ",";
  j += "\"snr\":" + String(NewSNR) + ",";
  j += "\"sLabel\":\"" + rssiToSLabel(rssi, currentMode == FM) + "\",";
  j += "\"volume\":" + String(currentVOL) + ",";
  j += "\"muted\":" + String(Mutestat ? "true" : "false") + ",";
  j += "\"agcMode\":" + String(AGCgain) + ",";
  j += "\"agcLevel\":" + String(currentAGCgain) + ",";
  j += "\"vfo\":\"" + String(vfoActiveIsB ? "B" : "A") + "\",";
  j += "\"priorityOn\":" + String(priorityOn ? "true" : "false") + ",";
  j += "\"priorityActive\":" + String(priorityActive ? "true" : "false") + ",";
  j += "\"stopwatch\":\"" + String(swbuf) + "\",";
  j += "\"stopwatchRunning\":" + String(stopwatchRunning ? "true" : "false") + ",";
  j += "\"locked\":" + String(screenLocked ? "true" : "false") + ",";
  j += "\"battery\":" + String(readVsupply(), 2) + ",";
  j += "\"rdsStation\":\"" + rdsStation + "\",";
  j += "\"rdsText\":\"" + rdsText + "\",";
  j += "\"scanning\":" + String(SCANbut ? "true" : "false") + ",";
  j += "\"scanPaused\":" + String(SCANpause ? "true" : "false") + ",";
  j += "\"scanFreq\":" + String(currentScanFreq);
  j += "}";
  return j;
}

void webHandleStatus() {
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleBandsList() {
  String j = "[";
  for (int i = 0; i <= lastBand; i++) {
    if (i) j += ",";
    j += "{\"idx\":" + String(i) + ",\"name\":\"" + String(band[i].bandName) + "\"}";
  }
  j += "]";
  webServer.send(200, "application/json", j);
}

void webHandleMemoryList() {
  const char* modeNames[] = {"FM", "LSB", "USB", "AM", "CW"};
  String j = "[";
  bool first = true;
  for (int i = 0; i <= lastMemoBank; i++) {
    if (MemoBank[i].freq < 153 or MemoBank[i].freq > 30000) continue;
    if (!first) j += ",";
    first = false;
    String name = String(MemoBank[i].Name);
    name.trim();
    uint8_t mb = MemoBank[i].band & 0x1F;
    uint8_t mm = MemoBank[i].band / 32;
    j += "{\"slot\":" + String(i) + ",\"name\":\"" + name + "\",\"freq\":" + String(MemoBank[i].freq);
    j += ",\"band\":\"" + String(band[mb].bandName) + "\",\"mode\":\"" + String(modeNames[mm]) + "\",\"fav\":" + String(memoFavorite[i] ? "true" : "false") + "}";
  }
  j += "]";
  webServer.send(200, "application/json", j);
}

void webHandleMemoryFavorite() {
  if (webServer.hasArg("slot")) {
    int i = webServer.arg("slot").toInt();
    if (i >= 0 and i <= lastMemoBank) memoFavorite[i] = !memoFavorite[i];
  }
  webHandleMemoryList();
}

void webHandleMemoryImport() {
  String body = webServer.arg("plain");
  int imported = 0, skipped = 0;
  int start = 0;
  int slot = 0;
  const char* modeNamesImp[] = {"FM", "LSB", "USB", "AM", "CW"};
  while (start < (int)body.length()) {
    int nl = body.indexOf('\n', start);
    if (nl < 0) nl = body.length();
    String line = body.substring(start, nl);
    line.trim();
    start = nl + 1;
    if (line.length() == 0) continue;

    int p1 = line.indexOf(';');
    int p2 = (p1 >= 0) ? line.indexOf(';', p1 + 1) : -1;
    int p3 = (p2 >= 0) ? line.indexOf(';', p2 + 1) : -1;
    if (p1 < 0 or p2 < 0 or p3 < 0) { skipped++; continue; }

    long freq = line.substring(0, p1).toInt();
    String name = line.substring(p1 + 1, p2);
    String bandNameImp = line.substring(p2 + 1, p3);
    String modeNameImp = line.substring(p3 + 1);
    name.trim(); bandNameImp.trim(); modeNameImp.trim();

    int bIdx = -1;
    for (int i = 0; i <= lastBand; i++) if (String(band[i].bandName) == bandNameImp) { bIdx = i; break; }
    int mIdx = -1;
    for (int i = 0; i < 5; i++) if (modeNameImp.equalsIgnoreCase(modeNamesImp[i])) { mIdx = i; break; }

    if (bIdx < 0 or mIdx < 0 or freq < 153 or freq > 30000) { skipped++; continue; }

    while (slot <= lastMemoBank and MemoBank[slot].freq >= 153 and MemoBank[slot].freq <= 30000) slot++;
    if (slot > lastMemoBank) break;

    MemoBank[slot].freq = freq;
    MemoBank[slot].band = bIdx + (mIdx * 32);
    name.toCharArray(MemoBank[slot].Name, 21);
    slot++;
    imported++;
  }
  if (imported > 0) saveMemo();
  webServer.send(200, "application/json", "{\"imported\":" + String(imported) + ",\"skipped\":" + String(skipped) + "}");
}

void webHandleMemoryRecall() {
  if (webServer.hasArg("slot")) {
    int i = webServer.arg("slot").toInt();
    if (i >= 0 and i <= lastMemoBank and MemoBank[i].freq >= 153 and MemoBank[i].freq <= 30000) {
      uint8_t mb = MemoBank[i].band & 0x1F;
      uint8_t mm = MemoBank[i].band / 32;
      if (bandIdx != mb or currentMode != mm) {
        bandIdx = mb;
        currentMode = mm;
        BandSet();
      }
      si4735.setFrequency(MemoBank[i].freq);
      band[bandIdx].currentFreq = MemoBank[i].freq;
      currentFrequency = si4735.getFrequency();
      FreqDispl();
    }
  }
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleSetFreq() {
  if (webServer.hasArg("f")) {
    float f = webServer.arg("f").toFloat();
    uint16_t target = (currentMode == FM) ? (uint16_t)(f * 100) : (uint16_t)f;
    if (target >= band[bandIdx].minimumFreq and target <= band[bandIdx].maximumFreq) {
      si4735.setFrequency(target);
      currentFrequency = si4735.getFrequency();
      band[bandIdx].currentFreq = target;
      FreqDispl();
    }
  }
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleBand() {
  if (webServer.hasArg("idx")) {
    int i = webServer.arg("idx").toInt();
    if (i >= 0 and i <= lastBand and i != bandIdx) {
      bandIdx = i;
      BandSet();
      currentFrequency = si4735.getFrequency();
      FreqDispl();
    }
  }
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleMode() {
  if (webServer.hasArg("m")) {
    int m = webServer.arg("m").toInt();
    if (m >= 0 and m <= 4 and m != currentMode and bandIdx != 0) {
      currentMode = m;
      BandSet();
      currentFrequency = si4735.getFrequency();
      FreqDispl();
    }
  }
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleVolume() {
  if (webServer.hasArg("v")) {
    int v = webServer.arg("v").toInt();
    if (v < MinVOL) v = MinVOL;
    if (v > MaxVOL) v = MaxVOL;
    currentVOL = v;
    si4735.setVolume(currentVOL);
  }
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleMute() {
  if (webServer.hasArg("on")) {
    Mutestat = (webServer.arg("on").toInt() == 1);
    si4735.setAudioMute(Mutestat ? audioMuteOn : audioMuteOff);
  }
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleAGC() {
  if (webServer.hasArg("mode")) {
    AGCgain = webServer.arg("mode").toInt();
    if (webServer.hasArg("level")) {
      uint8_t maxG = (currentMode == FM) ? MaxAGCgainFM : MaxAGCgainAM;
      int lvl = webServer.arg("level").toInt();
      if (lvl < MinAGCgain) lvl = MinAGCgain;
      if (lvl > maxG) lvl = maxG;
      currentAGCgain = lvl;
    }
    checkAGC();
  }
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleVFOSwap() {
  swapVFO();
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandlePriority() {
  if (webServer.hasArg("action")) {
    String a = webServer.arg("action");
    if (a == "save") savePriorityTarget();
    else if (a == "toggle") { priorityOn = !priorityOn; drawPriorityIndicator(); }
  }
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleStopwatch() {
  if (webServer.hasArg("action")) {
    String a = webServer.arg("action");
    if (a == "start" or a == "stop") toggleStopwatch();
    else if (a == "reset") resetStopwatch();
  }
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleLock() {
  if (webServer.hasArg("on")) {
    screenLocked = (webServer.arg("on").toInt() == 1);
    drawLockIndicator();
  }
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleScan() {
  if (webServer.hasArg("action") and SCANbut) {
    String a = webServer.arg("action");
    if (a == "pause" and !SCANpause) { SCANpause = true; pauseSCAN(); }
    else if (a == "resume" and SCANpause) { SCANpause = false; pauseSCAN(); }
  }
  webServer.send(200, "application/json", webJsonStatus());
}

String webJsonSettings() {
  String j = "{";
  j += "\"wifiEnable\":" + String(wifiEnable ? "true" : "false") + ",";
  j += "\"RDSalways\":" + String(RDSalways ? "true" : "false") + ",";
  j += "\"saverOn\":" + String(saverOn ? "true" : "false") + ",";
  j += "\"digitLigth\":" + String(digitLigth ? "true" : "false") + ",";
  j += "\"batShow\":" + String(batShow ? "true" : "false") + ",";
  j += "\"nightModeOn\":" + String(nightModeOn ? "true" : "false") + ",";
  j += "\"analogMeterOn\":" + String(analogMeterOn ? "true" : "false") + ",";
  j += "\"rssiHistoryOn\":" + String(rssiHistoryOn ? "true" : "false") + ",";
  j += "\"cwDecoderOn\":" + String(cwDecoderOn ? "true" : "false") + ",";
  j += "\"scanWaterfallOn\":" + String(scanWaterfallOn ? "true" : "false") + ",";
  j += "\"scanStopOnSignal\":" + String(scanStopOnSignal ? "true" : "false") + ",";
  j += "\"ScanMarkSNR\":" + String(ScanMarkSNR) + ",";
  j += "\"screenV\":" + String(screenV ? "true" : "false") + ",";
  j += "\"VHFon\":" + String(VHFon ? "true" : "false") + ",";
  j += "\"seekAccuracy\":" + String(seekAccuracy ? "true" : "false") + ",";
  j += "\"closeCallOn\":" + String(closeCallOn ? "true" : "false");
  j += "}";
  return j;
}

void webHandleSettingsGet() {
  webServer.send(200, "application/json", webJsonSettings());
}

void webHandleSettingsSet() {
  if (webServer.hasArg("key") and webServer.hasArg("value")) {
    String k = webServer.arg("key");
    String v = webServer.arg("value");
    bool bv = (v == "1" or v == "true");
    if (k == "wifiEnable") wifiEnable = bv;
    else if (k == "RDSalways") RDSalways = bv;
    else if (k == "saverOn") saverOn = bv;
    else if (k == "digitLigth") digitLigth = bv;
    else if (k == "batShow") batShow = bv;
    else if (k == "nightModeOn") nightModeOn = bv;
    else if (k == "analogMeterOn") analogMeterOn = bv;
    else if (k == "rssiHistoryOn") rssiHistoryOn = bv;
    else if (k == "cwDecoderOn") { cwDecoderOn = bv; updateCWDecoderPin(); }
    else if (k == "scanWaterfallOn") scanWaterfallOn = bv;
    else if (k == "scanStopOnSignal") scanStopOnSignal = bv;
    else if (k == "ScanMarkSNR") { int sv = v.toInt(); if (sv >= 1 and sv <= 20) ScanMarkSNR = sv; }
    else if (k == "screenV") screenV = bv;
    else if (k == "VHFon") VHFon = bv;
    else if (k == "seekAccuracy") seekAccuracy = bv;
    else if (k == "closeCallOn") closeCallOn = bv;
  }
  webServer.send(200, "application/json", webJsonSettings());
}

void webHandleMeterCycle() {
  cycleMeterMode();
  webServer.send(200, "application/json", webJsonStatus());
}

void webHandleConfigExport() {
  static const char hexch[] = "0123456789abcdef";
  String hex = "";
  uint8_t* raw = (uint8_t*)&storage;
  for (size_t i = 0; i < sizeof(StoreStruct); i++) {
    hex += hexch[(raw[i] >> 4) & 0xF];
    hex += hexch[raw[i] & 0xF];
  }
  webServer.sendHeader("Content-Disposition", "attachment; filename=ats25x2_config.txt");
  webServer.send(200, "text/plain", hex);
}

uint8_t webHexNibble(char c) {
  if (c >= '0' and c <= '9') return c - '0';
  if (c >= 'a' and c <= 'f') return c - 'a' + 10;
  if (c >= 'A' and c <= 'F') return c - 'A' + 10;
  return 0;
}

void webHandleConfigImport() {
  String body = webServer.arg("plain");
  body.trim();
  if (body.length() == sizeof(StoreStruct) * 2) {
    uint8_t* raw = (uint8_t*)&storage;
    for (size_t i = 0; i < sizeof(StoreStruct); i++) {
      raw[i] = (webHexNibble(body[i * 2]) << 4) | webHexNibble(body[i * 2 + 1]);
    }
    saveConfig();
    webServer.send(200, "text/plain", "OK");
  } else {
    webServer.send(400, "text/plain", "Zla dlugosc danych - plik nie pasuje do tej wersji firmware");
  }
}

void webHandleSkipList() {
  String j = "[";
  for (int i = 0; i < skipListCount; i++) { if (i) j += ","; j += String(skipList[i]); }
  j += "]";
  webServer.send(200, "application/json", j);
}

void webHandleSkipListAdd() {
  if (webServer.hasArg("freq") and skipListCount < SKIP_LIST_MAX) {
    long f = webServer.arg("freq").toInt();
    if (!isFreqSkipped(f)) skipList[skipListCount++] = f;
  }
  webHandleSkipList();
}

void webHandleSkipListRemove() {
  if (webServer.hasArg("freq")) {
    long f = webServer.arg("freq").toInt();
    for (int i = 0; i < skipListCount; i++) {
      if (skipList[i] == f) {
        for (int k = i; k < skipListCount - 1; k++) skipList[k] = skipList[k + 1];
        skipListCount--;
        break;
      }
    }
  }
  webHandleSkipList();
}

void webHandleSkipListExport() {
  String txt = "";
  for (int i = 0; i < skipListCount; i++) txt += String(skipList[i]) + "\n";
  webServer.sendHeader("Content-Disposition", "attachment; filename=skiplist.txt");
  webServer.send(200, "text/plain", txt);
}

void webHandleSkipListImport() {
  String body = webServer.arg("plain");
  skipListCount = 0;
  int start = 0;
  while (start < (int)body.length() and skipListCount < SKIP_LIST_MAX) {
    int nl = body.indexOf('\n', start);
    if (nl < 0) nl = body.length();
    String line = body.substring(start, nl);
    line.trim();
    if (line.length() > 0) {
      long f = line.toInt();
      if (f > 0) skipList[skipListCount++] = f;
    }
    start = nl + 1;
  }
  webHandleSkipList();
}

void webHandleDiscovery() {
  String j = "[";
  for (int n = 0; n < discoveryLogCount; n++) {
    int i = (discoveryLogHead - 1 - n + DISCOVERY_LOG_MAX * 2) % DISCOVERY_LOG_MAX;
    if (n) j += ",";
    j += "{\"time\":\"" + String(discoveryLog[i].timeStr) + "\",\"freq\":" + String(discoveryLog[i].freq) + ",\"snr\":" + String(discoveryLog[i].snr) + "}";
  }
  j += "]";
  webServer.send(200, "application/json", j);
}

void webHandleDiscoveryExport() {
  String txt = "CZAS;CZESTOTLIWOSC_KHZ;SNR\n";
  for (int n = 0; n < discoveryLogCount; n++) {
    int i = (discoveryLogHead - 1 - n + DISCOVERY_LOG_MAX * 2) % DISCOVERY_LOG_MAX;
    txt += String(discoveryLog[i].timeStr) + ";" + String(discoveryLog[i].freq) + ";" + String(discoveryLog[i].snr) + "\n";
  }
  webServer.sendHeader("Content-Disposition", "attachment; filename=discovery.txt");
  webServer.send(200, "text/plain", txt);
}

void webHandleRoot() {
  String html = R"HTMLPAGE(<!DOCTYPE html><html><head><meta charset='utf-8'>
<meta name='viewport' content='width=device-width,initial-scale=1'>
<title>ATS25X2 - Panel sterowania</title>
<style>
:root{--amber:#ffae00;--teal:#29c6de;--green:#39e66a;--red:#e62829;--text:#ddd;--dim:#888}
*{box-sizing:border-box}
body{font-family:-apple-system,Segoe UI,Roboto,sans-serif;background:#050505;color:var(--text);margin:0;padding:10px}
.rig{max-width:900px;margin:0 auto;background:linear-gradient(180deg,#3a3a3c,#1c1c1e 6%,#232325 94%,#38383a);
  border-radius:14px;padding:10px;box-shadow:0 8px 24px rgba(0,0,0,.6),inset 0 1px 0 rgba(255,255,255,.08);border:1px solid #000}
.brand{display:flex;justify-content:space-between;align-items:baseline;padding:2px 8px 8px;color:#aaa;font-size:.7em;letter-spacing:2px;text-transform:uppercase}
.brand b{color:var(--amber);letter-spacing:1px}

.topbar{display:flex;gap:14px;flex-wrap:wrap;background:#0a0a0a;border:1px solid #000;border-radius:6px;padding:6px 12px;margin-bottom:8px;font-size:.72em;color:var(--teal);font-family:monospace}
.topbar span b{color:#666;font-weight:normal;margin-right:3px}

.mainGrid{display:flex;gap:10px}
.sidebar{display:flex;flex-direction:column;gap:5px;width:78px;flex-shrink:0}
.sideBtn{background:linear-gradient(180deg,#3d3d3f,#252527);color:#bbb;border:1px solid #111;border-radius:5px;padding:8px 3px;font-size:.68em;cursor:pointer;text-align:center;line-height:1.25;box-shadow:0 1px 0 rgba(255,255,255,.06) inset}
.sideBtn .l1{display:block;color:#eee;font-weight:bold}
.sideBtn.on{background:linear-gradient(180deg,var(--amber),#b37800);color:#000}
.sideBtn.on .l1{color:#000}
.sideBtn.red.on{background:linear-gradient(180deg,var(--red),#8f1414);color:#fff}
.sideBtn.red.on .l1{color:#fff}

.content{flex:1;min-width:0}
.metersRow{display:flex;gap:8px;background:#0a0a0a;border:1px solid #000;border-radius:6px;padding:6px;margin-bottom:8px}
.gaugeBox{flex:1;text-align:center}
.gaugeBox .lbl{font-size:.65em;color:var(--dim);letter-spacing:1px;margin-top:2px}
.gaugeBox .rd{font-size:.85em;color:var(--amber);font-family:monospace}
.gaugeSvg{width:100%;height:70px}

.vfoRow{display:flex;gap:8px;margin-bottom:8px}
.vfoBox{flex:1;border:1px solid #1c2b2f;border-radius:6px;padding:6px 10px;background:#040505;position:relative}
.vfoBox.active{border-color:var(--amber);box-shadow:0 0 8px rgba(255,174,0,.25) inset}
.vfoTag{font-size:.62em;letter-spacing:2px;color:var(--dim)}
.vfoBox.active .vfoTag{color:var(--amber)}
.vfoFreq{font-family:'Courier New',monospace;font-size:1.3em;color:#3a3a3a;font-weight:bold}
.vfoBox.active .vfoFreq{color:var(--amber);text-shadow:0 0 6px rgba(255,174,0,.35)}
.swapBtn{align-self:center;background:#222;border:1px solid #444;color:var(--teal);border-radius:6px;width:30px;cursor:pointer}

.freqEntry{display:flex;gap:6px;margin-bottom:8px}
.freqEntry input{background:#050505;color:var(--text);border:1px solid #222;border-radius:5px;padding:7px;font-size:.85em;flex:1}
.freqEntry button{background:linear-gradient(180deg,#3d3d3f,#252527);color:#ccc;border:1px solid #111;border-radius:5px;padding:7px 12px;font-size:.8em;cursor:pointer}

.rdsPanel{background:#0a0f0a;border:1px solid #000;border-radius:6px;padding:8px 10px;margin-bottom:8px;min-height:20px;font-size:.82em;color:var(--green);display:flex;flex-wrap:wrap;gap:6px 14px;align-items:baseline}
.rdsPanel .lbl{color:var(--dim);font-size:.75em;text-transform:uppercase;letter-spacing:1px;margin-right:4px}
.rdsPanel .txt{color:var(--dim);font-size:.9em}
.rdsPanel .empty{color:#444;font-style:italic}

.quickRow{display:flex;gap:5px;flex-wrap:wrap;margin-bottom:4px}
.quickRow button{background:linear-gradient(180deg,#3d3d3f,#252527);color:#ccc;border:1px solid #111;border-radius:5px;padding:6px 4px;font-size:.72em;cursor:pointer;flex:1;min-width:44px}
.quickRow button.on{background:linear-gradient(180deg,var(--amber),#b37800);color:#000;font-weight:bold}

.section{margin-top:8px;background:linear-gradient(180deg,#2a2a2c,#202022);border:1px solid #000;border-radius:8px;padding:8px 10px 10px;box-shadow:inset 0 1px 0 rgba(255,255,255,.05);display:none}
.section.open{display:block}
.section h2{font-size:.66em;color:#999;text-transform:uppercase;letter-spacing:2px;margin:0 0 6px;border-bottom:1px solid #151515;padding-bottom:5px}
.row{display:flex;gap:8px;align-items:center;margin-bottom:6px}
.row:last-child{margin-bottom:0}
.row label{font-size:.75em;color:var(--dim);min-width:48px}
input[type=range]{flex:1;accent-color:var(--amber)}
.val{color:var(--amber);font-family:monospace;min-width:34px;text-align:right;font-size:.8em}
.memlist{max-height:170px;overflow-y:auto}
.memitem{display:flex;justify-content:space-between;padding:6px 3px;border-bottom:1px solid #1a1a1a;cursor:pointer;font-size:.8em}
.memitem:hover{background:#1e1e1e}
.memitem .n{color:#ddd}
.memitem .f{color:var(--dim);font-family:monospace}
.settingsRow{display:flex;justify-content:space-between;align-items:center;padding:6px 0;border-bottom:1px solid #1a1a1a;font-size:.8em}
.settingsRow:last-child{border-bottom:none}
.toggle{width:38px;height:20px;background:#151515;border:1px solid #333;border-radius:11px;position:relative;cursor:pointer;flex-shrink:0}
.toggle.on{background:linear-gradient(180deg,var(--green),#1c8f3d);border-color:#000}
.toggle .dot{width:14px;height:14px;background:#ccc;border-radius:50%;position:absolute;top:2px;left:2px;transition:left .2s}
.toggle.on .dot{left:20px;background:#fff}
.rigBtn{background:linear-gradient(180deg,#3d3d3f,#252527);color:#ccc;border:1px solid #111;border-radius:5px;padding:7px;font-size:.78em;cursor:pointer}
.rigBtn.wide{width:100%}
.btnGrid{display:grid;grid-template-columns:repeat(4,1fr);gap:5px}
.btnGrid button.on{background:linear-gradient(180deg,var(--amber),#b37800);color:#000;font-weight:bold}

#toast{position:fixed;bottom:16px;left:50%;transform:translateX(-50%);background:var(--amber);color:#000;padding:8px 16px;border-radius:6px;opacity:0;transition:opacity .3s;pointer-events:none;font-size:.85em;font-weight:bold}
@media(max-width:520px){.mainGrid{flex-direction:column}.sidebar{width:100%;flex-direction:row;flex-wrap:wrap}.sideBtn{flex:1;min-width:70px}}

/* ZMIANA: tryb kompaktowy - mniejsze przyciski/odstepy, wiecej widocznych informacji na raz */
body.compact .rig{max-width:1100px}
body.compact .section{padding:5px 7px 6px;margin-top:5px}
body.compact .section h2{font-size:.6em;margin-bottom:4px;padding-bottom:3px}
body.compact .rigBtn{padding:4px;font-size:.68em}
body.compact .sideBtn{padding:5px 2px;font-size:.6em}
body.compact .row{margin-bottom:4px;gap:5px}
body.compact .memitem{padding:3px 2px;font-size:.72em}
body.compact .gaugeSvg{height:52px}
body.compact .vfoFreq{font-size:1.05em}
body.compact .lcd{padding:7px 9px}
body.compact .mainGrid{gap:6px}
body.compact .sidebar{width:64px;gap:3px}
</style></head><body>

<div class='rig'>
  <div class='brand'><b>ATS25X2</b> <span>PANEL STEROWANIA</span> <span onclick='toggleCompact()' style='cursor:pointer;color:var(--teal)' id='compactBtn'>[kompakt]</span></div>

  <div class='topbar'>
    <span><b>PASMO</b><span id='tbBand'>-</span></span>
    <span><b>MOD</b><span id='tbMode'>-</span></span>
    <span><b>VFO</b><span id='tbVfo'>-</span></span>
    <span><b>AGC</b><span id='tbAgc'>-</span></span>
    <span><b>BAT</b><span id='tbBat'>-</span></span>
  </div>

  <div class='mainGrid'>
    <div class='sidebar'>
      <button class='sideBtn' id='btnMute' onclick='toggleMute()'><span class='l1'>MUTE</span>audio</button>
      <button class='sideBtn' id='btnLock' onclick='toggleLock()'><span class='l1'>LOCK</span>ekran</button>
      <button class='sideBtn' id='btnAgc' onclick='cycleAgc()'><span class='l1'>AGC</span>-</button>
      <button class='sideBtn' onclick='vfoSwap()'><span class='l1'>A/B</span>VFO</button>
      <button class='sideBtn' onclick='prioSave()'><span class='l1'>PRIO</span>zapisz</button>
      <button class='sideBtn' id='btnPrio' onclick='prioToggle()'><span class='l1'>PRIO</span>wl/wyl</button>
      <button class='sideBtn' id='btnSw' onclick='swToggle()'><span class='l1'>STOP.</span>start</button>
      <button class='sideBtn' id='btnMenu' onclick='toggleSection("menuSec")'><span class='l1'>MENU</span>opcje</button>
    </div>

    <div class='content'>
      <div class='metersRow'>
        <div class='gaugeBox'>
          <svg class='gaugeSvg' viewBox='0 0 200 110'>
            <path d='M14,100 A86,86 0 0,1 186,100' fill='none' stroke='#222' stroke-width='10'/>
            <path d='M14,100 A86,86 0 0,1 100,14' fill='none' stroke='#264d2e' stroke-width='10'/>
            <path d='M100,14 A86,86 0 0,1 186,100' fill='none' stroke='#4d2020' stroke-width='10'/>
            <line id='rssiNeedle' x1='100' y1='100' x2='100' y2='24' stroke='#e62829' stroke-width='3' style='transform-origin:100px 100px;transition:transform .3s'/>
            <circle cx='100' cy='100' r='6' fill='#ccc'/>
          </svg>
          <div class='lbl'>SIGNAL</div>
          <div class='rd' id='sigVal'>S0</div>
        </div>
        <div class='gaugeBox'>
          <svg class='gaugeSvg' viewBox='0 0 200 110'>
            <path d='M14,100 A86,86 0 0,1 186,100' fill='none' stroke='#222' stroke-width='10'/>
            <line id='snrNeedle' x1='100' y1='100' x2='100' y2='24' stroke='#29c6de' stroke-width='3' style='transform-origin:100px 100px;transition:transform .3s'/>
            <circle cx='100' cy='100' r='6' fill='#ccc'/>
          </svg>
          <div class='lbl'>SNR</div>
          <div class='rd' id='snrVal'>0 dB</div>
        </div>
      </div>
      <div class='row' style='margin-bottom:8px'>
        <button class='rigBtn wide' onclick='meterCycle()'>PRZELACZ METER NA URZADZENIU</button>
      </div>

      <div class='freqEntry'>
        <input type='text' id='freqInput' placeholder='wpisz czestotliwosc'>
        <button onclick='setFreq()'>USTAW</button>
      </div>

      <div class='vfoRow'>
        <div class='vfoBox' id='vfoABox'>
          <div class='vfoTag'>VFO A</div>
          <div class='vfoFreq' id='vfoAFreq'>---</div>
        </div>
        <button class='swapBtn' onclick='vfoSwap()'>&#8646;</button>
        <div class='vfoBox' id='vfoBBox'>
          <div class='vfoTag'>VFO B</div>
          <div class='vfoFreq' id='vfoBFreq'>---</div>
        </div>
      </div>

      <div class='rdsPanel' id='rdsPanel'>
        <span id='rdsEmpty' class='empty'>RDS: brak danych</span>
        <span id='rdsStationWrap' style='display:none'><span class='lbl'>Stacja</span><span id='rdsStation'></span></span>
        <span id='rdsTextWrap' style='display:none'><span class='lbl'>RDS</span><span class='txt' id='rdsText'></span></span>
      </div>

      <div class='quickRow' id='bandRow'></div>
      <div class='quickRow' id='modeRow'>
        <button data-m='0' onclick='setModeBtn(0)'>FM</button>
        <button data-m='1' onclick='setModeBtn(1)'>LSB</button>
        <button data-m='2' onclick='setModeBtn(2)'>USB</button>
        <button data-m='3' onclick='setModeBtn(3)'>AM</button>
        <button data-m='4' onclick='setModeBtn(4)'>CW</button>
      </div>
    </div>
  </div>

  <div class='section' id='scanSec'>
    <h2>Skaner</h2>
    <div class='row'><span style='flex:1'>Czestotliwosc skanera: <b class='val' id='scanFreqVal'>-</b> kHz</span></div>
    <div class='row'><button class='rigBtn wide' onclick='scanToggle()' id='scanBtn'>PAUZA</button></div>
  </div>

  <div class='section' id='skipSec'>
    <h2>Lista wykluczen (skip list)</h2>
    <div class='row'>
      <input type='text' id='skipInput' placeholder='czestotliwosc kHz' style='background:#050505;color:#ddd;border:1px solid #222;border-radius:5px;padding:7px;flex:1'>
      <button class='rigBtn' onclick='skipAdd()'>DODAJ</button>
    </div>
    <div class='memlist' id='skipList'>ladowanie...</div>
    <div class='row' style='margin-top:6px'>
      <button class='rigBtn wide' onclick='skipExport()'>Pobierz .txt</button>
      <button class='rigBtn wide' onclick='document.getElementById("skipFile").click()'>Wczytaj .txt</button>
      <input type='file' id='skipFile' accept='.txt' style='display:none' onchange='skipImport(this.files[0])'>
    </div>
  </div>

  <div class='section' id='discSec'>
    <h2>Discovery - log wykrytych sygnalow</h2>
    <div class='memlist' id='discList'>ladowanie...</div>
    <div class='row' style='margin-top:6px'>
      <button class='rigBtn wide' onclick='discExport()'>Pobierz .txt</button>
      <button class='rigBtn wide' onclick='loadDiscovery()'>Odswiez</button>
    </div>
  </div>

  <div class='section' id='audioSec'>
    <h2>AF Gain</h2>
    <div class='row'>
      <label>VOL</label>
      <input type='range' id='volSlider' min='0' max='63' oninput='setVolLive(this.value)' onchange='setVol(this.value)'>
      <span class='val' id='volVal'>0</span>
    </div>
    <div class='row' id='agcLevelRow' style='display:none'>
      <label>AGC LVL</label>
      <input type='range' id='agcSlider' min='1' max='37' oninput='agcLevelLive(this.value)' onchange='setAgcLevel(this.value)'>
      <span class='val' id='agcVal'>1</span>
    </div>
    <div class='row'>
      <label>STOPER</label>
      <span class='val' id='swVal' style='flex:1;text-align:left'>00:00</span>
      <button class='rigBtn' onclick='swReset()'>RESET</button>
    </div>
  </div>

  <div class='section' id='memSec'>
    <h2>Pamiec stacji</h2>
    <div class='row'>
      <input type='text' id='memSearch' placeholder='szukaj po nazwie...' oninput='renderMemList()' style='background:#050505;color:#ddd;border:1px solid #222;border-radius:5px;padding:7px;flex:1'>
    </div>
    <div class='row'>
      <select id='memSort' onchange='renderMemList()' style='background:#050505;color:#ddd;border:1px solid #222;border-radius:5px;padding:6px;flex:1'>
        <option value='none'>Bez sortowania</option>
        <option value='name'>Sortuj: nazwa</option>
        <option value='freq'>Sortuj: czestotliwosc</option>
        <option value='band'>Sortuj: pasmo</option>
      </select>
      <button class='rigBtn' id='favFilterBtn' onclick='toggleFavFilter()'>&#9733; TYLKO ULUBIONE</button>
    </div>
    <div class='row'>
      <label style='display:flex;align-items:center;gap:5px;font-size:.8em;color:var(--dim)'>
        <input type='checkbox' id='memGroupBand' onchange='renderMemList()'> grupuj wg pasma
      </label>
    </div>
    <div class='memlist' id='memList'>ladowanie...</div>
    <div class='row' style='margin-top:6px;border-top:1px solid #1a1a1a;padding-top:8px'>
      <button class='rigBtn wide' onclick='document.getElementById("memFile").click()'>Import masowy (.txt)</button>
      <input type='file' id='memFile' accept='.txt' style='display:none' onchange='memImport(this.files[0])'>
    </div>
    <div class='row' style='font-size:.68em;color:var(--dim)'>Format: czestotliwosc;nazwa;pasmo;tryb (jedna stacja na linie), np. 7175;HAM RADIO;40M;LSB</div>
  </div>

  <div class='section' id='menuSec'>
    <h2>Menu opcji</h2>
    <div class='row'>
      <button class='rigBtn' onclick='toggleSection("audioSec")'>Audio/Stoper</button>
      <button class='rigBtn' onclick='toggleSection("memSec")'>Pamiec</button>
      <button class='rigBtn' onclick='toggleSection("skipSec")'>Skip list</button>
      <button class='rigBtn' onclick='toggleSection("discSec"); loadDiscovery()'>Discovery</button>
    </div>
    <div id='settingsList' style='margin-top:6px'>ladowanie...</div>
    <div class='row' style='margin-top:8px;border-top:1px solid #1a1a1a;padding-top:8px'>
      <button class='rigBtn wide' onclick='configExport()'>Pobierz konfiguracje (.txt)</button>
      <button class='rigBtn wide' onclick='document.getElementById("cfgFile").click()'>Wczytaj konfiguracje</button>
      <input type='file' id='cfgFile' accept='.txt' style='display:none' onchange='configImport(this.files[0])'>
    </div>
  </div>
</div>

<div id='toast'></div>

<script>
let lastVolSet = -1, lastAgcSet = -1, agcModeCur = 1;
let lastRdsText = '', lastRdsStation = ''; 
async function api(path, params) {
  let url = path;
  if (params) url += '?' + new URLSearchParams(params).toString();
  const r = await fetch(url);
  const j = await r.json();
  render(j);
  return j;
}
function toast(msg) {
  const t = document.getElementById('toast');
  t.textContent = msg; t.style.opacity = 1;
  setTimeout(()=>t.style.opacity=0, 1200);
}
function toggleSection(id) {
  document.querySelectorAll('.section').forEach(s => { if (s.id !== id) s.classList.remove('open'); });
  document.getElementById(id).classList.toggle('open');
}
function needleAngle(pct) { return -90 + Math.min(1, Math.max(0, pct)) * 180; }
function render(j) {
  document.getElementById('tbBand').textContent = j.band;
  document.getElementById('tbMode').textContent = j.mode;
  document.getElementById('tbVfo').textContent = j.vfo;
  document.getElementById('tbAgc').textContent = ['OFF','AUTO','MAN'][j.agcMode];
  document.getElementById('tbBat').textContent = j.battery + 'V';

  const activeIsB = (j.vfo == 'B');
  document.getElementById('vfoABox').className = 'vfoBox' + (activeIsB ? '' : ' active');
  document.getElementById('vfoBBox').className = 'vfoBox' + (activeIsB ? ' active' : '');
  document.getElementById(activeIsB?'vfoBFreq':'vfoAFreq').textContent = j.freq + ' ' + j.unit;

  document.getElementById('sigVal').textContent = j.sLabel;
  document.getElementById('snrVal').textContent = j.snr + ' dB';
  document.getElementById('rssiNeedle').style.transform = 'rotate(' + needleAngle(j.rssi/70) + 'deg)';
  document.getElementById('snrNeedle').style.transform = 'rotate(' + needleAngle(j.snr/30) + 'deg)';

  if (lastVolSet < 0 || Date.now() - lastVolSet > 1500) {
    document.getElementById('volSlider').value = j.volume;
    document.getElementById('volVal').textContent = j.volume;
  }
  document.getElementById('btnMute').className = j.muted ? 'sideBtn red on' : 'sideBtn';

  document.querySelectorAll('#modeRow button').forEach(b => b.className = (parseInt(b.dataset.m)===j.modeIdx)?'on':'');
  document.querySelectorAll('#bandRow button').forEach(b => b.className = (parseInt(b.dataset.idx)===j.bandIdx)?'on':'');

  agcModeCur = j.agcMode;
  document.getElementById('btnAgc').className = j.agcMode ? 'sideBtn on' : 'sideBtn';
  document.getElementById('agcLevelRow').style.display = (j.agcMode==2) ? 'flex' : 'none';
  if (lastAgcSet < 0 || Date.now() - lastAgcSet > 1500) {
    document.getElementById('agcSlider').value = j.agcLevel;
    document.getElementById('agcVal').textContent = j.agcLevel;
  }

  document.getElementById('btnPrio').className = j.priorityActive ? 'sideBtn red on' : (j.priorityOn ? 'sideBtn on' : 'sideBtn');
  document.getElementById('swVal').textContent = j.stopwatch;
  document.getElementById('btnSw').className = j.stopwatchRunning ? 'sideBtn on' : 'sideBtn';
  document.getElementById('btnLock').className = j.locked ? 'sideBtn red on' : 'sideBtn';

  const rdsEmptyEl = document.getElementById('rdsEmpty');
  const rdsStationWrap = document.getElementById('rdsStationWrap');
  const rdsTextWrap = document.getElementById('rdsTextWrap');
  
  if (j.modeIdx == 0) {
    if (j.rdsStation && j.rdsStation !== lastRdsStation) lastRdsText = ''; 
    if (j.rdsStation) lastRdsStation = j.rdsStation;
    if (j.rdsText && j.rdsText.length >= lastRdsText.length) lastRdsText = j.rdsText;
    if (lastRdsStation || lastRdsText) {
      rdsEmptyEl.style.display = 'none';
      if (lastRdsStation) {
        rdsStationWrap.style.display = 'inline';
        document.getElementById('rdsStation').textContent = lastRdsStation;
      } else rdsStationWrap.style.display = 'none';
      if (lastRdsText) {
        rdsTextWrap.style.display = 'inline';
        document.getElementById('rdsText').textContent = lastRdsText;
      } else rdsTextWrap.style.display = 'none';
    } else {
      rdsEmptyEl.style.display = 'inline';
      rdsStationWrap.style.display = 'none';
      rdsTextWrap.style.display = 'none';
    }
  } else {
    rdsEmptyEl.style.display = 'inline';
    rdsStationWrap.style.display = 'none';
    rdsTextWrap.style.display = 'none';
    lastRdsText = ''; lastRdsStation = '';
  }

  const scanSec = document.getElementById('scanSec');
  if (j.scanning) {
    scanSec.style.display = 'block';
    document.getElementById('scanFreqVal').textContent = j.scanFreq;
    document.getElementById('scanBtn').textContent = j.scanPaused ? 'WZNOW' : 'PAUZA';
  } else if (!scanSec.classList.contains('open')) scanSec.style.display = 'none';
}
function setFreq(){ const v=document.getElementById('freqInput').value; if(v) api('/api/freq',{f:v}); }
function setModeBtn(m){ api('/api/mode',{m:m}); }
function vfoSwap(){ api('/api/vfo'); }
function setVolLive(v){ document.getElementById('volVal').textContent=v; lastVolSet=Date.now(); }
function setVol(v){ api('/api/volume',{v:v}); }
function toggleMute(){ api('/api/mute',{on: document.getElementById('btnMute').classList.contains('on')?0:1}); }
function cycleAgc(){ const next=(agcModeCur+1)%3; api('/api/agc',{mode:next}); }
function agcLevelLive(v){ document.getElementById('agcVal').textContent=v; lastAgcSet=Date.now(); }
function setAgcLevel(v){ api('/api/agc',{mode:2,level:v}); }
function prioSave(){ api('/api/priority',{action:'save'}); toast('Zapisano priorytet'); }
function prioToggle(){ api('/api/priority',{action:'toggle'}); }
function swToggle(){ api('/api/stopwatch',{action: document.getElementById('btnSw').classList.contains('on')?'stop':'start'}); }
function swReset(){ api('/api/stopwatch',{action:'reset'}); }
function toggleLock(){ api('/api/lock',{on: document.getElementById('btnLock').classList.contains('on')?0:1}); }
function scanToggle(){ const paused=document.getElementById('scanBtn').textContent=='WZNOW'; api('/api/scan',{action:paused?'resume':'pause'}); }
function meterCycle(){ api('/api/meter/cycle'); toast('Zmieniono tryb metera'); }
function configExport(){ window.location.href = '/api/config/export'; }
function configImport(file){
  if (!file) return;
  const reader = new FileReader();
  reader.onload = async () => {
    const r = await fetch('/api/config/import', {method:'POST', body: reader.result});
    if (r.ok) toast('Konfiguracja wczytana - zapisano do EEPROM');
    else toast('Blad: plik nie pasuje do tej wersji firmware');
  };
  reader.readAsText(file);
}
async function loadBands(){
  const r = await fetch('/api/bands'); const list = await r.json();
  document.getElementById('bandRow').innerHTML = list.map(b=>`<button data-idx='${b.idx}' onclick='setBandBtn(${b.idx})'>${b.name}</button>`).join('');
}
function setBandBtn(idx){ api('/api/band',{idx:idx}); }
let memCache = [];
let favFilterOn = false;
async function loadMemory(){
  const r = await fetch('/api/memory'); memCache = await r.json();
  renderMemList();
}
function toggleFavFilter(){
  favFilterOn = !favFilterOn;
  document.getElementById('favFilterBtn').className = favFilterOn ? 'rigBtn on' : 'rigBtn';
  renderMemList();
}
function toggleFav(slot, ev){
  ev.stopPropagation();
  fetch('/api/memory/favorite?slot='+slot).then(()=>loadMemory());
}
function renderMemList(){
  const el = document.getElementById('memList');
  if (!memCache.length) { el.textContent = 'Brak zapisanych stacji'; return; }
  const q = (document.getElementById('memSearch').value || '').toLowerCase();
  const sortBy = document.getElementById('memSort').value;
  const groupBand = document.getElementById('memGroupBand').checked;
  let list = memCache.filter(m => !q || (m.name||'').toLowerCase().includes(q));
  if (favFilterOn) list = list.filter(m => m.fav);
  if (sortBy === 'name') list = [...list].sort((a,b)=>(a.name||'').localeCompare(b.name||''));
  else if (sortBy === 'freq') list = [...list].sort((a,b)=>a.freq-b.freq);
  else if (sortBy === 'band') list = [...list].sort((a,b)=>a.band.localeCompare(b.band));
  if (!list.length) { el.innerHTML = '<div style="color:var(--dim);padding:8px">Brak wynikow</div>'; return; }
  function row(m){
    const star = m.fav ? '&#9733;' : '&#9734;';
    return `<div class='memitem' onclick='recall(${m.slot})'><span class='n'><span onclick='toggleFav(${m.slot},event)' style='color:var(--amber);margin-right:6px;cursor:pointer'>${star}</span>${m.name||'(bez nazwy)'}</span><span class='f'>${m.freq} ${m.band} ${m.mode}</span></div>`;
  }
  if (groupBand) {
    const groups = {};
    list.forEach(m => { (groups[m.band] = groups[m.band]||[]).push(m); });
    el.innerHTML = Object.keys(groups).sort().map(b =>
      `<div style='color:var(--teal);font-size:.72em;letter-spacing:1px;margin:8px 0 3px;text-transform:uppercase'>${b}</div>` + groups[b].map(row).join('')
    ).join('');
  } else {
    el.innerHTML = list.map(row).join('');
  }
}
function recall(slot){ api('/api/memrecall',{slot:slot}); toast('Przywolano'); }
function memImport(file){
  if (!file) return;
  const reader = new FileReader();
  reader.onload = async () => {
    const r = await fetch('/api/memory/import', {method:'POST', body: reader.result});
    const j = await r.json();
    toast('Zaimportowano: ' + j.imported + ', pominieto: ' + j.skipped);
    loadMemory();
  };
  reader.readAsText(file);
}

async function loadSkipList(){
  const r = await fetch('/api/skiplist'); const list = await r.json();
  const el = document.getElementById('skipList');
  if (!list.length) { el.textContent = 'Brak wykluczen'; return; }
  el.innerHTML = list.map(f=>`<div class='memitem'><span class='n'>${f} kHz</span><span class='f' style='cursor:pointer;color:var(--red)' onclick='skipRemove(${f})'>USUN</span></div>`).join('');
}
function skipAdd(){
  const v = document.getElementById('skipInput').value;
  if (!v) return;
  fetch('/api/skiplist/add?freq='+v).then(()=>{ loadSkipList(); document.getElementById('skipInput').value=''; toast('Dodano do wykluczen'); });
}
function skipRemove(f){ fetch('/api/skiplist/remove?freq='+f).then(loadSkipList); }
function skipExport(){ window.location.href = '/api/skiplist/export'; }
function skipImport(file){
  if (!file) return;
  const reader = new FileReader();
  reader.onload = async () => {
    await fetch('/api/skiplist/import', {method:'POST', body: reader.result});
    loadSkipList();
    toast('Zaimportowano liste');
  };
  reader.readAsText(file);
}
async function loadDiscovery(){
  const r = await fetch('/api/discovery'); const list = await r.json();
  const el = document.getElementById('discList');
  if (!list.length) { el.textContent = 'Brak wpisow'; return; }
  el.innerHTML = list.map(d=>`<div class='memitem'><span class='n'>${d.time}</span><span class='f'>${d.freq} kHz, SNR ${d.snr}</span></div>`).join('');
}
function discExport(){ window.location.href = '/api/discovery/export'; }

const settingsMeta = [
  {key:'wifiEnable', label:'WiFi wlaczone'},
  {key:'RDSalways', label:'RDS zawsze (nie tylko FM btn)'},
  {key:'saverOn', label:'Wygaszacz ekranu'},
  {key:'digitLigth', label:'Podswietlenie cyfr'},
  {key:'batShow', label:'Pokazuj baterie'},
  {key:'nightModeOn', label:'Tryb nocny (NTP)'},
  {key:'analogMeterOn', label:'Analogowy S-metr'},
  {key:'rssiHistoryOn', label:'Wykres trendu RSSI'},
  {key:'cwDecoderOn', label:'Dekoder CW'},
  {key:'scanWaterfallOn', label:'Waterfall w SCAN'},
  {key:'scanStopOnSignal', label:'Stop na sygnale (SCAN)'},
  {key:'screenV', label:'Orientacja pionowa'},
  {key:'VHFon', label:'FM od 64 MHz'},
  {key:'seekAccuracy', label:'Szukaj w AM 1 KHz'},
  {key:'closeCallOn', label:'Close Call (skok na peak po SCAN)'},
];
async function loadSettings() {
  const r = await fetch('/api/settings');
  const j = await r.json();
  const el = document.getElementById('settingsList');
  el.innerHTML = settingsMeta.map(m => {
    const on = j[m.key];
    return `<div class='settingsRow'><span>${m.label}</span><div class='toggle ${on?'on':''}' onclick='toggleSetting("${m.key}", ${on})'><div class='dot'></div></div></div>`;
  }).join('');
}
async function toggleSetting(key, current) {
  await fetch('/api/settings/set?key=' + key + '&value=' + (current ? '0' : '1'));
  loadSettings();
}
async function poll(){ try { await api('/api/status'); } catch(e){} }
function toggleCompact(){
  document.body.classList.toggle('compact');
  const on = document.body.classList.contains('compact');
  document.getElementById('compactBtn').textContent = on ? '[normalny]' : '[kompakt]';
  try { localStorage.setItem('ats25x2_compact', on ? '1' : '0'); } catch(e){}
}
try { if (localStorage.getItem('ats25x2_compact') === '1') toggleCompact(); } catch(e){}
loadBands(); loadMemory(); loadSettings(); loadSkipList(); poll();
setInterval(poll, 2000);
</script>
</body></html>)HTMLPAGE";
  webServer.send(200, "text/html", html);
}

void enableWebServer() {
  if (!wifiConnected) {
    if (strlen(storage.wifiSSID)) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(storage.wifiSSID, storage.wifiPassword);
      unsigned long t0 = millis();
      while (WiFi.status() != WL_CONNECTED and (millis() - t0) < 8000) delay(200);
      wifiConnected = (WiFi.status() == WL_CONNECTED);
    }
  }
  if (wifiConnected and !webServerRunning) {
    webServer.on("/", webHandleRoot);
    webServer.on("/api/status", webHandleStatus);
    webServer.on("/api/bands", webHandleBandsList);
    webServer.on("/api/memory", webHandleMemoryList);
    webServer.on("/api/memrecall", webHandleMemoryRecall);
    webServer.on("/api/memory/favorite", webHandleMemoryFavorite);
    webServer.on("/api/memory/import", HTTP_POST, webHandleMemoryImport);
    webServer.on("/api/freq", webHandleSetFreq);
    webServer.on("/api/band", webHandleBand);
    webServer.on("/api/mode", webHandleMode);
    webServer.on("/api/volume", webHandleVolume);
    webServer.on("/api/mute", webHandleMute);
    webServer.on("/api/agc", webHandleAGC);
    webServer.on("/api/vfo", webHandleVFOSwap);
    webServer.on("/api/priority", webHandlePriority);
    webServer.on("/api/stopwatch", webHandleStopwatch);
    webServer.on("/api/lock", webHandleLock);
    webServer.on("/api/scan", webHandleScan);
    webServer.on("/api/settings", webHandleSettingsGet);
    webServer.on("/api/settings/set", webHandleSettingsSet);
    webServer.on("/api/skiplist", webHandleSkipList);
    webServer.on("/api/meter/cycle", webHandleMeterCycle);
    webServer.on("/api/config/export", webHandleConfigExport);
    webServer.on("/api/config/import", HTTP_POST, webHandleConfigImport);
    webServer.on("/api/skiplist/add", webHandleSkipListAdd);
    webServer.on("/api/skiplist/remove", webHandleSkipListRemove);
    webServer.on("/api/skiplist/export", webHandleSkipListExport);
    webServer.on("/api/skiplist/import", HTTP_POST, webHandleSkipListImport);
    webServer.on("/api/discovery", webHandleDiscovery);
    webServer.on("/api/discovery/export", webHandleDiscoveryExport);
    webServer.begin();
    webServerRunning = true;
    if (MDNS.begin("ats25x2")) {
      MDNS.addService("http", "tcp", 80);
    }
  }
}

void disableWebServer() {
  if (webServerRunning) {
    webServer.stop();
    MDNS.end();
    webServerRunning = false;
  }
}

uint16_t scaledRowBuf[400];


void pushScaledImage(TFT_eSprite &target, int destW, int destH, const uint16_t *img, int srcW, int srcH) {
  for (int y = 0; y < destH; y++) {
    int srcY = y * srcH / destH;
    for (int x = 0; x < destW; x++) {
      int srcX = x * srcW / destW;
      scaledRowBuf[x] = img[srcY * srcW + srcX];
    }
    target.pushImage(0, y, destW, 1, scaledRowBuf);
  }
}

void pushScaledDimmedImage(TFT_eSprite &target, int destW, int destH, const uint16_t *img, int srcW, int srcH, float brightness) {
  for (int y = 0; y < destH; y++) {
    int srcY = y * srcH / destH;
    for (int x = 0; x < destW; x++) {
      int srcX = x * srcW / destW;
      uint16_t raw = img[srcY * srcW + srcX];
      uint16_t px = (raw << 8) | (raw >> 8); 
      uint16_t r = (uint16_t)(((px >> 11) & 0x1F) * brightness + 0.5);
      uint16_t g = (uint16_t)(((px >> 5)  & 0x3F) * brightness + 0.5);
      uint16_t b = (uint16_t)((px         & 0x1F) * brightness + 0.5);
      uint16_t dimmed = (r << 11) | (g << 5) | b;
      scaledRowBuf[x] = (dimmed << 8) | (dimmed >> 8); 
    }
    target.pushImage(0, y, destW, 1, scaledRowBuf);
  }
}

#ifdef IhaveSI5351
Si5351wire si5351wire;
#endif

void IRAM_ATTR RotaryEncFreq() {

  if (!writingEeprom) {
    encoderStatus = encoder.process();
    if (encoderStatus) {
      if (encoderStatus == DIR_CW) encoderCount = 1; else encoderCount = -1;
    }
  }
}

void OPTpack() {

  boolOpt = 0;
  boolOpt += digitLigth   * 1;
  boolOpt += batShow      * 2;
  boolOpt += langRetroEN  * 4;
  boolOpt += beeperOn     * 8;
  boolOpt += VHFon        * 16;
  boolOpt += loadMemory   * 32;
  boolOpt += memoPreset   * 64;
  boolOpt += saverOn      * 128;
  boolOpt += screenV      * 256;
  boolOpt += displayOff   * 512;
  boolOpt += SCANaccuracy * 1024;
  boolOpt += displayPower * 2048;
  boolOpt += RDSalways    * 4096;
  boolOpt += seekAccuracy * 8192;
  boolOpt += saverDisableOnScan * 16384;
  boolOpt += rssiHistoryOn * 32768;
}

void OPTunpack() {

  digitLigth    = bool((boolOpt >>  0) & 1);
  batShow       = bool((boolOpt >>  1) & 1);
  langRetroEN   = bool((boolOpt >>  2) & 1);
  beeperOn      = bool((boolOpt >>  3) & 1);
  VHFon         = bool((boolOpt >>  4) & 1);
  loadMemory    = bool((boolOpt >>  5) & 1);
  memoPreset    = bool((boolOpt >>  6) & 1);
  saverOn       = bool((boolOpt >>  7) & 1);
  screenV       = bool((boolOpt >>  8) & 1);
  displayOff    = bool((boolOpt >>  9) & 1);
  SCANaccuracy  = bool((boolOpt >> 10) & 1);
  displayPower  = bool((boolOpt >> 11) & 1);
  RDSalways     = bool((boolOpt >> 12) & 1);
  seekAccuracy  = bool((boolOpt >> 13) & 1);
  saverDisableOnScan = bool((boolOpt >> 14) & 1);
  rssiHistoryOn = bool((boolOpt >> 15) & 1);
}

void scanOPTpack() {

  SCANscale = uint8_t((autoSCANstep * 128) + (maxSCANstep * 8) + (minSCANstep * 8));
  if (countScanSignal == 3) SCANaccuracy = true; else SCANaccuracy = false;
}

void scanOPTunpack() {

  if (SCANscale > 128) autoSCANstep = true; else autoSCANstep = false;
  minSCANstep = float(SCANscale & 0x07) / 8;
  maxSCANstep = float(SCANscale & 0x78) / 8;
  if (SCANaccuracy) countScanSignal = 3; else countScanSignal = 1;
}

String wifiStatusText() {
  if (WiFi.status() == WL_CONNECTED) return "Połączono: " + WiFi.SSID();
  if (strlen(storage.wifiSSID)) return "Zapisano: " + String(storage.wifiSSID) + " (offline)";
  return "Brak zapisanej sieci";
}

bool touchIn(int16_t rx, int16_t ry, int16_t rw, int16_t rh) {
  return (x >= rx && x < rx + rw && y >= ry && y < ry + rh);
}

void drawSimpleButton(int16_t rx, int16_t ry, int16_t rw, int16_t rh, String label, uint16_t bg) {
  tft.fillRoundRect(rx, ry, rw, rh, 4, bg);
  tft.drawRoundRect(rx, ry, rw, rh, 4, TFT_WHITE);
  tftPlSetSize(1);
  tftPlSetStyle(REG_T);
 tftPlSetFont(T1516_T);
  int chosenHeight = 16;
  if (tftPlTextWidth(label) > rw - 6) {
    tftPlSetFont(T1012_T);
    chosenHeight = 12;
  }
  tftPlSetDatum(BC_T);
  tftPlSetColor(TFT_WHITE, bg);
  tftPlPrint(label, rx + rw / 2, ry + (rh + chosenHeight) / 2);
}

#define WIFI_MAX_APS       40
#define WIFI_ROWS_PER_PAGE  5

struct WifiApEntry { String ssid; int32_t rssi; bool open; };
WifiApEntry wifiAps[WIFI_MAX_APS];
int wifiApCount = 0;
bool wifiPickedOpen = false;

void wifiScanAndSort() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(10, 100);
  tft.println("Szukam sieci WiFi...");

  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks();
  wifiApCount = 0;
  for (int i = 0; i < n && wifiApCount < WIFI_MAX_APS; i++) {
    String s = WiFi.SSID(i);
    if (s.length() == 0) continue;
    bool found = false;
    for (int j = 0; j < wifiApCount; j++) {
      if (wifiAps[j].ssid == s) {
        found = true;
        if (WiFi.RSSI(i) > wifiAps[j].rssi) {
          wifiAps[j].rssi = WiFi.RSSI(i);
          wifiAps[j].open = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN);
        }
        break;
      }
    }
    if (!found) {
      wifiAps[wifiApCount].ssid = s;
      wifiAps[wifiApCount].rssi = WiFi.RSSI(i);
      wifiAps[wifiApCount].open = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN);
      wifiApCount++;
    }
  }
  for (int i = 0; i < wifiApCount; i++)
    for (int j = i + 1; j < wifiApCount; j++)
      if (wifiAps[j].rssi > wifiAps[i].rssi) {
        WifiApEntry tmp = wifiAps[i]; wifiAps[i] = wifiAps[j]; wifiAps[j] = tmp;
      }
  WiFi.scanDelete();
}

String wifiPickNetwork(bool &skipped) {
  skipped = false;
  int page = 0;
  wifiScanAndSort();

  while (true) {
    int totalPages = max(1, (wifiApCount + WIFI_ROWS_PER_PAGE - 1) / WIFI_ROWS_PER_PAGE);
    if (page >= totalPages) page = totalPages - 1;
    int startIdx = page * WIFI_ROWS_PER_PAGE;

    tft.fillScreen(TFT_BLACK);
    tftPlSetSize(2);
    tftPlSetStyle(REG_T);
    tftPlSetFont(T1012_T);
    tftPlSetDatum(BL_T);
    tftPlSetColor(TFT_YELLOW, TFT_BLACK);
    tftPlPrint("Wybierz sieć WiFi", 10, 29);

    const int rowTop = 34, rowH = 32;
    tft.setTextSize(1);
    if (wifiApCount == 0) {
      tftPlSetSize(1);
      tftPlSetStyle(REG_T);
      tftPlSetFont(T1012_T);
      tftPlSetDatum(BL_T);
      tftPlSetColor(TFT_WHITE, TFT_BLACK);
      tftPlPrint("Nie znaleziono żadnych sieci.", 10, rowTop + 22);
    }
    for (int i = 0; i < WIFI_ROWS_PER_PAGE; i++) {
      int idx = startIdx + i;
      if (idx >= wifiApCount) break;
      int ry = rowTop + i * rowH;
      tft.drawRoundRect(10, ry, 300, rowH - 4, 3, TFT_CYAN);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setCursor(18, ry + 9);
      String line = wifiAps[idx].ssid;
      if (!wifiAps[idx].open) line += "  (haslo)";
      tft.println(line);
    }

    int navY = rowTop + WIFI_ROWS_PER_PAGE * rowH + 4;
    drawSimpleButton(10,  navY, 68, 34, "<<<<",   TFT_BLUE);
    drawSimpleButton(83,  navY, 68, 34, ">>>>",   TFT_BLUE);
    drawSimpleButton(156, navY, 78, 34, "ODŚWIEŻ",TFT_BLUE);
    drawSimpleButton(239, navY, 71, 34, "POMIŃ",  TFT_RED);

    while (!tft.getTouch(&x, &y)) delay(30);

    if (touchIn(10, navY, 68, 34)) {
      if (page > 0) page--;
    } else if (touchIn(83, navY, 68, 34)) {
      if (page < totalPages - 1) page++;
    } else if (touchIn(156, navY, 78, 34)) {
      page = 0; wifiScanAndSort();
    } else if (touchIn(239, navY, 71, 34)) {
      skipped = true; x = y = 0; delay(150); return "";
    } else {
      for (int i = 0; i < WIFI_ROWS_PER_PAGE; i++) {
        int idx = startIdx + i;
        if (idx >= wifiApCount) break;
        int ry = rowTop + i * rowH;
        if (touchIn(10, ry, 300, rowH - 4)) {
          wifiPickedOpen = wifiAps[idx].open;
          String chosen = wifiAps[idx].ssid;
          x = y = 0; delay(150);
          return chosen;
        }
      }
    }
    x = y = 0; delay(150);
  }
}

#define KBD_MAX_LEN 63
#define KC_CHAR   0
#define KC_SHIFT  1
#define KC_BKSP   2
#define KC_SPACE  3
#define KC_NUMSYM 4
#define KC_ABC    5
#define KC_SHOW   6

struct KbdKeyDef { const char* label; uint8_t code; uint8_t weight; };
struct KbdRect   { int16_t x, y, w, h; const KbdKeyDef* def; };

const KbdKeyDef kbdRow1Letters[] = { {"q",KC_CHAR,1},{"w",KC_CHAR,1},{"e",KC_CHAR,1},{"r",KC_CHAR,1},{"t",KC_CHAR,1},{"y",KC_CHAR,1},{"u",KC_CHAR,1},{"i",KC_CHAR,1},{"o",KC_CHAR,1},{"p",KC_CHAR,1} };
const KbdKeyDef kbdRow2Letters[] = { {"a",KC_CHAR,1},{"s",KC_CHAR,1},{"d",KC_CHAR,1},{"f",KC_CHAR,1},{"g",KC_CHAR,1},{"h",KC_CHAR,1},{"j",KC_CHAR,1},{"k",KC_CHAR,1},{"l",KC_CHAR,1} };
const KbdKeyDef kbdRow3Letters[] = { {"Shift",KC_SHIFT,2},{"z",KC_CHAR,1},{"x",KC_CHAR,1},{"c",KC_CHAR,1},{"v",KC_CHAR,1},{"b",KC_CHAR,1},{"n",KC_CHAR,1},{"m",KC_CHAR,1},{"Bksp",KC_BKSP,2} };

const KbdKeyDef kbdRow1Nums[] = { {"1",KC_CHAR,1},{"2",KC_CHAR,1},{"3",KC_CHAR,1},{"4",KC_CHAR,1},{"5",KC_CHAR,1},{"6",KC_CHAR,1},{"7",KC_CHAR,1},{"8",KC_CHAR,1},{"9",KC_CHAR,1},{"0",KC_CHAR,1} };
const KbdKeyDef kbdRow2Nums[] = { {"!",KC_CHAR,1},{"@",KC_CHAR,1},{"#",KC_CHAR,1},{"$",KC_CHAR,1},{"%",KC_CHAR,1},{"^",KC_CHAR,1},{"&",KC_CHAR,1},{"*",KC_CHAR,1},{"(",KC_CHAR,1},{")",KC_CHAR,1} };
const KbdKeyDef kbdRow3Nums[] = { {"-",KC_CHAR,1},{"_",KC_CHAR,1},{"=",KC_CHAR,1},{"+",KC_CHAR,1},{".",KC_CHAR,1},{",",KC_CHAR,1},{"/",KC_CHAR,1},{"Bksp",KC_BKSP,2} };

const KbdKeyDef kbdRow4[] = { {"123",KC_NUMSYM,2},{"Spacja",KC_SPACE,3},{"Pokaz",KC_SHOW,2} };
const KbdKeyDef kbdRow4Num[] = { {"ABC",KC_ABC,2},{"Spacja",KC_SPACE,3},{"Pokaz",KC_SHOW,2} };

KbdRect kbdRects[16];
int kbdRectCount;

void kbdLayoutRow(const KbdKeyDef* keys, int count, int16_t y, int16_t h) {
  int16_t marginX = 6, gap = 3;
  int16_t totalW = tft.width() - 2 * marginX - gap * (count - 1);
  int weightSum = 0;
  for (int i = 0; i < count; i++) weightSum += keys[i].weight;
  int16_t unit = totalW / weightSum;
  int16_t x0 = marginX;
  for (int i = 0; i < count; i++) {
    int16_t w = keys[i].weight * unit;
    kbdRects[kbdRectCount].x = x0;
    kbdRects[kbdRectCount].y = y;
    kbdRects[kbdRectCount].w = w;
    kbdRects[kbdRectCount].h = h;
    kbdRects[kbdRectCount].def = &keys[i];
    kbdRectCount++;
    x0 += w + gap;
  }
}

String wifiKeyboardInput(String title, bool &cancelled) {
  cancelled = false;
  String val = "";
  bool shiftOn = false, numMode = false, showText = false;
  const int16_t rowH = 32, rowGap = 3;
  const int16_t row1Y = 56, row2Y = row1Y + rowH + rowGap, row3Y = row2Y + rowH + rowGap, row4Y = row3Y + rowH + rowGap;
  const int16_t actY = row4Y + rowH + 8;

  while (true) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.setTextSize(1);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(6, 4);
    tft.println(title);

    tft.drawRoundRect(6, 20, tft.width() - 12, 26, 3, TFT_CYAN);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(12, 28);
    String shown = showText ? val : String("");
    if (!showText) for (uint16_t i = 0; i < val.length(); i++) shown += "*";
    const int maxChars = (tft.width() - 24) / 6;
    if ((int)shown.length() > maxChars) shown = shown.substring(shown.length() - maxChars);
    tft.print(shown);

    kbdRectCount = 0;
    if (!numMode) {
      kbdLayoutRow(kbdRow1Letters, 10, row1Y, rowH);
      kbdLayoutRow(kbdRow2Letters, 9,  row2Y, rowH);
      kbdLayoutRow(kbdRow3Letters, 9,  row3Y, rowH);
      kbdLayoutRow(kbdRow4,        3,  row4Y, rowH);
    } else {
      kbdLayoutRow(kbdRow1Nums, 10, row1Y, rowH);
      kbdLayoutRow(kbdRow2Nums, 10, row2Y, rowH);
      kbdLayoutRow(kbdRow3Nums, 8,  row3Y, rowH);
      kbdLayoutRow(kbdRow4Num,  3,  row4Y, rowH);
    }
    for (int i = 0; i < kbdRectCount; i++) {
      KbdRect &r = kbdRects[i];
      uint16_t bg = (r.def->code == KC_CHAR) ? TFT_NAVY : TFT_BLUE;
      if (r.def->code == KC_SHIFT && shiftOn) bg = TFT_ORANGE;
      tft.fillRoundRect(r.x, r.y, r.w, r.h, 3, bg);
      tft.drawRoundRect(r.x, r.y, r.w, r.h, 3, TFT_WHITE);
      tft.setTextColor(TFT_WHITE, bg);
      tft.setTextDatum(MC_DATUM);
      String lbl = r.def->label;
      if (r.def->code == KC_CHAR && shiftOn && !numMode) lbl.toUpperCase();
      tft.drawString(lbl, r.x + r.w / 2, r.y + r.h / 2);
      tft.setTextDatum(TL_DATUM);
    }

    drawSimpleButton(6,                       actY, 145, 30, "ANULUJ", TFT_RED);
    drawSimpleButton(tft.width() - 6 - 145,    actY, 145, 30, "POLACZ", TFT_GREEN);

    while (!tft.getTouch(&x, &y)) delay(30);

    if (touchIn(6, actY, 145, 30)) { cancelled = true; x = y = 0; delay(150); return ""; }
    if (touchIn(tft.width() - 6 - 145, actY, 145, 30)) { x = y = 0; delay(150); return val; }

    for (int i = 0; i < kbdRectCount; i++) {
      KbdRect &r = kbdRects[i];
      if (touchIn(r.x, r.y, r.w, r.h)) {
        switch (r.def->code) {
          case KC_CHAR: {
            char c = r.def->label[0];
            if (shiftOn && !numMode) c = toupper(c);
            if (val.length() < KBD_MAX_LEN) val += c;
            if (shiftOn && !numMode) shiftOn = false;
            break;
          }
          case KC_SHIFT:  shiftOn = !shiftOn; break;
          case KC_BKSP:   if (val.length()) val.remove(val.length() - 1); break;
          case KC_SPACE:  if (val.length() < KBD_MAX_LEN) val += ' '; break;
          case KC_NUMSYM: numMode = true; break;
          case KC_ABC:    numMode = false; break;
          case KC_SHOW:   showText = !showText; break;
        }
        break;
      }
    }
    x = y = 0; delay(120);
  }
}

bool wifiTryConnect(String ssid, String pass, uint32_t timeoutMs = 15000) {
  WiFi.mode(WIFI_STA);
  if (pass.length()) WiFi.begin(ssid.c_str(), pass.c_str());
  else WiFi.begin(ssid.c_str());

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(10, 10);
  tft.println("Laczenie z siecia:");
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(10, 45);
  tft.println(ssid);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(10, 75);
  tft.println("Dotknij ekran, aby anulowac.");

  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < timeoutMs) {
    if (tft.getTouch(&x, &y)) { x = y = 0; WiFi.disconnect(true); delay(100); return false; }
    delay(150);
  }
  return WiFi.status() == WL_CONNECTED;
}

bool nativeWifiSetup() {
  drawProgressScreenReady = false;
  while (true) {
    bool skipped = false;
    String ssid = wifiPickNetwork(skipped);
    if (skipped || ssid.length() == 0) return false;

    String pass = "";
    if (!wifiPickedOpen) {
      bool cancelled = false;
      pass = wifiKeyboardInput("Haslo dla: " + ssid, cancelled);
      if (cancelled) continue;
    }

    if (wifiTryConnect(ssid, pass)) return true;

    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.println("Nie udalo sie polaczyc");
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(10, 50);
    tft.println("Sprawdz haslo i sprobuj ponownie.");
    tft.println("Dotknij ekran, aby kontynuowac.");
    while (!tft.getTouch(&x, &y)) delay(50);
    x = y = 0; delay(200);

  }
}

bool connectWifi()
{
  if (WiFi.status() == WL_CONNECTED)
    return true;

  WiFi.mode(WIFI_STA);
  delay(200);

  bool hasSaved =
      (storage.chk6 == '@') &&
      (storage.chk7 == '@') &&
      strlen(storage.wifiSSID) > 0;

  if (hasSaved)
  {
    drawProgress(0, "Łączenie z WiFi...");

    Serial.print("SSID: ");
    Serial.println(storage.wifiSSID);

    WiFi.begin(storage.wifiSSID, storage.wifiPassword);

    uint32_t t0 = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - t0 < 15000)
    {
      delay(100);
      yield();
    }
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Brak zapisanej sieci lub połączenie nieudane.");

    if (!nativeWifiSetup())
    {
      drawProgress(100, "Brak WiFi - offline");

      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);

      return false;
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    drawProgress(100, "Połączono");

    strncpy(storage.wifiSSID,
            WiFi.SSID().c_str(),
            sizeof(storage.wifiSSID) - 1);

    storage.wifiSSID[sizeof(storage.wifiSSID) - 1] = 0;

    strncpy(storage.wifiPassword,
            WiFi.psk().c_str(),
            sizeof(storage.wifiPassword) - 1);

    storage.wifiPassword[sizeof(storage.wifiPassword) - 1] = 0;

    storage.chk6 = '@';
    storage.chk7 = '@';

    saveConfig();
    EEPROM.commit();

    Serial.println("Konfiguracja WiFi zapisana.");

    return true;
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  return false;
}

void configureWifiNow()
{
  WiFi.mode(WIFI_STA);

  bool ok = nativeWifiSetup();

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 100);

  if (ok)
  {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Zapisano siec:");
    tft.setTextSize(1);
    tft.setCursor(10, 130);
    tft.println(WiFi.SSID());

    Serial.println("Skonfigurowano WiFi: " + WiFi.SSID());

    memset(storage.wifiSSID, 0, sizeof(storage.wifiSSID));
    memset(storage.wifiPassword, 0, sizeof(storage.wifiPassword));

    strncpy(storage.wifiSSID,
            WiFi.SSID().c_str(),
            sizeof(storage.wifiSSID) - 1);

    strncpy(storage.wifiPassword,
            WiFi.psk().c_str(),
            sizeof(storage.wifiPassword) - 1);

    storage.wifiSSID[sizeof(storage.wifiSSID) - 1] = '\0';
    storage.wifiPassword[sizeof(storage.wifiPassword) - 1] = '\0';

    storage.chk6 = '@';
    storage.chk7 = '@';

    saveConfig();

    Serial.println();
    Serial.println("--------------------------------");
    Serial.println("Konfiguracja WiFi zapisana.");
    Serial.print("SSID : ");
    Serial.println(storage.wifiSSID);
    Serial.println("--------------------------------");

    wifiConnected = true;
  }
  else
  {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.println("Anulowano / offline");

    Serial.println("Anulowano / limit czasu");

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    wifiConnected = false;
  }

  uint32_t t = millis();
  while (millis() - t < 1500)
  {
    yield();
  }
}

void initTime() {
  time_t now = 0;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  const int maxTries = 20;
  int i = 0;
  while ((now = time(nullptr)) < NTP_MIN_VALID_EPOCH and i < maxTries) {
    drawProgress((i * 100) / maxTries, "Synchronizacja czasu...");
    Serial.print(".");
    delay(300);
    yield();
    i++;
  }

  if (now >= NTP_MIN_VALID_EPOCH) {
    drawProgress(100, "Czas zsynchronizowany");
    Serial.println("Czas zsynchronizowany.");
  } else {
    drawProgress(100, "Brak odpowiedzi serwera NTP");
    Serial.println("Brak odpowiedzi serwera NTP");
  }
  delay(500);
}

void setup() {

  Serial.begin(115200);

  pinMode(DISPLAY_LED, OUTPUT);
  pinMode(BEEPER, OUTPUT);
  digitalWrite(DISPLAY_LED, 0);

  si4735.setAudioMuteMcuPin(AUDIO_MUTE);
  si4735.setAudioMute(audioMuteOn);
  ledcSetup(LedChannelforTFT, LedFreq, LedResol);
  ledcAttachPin(DISPLAY_LED, LedChannelforTFT);

  int16_t si4735Addr = si4735.getDeviceI2CAddress(RESET_PIN);
  Beep(1, 200);

 tft.init();

  tft.setRotation(1);

  uint16_t calData[5] = { 387, 3530, 246, 3555, 7 };
  tft.setTouch(calData);

#ifdef IhaveSI5351
  si5351wire.output_enable(CLK_Xtal, 1);
  if (si5351wire.init(SI5351wire_CRYSTAL_LOAD_8PF, CLK_Xtal, 0) == false)
  {
    Serial.println ( "SI5351 nie znaleziony" );
  }

  si5351wire.set_correction(0, SI5351wire_PLL_INPUT_XO);

  si5351wire.set_freq(FreqSI5351, CLK_Xtal);
#endif

  if (!EEPROM.begin(EEPROM_SIZE))
{
    Serial.println();
    Serial.println("====================================");
    Serial.println("BŁĄD inicjalizacji EEPROM !");
    Serial.println("====================================");

    while (true)
    {
        delay(100);
    }
}

Serial.println();
Serial.println("===== EEPROM =====");

Serial.print("StoreStruct = ");
Serial.print(sizeof(storage));
Serial.println(" bajtów");

Serial.print("EEPROM_SIZE = ");
Serial.println(EEPROM_SIZE);

Serial.print("Pozostało = ");
Serial.print(EEPROM_SIZE - sizeof(storage));
Serial.println(" bajtów");

Serial.println("==================");

  tft.fillScreen(TFT_BLACK);

  loadConfig();
  wifiEnable = (storage.chk6 == '@') ? (bool)storage.wifiEnableAtBoot : true;

if (wifiEnable) {
  wifiConnected = connectWifi();
  if (wifiConnected) {
    initTime();
    if (webServerOn) { 
      enableWebServer();
      Serial.println("Serwer WWW uruchomiony.");
    } else {
      WiFi.disconnect(false);
      WiFi.mode(WIFI_OFF);
      wifiConnected = false;
      Serial.println("WiFi wyłączone - oszczędność energii.");
    }
  } else {
    Serial.println("Brak WiFi – praca offline.");
  }
} else {
    Serial.println("WiFi wyłączone w ustawieniach.");
}

  delay(500);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 10);
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);

  Serial.println("ATS25X2 Polish Project");
  Serial.println("Versja 0.17PL 12-08-2026");

  spr.createSprite(320, 144);
  spr.fillScreen(COLOR_BACKGROUND);
  pushScaledDimmedImage(spr, 320, 144, (const uint16_t *)logo, 265, 120, 0.35); 
  spr.pushSprite(0, 20);
  spr.deleteSprite();

  tft.println("ATS25X2 Polish Project");
  tft.setCursor(7, 33);
  tft.println(" Wersja 0.17PL");
  tft.setCursor(7, 56);
  tft.println(" 12-08-2026");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(7, 79);
  tft.println(" RoX10 PL MOD");
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 160);
  tftPlSetSize(1);
  tftPlSetStyle(NRG_T);
  tftPlSetDatum(BL_T);
  tftPlSetFont(T1012_T);
  tftPlSetColor(TFT_CYAN, TFT_TRANS);
  tftPlPrint("NACIŚNIJ I TRZYMAJ ENKODER PODCZAS", 0, 172);
  tftPlPrint("URUCHAMIANIA BY ZRESETOWAĆ USTAWIENIA", 0, 184);
  tftPlPrint("PRZYTRZYMAJ ENKODER I DOTKNIJ EKRAN", 0, 196);
  tftPlPrint("BY GO OBRÓCIĆ", 0, 208);
  tft.setTextSize(2);
  tft.setCursor(20, 130);
  delay(1500);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  if ( si4735Addr == 0 ) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.print("NIE WYKRYTO Si4735");
    Serial.println("NIE WYKRYTO Si4735");
    while (1);
  } else {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("Si473X addr :  ");
    tft.println(si4735Addr, HEX);
	delay(3000);
  }

  delay(3000);

  if (EEPROM.read(offsetEEPROM) != storage.chkDigit || analogRead(ENCODER_SWITCH) < 500) {
    ErrorBeep();
    Serial.println(F("Zapisywanie...."));
    saveConfig();
  }
  loadConfig();
  printConfig();
  updateCWDecoderPin();
  tft.setTouch(touchCalData);

  pinMode(ENCODER_PIN_A , INPUT_PULLUP);
  pinMode(ENCODER_PIN_B , INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), RotaryEncFreq, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), RotaryEncFreq, CHANGE);

  for (int i = 0; i <= lastBand; i++) bandMode[i] = band[i].prefmod;

  if (si4735Addr == 17)
  {
    si4735.setDeviceI2CAddress(0);
  }
  else
  {
    si4735.setDeviceI2CAddress(1);
  }

  bandIdx                   =  storage.bandIdx;
  band[bandIdx].currentFreq =  storage.Freq;
  currentMode               =  storage.currentMode;
  bwIdxSSB                  =  storage.bwIdxSSB;
  bwIdxAM                   =  storage.bwIdxAM;
  bwIdxFM                   =  storage.bwIdxFM;
  ssIdxMW                   =  storage.ssIdxMW;
  ssIdxAM                   =  storage.ssIdxAM;
  ssIdxFM                   =  storage.ssIdxFM;
  currentBFO                =  storage.currentBFO;
  currentBFOmanu            =  storage.currentBFOmanu;
  AGCgain                   =  storage.AGCgain;
  currentVOL                =  storage.currentVOL;
  currentBFOStep            =  storage.currentBFOStep;
  RDS                       =  storage.RDS;
  FreqSI5351                =  storage.FreqSI5351;
  currentBrightness         =  storage.currentBrightness;
  currentAGCgain            =  storage.currentAGCgain;
  calibratvalSI5351         =  storage.calibratvalSI5351;
  band[ 1].lastmanuBFO      =  storage.BFOLW;
  band[ 2].lastmanuBFO      =  storage.BFOMW;
  band[ 3].lastmanuBFO      =  storage.BFO600M;
  band[ 4].lastmanuBFO      =  storage.BFO630M;
  band[ 5].lastmanuBFO      =  storage.BFO160M;
  band[ 6].lastmanuBFO      =  storage.BFO120M;
  band[ 7].lastmanuBFO      =  storage.BFO90M;
  band[ 8].lastmanuBFO      =  storage.BFO80M;
  band[ 9].lastmanuBFO      =  storage.BFO75M;
  band[10].lastmanuBFO      =  storage.BFO60M;
  band[11].lastmanuBFO      =  storage.BFO49M;
  band[12].lastmanuBFO      =  storage.BFO40M;
  band[13].lastmanuBFO      =  storage.BFO41M;
  band[14].lastmanuBFO      =  storage.BFO31M;
  band[15].lastmanuBFO      =  storage.BFO30M;
  band[16].lastmanuBFO      =  storage.BFO25M;
  band[17].lastmanuBFO      =  storage.BFO22M;
  band[18].lastmanuBFO      =  storage.BFO20M;
  band[19].lastmanuBFO      =  storage.BFO19M;
  band[20].lastmanuBFO      =  storage.BFO17M;
  band[21].lastmanuBFO      =  storage.BFO16M;
  band[22].lastmanuBFO      =  storage.BFO15M;
  band[23].lastmanuBFO      =  storage.BFO15H;
  band[24].lastmanuBFO      =  storage.BFO13M;
  band[25].lastmanuBFO      =  storage.BFO12M;
  band[26].lastmanuBFO      =  storage.BFO11M;
  band[27].lastmanuBFO      =  storage.BFOCB;
  band[28].lastmanuBFO      =  storage.BFO10M;
  band[29].lastmanuBFO      =  storage.BFOSW;

  if (storage.chk4 == '@') {
    PresetId                  =  storage.PresetId;
    currentPRES               =  storage.currentPRES;
    bandRetro[0].currentFreq  =  float(storage.currentFreqRetro0) / 100;
    bandRetro[1].currentFreq  =  float(storage.currentFreqRetro1) / 100;
    bandRetro[2].currentFreq  =  float(storage.currentFreqRetro2);
    bandRetro[3].currentFreq  =  float(storage.currentFreqRetro3);
    bandRetro[4].currentFreq  =  float(storage.currentFreqRetro4);
    bandRetro[5].currentFreq  =  float(storage.currentFreqRetro5);
    bandRetro[6].currentFreq  =  float(storage.currentFreqRetro6);
    bandRetro[7].currentFreq  =  float(storage.currentFreqRetro7);
    bandRetro[8].currentFreq  =  float(storage.currentFreqRetro8);
    saverTime                 =  storage.saverTime;
    RETROband                 =  storage.RETROband;
    SCANscale                 =  storage.SCANscale;
    boolOpt                   =  storage.boolOpt;
  } else {
    storage.chk4              =  '@';
    PresetId                  =  777;
    currentPRES               =  0;
    bandRetro[0].currentFreq  =  8750;
    bandRetro[1].currentFreq  =  6400;
    bandRetro[2].currentFreq  =  153;
    bandRetro[3].currentFreq  =  522;
    bandRetro[4].currentFreq  =  2300;
    bandRetro[5].currentFreq  =  5900;
    bandRetro[6].currentFreq  =  9400;
    bandRetro[7].currentFreq  =  13570;
    bandRetro[8].currentFreq  =  18900;
    saverTime                 =  600;
    RETROband                 =  0;
    SCANscale                 =  193;
    boolOpt                   =  1181;
  }

  if (storage.chk5 == '@') {
    currentSquelch =  storage.SquelchVal;
  } else {
    storage.chk5              = '@';
    storage.SquelchVal        = 0;
  }

  if (storage.chk6 == '@') {
    wifiEnable = storage.wifiEnableAtBoot;
  } else {
    storage.chk6             = '@';
    storage.wifiEnableAtBoot = 1;
    wifiEnable                = true;
  }
  if (storage.chk7 != '@') {
    storage.chk7 = '@';
    storage.wifiSSID[0] = '\0';
    storage.wifiPassword[0] = '\0';
  }
  if (storage.chk8 == '@') {
    batMinV = storage.batMinV;
    batMaxV = storage.batMaxV;
  } else {
    storage.chk8   = '@';
    storage.batMinV = batMinV = 270;
    storage.batMaxV = batMaxV = 405;
  }
  if (storage.chk9 == '@') {
    ScanMarkSNR = storage.ScanMarkSNR;
    scanStopOnSignal = storage.scanStopOnSignal;
    scanStopSeconds = storage.scanStopSeconds;
    scanWaterfallOn = storage.scanWaterfallOn;
  } else {
    storage.chk9 = '@';
    storage.ScanMarkSNR = ScanMarkSNR = 3;
    storage.scanStopOnSignal = scanStopOnSignal = 0;
    storage.scanStopSeconds = scanStopSeconds = 5;
    storage.scanWaterfallOn = scanWaterfallOn = 0;
  }
  if (storage.chk10 == '@') {
    cwDecoderOn = storage.cwDecoderOn;
  } else {
    storage.chk10 = '@';
    storage.cwDecoderOn = cwDecoderOn = 0;
  }
  if (storage.chk11 == '@') {
    for (int i = 0; i < 5; i++) touchCalData[i] = storage.touchCalData[i];
  } else {
    storage.chk11 = '@';
    uint16_t defCal[5] = {387, 3530, 246, 3555, 7};
    for (int i = 0; i < 5; i++) storage.touchCalData[i] = touchCalData[i] = defCal[i];
  }

  OPTunpack();
  scanOPTunpack();

  pressed = tft.getTouch(&x, &y);
  if (pressed) {
    ErrorBeep();
    screenV = !screenV;
  }
  screenRotate();

  if (VHFon) band[0].minimumFreq = 6400; else band[0].minimumFreq = 8750;

  for (int i = 0; i <= lastMemoBank; i++) {
    if (i > lastMemoBankFile) {
      MemoBank[i].freq = 0;
      MemoBank[i].band = 0;
      for (int j = 0; j < 21; j++) MemoBank[i].Name[j] = char(32);
    } else {
      MemoBank[i].freq = MemoBankFile[i].freq;
      MemoBank[i].band = MemoBankFile[i].band + (MemoBankFile[i].mode * 32);
      int n = 0;
      int j = 0;
      while (MemoBankFile[i].Name[j] != NULL and n < 21) {
        if (char(MemoBankFile[i].Name[j]) < 208) {
          MemoBank[i].Name[n] = MemoBankFile[i].Name[j];
          n++;
        }
        j++;
      }
      if (n < 21) for (int j = n; j < 21; j++) MemoBank[i].Name[j] = char(32);
    }
  }
  if (loadMemory) {
    saveMemo();
    loadMemory = false;
  }
  loadMemo();

#ifdef IhaveCrystal
  if (bandIdx == 0) si4735.setup(RESET_PIN, FM_BAND_TYPE);
  else si4735.setup(RESET_PIN, 1);
  if (bandIdx != 0) si4735.setAM();
#endif

#ifdef IhaveSI5351
  si5351wire.set_freq(FreqSI5351, CLK_Xtal);
  si4735.setRefClock(32768);
  si4735.setRefClockPrescaler(1);
  if (bandIdx == 0)  si4735.setup(RESET_PIN, -1, POWER_UP_FM, SI473X_ANALOG_AUDIO, XOSCEN_RCLK);
  else si4735.setup(RESET_PIN, -1, POWER_UP_AM, SI473X_ANALOG_AUDIO, XOSCEN_RCLK);
  if (bandIdx != 0) si4735.setAM();
#endif

  si4735.setAudioMute(audioMuteOn);
  if (!displayPower) ledcWrite(LedChannelforTFT, currentBrightness);
  freqstep = 1000;
  previousBFO = -1;
  band[bandIdx].lastBFO  = currentBFO;
  freqDec = currentBFO;
  band[bandIdx].prefmod = currentMode;
  si4735.setVolume(0);
  previousVOL = currentVOL;
  previousAGCgain = currentAGCgain;
  BandSet();
  currentFrequency = previousFrequency = band[bandIdx].currentFreq;
  si4735.setVolume(0);
  delay(200);
  si4735.setAudioMute(audioMuteOff);
 delay(300);
  for (uint8_t v = 1; v <= currentVOL; v++) {
    si4735.setVolume(v);
    delay(15);
  }
  if (currentVOL == 0) {
    si4735.setVolume(1);
}
  encBut = 600;
  x = y = 0;
  DrawFila();
  Beep(2, 200);
  si4735.setSeekFmSpacing(10);
  si4735.setSeekFmLimits(band[0].minimumFreq, band[0].maximumFreq);
  si4735.setSeekAmRssiThreshold(50);
  si4735.setSeekAmSrnThreshold(20);
  si4735.setSeekFmRssiThreshold(5);
  si4735.setSeekFmSrnThreshold(5);
  xTaskCreate(SaveInEeprom, "SaveInEeprom", 2048, NULL, 1, NULL);
  delay(10);
}

void drawProgress(uint8_t percentage, String text) {

  if (!drawProgressScreenReady) {
    tft.fillScreen(TFT_BLACK);
    const int logoW = 320; 
    const int logoH = 120 * logoW / 265; 
    spr.createSprite(logoW, logoH);

    for (float b = 0.15; b < 1.0; b += 0.17) {
      spr.fillSprite(TFT_BLACK);
      pushScaledDimmedImage(spr, logoW, logoH, (uint16_t *)logo, 265, 120, b);
      spr.pushSprite(0, 20);
      delay(25);
    }

    pushScaledImage(spr, logoW, logoH, (uint16_t *)logo, 265, 120);
    spr.pushSprite(0, 20);
    spr.deleteSprite();
    drawProgressScreenReady = true;
  }

  tft.fillRect(0, 190, 320, 34, TFT_BLACK);
  tftPlSetSize(1);
  tftPlSetStyle(NRG_T);
  tftPlSetDatum(BL_T);
  tftPlSetFont(T1012_T);
  tftPlSetColor(TFT_YELLOW, TFT_TRANS);
  tftPlPrint(text, 10, 202);
  tft.drawRect(10, 208, 320 - 20, 15, TFT_WHITE);
  tft.fillRect(12, 210, 296 * percentage / 100, 12, TFT_BLUE);
}

void SaveInEeprom (void* arg)  {

  while (1) {
    OPTpack();
    scanOPTpack();

    storage.bandIdx           = bandIdx;
    storage.Freq              = band[bandIdx].currentFreq;
    storage.currentMode       = currentMode;
    storage.bwIdxSSB          = bwIdxSSB;
    storage.bwIdxAM           = bwIdxAM;
    storage.bwIdxFM           = bwIdxFM;
    storage.ssIdxMW           = ssIdxMW;
    storage.ssIdxAM           = ssIdxAM;
    storage.ssIdxFM           = ssIdxFM;
    storage.currentBFO        = currentBFO;
    storage.currentBFOmanu    = currentBFOmanu;
    storage.AGCgain           = AGCgain;
    storage.currentVOL        = currentVOL;
    storage.currentBFOStep    = currentBFOStep;
    storage.RDS               = RDS;
    storage.FreqSI5351        = FreqSI5351;
    storage.currentBrightness = currentBrightness;
    storage.currentAGCgain    = currentAGCgain;
    storage.calibratvalSI5351 = calibratvalSI5351;
    storage.BFOLW = band[1].lastmanuBFO;
    storage.BFOMW = band[2].lastmanuBFO;
    storage.BFO600M = band[3].lastmanuBFO;
    storage.BFO630M = band[4].lastmanuBFO;
    storage.BFO160M = band[5].lastmanuBFO;
    storage.BFO120M = band[6].lastmanuBFO;
    storage.BFO90M = band[7].lastmanuBFO;
    storage.BFO80M = band[8].lastmanuBFO;
    storage.BFO75M = band[9].lastmanuBFO;
    storage.BFO60M = band[10].lastmanuBFO;
    storage.BFO49M = band[11].lastmanuBFO;
    storage.BFO40M = band[12].lastmanuBFO;
    storage.BFO41M = band[13].lastmanuBFO;
    storage.BFO31M = band[14].lastmanuBFO;
    storage.BFO30M = band[15].lastmanuBFO;
    storage.BFO25M = band[16].lastmanuBFO;
    storage.BFO22M = band[17].lastmanuBFO;
    storage.BFO20M = band[18].lastmanuBFO;
    storage.BFO19M = band[19].lastmanuBFO;
    storage.BFO17M = band[20].lastmanuBFO;
    storage.BFO16M = band[21].lastmanuBFO;
    storage.BFO15M = band[22].lastmanuBFO;
    storage.BFO15H = band[23].lastmanuBFO;
    storage.BFO13M = band[24].lastmanuBFO;
    storage.BFO12M = band[25].lastmanuBFO;
    storage.BFO11M = band[26].lastmanuBFO;
    storage.BFOCB  = band[27].lastmanuBFO;
    storage.BFO10M = band[28].lastmanuBFO;
    storage.BFOSW  = band[29].lastmanuBFO;
    storage.PresetId = PresetId;
    storage.currentPRES = currentPRES;
    storage.currentFreqRetro0 = uint16_t(bandRetro[0].currentFreq * 100);
    storage.currentFreqRetro1 = uint16_t(bandRetro[1].currentFreq * 100);
    storage.currentFreqRetro2 = uint16_t(bandRetro[2].currentFreq);
    storage.currentFreqRetro3 = uint16_t(bandRetro[3].currentFreq);
    storage.currentFreqRetro4 = uint16_t(bandRetro[4].currentFreq);
    storage.currentFreqRetro5 = uint16_t(bandRetro[5].currentFreq);
    storage.currentFreqRetro6 = uint16_t(bandRetro[6].currentFreq);
    storage.currentFreqRetro7 = uint16_t(bandRetro[7].currentFreq);
    storage.currentFreqRetro8 = uint16_t(bandRetro[8].currentFreq);
    storage.saverTime = saverTime;
    storage.RETROband = RETROband;
    storage.SCANscale = SCANscale;
    storage.boolOpt = boolOpt;
	    storage.SquelchVal = currentSquelch;
    storage.wifiEnableAtBoot = wifiEnable;
    storage.batMinV = batMinV;
    storage.batMaxV = batMaxV;
    storage.ScanMarkSNR = ScanMarkSNR;
    storage.scanStopOnSignal = scanStopOnSignal;
    storage.scanStopSeconds = scanStopSeconds;
    storage.scanWaterfallOn = scanWaterfallOn;
    storage.cwDecoderOn = cwDecoderOn;
    for (int i = 0; i < 5; i++) storage.touchCalData[i] = touchCalData[i];

    bool eepromChanged = false;
    for (unsigned int t = 0; t < sizeof(storage); t++) {

      if (EEPROM.read(offsetEEPROM + t) != *((char*)&storage + t)) {

        EEPROM.write(offsetEEPROM + t, *((char*)&storage + t));
        eepromChanged = true;
      }
      if ((t & 0x3F) == 0) vTaskDelay(1);
    }

    for (unsigned int t = 0; t < sizeof(MemoBank); t++) {

      if (EEPROM.read(offsetMemoEEPROM + t) != *((char*)&MemoBank + t)) {

        EEPROM.write(offsetMemoEEPROM + t, *((char*)&MemoBank + t));
        eepromChanged = true;
      }
      if ((t & 0x3F) == 0) vTaskDelay(1);
    }

    if (eepromChanged) {
      writingEeprom = true;
      EEPROM.commit();
      writingEeprom = false;
    }
    vTaskDelay(5000 / portTICK_RATE_MS);
  }

}

void saveMemo() {

  delay(10);
  bool memoChanged = false;
  for (unsigned int t = 0; t < sizeof(MemoBank); t++) {
    if (EEPROM.read(offsetMemoEEPROM + t) != *((char*)&MemoBank + t)) {
      EEPROM.write(offsetMemoEEPROM + t, *((char*)&MemoBank + t));
      memoChanged = true;
    }
  }
  if (memoChanged) EEPROM.commit();
}

void loadMemo() {

  if (EEPROM.read(offsetEEPROM + 0) == storage.chkDigit) {
    for (unsigned int t = 0; t < sizeof(MemoBank); t++)
      *((char*)&MemoBank + t) = EEPROM.read(offsetMemoEEPROM + t);
    Serial.println("Załadowano konfigurację");
  }
}

void saveConfig() {

  delay(10);
  bool configChanged = false;
  for (unsigned int t = 0; t < sizeof(storage); t++) {
    if (EEPROM.read(offsetEEPROM + t) != *((char*)&storage + t)) {
      EEPROM.write(offsetEEPROM + t, *((char*)&storage + t));
      configChanged = true;
    }
  }
  if (configChanged) EEPROM.commit();
}

void loadConfig()
{

  byte chk = EEPROM.read(offsetEEPROM);

  if (chk != '@')
  {
    Serial.println("EEPROM pusty - używam ustawień domyślnych.");

    memset(storage.wifiSSID, 0, sizeof(storage.wifiSSID));
    memset(storage.wifiPassword, 0, sizeof(storage.wifiPassword));

    storage.chk6 = 0;
    storage.chk7 = 0;

    return;
  }

  for (unsigned int t = 0; t < sizeof(storage); t++)
  {
    *((char*)&storage + t) = EEPROM.read(offsetEEPROM + t);
  }

  if (storage.chk6 != '@' || storage.chk7 != '@')
  {
    Serial.println("Brak zapisanej konfiguracji WiFi.");

    memset(storage.wifiSSID, 0, sizeof(storage.wifiSSID));
    memset(storage.wifiPassword, 0, sizeof(storage.wifiPassword));

    storage.chk6 = 0;
    storage.chk7 = 0;
  }

  storage.wifiSSID[sizeof(storage.wifiSSID)-1] = '\0';
  storage.wifiPassword[sizeof(storage.wifiPassword)-1] = '\0';

  Serial.println("Załadowano konfigurację.");
}

void printConfig() {

  Serial.print("Storage = ");
  Serial.println(sizeof(storage));
  if (EEPROM.read(offsetEEPROM) == storage.chkDigit) {
    for (unsigned int t = 0; t < sizeof(storage); t++)
      Serial.write(EEPROM.read(offsetEEPROM + t));
    Serial.println();
  }
}

void BandSet()  {

  si4735.setAudioMute(audioMuteOn);
  if (bandIdx == 0) currentMode = FM;
  if ((currentMode == AM) or (currentMode == FM)) {
    ssbLoaded = false;
  }
  if ((currentMode == LSB) or  (currentMode == USB))
  {
    if (ssbLoaded == false) {
      loadSSB();
    }
  }
  useBand();
  setBandWidth();
  checkAGC();
  delay(20);
  si4735.setAudioMute(audioMuteOff);
}

void useBand()  {

  if ((band[bandIdx].bandType == MW_BAND_TYPE) || (band[bandIdx].bandType == LW_BAND_TYPE)) {
    band[bandIdx].currentStep = ssIdxMW;
  }
  if (band[bandIdx].bandType == SW_BAND_TYPE) {
    band[bandIdx].currentStep = ssIdxAM;
  }
  if (band[bandIdx].bandType == FM_BAND_TYPE)
  {
    bfoOn = false;
    si4735.setTuneFrequencyAntennaCapacitor(0);
    delay(100);
    band[bandIdx].currentStep = ssIdxFM;
    si4735.setFM(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq, band[bandIdx].currentFreq, band[bandIdx].currentStep);
    si4735.setFMDeEmphasis(1);
    ssbLoaded = false;
    si4735.RdsInit();
    si4735.setRdsConfig(1, 2, 2, 2, 2);
  }
  else
  {
    if (band[bandIdx].bandType == MW_BAND_TYPE || band[bandIdx].bandType == LW_BAND_TYPE) {
      si4735.setTuneFrequencyAntennaCapacitor(0);
    } else {
      si4735.setTuneFrequencyAntennaCapacitor(1);
    }
    if (ssbLoaded)
    {
      si4735.setSSB(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq, band[bandIdx].currentFreq, band[bandIdx].currentStep, currentMode);

      si4735.setSSBBfo(currentBFO + currentBFOmanu);
      int temp = 1;
      si4735.setFrequencyStep(temp);
      band[bandIdx].currentStep = temp;
    }
    else
    {
      si4735.setAM(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq, band[bandIdx].currentFreq, band[bandIdx].currentStep);

      bfoOn = false;
    }
  }
  delay(100);

}

void setBandWidth()  {

  if (currentMode == LSB || currentMode == USB)
  {
    si4735.setSSBAudioBandwidth(bwIdxSSB);

    if (bwIdxSSB == 0 || bwIdxSSB == 4 || bwIdxSSB == 5)
      si4735.setSBBSidebandCutoffFilter(0);
    else
      si4735.setSBBSidebandCutoffFilter(1);
  }
  if (currentMode == AM)
  {
    si4735.setBandwidth(bwIdxAM, 0);
  }
  if (currentMode == FM)
  {
    si4735.setFmBandwidth(bwIdxFM);
  }
}

void loadSSB()  {

  si4735.reset();
  si4735.queryLibraryId();
  si4735.patchPowerUp();
  delay(50);
  si4735.setI2CFastMode();

  si4735.downloadPatch(ssb_patch_content, size_content);
  si4735.setI2CStandardMode();

  si4735.setSSBConfig(bwIdxSSB, 1, 0, 1, 0, 1);
  delay(25);
  ssbLoaded = true;
}

void Freqcalq(int keyval)  {

  if (keyval > 11) {
    tft.fillRect(0, 80, 240, 40, TFT_NAVY);
    if (keyval == 12) {

      if (fact == 1) DisplayfreqNew = float(int(DisplayfreqNew / 100)) * 10;
      if (fact == 10) {
        Decipoint = false;
        fact = 1;
      }
      if (fact == 100) {
        dpfrq = 0;
        fact = 10;
      }
      if (fact == 1000) {
        dpfrq = float(int(dpfrq * 10)) / 10;
        fact = 100;
      }
    } else {

      DisplayfreqNew = 0;
      dpfrq = 0;
      fact = 1;
      Decipoint = false;
    }
  } else {
    if (Decipoint) {
      if (keyval < 10 and fact < 1000) {
        dpfrq = dpfrq + keyval / fact;
        fact = fact * 10;
      }
    } else {
      if ((DisplayfreqNew + keyval) <= 30000) DisplayfreqNew = (DisplayfreqNew + keyval) * 10;
    }
  }
  tft.setTextDatum(BL_DATUM);
  tft.setTextSize(3);
  float realFreq = (DisplayfreqNew / 10) + dpfrq;
  if ((realFreq > 0 and realFreq < 31) or (realFreq >= 64 and realFreq <= 108) or (realFreq > 152 and realFreq <= 30000)) tft.setTextColor(TFT_WHITE, TFT_NAVY); else tft.setTextColor(TFT_RED, TFT_NAVY);
  tft.setCursor(10, 90);
  String hhz = "    ";
  if (DisplayfreqNew > 0) {
    if (Decipoint) {
      hhz = " MHz";
      if (fact == 10) {
        tft.print((DisplayfreqNew / 10), 0);
        tft.print(".");
      }
      if (fact == 100) tft.print(((DisplayfreqNew / 10) + dpfrq), 1);
      if (fact == 1000) tft.print(((DisplayfreqNew / 10) + dpfrq), 2);
    } else {
      int khz = trunc(DisplayfreqNew / 10000);
      int hz = (DisplayfreqNew - (khz * 10000)) / 10;
      char s[6] = {'\0'};
      if (realFreq < 153) {
        sprintf(s, "%i", hz);
        if ((realFreq > 0 and realFreq < 31) or (realFreq >= 64 and realFreq <= 108)) hhz = " MHz";
      } else {
        hhz = " KHz";
        if (realFreq < 1000) sprintf(s, "%i", hz); else sprintf(s, "%i %03i", khz, hz);
      }
      tft.drawString(s, 10, 114);
    }
  } else tft.print("0");
  if (!Decipoint and DisplayfreqNew >= (band[0].minimumFreq / 10) and DisplayfreqNew <= (band[0].maximumFreq / 10)) drawButton(L_FREQ, 9, B_NORMAL); else drawButton(L_FREQ, 9, B_BLOCK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW, TFT_NAVY);
  tft.drawString(hhz, 186, 114);
}

void SmeterHistory() {

  tft.fillRect(Xsmtr + 2, Ysmtr + 6, 236, 46, TFT_BLACK);
  tft.drawRect(Xsmtr + 2, Ysmtr + 6, 236, 46, TFT_DARKGREY);

  int baseY = Ysmtr + 50;
  int top   = Ysmtr + 20;
  int maxH  = baseY - top;

  for (int i = 0; i < RSSI_HIST_LEN; i++) {
    int v = rssiHist[i];
    if (v > 100) v = 100;
    int h = (v * maxH) / 100;
    uint16_t col;
    if (v < 30) col = TFT_GREEN; else if (v < 60) col = TFT_YELLOW; else col = TFT_RED;
    int x = Xsmtr + 15 + i * 2;
    if (h > 0) tft.fillRect(x, baseY - h, 2, h, col);
  }
  tft.drawFastHLine(Xsmtr + 3, baseY + 1, 234, TFT_DARKGREY);

  tftPlSetFont(T1012_T);
  tftPlSetSize(1);
  tftPlSetStyle(NRG_T);
  tftPlSetDatum(BL_T);
  tftPlSetColor(TFT_CYAN, TFT_TRANS);
  tftPlPrint("TREND RSSI", Xsmtr + 8, Ysmtr + 17);

  uint8_t lastVal = rssiHist[RSSI_HIST_LEN - 1];
  tftPlSetDatum(BR_T);
  tftPlSetColor(TFT_YELLOW, TFT_TRANS);
  tftPlPrint(String(lastVal) + "%", Xsmtr + 231, Ysmtr + 17);
}

void updateRssiHistory(uint8_t level) {
  if ((millis() - elapsedRssiHist) < RSSI_HIST_INTERVAL) return;
  elapsedRssiHist = millis();
  for (int i = 0; i < RSSI_HIST_LEN - 1; i++) rssiHist[i] = rssiHist[i + 1];
  rssiHist[RSSI_HIST_LEN - 1] = level;
}

int liveMeterPeak = 0;
unsigned long lastPeakDecayTime = 0;
#define METER_PEAK_DECAY_MS 80

void cycleMeterMode() {
  if (!rssiHistoryOn and !analogMeterOn) rssiHistoryOn = true;
  else if (rssiHistoryOn) { rssiHistoryOn = false; analogMeterOn = true; }
  else analogMeterOn = false;
}

void updateLiveMeterPeak(int spoint) {
  if (spoint > liveMeterPeak) {
    liveMeterPeak = spoint;
    lastPeakDecayTime = millis();
  } else if (millis() - lastPeakDecayTime > METER_PEAK_DECAY_MS) {
    lastPeakDecayTime = millis();
    if (liveMeterPeak > 0) liveMeterPeak--;
  }
}

void Smeter() {

  int spoint;
  if (currentMode != FM) {

    if ((rssi >= 0) and (rssi <=  1)) spoint =  12;
    if ((rssi >  1) and (rssi <=  2)) spoint =  24;
    if ((rssi >  2) and (rssi <=  3)) spoint =  36;
    if ((rssi >  3) and (rssi <=  4)) spoint =  48;
    if ((rssi >  4) and (rssi <= 10)) spoint =  48 + (rssi - 4) * 2;
    if ((rssi > 10) and (rssi <= 16)) spoint =  60 + (rssi - 10) * 2;
    if ((rssi > 16) and (rssi <= 22)) spoint =  72 + (rssi - 16) * 2;
    if ((rssi > 22) and (rssi <= 28)) spoint =  84 + (rssi - 22) * 2;
    if ((rssi > 28) and (rssi <= 34)) spoint =  96 + (rssi - 28) * 2;
    if ((rssi > 34) and (rssi <= 44)) spoint = 108 + (rssi - 34) * 2;
    if ((rssi > 44) and (rssi <= 54)) spoint = 124 + (rssi - 44) * 2;
    if ((rssi > 54) and (rssi <= 64)) spoint = 140 + (rssi - 54) * 2;
    if ((rssi > 64) and (rssi <= 74)) spoint = 156 + (rssi - 64) * 2;
    if ((rssi > 74) and (rssi <= 84)) spoint = 172 + (rssi - 74) * 2;
    if ((rssi > 84) and (rssi <= 94)) spoint = 188 + (rssi - 84) * 2;
    if  (rssi > 94)                   spoint = 204;
    if  (rssi > 95)                   spoint = 208;
  }
  else
  {

    if  (rssi <  1) spoint = 36;
    if ((rssi >  1) and (rssi <=  2)) spoint =  60;
    if ((rssi >  2) and (rssi <=  8)) spoint =  84 + (rssi - 2) * 2;
    if ((rssi >  8) and (rssi <= 14)) spoint =  96 + (rssi - 8) * 2;
    if ((rssi > 14) and (rssi <= 24)) spoint = 108 + (rssi - 14) * 2;
    if ((rssi > 24) and (rssi <= 34)) spoint = 124 + (rssi - 24) * 2;
    if ((rssi > 34) and (rssi <= 44)) spoint = 140 + (rssi - 34) * 2;
    if ((rssi > 44) and (rssi <= 54)) spoint = 156 + (rssi - 44) * 2;
    if ((rssi > 54) and (rssi <= 64)) spoint = 172 + (rssi - 54) * 2;
    if ((rssi > 64) and (rssi <= 74)) spoint = 188 + (rssi - 64) * 2;
    if  (rssi > 74)                   spoint = 204;
    if  (rssi > 76)                   spoint = 208;
  }

  int tik = 0;
  int met = spoint + 2;
  while (met > 11 and tik < 9) {
    if (tik) tft.fillRect(Xsmtr + 20 + (tik * 12), Ysmtr + 38 , 10, 6, TFT_ORANGE); else tft.fillRect(Xsmtr + 15, Ysmtr + 38 , 15, 6, TFT_RED);
    met -= 12;
    tik++;
  }
  while (met > 15 and tik < 15) {
    tft.fillRect(Xsmtr + 20 + ((tik - 9) * 16) + 108, Ysmtr + 38 , 14, 6, TFT_GREEN);
    met -= 16;
    tik++;
  }
  if (tik == 15 and met > 4) {
    tft.fillRect(Xsmtr + 20 + 204, Ysmtr + 38, 3, 6, TFT_ORANGE);
  } else {
    tft.fillRect(Xsmtr + 22 + spoint - met, Ysmtr + 38, 207 - (2 + spoint) + met, 6, TFT_BLACK);
  }

  updateLiveMeterPeak(spoint);
  if (liveMeterPeak > spoint + 2) {
    tft.fillRect(Xsmtr + 20 + liveMeterPeak, Ysmtr + 38, 2, 6, TFT_WHITE);
  }

  tft.fillRect(Xsmtr + 190, Ysmtr + 6, 46, 14, TFT_BLACK);
  tftPlSetFont(T1012_T);
  tftPlSetSize(1);
  tftPlSetStyle(NRG_T);
  tftPlSetDatum(BR_T);
  tftPlSetColor(TFT_YELLOW, TFT_TRANS);
  tftPlPrint(rssiToSLabel(rssi, currentMode == FM), Xsmtr + 231, Ysmtr + 17);
}

String rssiToSLabel(int rssiVal, bool isFM) {
  if (!isFM) {
    if (rssiVal <=  1) return "S0";
    if (rssiVal <=  2) return "S1";
    if (rssiVal <=  3) return "S2";
    if (rssiVal <=  4) return "S3";
    if (rssiVal <= 10) return "S4";
    if (rssiVal <= 16) return "S5";
    if (rssiVal <= 22) return "S6";
    if (rssiVal <= 28) return "S7";
    if (rssiVal <= 34) return "S8";
    if (rssiVal <= 44) return "S9";
    if (rssiVal <= 54) return "S9+10";
    if (rssiVal <= 64) return "S9+20";
    if (rssiVal <= 74) return "S9+30";
    if (rssiVal <= 84) return "S9+40";
    if (rssiVal <= 94) return "S9+50";
    return "S9+60";
  } else {
    if (rssiVal <=  2) return "S6";
    if (rssiVal <=  8) return "S7";
    if (rssiVal <= 14) return "S8";
    if (rssiVal <= 24) return "S9";
    if (rssiVal <= 34) return "S9+10";
    if (rssiVal <= 44) return "S9+20";
    if (rssiVal <= 54) return "S9+30";
    if (rssiVal <= 64) return "S9+40";
    if (rssiVal <= 74) return "S9+50";
    return "S9+60";
  }
}

int rssiToMeterLevel(int rssiVal, bool isFM) {
  if (!isFM) {
    if (rssiVal <=  1) return 0;
    if (rssiVal <=  2) return 1;
    if (rssiVal <=  3) return 2;
    if (rssiVal <=  4) return 3;
    if (rssiVal <= 10) return 4;
    if (rssiVal <= 16) return 5;
    if (rssiVal <= 22) return 6;
    if (rssiVal <= 28) return 7;
    if (rssiVal <= 34) return 8;
    if (rssiVal <= 44) return 9;
    if (rssiVal <= 54) return 10;
    if (rssiVal <= 64) return 11;
    if (rssiVal <= 74) return 12;
    if (rssiVal <= 84) return 13;
    if (rssiVal <= 94) return 14;
    return 15;
  } else {
    if (rssiVal <=  2) return 6;
    if (rssiVal <=  8) return 7;
    if (rssiVal <= 14) return 8;
    if (rssiVal <= 24) return 9;
    if (rssiVal <= 34) return 10;
    if (rssiVal <= 44) return 11;
    if (rssiVal <= 54) return 12;
    if (rssiVal <= 64) return 13;
    if (rssiVal <= 74) return 14;
    return 15;
  }
}

void drawAnalogMeter() {
  tft.fillRect(Xsmtr + 2, Ysmtr + 6, 236, 46, TFT_BLACK);
  tft.drawRect(Xsmtr + 2, Ysmtr + 6, 236, 46, TFT_DARKGREY);

  int cx = Xsmtr + 118, cy = Ysmtr + 54;
  const int rTickOuter = 44, rTickInner = 36, rNeedle = 38;

  for (int lvl = 0; lvl <= 15; lvl++) {
    float ang = radians(-50.0 + (100.0 * lvl / 15.0));
    int x1 = cx + sin(ang) * rTickInner;
    int y1 = cy - cos(ang) * rTickInner;
    int x2 = cx + sin(ang) * rTickOuter;
    int y2 = cy - cos(ang) * rTickOuter;
    uint16_t col = (lvl <= 9) ? TFT_SILVER : TFT_RED;
    tft.drawLine(x1, y1, x2, y2, col);
  }

  int level = rssiToMeterLevel(rssi, currentMode == FM);
  float needleAng = radians(-50.0 + (100.0 * level / 15.0));
  int nx = cx + sin(needleAng) * rNeedle;
  int ny = cy - cos(needleAng) * rNeedle;
  tft.drawLine(cx, cy, nx, ny, TFT_RED);
  tft.drawLine(cx + 1, cy, nx + 1, ny, TFT_RED);
  tft.fillCircle(cx, cy, 3, TFT_SILVER);

  tftPlSetFont(T1012_T);
  tftPlSetSize(1);
  tftPlSetStyle(NRG_T);
  tftPlSetDatum(BR_T);
  tftPlSetColor(TFT_YELLOW, TFT_TRANS);
  tftPlPrint(rssiToSLabel(rssi, currentMode == FM), Xsmtr + 231, Ysmtr + 17);
}

void checkNightMode() {
  if (!nightModeOn) {
    if (nightModeActive) {
      currentBrightness = nightModeSavedBrightness;
      ledcWrite(LedChannelforTFT, currentBrightness);
      nightModeActive = false;
    }
    return;
  }
  if (millis() - lastNightModeCheck < NIGHT_MODE_CHECK_MS) return;
  lastNightModeCheck = millis();

  struct tm t;
  if (!getLocalTime(&t, 5)) return;
  int h = t.tm_hour;
  bool shouldBeNight = (h >= NIGHT_MODE_START_HOUR or h < NIGHT_MODE_END_HOUR);

  if (shouldBeNight and !nightModeActive) {
    nightModeSavedBrightness = currentBrightness;
    currentBrightness = MinBrightness;
    ledcWrite(LedChannelforTFT, currentBrightness);
    nightModeActive = true;
  } else if (!shouldBeNight and nightModeActive) {
    currentBrightness = nightModeSavedBrightness;
    ledcWrite(LedChannelforTFT, currentBrightness);
    nightModeActive = false;
  }
}

float readVsupply() {
  uint32_t sum = 0;
  const uint8_t samples = 8;
  for (uint8_t i = 0; i < samples; i++) sum += analogRead(BAT_INFO);
  return 3.724 * (sum / (float)samples) / 2047;
}

void Battery() {

  float vsupply = readVsupply();
  int bat = map(int(vsupply * 100), batMinV, batMaxV, 0, 100);
  if ((FirstLayer or ThirdLayer) and ((elapsedBat + 10000) < millis())) {
    if (bat < 0) bat = 0;
    if (bat > 100) bat = 100;
    int colorBatt = TFT_WHITE;
    if (bat < 15) colorBatt = TFT_ORANGE;
    if (bat < 5) colorBatt = 64528;
    tft.drawRect(XVolInd + 175, YVolInd + 4, 48, 18, colorBatt);
    tft.drawRect(XVolInd + 224, YVolInd + 8, 2, 10, colorBatt);
    tftPlSetFont(T1012_T);
    tftPlSetSize(1);
    tftPlSetColor(TFT_WHITE, TFT_NAVY);
    tftPlSetDatum(BC_T);
    tftPlSetStyle(NRG_T);
    if (batVolt) {
      tftPlPrint(String(vsupply, 2) + "V", XVolInd + 199, YVolInd + 19);
    } else {
      int tmp = (46 - tftPlTextWidth(String(bat) + "%")) / 2;
      tft.fillRect(XVolInd + 176, YVolInd + 5, tmp, 16, TFT_NAVY);
      tft.fillRect(XVolInd + 221 - tmp, YVolInd + 5, tmp, 16, TFT_NAVY);
      tftPlPrint(String(bat) + "%", XVolInd + 199, YVolInd + 19);
    }
  }
  if ((elapsedBat + 10000) < millis()) {
    elapsedBat = millis();
    if (bat < 5) ErrorBeep();
  }
}

void VolumeIndicator(int val) {

  tft.setTextColor(TFT_WHITE, TFT_GREY);
  tft.setTextSize(1);
  tft.setCursor(XVolInd + 57, YVolInd + 3);
  tft.print("VOLUME");
  val = map(val, MinVOL, MaxVOL, 0, 128);
  tft.fillRect(XVolInd + 15, YVolInd + 16 , (2 + val), 6, TFT_GREEN);
  tft.fillRect(XVolInd + 17 + val, YVolInd + 16 , 130 - (2 + val), 6, TFT_NAVY);
}

void brightnessIndicator(int val) {

  tft.setTextColor(TFT_WHITE, TFT_GREY);
  tft.setTextSize(1);
  tft.setCursor(XVolInd + 57, YVolInd + 3);
  tft.print("BRIGHT");
  val = map(val, MinBrightness, MaxBrightness, 0, 128);
  tft.fillRect(XVolInd + 15, YVolInd + 16 , (2 + val), 6, TFT_CYAN);
  tft.fillRect(XVolInd + 17 + val, YVolInd + 16 , 130 - (2 + val), 6, TFT_MAROON);
}

void squelchIndicator(int val) {

  tft.setTextColor(TFT_WHITE, TFT_GREY);
  tft.setTextSize(1);
  tft.setCursor(XVolInd + 57, YVolInd + 3);
  tft.print("SQUELCH");
  val = map(val, MinSQUELCH, MaxSQUELCH, 0, 128);
  tft.fillRect(XVolInd + 15, YVolInd + 16 , (2 + val), 6, TFT_ORANGE);
  tft.fillRect(XVolInd + 17 + val, YVolInd + 16 , 130 - (2 + val), 6, TFT_MAROON);
  }

void saver() {

  float freq;
  tft.fillScreen(TFT_BLACK);
  if (displayOff) {
    if (displayPower) digitalWrite(DISPLAY_LED, 1); else ledcWrite(LedChannelforTFT, MinBrightness);
  }
  if (saverOn) {
    Saver = true;
    SEEK = false;
    bright = false;
    AGCgainbut = false;
    freq = si4735.getFrequency();
    if (SCANbut) freq = currentScanFreq + int((currentScanLine - 159 + deltaScanLine) * SCANstep);
    else if (currentMode == LSB || currentMode == USB || currentMode == CW) freq -= int(currentBFO / 1000);

    saverX = random(tft.width()  - SAVER_BLOCK_W  - 2) + 1;
    saverY = random(tft.height() - SAVER_BLOCK_H - 2) + 1;
    saverVX = random(0, 2) ? 2 : -2;
    saverVY = random(0, 2) ? 2 : -2;
    saverColorIdx = 0;
    saverColor = saverColors[saverColorIdx];
    elapsedSaverMove = millis();
  }

  while (((pressed == false) and (encoderCount == 0) and (encoderButton == 0) and (analogRead(ENCODER_SWITCH) > 500)) or (writingEeprom)) {
    pressed = tft.getTouch(&x, &y);
    if (webServerRunning) webServer.handleClient();
    if (saverOn) {
      if ((millis() - elapsedSaverMove) > 40) {
        elapsedSaverMove = millis();

        tft.fillRect(saverX, saverY, SAVER_BLOCK_W, SAVER_BLOCK_H, TFT_BLACK);

        saverX += saverVX;
        saverY += saverVY;

        bool bounced = false;
        if (saverX <= 0) { saverX = 0; saverVX = -saverVX; bounced = true; }
        if (saverX + SAVER_BLOCK_W >= tft.width())  { saverX = tft.width()  - SAVER_BLOCK_W;  saverVX = -saverVX; bounced = true; }
        if (saverY <= 0) { saverY = 0; saverVY = -saverVY; bounced = true; }
        if (saverY + SAVER_BLOCK_H >= tft.height()) { saverY = tft.height() - SAVER_BLOCK_H; saverVY = -saverVY; bounced = true; }

        if (bounced) {
          saverColorIdx = (saverColorIdx + 1) % saverColorsCount;
          saverColor = saverColors[saverColorIdx];
        }

        FreqDraw(freq, 0);

        if (getLocalTime(&timeinfo, 5)) {
          char timeHM[6];
          strftime(timeHM, 6, "%H:%M", &timeinfo);
          spr.createSprite(140, 38);
          spr.fillScreen(COLOR_BACKGROUND);
          spr.setTextSize(1);
          spr.setTextPadding(0);
          spr.setFreeFont(&DSEG7_Classic_Mini_Regular_34);
          spr.setTextDatum(BR_DATUM);
          spr.setTextColor(saverColor);
          spr.drawString(String(timeHM), 140, 38);
          spr.pushSprite(saverX, saverY + 42);
          spr.setFreeFont(NULL);
          spr.deleteSprite();
        }

        {
          String modeBandText;
          if (band[bandIdx].bandType == LW_BAND_TYPE || band[bandIdx].bandType == MW_BAND_TYPE) {
            modeBandText = String(band[bandIdx].bandName);
          } else {
            modeBandText = bandModeDesc[currentMode];
            if (modeBandText == "USB" and CWShift) modeBandText = "CW";
          }

          tftPlSetFont(T1012_T);
          tftPlSetSize(1);
          tftPlSetStyle(NRG_T);
          tftPlSetColor(saverColor, TFT_TRANS);
          tftPlSetDatum(BL_T);
          tftPlPrint(modeBandText, saverX, saverY + 98);

          if (batShow) {
            float vsupply = readVsupply();
            int bat = map(int(vsupply * 100), batMinV, batMaxV, 0, 100);
            if (bat < 0) bat = 0;
            if (bat > 100) bat = 100;
            uint16_t colorBatt = TFT_GREEN;
            if (bat < 15) colorBatt = TFT_ORANGE;
            if (bat < 5) colorBatt = TFT_RED;
            tftPlSetColor(colorBatt, TFT_TRANS);
            tftPlSetDatum(BR_T);
            tftPlPrint(String(bat) + "%", saverX + 140, saverY + 98);
          }
        }
      }
    }
    if (SCANbut and !SCANpause) {
      if (scanStopUntil) {
        if (millis() < scanStopUntil) {
        } else {
          scanStopUntil = 0;
          si4735.setAudioMute(audioMuteOn);
          si4735.setFrequencyStep(1);
        }
      } else {
        DisplaySCAN();
      }
    }
  }

  Saver = false;
  pressed = false;
  encoderCount = 0;
  encoderButton = 0;
  if (displayOff) {
    if (displayPower) digitalWrite(DISPLAY_LED, 0); else ledcWrite(LedChannelforTFT, currentBrightness);
  }
  returnLayer();
  elapsedSaver = millis();
}

void returnLayer() {

  if (FirstLayer) DrawFila();
  else if (ThirdLayer) DrawThla();
  if (HamBand) drawList(L_HAM, "PASMO KRÓTKOFALARSKIE");
  if (FREQbut) {
    drawList(L_FREQ, "CZĘSTOTLIWOŚĆ");
    tft.fillRect(0, 80, 240, 40, TFT_NAVY);
    Freqcalq(0);
  }
  if (Modebut) drawList(L_MODE, "MODULACJA");
  if (BandWidth) {
    if (currentMode == AM) drawList(L_BANDW_AM, "Filtr AM w KHz");
    else if (currentMode == FM) drawList(L_BANDW_FM, "Filtr FM w KHz");
    else drawList(L_BANDW_SSB, "Filtr SSB w KHz");
  }
  if (STEPbut) {
    if (currentMode == AM) drawList(L_STEP_AM, "KROK w AM"); else drawList(L_STEP_FM, "KROK w FM");
  }
  if (BroadBand) drawList(L_BAND, "PASMO");
  if (cityRETRObut) drawRetroCity();
  if (bandRETRObut) drawRetroBand();
  if (RETRObut) {
    drawRETRO();
    drawRETROscale();
  }
  if (MEMObut) {
    drawList(L_MEMO, "PAMIĘĆ");
    if (MEMOadd) {
      drawButton(L_MEMO, 0, B_NORMAL, "OK");
      drawButton(L_MEMO, 1, B_SELECT);
      drawButton(L_MEMO, 3, B_NORMAL, "X");
    }
    if (MEMOdel) {
      drawButton(L_MEMO, 0, B_NORMAL, "OK");
      drawButton(L_MEMO, 1, B_BLOCK);
      drawButton(L_MEMO, 2, B_SELECT);
      drawButton(L_MEMO, 3, B_NORMAL, "X");
    }
    displMEMO();
  }
  if (SETUPbut) {
    drawList(L_SETUP,"USTAWIENIA");
    if (!pageSetup) drawButton(L_SETUP, 0, B_BLOCK);
    if (pageSetup == maxPageSetup) drawButton(L_SETUP, 1, B_BLOCK);
    displSETUP();
  }
  if (SCANbut and STEPbut == false) {
    drawSCAN();
    if (SCANpause) drawButton(L_SCAN, 1, B_JAM);
    drawSCANgraf(false);
    DrawSCANtxt(true);
  }
  if (PRESbut) {
    tft.fillRect(XFreqDispl, YFreqDispl + 20 , 239, 65, TFT_DARKCYAN);
    drawButton(L_THIRD, B_FM, B_SELECT);
    tft.setTextSize(1);
    tft.setTextDatum(BL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_DARKCYAN );
    tft.drawString(String(preset[currentPRES].presetIdx, 2) + " MHz ", 5, 83);
    if (!directScroll) {
      tftPlSetFont(T1516_T);
      tftPlSetSize(1);
      tftPlSetColor(TFT_WHITE, TFT_TRANS);
      tftPlSetDatum(BC_T);
      tftPlSetStyle(NBL_T);
      tftPlSetCut(0, 18);
      tftPlPrint(String(preset[currentPRES].PresetName), 120, 50);
      tftPlSetCut(0, 0);
    }
  }
  if (FirstLayer or ThirdLayer) VolumeIndicator(si4735.getVolume());
}

void loop() {

  unsigned long now = millis();
  if (((FirstLayer == true) or (ThirdLayer == true)) and (bright == false ) and (squelch == false )) VolumeIndicator(si4735.getVolume());
  if ((ThirdLayer == true) and (bright)) brightnessIndicator(currentBrightness);
  if ((ThirdLayer == true) and (squelch)) squelchIndicator(currentSquelch);

   if (!Mutestat) {
    si4735.getCurrentReceivedSignalQuality();
    if (SquelchUsesRSSI){
      SignalQuality = si4735.getCurrentRSSI();
    } else {
      SignalQuality = si4735.getCurrentSNR();
    }
    if (SignalQuality >= currentSquelch){
      if (SCANpause == true) {
        si4735.setAudioMute(audioMuteOff);
        squelchDecay = millis();
      }
    } else {
      if (millis() > (squelchDecay + squelchDecayTime)) {
        si4735.setAudioMute(audioMuteOn);
     }
    }
   }

  while (((pressed == false) and (encoderCount == 0) and (encoderButton == 0) and (analogRead(ENCODER_SWITCH) > 500)) or (writingEeprom)) {
    pressed = tft.getTouch(&x, &y);
    if (webServerRunning) webServer.handleClient();
	if (saverDisableOnScan and SCANbut) elapsedSaver = millis();
    if ((elapsedSaver + ((unsigned long)saverTime * 1000)) < millis() and (saverOn or displayOff)) saver();
    showtimeRSSI();
    if (batShow) Battery();
    if (screenLocked) drawLockIndicator();
    if (FirstLayer or ThirdLayer) drawVFOIndicator();
    if ((FirstLayer or ThirdLayer) and stopwatchRunning) {
      static unsigned long lastStopwatchDraw = 0;
      if (millis() - lastStopwatchDraw >= 1000) {
        lastStopwatchDraw = millis();
        drawStopwatch();
      }
    }
    if (priorityOn and (FirstLayer or ThirdLayer) and !screenLocked and !SCANbut and !MEMObut) checkPriorityChannel();
    if ((FirstLayer or ThirdLayer) and !SCANbut) updateFreqHistory();
    checkNightMode();
    if (PRESbut or RDSalways) DisplayRDS();
    if (PREtap and (elapsedPRE + 500) < millis()) {
      PREtap = false;
      FreqDispl();
    }
    if (SCANbut) {
      if (SCANpause) DisplaySCANsignal(); else DisplaySCAN();
    }
    if (RETRObut) {
      if (tftPlLength(String(presetNameLoad())) > (10 + (screenV * 8)) and (elapsedScroll + 200) < millis()) {
        textScroll++;
        if (textScroll == tftPlLength(String(presetNameLoad())) + 5) textScroll = 0;
        tftPlSetFont(T1516_T);
        tftPlSetSize(1);
        tftPlSetColor(TFT_OLIVE, TFT_BLACK);
        tftPlSetDatum(BL_T);
        tftPlSetStyle(NCB_T);
        tftPlSetCut(textScroll, 10 + (screenV * 8));
        tftPlPrint(String(presetNameLoad() + " "), 0, 18);
        tftPlSetCut(0, 0);
        if (textScroll) elapsedScroll = millis(); else elapsedScroll = millis() + 3000;
      }
    }
    if (scrollRetro) {
      int i = 0;
      while (i <= lastPreset and scrollRetro) {
        float freq = preset[i].presetIdx;
        if (RETROband < 2) freq = int(freq * 100);
        if ((scrollRetro == 1 and freq > currentRetroFreq and freq <= (currentRetroFreq + band[bandIdx].currentStep)) or
            (scrollRetro == -1 and freq < currentRetroFreq and freq >= (currentRetroFreq - band[bandIdx].currentStep))) {
          si4735.setFrequency(freq);
          scrollRetro = false;
        } else i++;
      }
      if (scrollRetro) {
        float tmpmax = bandRetro[RETROband].maximumFreq;
        float tmpmin = bandRetro[RETROband].minimumFreq;
        if (bandIdx == 0) {
          tmpmax *= 100;
          tmpmin *= 100;
        }
        if ((scrollRetro == 1 and (currentRetroFreq + band[bandIdx].currentStep) <= tmpmax) or (scrollRetro != 1 and (currentRetroFreq - band[bandIdx].currentStep) >= tmpmin)) {
          if (scrollRetro == 1) si4735.frequencyUp(); else si4735.frequencyDown();
        } else scrollRetro = false;
      }
      currentRetroFreq = si4735.getFrequency();
      if (RETROband < 2) bandRetro[RETROband].currentFreq = currentRetroFreq / 100; else bandRetro[RETROband].currentFreq = currentRetroFreq;
      drawRETROscale();
    }
    if (PRESbut) {
      if (directScroll and (elapsedScroll + 200) < millis()) {
        textScroll += directScroll;
        tftPlSetFont(T1516_T);
        tftPlSetSize(1);
        tftPlSetColor(TFT_WHITE, TFT_DARKCYAN);
        tftPlSetDatum(BC_T);
        tftPlSetStyle(NBL_T);
        tftPlSetCut(textScroll, 18);
        tftPlPrint(String(preset[currentPRES].PresetName), 120, 50);
        tftPlSetCut(0, 0);
        if (textScroll == (tftPlLength(String(preset[currentPRES].PresetName)) - 18)) {
          elapsedScroll = millis() + 3000;
          directScroll = -1;
        } else
        if (!textScroll) {
          elapsedScroll = millis() + 3000;
          directScroll = 1;
        } else elapsedScroll = millis();
      }
    }
    if (MEMOadd) {
      if (posMemoName == 20) {
        MEMOadd = false;
        MemoBank[currentMemo].freq = addMemoFreq;
        MemoBank[currentMemo].band = addMemoBand;
        for (int i = 0; i < 21; i++) MemoBank[currentMemo].Name[i] = addMemoName[i];
        drawButtons(L_MEMO);
        displMEMO();
      } else {
        int d = !screenV * 40;
        tftPlSetDatum(BL_T);
        tftPlSetFont(T1516_T);
        tftPlSetColor(TFT_WHITE, TFT_DARKCYAN);
        tftPlSetStyle(NBL_T);
        tftPlWidth = 12;
        if (!charMemoName) charMemoName = 32;
        tftPlPrint(String(char(charMemoName)),(posMemoName * 12) + d, 157);
        if ((elapsedCursor + 200) > millis()) {
          tft.fillRect((posMemoName * 12) + d, 154, 10, 3, TFT_WHITE);
        } else {
          if ((elapsedCursor + 400) < millis()) elapsedCursor = millis();
        }
        if (posMemoName) tftPlPrint(String(addMemoName[posMemoName - 1]),(posMemoName * 12) - 12 + d, 157);
        if (posMemoName < 19) tftPlPrint(String(addMemoName[posMemoName + 1]),(posMemoName * 12) + 12 + d, 157);
        addMemoName[posMemoName] = char(charMemoName);
      }
    }
    MuteAud();

  }
  elapsedSaver = millis();

  if (pressed and (FirstLayer or ThirdLayer) and x > 0 and x < 139 and y > 20 and y < 68) {
    unsigned long holdStart = millis();
    bool stillTouching = true;
    while (stillTouching and (millis() - holdStart) < 600) {
      stillTouching = tft.getTouch(&x, &y);
    }
    if (stillTouching) {
      screenLocked = !screenLocked;
      Beep(1, 0);
      drawLockIndicator();
    } else if (!screenLocked) {
      Beep(1, 0);
      showFreqHistory();
    }
    pressed = false; x = y = 0;
  }

  if (pressed and !screenLocked and (FirstLayer or ThirdLayer) and x > 220 and y > 20 and y < 68) {
    unsigned long holdStart = millis();
    bool stillTouching = true;
    while (stillTouching and (millis() - holdStart) < 600) {
      stillTouching = tft.getTouch(&x, &y);
    }
    if (stillTouching) {
      Beep(1, 0);
      swapVFO();
    }
    pressed = false; x = y = 0;
  }

  if (pressed and !screenLocked and (FirstLayer or ThirdLayer) and x > 139 and x < 219 and y > 20 and y < 68
      and currentMode != LSB and currentMode != USB and currentMode != CW and !STEPbut and !VOLbut and !bfoOn) {
    cycleStep();
    pressed = false; x = y = 0;
  }
  if (screenLocked) {
    pressed = false; x = y = 0;
    encoderCount = 0;
  }

  if (pressed and !screenLocked and (FirstLayer or ThirdLayer) and x > Xsmtr and x < (Xsmtr + 240) and y > (Ysmtr + 6) and y < (Ysmtr + 52)) {
    unsigned long holdStart = millis();
    bool stillTouching = true;
    while (stillTouching and (millis() - holdStart) < 600) {
      stillTouching = tft.getTouch(&x, &y);
    }
    Beep(1, 0);
    if (stillTouching) {
      priorityOn = !priorityOn;
      drawPriorityIndicator();
    } else {
      savePriorityTarget();
    }
    pressed = false; x = y = 0;
  }

  if (pressed and !screenLocked and (FirstLayer or ThirdLayer) and x > 163 and x < 225 and y > 58 and y < 68) {
    unsigned long holdStart = millis();
    bool stillTouching = true;
    while (stillTouching and (millis() - holdStart) < 600) {
      stillTouching = tft.getTouch(&x, &y);
    }
    Beep(1, 0);
    if (stillTouching) resetStopwatch(); else toggleStopwatch();
    pressed = false; x = y = 0;
  }

  encoderCheck();
  if (!screenLocked) encoderButtonCheck();

  boolean PRESoff = false;
  if (pressed) {
    pressed = false;

    if (scrollRetro) {
      scrollRetro = 0;
      x = y = 0;
    }

    if (FirstLayer) {

      if ((currentMode == LSB || currentMode == USB || currentMode == CW) and ( x > 139) and (x < 219) and (y > 25) and (y < 60)) {
        for (int n = 0 ; n <= lastdignum ; n++) {
          if ((x > (dn[n].Xdignumos) + (dn[n].Xdignumnr)) and (x < ((dn[n].Xdignumos) + (dn[n].Xdignumsr) + (dn[n].Xdignumnr))) and (y > (dn[n].Ydignumos) and (y < ((dn[n].Ydignumos) + (dn[n].Ydignumsr) )))) {
            Beep(1, 0);
            delay(200);
            if (bfoOn == false) {
              freqstepnr = n;
              if (freqstepnr == 0)  freqstep = 1000;
              if (freqstepnr == 1)  freqstep = 100;
              if (freqstepnr == 2)  freqstep = 10;
              FreqDispl();
            } else {
              if (n == 1) stepsizesynth = 10;
              if (n == 2) stepsizesynth = 1;
              FreqDispl();
            }
          }
        }
        x = y = 0;
      }

      if (( x > XVolInd + 161) and (x < XVolInd + 237) and (y > YVolInd - 1) and (y < YVolInd + 26)) {
        Beep(1, 0);
        delay(200);
        batVolt = !batVolt;
        elapsedBat = 0;
      }

      if (x < (150 + (bfoOn * 80)) and y > 25 and y < 60 and !VOLbut and !AGCgainbut and !SQUELCHbut) {
        Beep(1, 0);
        delay(200);
        if (PREtap) {
          Beep(1, 0);
          PREtap = false;
          uint16_t  tmpFreq = PREfreq;
          uint8_t   tmpBand = PREband;
          uint8_t   tmpMode = PREmode;
          int       tmpBfo  = PREbfo;
          uint8_t   tmpStep = PREstep;
          uint8_t   tmpBw   = PREbw;

          PREfreq = si4735.getFrequency();
          PREband = bandIdx;
          PREmode = currentMode;
          if (currentMode == LSB or currentMode == USB or currentMode == CW) PREbfo  = currentBFOmanu; else PREbfo  = 0;
          PREstep = band[bandIdx].currentStep;
          if (currentMode == AM) PREbw = bwIdxAM; else if (currentMode == FM) PREbw = bwIdxFM; else PREbw = bwIdxSSB;

          if (PRE) {
            bandIdx = tmpBand;
            si4735.setFrequency(tmpFreq);
            band[bandIdx].currentFreq = tmpFreq;
            currentMode = tmpMode;
            currentBFOmanu = tmpBfo;
            band[bandIdx].currentStep = tmpStep;
            si4735.setFrequencyStep(tmpStep);
            if (((band[bandIdx].bandType == MW_BAND_TYPE) or (band[bandIdx].bandType == LW_BAND_TYPE)) and ( currentMode == AM)) ssIdxMW = tmpStep;
            if ((band[bandIdx].bandType == SW_BAND_TYPE) and ( currentMode == AM)) ssIdxAM = tmpStep;
            if (currentMode == FM) ssIdxFM = tmpStep;
            if (currentMode == AM) bwIdxAM = tmpBw; else if (currentMode == FM) bwIdxFM = tmpBw; else bwIdxSSB = tmpBw;
            BandSet();
          } else PRE = true;
          DrawDispl();
          delay(200);
        } else {
          PREtap = true;
          elapsedPRE = millis();
          FreqDispl();
        }
      }

        int n = jamButton(L_FIRST);
        if (n >= 0) {
          if ((VOLbut) and (n != B_VOL)) {
            VOLbut = false;
            drawButton(L_FIRST, B_VOL, B_NORMAL);
            DrawDispl ();
          }

          if (AGCgainbut and (n != B_ATT)) {
            AGCgainbut = false;
            drawButton(L_FIRST, B_ATT, B_NORMAL);
            FreqDispl();
          }

          if (n == B_ATT) {
            if (AGCgainbut) AGCgainbut = false;
            else {
              AGCgainbut = true;
              si4735.getAutomaticGainControl();
              previousAGCgain = 38;
            }
            FreqDispl();
            if (AGCgainbut) drawButton(L_FIRST, B_ATT, B_SELECT); else drawButton(L_FIRST, B_ATT, B_NORMAL);
            drawButton(L_FIRST, B_AGC, B_NORMAL);
          }

          if (n == B_AGC) {
            if  (AGCgain == 1) {
              AGCgain = 0;
              drawButton(L_FIRST, B_AGC, B_NORMAL);
            } else AGCgain = 1;
            checkAGC();
            AGCfreqdisp();
          }

          if (n == B_HAM) {
            HamBand = true;
            drawList(L_HAM, "HAM RADIO BAND");
            FirstLayer = false;
            SecondLayer = true;
          }

          if (n == B_BFO) {
            if (currentMode == LSB || currentMode == USB || currentMode == CW)  {
              if (bfoOn) bfoOn = false; else bfoOn = true;
              if (bfoOn) {
                drawButton(L_FIRST, B_BFO, B_SELECT);
                drawButton(L_FIRST, B_STEP, B_NORMAL);
             } else {
                drawButton(L_FIRST, B_BFO, B_NORMAL);
                drawButton(L_FIRST, B_STEP, B_BLOCK);
              }
              bfoTr = true;
              DrawDispl ();
            } else ErrorBeep();
          }

          if (n == B_FREQ) {
            FREQbut = true;
            drawList(L_FREQ, "CZĘSTOTLIWOŚĆ");
            tft.fillRect(0, 80, 240, 40, TFT_NAVY);
            Decipoint = false;
            DisplayfreqNew = 0;
            dpfrq = 0;
            Freqcalq(0);
            FirstLayer = false;
            SecondLayer = true;
          }

          if (n == B_MODE) {
            if (currentMode != FM)  {
              Modebut = true;
              drawList(L_MODE, "MODULACJA");
              FirstLayer = false;
              SecondLayer = true;
            } else ErrorBeep();
          }

          if (n == B_BANDW) {
            BandWidth = true;
            if (currentMode == AM) drawList(L_BANDW_AM, "Filtr AM w KHz");
            else if (currentMode == FM) drawList(L_BANDW_FM, "Filtr FM w KHz");
            else drawList(L_BANDW_SSB, "Filtr SSB w KHz");
            FirstLayer = false;
            SecondLayer = true;
          }

          if (n == B_STEP) {
            if (bfoOn) {
              drawButton(L_FIRST, B_STEP, B_NORMAL);
              setStep();
            } else if (currentMode == LSB || currentMode == USB || currentMode == CW) {
              ErrorBeep();
            } else {
              STEPbut = true;
              FirstLayer = false;
              SecondLayer = true;
              if (currentMode == AM) drawList(L_STEP_AM, "KROK w AM"); else drawList(L_STEP_FM, "KROK w FM");
            }
          }

          if (n == B_BAND)  {
            BroadBand = true;
            drawList(L_BAND, "BAND");
            FirstLayer = false;
            SecondLayer = true;
          }

          if (n == B_VOL) {
            if (bfoOn) {
              bfoOn = false;
              drawButton(L_FIRST, B_BFO, B_NORMAL);
            }
            if (VOLbut == false) {
              VOLbut = true;
              currentVOL = si4735.getVolume();
              previousVOL = currentVOL;
            } else {
              VOLbut = false;
            }
            FreqDispl();
            if (VOLbut) drawButton(L_FIRST, B_VOL, B_SELECT); else drawButton(L_FIRST, B_VOL, B_NORMAL);
          }

          if (n == B_MUTE) {
            if (Mutestat) Mutestat = false; else Mutestat = true;
            if (!Mutestat) drawButton(L_FIRST, B_MUTE, B_NORMAL);
            if (Mutestat) si4735.setAudioMute(audioMuteOn); else si4735.setAudioMute(audioMuteOff);
          }

          if (n == B_NEXT) {
            FirstLayer  = false;
            SecondLayer = false;
            ThirdLayer  = true;
            ForthLayer  = false;
            DrawThla();
          }
        }
    }

    if (SecondLayer) {
      if (Modebut) {
        if (x > 20 and x < 220 and y > 20 and y < 60 and !VOLbut) {
          Modebut = false;
          DrawFila();
          delay(400);
        } else {
          int n = jamButton(L_MODE);
          if (n >= 0) {
            currentMode = n;
            drawListBut(L_MODE);
            delay(400);
            if ((CWShift == true) and (previousMode == USB)  ) {
              currentBFO = currentBFO - 700;
              band[bandIdx].lastBFO = currentBFO;
              freqDec = currentBFO;
              CWShift = false;
            }
            if ((currentMode !=  previousMode) and (currentMode == CW) and (CWShift == false)) {
              currentMode = USB;
              CWShift = true;
              currentBFO = currentBFO + 700;
              band[bandIdx].lastBFO = currentBFO;
              freqDec = currentBFO;
            }
            Modebut = false;
            previousMode = currentMode;
            band[bandIdx].prefmod = currentMode;
            BandSet();
            DrawFila();
          }
        }
      }

      if (BandWidth) {
        if (x > 20 and x < 220 and y > 20 and y < 60 and !VOLbut) {
          BandWidth = false;
          DrawFila();
          delay(200);
        } else {
          int b = L_BANDW_SSB;
          if (currentMode == AM) b = L_BANDW_AM;
          else if (currentMode == FM) b = L_BANDW_FM;
          int n = jamButton(b);
          if (n >= 0) {
            if (b == L_BANDW_AM) bwIdxAM = n;
            else if (b == L_BANDW_FM) bwIdxFM = n;
            else bwIdxSSB = n;
            drawListBut(b);
            delay(400);
            BandWidth = false;
            BandSet();
            DrawFila();
          }
        }
      }

      if (cityRETRObut) {
        Beep(1, 0);
        x = 0;
        y = 0;

        cityRETRObut = false;
        RETRObut = true;
        if (prevPresetId != PresetId) {
          prevPresetId = PresetId;
          currentPRES = 0;
          presetLoad();
          presetSort();
          presetSetPos();
        }
        drawRETRO();
        currentRetroFreq = 0;
      }

      if (bandRETRObut) {

        for (int n = 0 ; n <= lastBandRetro; n++) {
          if ((screenV and x > bandRetro[n].xPosV and x < (bandRetro[n].xPosV + But_Width) and y > bandRetro[n].yPosV and y < (bandRetro[n].yPosV + But_Height)) or
              (!screenV and x > bandRetro[n].xPosH and x < (bandRetro[n].xPosH + But_Width) and y > bandRetro[n].yPosH and y < (bandRetro[n].yPosH + But_Height))) {
            Beep(1, 0);
            x = 0;
            y = 0;

            if (VHFon or n != 1) {
              if (n != RETROband) {
                RETROband = n;
                drawRetroBandBut();
                delay(200);
                presetSetPos();
                initRetro();
              } else drawRETRO();
              bandRETRObut = false;
              RETRObut = true;
              currentRetroFreq = 0;
            } else ErrorBeep();
          }
        }
      }

      if (RETRObut) {
        if (y > 40 and y < 200) {
          Beep(1, 0);
          x = 0;
          y = 0;

          if (bandRetro[RETROband].hardStep != bandRetro[RETROband].softStep or RETROband > 3) {
            if (bandHamRetro) {
              if (bfoOn) bfoOn = false; else bfoOn = true;
              if (VOLbut) {
                VOLbut = false;
                drawButton(L_RETRO, 2, B_NORMAL);
              }
            } else {
              if (band[bandIdx].currentStep == bandRetro[RETROband].hardStep) {
                si4735.setFrequencyStep(bandRetro[RETROband].softStep);
                band[bandIdx].currentStep = bandRetro[RETROband].softStep;
              } else {
                if (RETROband > 3 and band[bandIdx].currentStep == bandRetro[RETROband].softStep) {
                  si4735.setFrequencyStep(10);
                  band[bandIdx].currentStep = 10;
                } else {
                  si4735.setFrequencyStep(bandRetro[RETROband].hardStep);
                  band[bandIdx].currentStep = bandRetro[RETROband].hardStep;
                }
              }
            }
            drawRETROscale();
          } else ErrorBeep();
        }

          int n = jamButton(L_RETRO);
          if (n >= 0) {
            if (n == 0) {
              cityRETRObut = true;
              RETRObut = false;
              VOLbut = false;
              prevPresetId = PresetId;
              drawRetroCity();
            }

            if (n == 1) {
              bandRETRObut = true;
              RETRObut = false;
              VOLbut = false;
              drawRetroBand();
            }

            if (n == 2) {
              if (bfoOn) {
                bfoOn = false;
                drawRETROscale();
              }
              if (VOLbut) VOLbut = false; else VOLbut = true;
              if (VOLbut) drawButton(L_RETRO, 2, B_SELECT); else drawButton(L_RETRO, 2, B_NORMAL);
              delay(100);
            }

            if (n == 3) {
              band[bandIdx].currentFreq = si4735.getFrequency();
              RETRObut = false;
              Beep(1, 0);
              DrawThla();
            }
          }
      }

      if (MEMObut) {
       if (x > 20 and x < 220 and y > 20 and y < 60) {
          MEMObut = false;
          DrawThla();
          delay(200);
       } else {
        int d = !screenV * 40;
        if (x > d and x < (240 + d) and y > 80 and y < (240 - d)) {
          if ((int(y / 40) - 3 + currentMemo) >= 0 and (int(y / 40) - 3 + currentMemo) <= lastMemoBank) {
            tftTransRect(d, int(y / 40) * 40, 240, 40, TFT_OLIVE);
            if (presetBank) {
              int tmpBand = bandFreq(preset[int(y / 40) - 3 + currentMemo].presetIdx);
              if (tmpBand == 29 and (preset[int(y / 40) - 3 + currentMemo].presetIdx < 153 or preset[int(y / 40) - 3 + currentMemo].presetIdx > 30000)) {
                ErrorBeep();
              } else {
                Beep(1, 0);
                if (bandIdx != tmpBand or currentMode != bandMode[tmpBand]) {
                  bandIdx = tmpBand;
                  currentMode = bandMode[tmpBand];
                  BandSet();
                }
                if (tmpBand) si4735.setFrequency(preset[int(y / 40) - 3 + currentMemo].presetIdx); else si4735.setFrequency(preset[int(y / 40) - 3 + currentMemo].presetIdx * 100);
                band[bandIdx].currentFreq = si4735.getFrequency();
                FreqDispl();
              }
            } else {
              if (MemoBank[int(y / 40) - 3 + currentMemo].freq < 153 or MemoBank[int(y / 40) - 3 + currentMemo].freq > 30000) {
                ErrorBeep();
              } else {
                Beep(1, 0);
                if (bandIdx != (MemoBank[int(y / 40) - 3 + currentMemo].band & 0x1F) or currentMode != trunc(MemoBank[int(y / 40) - 3 + currentMemo].band / 32)) {
                  bandIdx = MemoBank[int(y / 40) - 3 + currentMemo].band & 0x1F;
                  currentMode = trunc(MemoBank[int(y / 40) - 3 + currentMemo].band / 32);
                  BandSet();
                }
                si4735.setFrequency(MemoBank[int(y / 40) - 3 + currentMemo].freq);
                band[bandIdx].currentFreq = MemoBank[int(y / 40) - 3 + currentMemo].freq;
                FreqDispl();
              }
            }
            displMEMO();
          }
        }

          int n = jamButton(L_MEMO);
          if (n >= 0) {
            if (n == 0 and !presetBank) {
              if (MEMOadd or MEMOdel) {
                drawButton(L_MEMO, 0, B_NORMAL);
                drawButton(L_MEMO, 3, B_NORMAL);
                if (MEMOadd) {
                  MEMOadd = false;
                  drawButton(L_MEMO, 1, B_NORMAL);
                  MemoBank[currentMemo].freq = addMemoFreq;
                  MemoBank[currentMemo].band = addMemoBand + (addMemoMode * 32);
                  for (int i = 0; i < 21; i++) MemoBank[currentMemo].Name[i] = addMemoName[i];
                } else {
                  MEMOdel = false;
                  drawButton(L_MEMO, 2, B_NORMAL);
                  drawButton(L_MEMO, 1, B_NORMAL);
                  MemoBank[currentMemo].freq = 0;
                }
                displMEMO();
              } else {
                if (MemoBank[currentMemo].freq < 153 or MemoBank[currentMemo].freq > 30000) {
                  drawButton(L_MEMO, 0, B_NORMAL);
                  ErrorBeep();
                } else {
                  MEMOadd = true;
                  drawButton(L_MEMO, 0, B_NORMAL, "OK");
                  drawButton(L_MEMO, 3, B_NORMAL, "X");
                  drawButton(L_MEMO, 1, B_SELECT);
                  addMemoFreq = MemoBank[currentMemo].freq;
                  addMemoBand = MemoBank[currentMemo].band & 0x1F;
                  addMemoMode = trunc(MemoBank[currentMemo].band / 32);
                  for (int i = 0; i < 21; i++) addMemoName[i] = MemoBank[currentMemo].Name[i];
                  posMemoName = 0;
                  charMemoName = addMemoName[0];
                  displMEMO();
                }
              }
            }

            if (n == 1 and !presetBank) {
              if (!MEMOadd and !MEMOdel) {
                MEMOadd = true;
                drawButton(L_MEMO, 0, B_NORMAL, "OK");
                drawButton(L_MEMO, 3, B_NORMAL, "X");
                drawButton(L_MEMO, 1, B_SELECT);
                for (int i = 0; i < 21; i++) addMemoName[i] = char(32);
                addMemoFreq = uint16_t(si4735.getFrequency());
                addMemoBand = bandIdx;
                addMemoMode = currentMode;
                posMemoName = 0;
                charMemoName = 32;
                displMEMO();
              } else {
                ErrorBeep();
                if (MEMOadd) drawButton(L_MEMO, 1, B_SELECT);
              }
            }

            if (n == 2 and !presetBank) {
              if (!MEMOadd and !MEMOdel) {
                MEMOdel = true;
                drawButton(L_MEMO, 0, B_NORMAL, "OK");
                drawButton(L_MEMO, 3, B_NORMAL, "X");
                drawButton(L_MEMO, 1, B_BLOCK);
                drawButton(L_MEMO, 2, B_SELECT);
                displMEMO();
                if (!MEMOdel) {
                  displMEMO();
                  drawButtons(L_MEMO);
                }
              } else if (MEMOadd) {
                if (charMemoName == 32 and posMemoName) {
                  posMemoName--;
                } else {
                  charMemoName = 32;
                }
                drawButton(L_MEMO, 2, B_NORMAL);
              } else {
                ErrorBeep();
                drawButton(L_MEMO, 2, B_SELECT);
              }
            }

            if (n == 3) {
              if (MEMOadd or MEMOdel) {
                drawButtons(L_MEMO);
                MEMOadd = false;
                MEMOdel = false;
                displMEMO();
              } else {
                MEMObut = false;
                Beep(1, 0);
                DrawThla();
              }
            }
          }
        }
      }

      if (SETUPbut) {
        if (x > (!screenV * 5) and x < (240 + (!screenV * 5)) and y > 40 and y < 200) {
          Beep(1, 0);
          changeSETUP(int((y - 20) / 32));
          displSETUP();
        }

          int n = jamButton(L_SETUP);
          if (n >= 0) {
            if (n == 0) {
              if (!pageSetup) {
                ErrorBeep();

              } else {
                pageSetup--;
                displSETUP();

              }
              drawButtons(L_SETUP);
              if (!pageSetup) drawButton(L_SETUP, 0, B_BLOCK);
              if (pageSetup == maxPageSetup) drawButton(L_SETUP, 1, B_BLOCK);
            }

            if (n == 1) {
              if (pageSetup == maxPageSetup) {
                ErrorBeep();

              } else {
                pageSetup++;
                displSETUP();

              }
              drawButtons(L_SETUP);
              if (!pageSetup) drawButton(L_SETUP, 0, B_BLOCK);
              if (pageSetup == maxPageSetup) drawButton(L_SETUP, 1, B_BLOCK);
            }

            if (n == 2) {
              defaultSETUP();
              displSETUP();
              drawButtons(L_SETUP);
              if (!pageSetup) drawButton(L_SETUP, 0, B_BLOCK);
              if (pageSetup == maxPageSetup) drawButton(L_SETUP, 1, B_BLOCK);
            }

            if (n == 3) {
              SETUPbut = false;
              saveSETUP();
              if (!SETUPbut) {
                DrawThla();
                Beep(1, 0);
              }
            }
          }
      }

      if (SCANbut and STEPbut == false) {
        int d = screenV * 40;
        if (x > 20 and x < 220 and y > 20 and y < 60 and !VOLbut) {
          SCANstep = 0;
          SCANpause = true;
          pauseSCAN();
          if (currentMode == LSB or currentMode == USB or currentMode == CW) band[bandIdx].lastBFO = currentBFO = 0;
          if (Mutestat) si4735.setAudioMute(audioMuteOn);
          SCANbut = false;
          Beep(1, 0);
          DrawThla();
          x = y = 0;
          delay(200);
        }
        if (y > 80 and y < 200) {
          Beep(1, 0);
          if (SCANpause) {

            float tmpdelta = deltaScanLine;
            if (x < 40 and (currentScanFreq + int((deltaScanLine - 159 + d) * SCANstep)) > band[bandIdx].minimumFreq) deltaScanLine -= (40 - x);
            if (x > (280 - (d * 2)) and (currentScanFreq + int((deltaScanLine + 160 - d) * SCANstep)) < band[bandIdx].maximumFreq) deltaScanLine += (x - 280 + (d * 2));
            float tmpfreq = currentScanFreq + int((x - 159 + d + deltaScanLine) * SCANstep);
            int tmpline = currentScanLine;
            if (tmpfreq <= band[bandIdx].maximumFreq and tmpfreq >= band[bandIdx].minimumFreq) {
              currentScanLine = x;
            } else {
              if (tmpdelta != deltaScanLine) {
                if (tmpfreq > band[bandIdx].maximumFreq) {
                  deltaScanLine -= ((tmpfreq - band[bandIdx].maximumFreq) / SCANstep);
                } else {
                  deltaScanLine += ((band[bandIdx].minimumFreq - tmpfreq) / SCANstep);
                }
                tmpfreq = currentScanFreq + int((x - 159 + d + deltaScanLine) * SCANstep);
                currentScanLine = x;
              }
            }
            if (tmpline != currentScanLine or tmpdelta != deltaScanLine) {
              setFreq(tmpfreq);
              if (currentMode == LSB or currentMode == USB or currentMode == CW) band[bandIdx].lastBFO = currentBFO = 0;
              if (tmpdelta == deltaScanLine) {
                if (currentScanLine < tmpline) currentScanLine -= 10;
                drawSCANline(tmpline);
                if (currentScanLine < tmpline) currentScanLine += 10;
                drawSCANline(currentScanLine);
              } else {
                tmpdelta = abs(tmpdelta - deltaScanLine);
                if (x < 40) {
                  for (int i = 319 - (d * 2); i > tmpdelta - 1; i--) {
                    ScanValueRSSI[i] = ScanValueRSSI[i - int(tmpdelta)];
                    ScanValueSNR[i] = ScanValueSNR[i - int(tmpdelta)];
                  }
                  for (int i = 0; i < tmpdelta; i++) {
                    ScanValueRSSI[i] = 198 + d;
                    ScanValueSNR[i] = 0;
                  }
                } else {
                  for (int i = tmpdelta; i < (320 - (d * 2)); i++) {
                    ScanValueRSSI[i - int(tmpdelta)] = ScanValueRSSI[i];
                    ScanValueSNR[i - int(tmpdelta)] = ScanValueSNR[i];
                  }
                  for (int i = 320 - (d * 2) - tmpdelta; i < (320 - (d * 2)); i++) {
                    ScanValueRSSI[i] = 198 + d;
                    ScanValueSNR[i] = 0;
                  }
                }
                ScanEmpty = true;
                for (int i = 0; i < 320; i++) ScanMark[i] = false;
                drawSCANgraf(false);
                posScanFreq = currentScanFreq + int((deltaScanLine - 159 + d) * SCANstep);
                posScan = 0;
              }
              FreqDispl();
              DrawSCANtxt(true);
              DisplaySCANsignal();
              DrawSCANind();
            }
          } else {

            if (ScanEmpty) {
              ErrorBeep();
            } else {
              Beep(1, 0);
              int tmpMax = 198 + d;
              float tmpMid = 0;
              for (int i = 0; i < (320 - (d * 2)); i++) {
                tmpMid += 198 + d - ScanValueRSSI[i];
                if (ScanValueRSSI[i] < tmpMax) tmpMax = ScanValueRSSI[i];
              }
              tmpMid = (140 + (d / 2)) / (float(tmpMid) / (320 - (d * 2)));
              if ((198 + d - ((198 + d - tmpMax) * tmpMid)) < (100 + (d / 2))) tmpMid = (98 + (d / 2)) / float(198 + d - tmpMax);
              if (tmpMid != 1 and tmpMid > 0) {
                if ((signalScale * tmpMid) > 10) tmpMid = 10 / signalScale;
                signalScale *= tmpMid;
                for (int i = 0; i < (320 - (d * 2)); i++) ScanValueRSSI[i] = 198 + d - ((198 + d - ScanValueRSSI[i]) * tmpMid);
                drawSCANgraf(false);
                DrawSCANtxt(true);
              }
            }
          }
        }

          int n = jamButton(L_SCAN);
          if (n >= 0) {
            if (n == 0) {
              drawButton(L_SCAN, 0, B_NORMAL);
              deltaScanLine += currentScanLine - 159 + d;
              currentScanLine = 159 - d;
              SCANstep *= 2;
              if (SCANstep > currentMaxScanStep) SCANstep = currentMinScanStep;
              if (SCANstep == currentMinScanStep) deltaScanLine *= (currentMaxScanStep / currentMinScanStep); else deltaScanLine /= 2;
              SCANpause = false;
              pauseSCAN();
              drawSCANgraf(true);
              DrawSCANtxt(true);
            }

            if (n == 1) {
              SCANpause = !SCANpause;
              pauseSCAN();
            }

            if (n == 2) {
              if (currentMode == LSB or currentMode == USB or currentMode == CW) {
                if (bfoOn) setStep(); else ErrorBeep();
                drawButton(L_SCAN, 2, B_NORMAL);
              } else {
                SCANpause = true;
                pauseSCAN();
                STEPbut = true;
                if (currentMode == AM) drawList(L_STEP_AM, "KROK w AM"); else drawList(L_STEP_FM, "KROK w FM");
              }
            }

            if (n == 3) {
              SCANpause = true;
              pauseSCAN();
              band[bandIdx].currentFreq = si4735.getFrequency();
              if (currentMode == LSB or currentMode == USB or currentMode == CW) band[bandIdx].lastBFO = currentBFO = 0;
              if (Mutestat) si4735.setAudioMute(audioMuteOn);
              SCANbut = false;
              Beep(1, 0);
              DrawThla();
            }
          }
      }

      if (STEPbut) {
        if (x > 20 and x < 220 and y > 20 and y < 60 and !VOLbut) {
          STEPbut = false;
          if (SCANbut) {
            SCANpause = false;
            pauseSCAN();
            drawSCAN();
            drawSCANgraf(false);
            DrawSCANtxt(true);
            x = y = 0;
          } else DrawFila();
          delay(200);
        } else {
          int b = L_STEP_AM;
          if (currentMode == FM) b = L_STEP_FM;
          int n = jamButton(b);
          if (n >= 0) {
            if (band[bandIdx].bandType == MW_BAND_TYPE or band[bandIdx].bandType == LW_BAND_TYPE) {
              ssIdxMW = n;
              si4735.setFrequencyStep(ssIdxMW);
              band[bandIdx].currentStep = ssIdxMW;
            } else if (currentMode == FM) {
              ssIdxFM = n;
              si4735.setFrequencyStep(ssIdxFM);
              band[bandIdx].currentStep = ssIdxFM;
            } else {
              ssIdxAM = n;
              si4735.setFrequencyStep(ssIdxAM);
              band[bandIdx].currentStep = ssIdxAM;
            }
            setStep();
            drawListBut(b);
            delay(400);
            STEPbut = false;
            if (SCANbut) {
              drawSCAN();
              SCANpause = false;
              pauseSCAN();
              drawSCANgraf(false);
              DrawSCANtxt(true);
            } else DrawFila();
          }
        }
      }

      if (BroadBand) {
        if (CWShift == true)  {
          currentBFO = currentBFO - 700;
          band[bandIdx].lastBFO = currentBFO;
          CWShift = false;
        }
#ifdef IhaveCrystal
        band[bandIdx].lastmanuBFO = currentBFOmanu;
#endif

        if (x > 20 and x < 220 and y > 20 and y < 60 and !VOLbut) {
          BroadBand = false;
          DrawFila();
          delay(200);
        } else {
          int n = jamButton(L_BAND);
          if (n >= 0) {
            bandIdx = n;
            drawListBut(L_BAND);
            delay(400);
            BroadBand = false;
            if (bandIdx == 0 and currentAGCgain > 26) currentAGCgain = previousAGCgain = 26;
            si4735.setAM();
            delay(50);
            currentBFO = band[bandIdx].lastBFO;
            freqDec = currentBFO;
            currentMode = band[bandIdx].prefmod;
#ifdef IhaveCrystal
            currentBFOmanu = band[bandIdx].lastmanuBFO;
#endif
            ssbLoaded = false;
            BandSet();
            DrawFila();
          }
        }
      }

      if (HamBand) {
        if (CWShift == true)  {
          currentBFO = currentBFO - 700;
          band[bandIdx].lastBFO = currentBFO;
          CWShift = false;
        }
#ifdef IhaveCrystal
        band[bandIdx].lastmanuBFO = currentBFOmanu;
#endif

        if (x > 20 and x < 220 and y > 20 and y < 60 and !VOLbut) {
          HamBand = false;
          DrawFila();
          delay(200);
        } else {
          int n = jamButton(L_HAM);
          if (n >= 0) {
            bandIdx = n;
            drawListBut(L_HAM);
            delay(400);
            HamBand = false;
            if (ssbLoaded == false) {
              si4735.setAM();
              delay(50);
            }
#ifdef IhaveCrystal
            currentBFOmanu = band[bandIdx].lastmanuBFO;
#endif
            currentBFO = band[bandIdx].lastBFO;
            freqDec = currentBFO;
            currentMode = band[bandIdx].prefmod;
            BandSet();
            DrawFila();
          }
        }
      }

      if (FREQbut) {
        if (CWShift == true)  {
          currentBFO = currentBFO - 700;
          band[bandIdx].lastBFO = currentBFO;
          CWShift = false;
        }
#ifdef IhaveCrystal
        band[bandIdx].lastmanuBFO = currentBFOmanu;
#endif

        if (x > 20 and x < 220 and y > 20 and y < 60 and !VOLbut) {
          FREQbut = false;
          DrawFila();
          delay(200);
        } else {
          int n = jamButton(L_FREQ);
          if (n >= 0) {
           if (n != 9 or (!Decipoint and DisplayfreqNew >= (band[0].minimumFreq / 10) and DisplayfreqNew <= (band[0].maximumFreq / 10))) {
            drawButton(L_FREQ, n, B_NORMAL);

            if ((n >= 0) and (n <= 8)) Freqcalq(n + 1);
            if (n == 10) Freqcalq(0);
            if (n == 9 and Decipoint == false and DisplayfreqNew >= (band[0].minimumFreq / 10) and DisplayfreqNew <= (band[0].maximumFreq / 10)) {
              Decipoint = true;
              fact = 10;
              Freqcalq(10);
            }
            if (n > 11 and n < 14) Freqcalq(n);
            if (n == 14) {
              FREQbut = false;
              DrawFila();
            }
            if (n == 11) {
                FREQbut = false;
                DisplayfreqNew = (DisplayfreqNew / 10) + dpfrq;
                if ((DisplayfreqNew > 30 and DisplayfreqNew < (band[0].minimumFreq / 100)) or (DisplayfreqNew > 108 and DisplayfreqNew < 153 ) or DisplayfreqNew == 0) {
                  tft.setTextSize(2);
                  tft.setTextColor(TFT_WHITE, TFT_RED);
                  tft.setCursor(0, 97);
                  tft.print("Freqency not support");
                  ErrorBeep();
                } else {
                    if ((DisplayfreqNew >= (band[0].minimumFreq / 100)) and (DisplayfreqNew <= (band[0].maximumFreq / 100))) {

                      currentFrequency = DisplayfreqNew * 100;
                      bandIdx = 0;
                      band[bandIdx].currentFreq = currentFrequency;
                    } else {

                      if (DisplayfreqNew < 153) currentFrequency = DisplayfreqNew * 1000; else currentFrequency = DisplayfreqNew;
                      for (int q = 1 ; q <= lastBand; q++) {
                        if (((currentFrequency) >= band[q].minimumFreq) and ((currentFrequency) <= band[q].maximumFreq)) {
                          bandIdx = q;
                          currentMode = band[q].prefmod;
                          if (((band[bandIdx].bandType == MW_BAND_TYPE) or (band[bandIdx].bandType == LW_BAND_TYPE)) and ( currentMode == AM)) {
                            ssIdxMW = band[bandIdx].currentStep;
                          }

                          if ((band[bandIdx].bandType == SW_BAND_TYPE) and ( currentMode == AM)) {
                            ssIdxAM = band[bandIdx].currentStep;
                          }

                          if (currentMode == FM) {
                            ssIdxFM = band[bandIdx].currentStep;
                          }
                          break;
                        }
                      }
                      delay(100);
                      band[bandIdx].currentFreq = currentFrequency;
                      freqDec = currentBFO = band[bandIdx].lastBFO = 0;
                    }
                }
#ifdef IhaveCrystal
              currentBFOmanu = band[bandIdx].lastmanuBFO;
#endif
              BandSet();
              DrawFila();
            }
           }
          }
        }
      }

      if (VOLbut and !RETRObut) {
        VOLbut = false;
        FreqDispl();
        if (SCANbut) {
          DrawSCANind();
          DisplaySCANsignal();
        }
      }

    }

    if (ThirdLayer) {
      if ((currentMode == LSB || currentMode == USB || currentMode == CW) and ( x > 139) and (x < 219) and (y > 25) and (y < 60)) {
        for (int n = 0 ; n <= lastdignum ; n++) {
          if ((x > (dn[n].Xdignumos) + (dn[n].Xdignumnr)) and (x < ((dn[n].Xdignumos) + (dn[n].Xdignumsr) + (dn[n].Xdignumnr))) and (y > (dn[n].Ydignumos) and (y < ((dn[n].Ydignumos) + (dn[n].Ydignumsr) )))) {
            Beep(1, 0);
            delay(200);
            if (bfoOn == false) {
              freqstepnr = n;
              if (freqstepnr == 0)  freqstep = 1000;
              if (freqstepnr == 1)  freqstep = 100;
              if (freqstepnr == 2)  freqstep = 10;
              FreqDispl();
            } else {
              if (n == 1) stepsizesynth = 10;
              if (n == 2) stepsizesynth = 1;
              FreqDispl();
            }
          }
        }
        x = y = 0;
      }
      if (( x > XVolInd + 161) and (x < XVolInd + 237) and (y > YVolInd - 1) and (y < YVolInd + 26)) {
        Beep(1, 0);
        delay(200);
        batVolt = !batVolt;
        elapsedBat = 0;
      }
      if (x < (150 + (bfoOn * 80)) and y > 25 and y < 60 and !VOLbut and !PRESbut) {
        Beep(1, 0);
        delay(200);
        if (PREtap) {
          Beep(1, 0);
          PREtap = false;
          uint16_t  tmpFreq = PREfreq;
          uint8_t   tmpBand = PREband;
          uint8_t   tmpMode = PREmode;
          int       tmpBfo  = PREbfo;
          uint8_t   tmpStep = PREstep;
          uint8_t   tmpBw   = PREbw;

          PREfreq = si4735.getFrequency();
          PREband = bandIdx;
          PREmode = currentMode;
          if (currentMode == LSB or currentMode == USB or currentMode == CW) PREbfo  = currentBFOmanu; else PREbfo  = 0;
          PREstep = band[bandIdx].currentStep;
          if (currentMode == AM) PREbw = bwIdxAM; else if (currentMode == FM) PREbw = bwIdxFM; else PREbw = bwIdxSSB;

          if (PRE) {
            bandIdx = tmpBand;
            si4735.setFrequency(tmpFreq);
            band[bandIdx].currentFreq = tmpFreq;
            currentMode = tmpMode;
            currentBFOmanu = tmpBfo;
            band[bandIdx].currentStep = tmpStep;
            si4735.setFrequencyStep(tmpStep);
            if (((band[bandIdx].bandType == MW_BAND_TYPE) or (band[bandIdx].bandType == LW_BAND_TYPE)) and ( currentMode == AM)) ssIdxMW = tmpStep;
            if ((band[bandIdx].bandType == SW_BAND_TYPE) and ( currentMode == AM)) ssIdxAM = tmpStep;
            if (currentMode == FM) ssIdxFM = tmpStep;
            if (currentMode == AM) bwIdxAM = tmpBw; else if (currentMode == FM) bwIdxFM = tmpBw; else bwIdxSSB = tmpBw;
            BandSet();
          } else PRE = true;
          DrawDispl();
          delay(400);
        } else {
          PREtap = true;
          elapsedPRE = millis();
          FreqDispl();
        }
      }

        int n = jamButton(L_THIRD);
        if (n >= 0) {

          if (SQUELCHbut and (n != B_SQUELCH)) {
            SQUELCHbut = false;
            squelch = false;
            drawButton(L_FIRST, B_SQUELCH, B_NORMAL);
            FreqDispl();
          }

          if ((bright) and (n != B_LIGHT)) {
            bright = false;
            drawButton(L_THIRD, B_LIGHT, B_NORMAL);
          }

          if (VOLbut) {
            VOLbut = false;
            DrawDispl ();
          }

          if (PRESbut and n != B_RDS and n != B_LIGHT) {
            PRESbut = false;
            drawButton(L_THIRD, B_FM, B_NORMAL);
            DrawDispl();
            PRESoff = true;
          }

          if (bfoOn and (n == B_SCAN or n == B_RETRO or n == B_FM or n == B_MEMO or n == B_SETUP)) {
            bfoOn = false;
            if (n == B_FM) drawButton(L_FIRST, B_BFO, B_BLOCK); else drawButton(L_FIRST, B_BFO, B_NORMAL);
            DrawDispl ();
          }

          if (n == B_FM) {
            if (!PRESoff) {
              if (currentMode != 0) {
                bandIdx = 0;
                currentMode = 0;
                BandSet();
                DrawDispl();
                DrawButThla();
              }
              PRESbut = true;
              drawButton(L_THIRD, B_FM, B_SELECT);
              presetBank = false;
              presetLoad();
              presetSort();
              bool flag = false;
              for (int i = 0; i <= lastPreset; i++) if ((preset[i].presetIdx * 100) >= band[0].minimumFreq and (preset[i].presetIdx * 100) <= band[0].maximumFreq) flag = true;
              if (flag) {
                tft.fillRect(XFreqDispl, YFreqDispl + 20 , 239, 65, TFT_DARKCYAN);
                previousPRES = -1;
                previousFrequency = 0;
              } else {
                delay(200);
                PRESbut = false;
                drawButton(L_THIRD, B_FM, B_NORMAL);
              }
            }
          }

          if (n == B_RETRO) {
            RETRObut = true;
            presetLoad();
            presetSort();
            presetSetPos();
            initRetro();
            currentRetroFreq = 0;
            ThirdLayer = false;
            SecondLayer  = true;
          }

          if (n == B_MEMO) {
            MEMObut = true;
            drawList(L_MEMO, "PAMIĘĆ");
            displMEMO();
            ThirdLayer = false;
            SecondLayer  = true;
          }

          if (n == B_SETUP) {
            SETUPbut = true;
            drawList(L_SETUP,"USTAWIENIA");
            drawButton(L_SETUP, 0, B_BLOCK);
            pageSetup = 0;

            prevVHFon = VHFon;
            prevlangRetroEN = langRetroEN;
            prevbeeperOn = beeperOn;
            prevdigitLigth = digitLigth;
            prevloadMemory = loadMemory;
            prevbatShow = batShow;
            prevmemoPreset = memoPreset;
            prevloadDefault = loadDefault;
            prevsaverOn = saverOn;
            prevsaverTime = saverTime;
            prevdisplayOff = displayOff;
            prevminSCANstep = minSCANstep;
            prevmaxSCANstep = maxSCANstep;
            prevautoSCANstep = autoSCANstep;
            prevSCANaccuracy = SCANaccuracy;
			prevsaverDisableOnScan = saverDisableOnScan;
            prevbatMinV = batMinV;
            prevbatMaxV = batMaxV;
            prevrssiHistoryOn = rssiHistoryOn;
            prevScanMarkSNR = ScanMarkSNR;
            prevscanStopOnSignal = scanStopOnSignal;
            prevscanWaterfallOn = scanWaterfallOn;
            prevcwDecoderOn = cwDecoderOn;
            prevwebServerOn = webServerOn;
            prevanalogMeterOn = analogMeterOn;
            prevnightModeOn = nightModeOn;
            prevscreenV = screenV;
            prevdisplayPower = displayPower;
            prevRDSalways = RDSalways;
            prevseekAccuracy = seekAccuracy;
            prevwifiEnable = wifiEnable;
            prevwifiConfigureNow = false;
            prevresetWifiConfig = false;

            displSETUP();
            ThirdLayer = false;
            SecondLayer  = true;
          }

          if (n == B_SCAN){
            int d = screenV * 40;
            SCANbut = true;
            ScanAGC = AGCgain;
            currentScanFreq = si4735.getFrequency();
            if (currentMode == USB or currentMode == LSB or currentMode == CW) {
              currentScanFreq -= (currentBFO / 1000);
              currentBFO = freqDec = band[bandIdx].lastBFO = 0;
              si4735.setSSBBfo(currentBFOmanu);
              si4735.setFrequency(currentScanFreq);
            }

            if (autoSCANstep) {
              float tmp = float(band[bandIdx].maximumFreq - band[bandIdx].minimumFreq) / (320 - (d * 2));
              float i = maxSCANstep / 2;
              while (i >= minSCANstep) {
                if (tmp > i) {
                  currentMinScanStep = i / 4;
                  currentMaxScanStep = i * 2;
                  i = 0;
                }
                i /= 2;
              }
              if (currentMinScanStep > 0.5) currentMinScanStep = 0.5;
              if ((currentMinScanStep < minSCANstep) or (currentMode == USB or currentMode == LSB or currentMode == CW)) currentMinScanStep = minSCANstep;
              if (currentMaxScanStep == minSCANstep) currentMaxScanStep *= 2;
            } else {
              currentMinScanStep = minSCANstep;
              currentMaxScanStep = maxSCANstep;
            }
            SCANstep = currentMaxScanStep / 2;

            currentScanLine = 159 - d;
            deltaScanLine = 0;
            drawSCAN();
            SCANpause = false;
            pauseSCAN();
            drawSCANgraf(true);
            DrawSCANtxt(true);
            signalScale = 1.5 + (d / 80);
            ThirdLayer = false;
            SecondLayer  = true;
          }

          if (n == B_SEEKUP) {
              SEEK = true;
              SEEKdispl(0);
              drawButton(L_THIRD, B_SEEKUP, B_SELECT);
              if (currentMode != FM) {
                if (band[bandIdx].bandType == MW_BAND_TYPE || band[bandIdx].bandType == LW_BAND_TYPE) {
                  if (seekAccuracy) si4735.setSeekAmSpacing(1); else si4735.setSeekAmSpacing(9);
                  si4735.setSeekAmLimits(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq);
                }
                else {
                  bandIdx = 29;
                  if (seekAccuracy) si4735.setSeekAmSpacing(1); else si4735.setSeekAmSpacing(5);
                  si4735.setSeekAmLimits(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq);
                }
              }
              si4735.seekStationProgress(SeekFreq, checkStopSeeking,  SEEK_UP);
              delay(300);
              SEEK = false;
              drawButton(L_THIRD, B_SEEKUP, B_NORMAL);
              currentFrequency = si4735.getFrequency();
              band[bandIdx].currentFreq = currentFrequency ;
              previousFrequency = currentFrequency;
              FreqDraw(currentFrequency, 0);
              delay(300);
          }

          if (n == B_SEEKDN) {
              SEEK = true;
              SEEKdispl(1);
              drawButton(L_THIRD, B_SEEKDN, B_SELECT);
              if (currentMode != FM) {
                if (band[bandIdx].bandType == MW_BAND_TYPE || band[bandIdx].bandType == LW_BAND_TYPE) {
                  if (seekAccuracy) si4735.setSeekAmSpacing(1); else si4735.setSeekAmSpacing(9);
                  si4735.setSeekAmLimits(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq);
                } else {
                  bandIdx = 29;
                  if (seekAccuracy) si4735.setSeekAmSpacing(1); else si4735.setSeekAmSpacing(5);
                  si4735.setSeekAmLimits(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq);
                }
              }
              si4735.seekStationProgress(SeekFreq, checkStopSeeking,  SEEK_DOWN);
              delay(300);
              SEEK = false;
              drawButton(L_THIRD, B_SEEKDN, B_NORMAL);
              currentFrequency = si4735.getFrequency();
              band[bandIdx].currentFreq = currentFrequency ;
              previousFrequency = currentFrequency;
              FreqDraw(currentFrequency, 0);
              delay(300);
          }

          if (n == B_INFO) {
            subrstatus();
            DrawThla();
          }

          if (n == B_LIGHT) {
            if (displayPower) {
              ErrorBeep();
            } else {
              if (bright == false)  {
                bright = true;
                if (bfoOn) bfoOn = false;
                if (PRESbut) drawButton(L_THIRD, B_FM, B_JAM);
                drawButton(L_THIRD, B_LIGHT, B_SELECT);
                previousBrightness = currentBrightness;
              }
              else {
                bright = false;
                if (PRESbut) drawButton(L_THIRD, B_FM, B_SELECT);
                drawButton(L_THIRD, B_LIGHT, B_NORMAL);
              }
            }
          }

          if (n == B_BACK) {
            FirstLayer  = true;
            SecondLayer = false;
            ThirdLayer  = false;
            ForthLayer  = false;
            DrawFila();
          }

          if (n == B_RDS) {
            if (RDS) RDS = false;
            else RDS = true;
            if (!RDS) drawButton(L_THIRD, B_RDS, B_NORMAL);
          }

          if (n == B_SQUELCH) {
            if (displayPower) {
              ErrorBeep();
            } else {
              if (squelch == false)  {
                squelch = true;
                SQUELCHbut = true;
                if (bfoOn) bfoOn = false;
                if (PRESbut) drawButton(L_THIRD, B_FM, B_JAM);
                drawButton(L_THIRD, B_SQUELCH, B_SELECT);
                previousSquelch = currentSquelch;
                DrawDispl();
              }
              else {
                squelch = false;
                SQUELCHbut = false;
                if (PRESbut) drawButton(L_THIRD, B_FM, B_SELECT);
                drawButton(L_THIRD, B_SQUELCH, B_NORMAL);
                FreqDispl();
              }
            }
          }

        }
    }

    if (ForthLayer) {

    }
  }

  if (currentRetroFreq != si4735.getFrequency() and RETRObut) {
    if (bandHamRetro) {
      if ((currentRetroFreq - (currentBFO / 1000)) < band[bandHamRetro].minimumFreq or (currentRetroFreq - (currentBFO / 1000)) > band[bandHamRetro].maximumFreq) {
        bandHamRetro = 0;
        currentRetroFreq = si4735.getFrequency() - (currentBFO / 1000);
        bandIdx = bandRetro[RETROband].band;
        currentMode = AM;
        BandSet();
        si4735.setFrequency(currentRetroFreq);
        si4735.setFrequencyStep(bandRetro[RETROband].hardStep);
        band[bandIdx].currentStep = bandRetro[RETROband].hardStep;
        currentBFO = 0;
      }
      if (bandHamRetro) {
        currentBFO += ((si4735.getFrequency() - currentRetroFreq) * 1000);
        currentRetroFreq = si4735.getFrequency();
        if (currentBFO == 16000) {
          currentBFO = 0;
          currentRetroFreq -= 16;
          si4735.setFrequency(currentRetroFreq);
        }
        if (currentBFO == -16000) {
          currentBFO = 0;
          currentRetroFreq += 16;
          si4735.setFrequency(currentRetroFreq);
        }
      }
    } else currentRetroFreq = si4735.getFrequency();
    if (RETROband < 2) bandRetro[RETROband].currentFreq = currentRetroFreq / 100; else bandRetro[RETROband].currentFreq = currentRetroFreq;
    if (bandHamRetro) bandRetro[RETROband].currentFreq -= (currentBFO / 1000);
    drawRETROscale();
  }

  if (currentMode == LSB || currentMode == USB || currentMode == CW)
  {
    if ((currentBFO != previousBFO) or (currentBFOmanu != previousBFOmanu))
    {
      previousBFO = currentBFO;
      previousBFOmanu = currentBFOmanu;
      si4735.setSSBBfo(currentBFO + currentBFOmanu);
      if (bfoOn and !RETRObut) FreqDispl();
    }
  }

  if (currentPRES != previousPRES and PRESbut) {
    if (currentPRES > lastPreset) currentPRES = 0;
    while ((preset[currentPRES].presetIdx * 100) < band[0].minimumFreq or (preset[currentPRES].presetIdx * 100) > band[0].maximumFreq) currentPRES++;

    previousPRES = currentPRES;
    tft.fillRect(XFreqDispl, YFreqDispl + 20 , 239, 36, TFT_DARKCYAN);
    tft.setTextSize(1);
    tft.setTextDatum(BL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_DARKCYAN );
    tft.drawString(String(preset[currentPRES].presetIdx, 2) + " MHz ", 1, 33);

    tftPlSetFont(T1516_T);
    tftPlSetSize(1);
    tftPlSetColor(TFT_WHITE, TFT_TRANS);
    tftPlSetDatum(BC_T);
    tftPlSetStyle(NBL_T);
    tftPlSetCut(0, 18);
    tftPlPrint(String(preset[currentPRES].PresetName), 120, 50);
    tftPlSetCut(0, 0);
    if (tftPlLength(String(preset[currentPRES].PresetName)) > 18) {
      textScroll = 0;
      elapsedScroll = millis() + 3000;
      directScroll = 1;
    } else directScroll = 0;

    bandIdx = 0;
    si4735.setFrequency((preset[currentPRES].presetIdx) * 100);
    band[bandIdx].currentFreq = si4735.getFrequency();
  }

  if (currentVOL != previousVOL)
  {
    if (currentVOL > MaxVOL) currentVOL = MaxVOL;
    if (currentVOL < MinVOL) currentVOL = MinVOL;
    if (currentVOL == MinVOL) {
      Mutestat = true;
      if (FirstLayer) drawButton(L_FIRST, B_MUTE, B_JAM);
      si4735.setAudioMute(audioMuteOn);
    } else {
      if (Mutestat) {
        Mutestat = false;
        if (FirstLayer) drawButton(L_FIRST, B_MUTE, B_NORMAL);
        si4735.setAudioMute(audioMuteOff);
      }
    }
    previousVOL = currentVOL;
    si4735.setVolume(currentVOL);
    if (RETRObut) drawRetroVol(); else FreqDispl();
  }

  if (currentSquelch != previousSquelch)
  {
    if (currentSquelch > MaxSQUELCH) currentSquelch = MaxSQUELCH;
    if (currentSquelch < MinSQUELCH) currentSquelch = MinSQUELCH;
    previousSquelch = currentSquelch;
    ledcWrite(LedChannelforTFT, currentSquelch);
    if (RETRObut) drawRetroVol(); else FreqDispl();
  }

  if (currentBrightness != previousBrightness)
  {
    if (currentBrightness < MaxBrightness) currentBrightness = MaxBrightness;
    if (currentBrightness > MinBrightness) currentBrightness = MinBrightness;
    previousBrightness = currentBrightness;
    ledcWrite(LedChannelforTFT, currentBrightness);
  }

  if (currentAGCgain != previousAGCgain)
  {
    AGCgain = 2;
    if (si4735.isCurrentTuneFM())  MaxAGCgain = MaxAGCgainFM;
    else MaxAGCgain = MaxAGCgainAM;

    if (currentAGCgain > MaxAGCgain) currentAGCgain = MaxAGCgain;
    if (currentAGCgain < MinAGCgain) currentAGCgain = MinAGCgain;

    previousAGCgain = currentAGCgain;
    si4735.setAutomaticGainControl(1, currentAGCgain);
    FreqDispl();
  }

}

void checkAGC()  {

  si4735.getAutomaticGainControl();
  if (si4735.isAgcEnabled()) {
    if (AGCgain == 2) si4735.setAutomaticGainControl(1, currentAGCgain);
    if (AGCgain == 0) si4735.setAutomaticGainControl(1, 0);
  } else if (AGCgain == 1) si4735.setAutomaticGainControl(0, 0);
}

void VOLbutoff()  {

  if (((millis() - VOLbutOnTime) > MIN_ELAPSED_VOLbut_TIME * 30) and (VOLbut == true)) {
    VOLbut = false;
    if (FirstLayer) drawButton(L_FIRST, B_VOL, B_NORMAL);
    if (RETRObut) drawButton(L_RETRO, 2, B_NORMAL);
    if (!RETRObut) FreqDispl();
  }
  if (VOLbut == false) VOLbutOnTime = millis();
}

void DisplayRDS()  {

  if (( currentMode == FM) and ((FirstLayer) or (ThirdLayer))) {
    if ( currentFrequency != previousFrequency ) {
      previousFrequency = currentFrequency;
      tft.fillRect(XFreqDispl + 70, YFreqDispl + 57, 115, 16, TFT_BLACK);
      tft.fillRect(0, YFreqDispl + 75, 242, 16, TFT_BLACK);

    }
    if ((RDS) and  (NewSNR >= 9) and !VOLbut and !AGCgainbut) checkRDS();
    else tft.fillRect(XFreqDispl + 70, YFreqDispl + 57, 115, 16, TFT_BLACK);

  }
}

void showtimeRSSI() {

  if ((millis() - elapsedRSSI) > MIN_ELAPSED_RSSI_TIME * RSSIfact)
  {
    si4735.getCurrentReceivedSignalQuality();
    NewRSSI = si4735.getCurrentRSSI();
    NewSNR = si4735.getCurrentSNR();

    if (SquelchUsesRSSI) {
      SignalQuality = NewRSSI;
    } else {
      SignalQuality = NewSNR;
    }
    if (!Mutestat) {
      if (SignalQuality >= currentSquelch){
        if (SCANpause == true) {
            si4735.setAudioMute(audioMuteOff);
            squelchDecay = millis();
        }
      } else {
        if (millis() > (squelchDecay + squelchDecayTime)) {
          si4735.setAudioMute(audioMuteOn);
        }
      }
   }

    OldRSSI = NewRSSI;
    showRSSI();
    elapsedRSSI = millis();
  }
}

void DisplayClock() {

if ((  currentMode == FM ) or (band[bandIdx].bandType == MW_BAND_TYPE) or (band[bandIdx].bandType == LW_BAND_TYPE)and ((FirstLayer) or (SecondLayer) or (ThirdLayer))) {
  if ((FirstLayer or ThirdLayer) and !PRESbut) {
  if(!getLocalTime(&timeinfo, 5)){
    return;
  }
  tft.fillRect(0, 23, 55, 33, TFT_BLACK);

  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);

  char timeMin[3];
  strftime(timeMin,3, "%M", &timeinfo);

  char timeSec[3];
  strftime(timeSec,3, "%S", &timeinfo);

   char timeWeekDay[10];
   strftime(timeWeekDay,10, "%A", &timeinfo);

   char timeDay[3];
   strftime(timeDay,3, "%d", &timeinfo);

   char timeMonth[3];
   strftime(timeMonth,3, "%x", &timeinfo);

   char timeYear[5];
   strftime(timeYear,5, "%Y", &timeinfo);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(TL_DATUM);
    tft.setTextPadding(0);
    tft.drawString(String(timeHour)+":"+String(timeMin),XFreqDispl + 3,XFreqDispl + 25);

    tft.setTextColor(TFT_GOLD, TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    tft.setTextPadding(0);

     tft.drawString(String(timeDay)+" / "+String(timeMonth),XFreqDispl + 8,XFreqDispl + 47);
     tft.drawString(String(timeYear),XFreqDispl + 18,XFreqDispl + 60);

 }
 }
}

void showRSSI() {

  if ((  currentMode == FM ) and ((FirstLayer) or (ThirdLayer) or (SecondLayer and RETRObut and !RETROband))) {
    if (RETRObut) {
      int d = screenV * 80;
      if (si4735.getCurrentPilot()) tft.fillRect(298 - d, 25, 14, 5, TFT_RED); else tft.fillRect(298 - d, 25, 14, 5, TFT_BLACK);
    } else {
      sprintf(buffer, "%s", (si4735.getCurrentPilot()) ? "STEREO" : "MONO");
      tft.setTextColor(TFT_WHITE, TFT_RED);
      tft.setTextSize(1);
      tft.setTextDatum(BC_DATUM);
      tft.setTextPadding(0);
      tft.fillRect(XFreqDispl + 195, YFreqDispl + 25 , 38, 12, TFT_RED);
      tft.drawString(buffer, XFreqDispl + 214, YFreqDispl + 36);
    }
  }

  rssi = NewRSSI;
  if ((FirstLayer) or (ThirdLayer)) {
    if (cwDecoderOn) {
      processCWEvents();
      drawCWDecoderView();
    } else if (analogMeterOn) {
      drawAnalogMeter();
    } else if (rssiHistoryOn) {
      updateRssiHistory(rssi);
      SmeterHistory();
    } else {
      Smeter();
    }
  }
  if ((  currentMode == AM ) or (  currentMode == LSB )or  (  currentMode == USB )or (  currentMode == CW )and ((FirstLayer) or (ThirdLayer) or (SecondLayer and RETRObut and !RETROband)))
    {
  tft.setTextSize(1);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  if ((FirstLayer or ThirdLayer) and !PRESbut) {
    tft.setTextDatum(TL_DATUM);
    tft.drawString("RSSI " + String(NewRSSI) + " dBuV " , XFreqDispl + 8, YFreqDispl + 75);
    tft.setTextDatum(TR_DATUM);
    tft.drawString(" SNR " + String(NewSNR) + " dB", XFreqDispl + 180, YFreqDispl + 75);
    }
  }
  VOLbutoff();
  DisplayClock();
}

void encoderCheck()  {

  if (encoderCount != 0)
  {
    bool mainpurp = true;

    if (SCANbut and VOLbut == false and bfoOn == false) {
      int d = screenV * 40;
      if (SCANpause == false) {
        SCANpause = true;
        pauseSCAN();
      }
      float tmpcurline = (encoderCount == 1) ? (currentScanLine + (float(band[bandIdx].currentStep) / SCANstep)) : (currentScanLine - (float(band[bandIdx].currentStep) / SCANstep));
      if ((encoderCount == 1 and (currentScanFreq + ((tmpcurline - 159 + d + deltaScanLine) * SCANstep)) <= band[bandIdx].maximumFreq) or (encoderCount != 1 and (currentScanFreq + ((tmpcurline - 159 + d + deltaScanLine) * SCANstep)) >= band[bandIdx].minimumFreq)) {
        int tmpline = currentScanLine;
        currentScanLine = tmpcurline;

        if (currentMode == USB or currentMode == LSB or currentMode == CW) {
          if (encoderCount == 1) currentBFO -= 1000; else currentBFO += 1000;
          if (currentBFO <= -16000) {
            currentBFO += 16000;
            si4735.setFrequency(si4735.getFrequency() + 16);
            checkAGC();
          }
          if (currentBFO >= 16000) {
            currentBFO -= 16000;
            si4735.setFrequency(si4735.getFrequency() - 16);
            checkAGC();
          }
          si4735.setSSBBfo(currentBFO + currentBFOmanu);
          band[bandIdx].lastBFO = currentBFO;
        } else {
          if (encoderCount == 1) si4735.frequencyUp(); else si4735.frequencyDown();
        }

        if (trunc(currentScanLine) < 0) {
          for (int i = 319 - (d * 2); i > (abs(currentScanLine) - 1); i--) {
            ScanValueRSSI[i] = ScanValueRSSI[int(i + currentScanLine)];
            ScanValueSNR[i] = ScanValueSNR[int(i + currentScanLine)];
          }
          for (int i = 0; i < abs(currentScanLine); i++) {
            ScanValueRSSI[i] = 198 + d;
            ScanValueSNR[i] = 0;
          }
          deltaScanLine += currentScanLine;
          currentScanLine = 0;
          ScanEmpty = true;
          for (int i = 0; i < 320; i++) ScanMark[i] = false;
          drawSCANgraf(false);
          DrawSCANtxt(true);
          posScan = 0;
        } else {
          if (trunc(currentScanLine) > (319 - (d * 2))) {
            for (int i = currentScanLine - 319 + (d * 2); i < (320 - (d * 2)); i++) {
              ScanValueRSSI[int(i - currentScanLine + 319 - (d * 2))] = ScanValueRSSI[i];
              ScanValueSNR[int(i - currentScanLine + 319 - (d * 2))] = ScanValueSNR[i];
            }
            for (int i = 639 - (d * 4) - currentScanLine; i < (320 - (d * 2)); i++) {
              ScanValueRSSI[i] = 198 + d;
              ScanValueSNR[i] = 0;
            }
            deltaScanLine += currentScanLine - 319 + (d * 2);
            currentScanLine = 319 - (d * 2);
            ScanEmpty = true;
            for (int i = 0; i < 320; i++) ScanMark[i] = false;
            drawSCANgraf(false);
            DrawSCANtxt(true);
            posScan = 0;
          } else {
            if (encoderCount != 1) currentScanLine -= 10;
            drawSCANline(tmpline);
            if (encoderCount != 1) currentScanLine += 10;
            drawSCANline(currentScanLine);
            DrawSCANtxt(true);
          }
        }
      }
      mainpurp = false;
    }

    if (bfoOn)  {
#ifdef IhaveSI5351
      currentBFOmanu = 0;
      if (encoderCount == 1) {
        FreqSI5351 = FreqSI5351 + stepsizesynth;
      } else {
        FreqSI5351 = FreqSI5351 - stepsizesynth;
      }
      calibratSI5351 = false;
      si5351wire.set_freq(FreqSI5351, CLK_Xtal);
      mainpurp = false;
#endif

#ifdef IhaveCrystal
      if (RETRObut) {
        currentBFOmanu = (encoderCount == 1) ? (currentBFOmanu - currentBFOStep) : (currentBFOmanu + currentBFOStep);
        if (currentBFOmanu == 1000) {
          currentBFOmanu = 0;
          currentRetroFreq--;
        }
        if (currentBFOmanu == -1000) {
          currentBFOmanu = 0;
          currentRetroFreq++;
        }
      } else {
        currentBFOmanu = (encoderCount == 1) ? (currentBFOmanu + currentBFOStep) : (currentBFOmanu - currentBFOStep);
        if (currentBFOmanu > 999 or currentBFOmanu < -999) currentBFOmanu = previousBFOmanu;
      }
      mainpurp = false;
#endif
    }

    if (RETRObut and !VOLbut and !bfoOn) {
      if (scrollRetro) {
        if (encoderCount == 1) scrollRetro = 1; else scrollRetro = -1;
      } else {
        float tmpmax = bandRetro[RETROband].maximumFreq;
        float tmpmin = bandRetro[RETROband].minimumFreq;
        if (bandIdx == 0) {
          tmpmax *= 100;
          tmpmin *= 100;
        }
        if ((encoderCount == 1 and (currentRetroFreq + band[bandIdx].currentStep) <= tmpmax) or (encoderCount != 1 and (currentRetroFreq - band[bandIdx].currentStep) >= tmpmin)) {
          if (bandHamRetro) {
            currentRetroFreq += encoderCount;
          } else {
            if (encoderCount == 1) si4735.frequencyUp(); else si4735.frequencyDown();
          }
        }
      }
      mainpurp = false;
    }

    if (cityRETRObut) {
      if (encoderCount == 1) cityRetroRotary = 1; else cityRetroRotary = -1;
      drawRetroCity();
      mainpurp = false;
    }

    if (PRESbut and !bright) {
      currentPRES = (encoderCount == 1) ? (currentPRES + currentPRESStep) : (currentPRES - currentPRESStep);
      if (currentPRES == 255) currentPRES = lastPreset;
      if (currentPRES > lastPreset) currentPRES = 0;
      while ((preset[currentPRES].presetIdx * 100) < band[0].minimumFreq or (preset[currentPRES].presetIdx * 100) > band[0].maximumFreq) {
        currentPRES = (encoderCount == 1) ? (currentPRES + currentPRESStep) : (currentPRES - currentPRESStep);
        if (currentPRES > lastPreset) {
          if (encoderCount == 1) currentPRES = 0; else currentPRES = lastPreset;
        }
      }
      mainpurp = false;
    }

    if (bright) {
      currentBrightness = (encoderCount == 1) ? (currentBrightness - 10) : (currentBrightness + 10);
      mainpurp = false;
    }

    if (SQUELCHbut) {
      currentSquelch = (encoderCount == 1) ? (currentSquelch + currentSQUELCHStep) : (currentSquelch - currentSQUELCHStep);
      SQUELCHbutOnTime = millis();
      squelchDecay = 0;
      mainpurp = false;
      si4735.getCurrentReceivedSignalQuality();
      if (SquelchUsesRSSI) {
        SignalQuality = NewRSSI;
      } else {
        SignalQuality = NewSNR;
      }
      if (SignalQuality >= currentSquelch){
        if (SCANpause == true) {
          si4735.setAudioMute(audioMuteOff);
        }
      } else {
        si4735.setAudioMute(audioMuteOn);
      }
    }

    if (VOLbut) {
      currentVOL = (encoderCount == 1) ? (currentVOL + currentVOLStep) : (currentVOL - currentVOLStep);
      VOLbutOnTime = millis();
      mainpurp = false;
    }

    if (AGCgainbut) {
      currentAGCgain = (encoderCount == 1) ? (currentAGCgain + currentAGCgainStep) : (currentAGCgain - currentAGCgainStep);
      mainpurp = false;
    }

    if (SETUPbut) {
      pageSetup = (encoderCount == 1) ? (pageSetup + 1) : (pageSetup - 1);
      if (pageSetup < 0) pageSetup = 0;

      if (pageSetup > maxPageSetup) pageSetup = maxPageSetup;

      displSETUP();
      drawButtons(L_SETUP);
      if (!pageSetup) drawButton(L_SETUP, 0, B_BLOCK);
      if (pageSetup == maxPageSetup) drawButton(L_SETUP, 1, B_BLOCK);
      mainpurp = false;
    }

    if (MEMObut) {
      if (MEMOadd) {
        charMemoName = (encoderCount == 1) ? (charMemoName + 1) : (charMemoName - 1);
        if (charMemoName == 144 and encoderCount == 1) charMemoName = 32;
        if (charMemoName == 128 and encoderCount == 1) charMemoName = 144;
        if (charMemoName == 192) charMemoName = 128;
        if (charMemoName == 127 and encoderCount != 1) charMemoName = 191;
        if (charMemoName == 143 and encoderCount != 1) charMemoName = 127;
        if (charMemoName == 31) charMemoName = 143;
      } else {
        currentMemo = (encoderCount == 1) ? (currentMemo + 1) : (currentMemo - 1);
        if (currentMemo < 0) currentMemo = 0;
        if (presetBank) {
          if (currentMemo > lastPreset) currentMemo = lastPreset;
        } else {
          if (currentMemo > lastMemoBank) currentMemo = lastMemoBank;
        }
        displMEMO();
      }
      mainpurp = false;
    }

    if (mainpurp)
    {

     if (!Mutestat) {
      si4735.getCurrentReceivedSignalQuality();
      if (SquelchUsesRSSI){
        SignalQuality = si4735.getCurrentRSSI();
      } else {
        SignalQuality = si4735.getCurrentSNR();
      }
      if (SignalQuality >= currentSquelch){
        if (SCANpause == true) {
          si4735.setAudioMute(audioMuteOff);
          squelchDecay = millis();
        }
      } else {
        if (millis() > (squelchDecay + squelchDecayTime)) {
          si4735.setAudioMute(audioMuteOn);
        }
      }
   }

      if (currentMode == LSB || currentMode == USB || currentMode == CW) {
        if (encoderCount == 1) {
          freqDec = freqDec - freqstep;
          int freqTot = (si4735.getFrequency() * 1000) + (freqDec * -1);
          if ( freqTot > (band[bandIdx].maximumFreq * 1000)) {
            si4735.setFrequency(band[bandIdx].maximumFreq);
            freqDec = 0;
          }
          if (freqDec <= -16000)  {
            freqDec = freqDec + 16000;
            int freqPlus16 = currentFrequency + 16;
            MuteAudOn();
            si4735.setFrequency(freqPlus16);
          }
          currentBFO = freqDec;
        } else {
          freqDec = freqDec + freqstep;
          int freqTot = (si4735.getFrequency() * 1000) - freqDec;
          if ( freqTot < (band[bandIdx].minimumFreq * 1000)) {
            si4735.setFrequency(band[bandIdx].minimumFreq);
            freqDec = 0;
          }
          if (freqDec >= 16000)  {
            freqDec = freqDec - 16000;
            int freqMin16 = currentFrequency - 16;
            MuteAudOn();
            si4735.setFrequency(freqMin16);
          }
          currentBFO = freqDec;
        }
        band[bandIdx].lastBFO = currentBFO;
        checkAGC();
      } else  {
        if (encoderCount == 1) {
          si4735.frequencyUp();
        } else {
          si4735.frequencyDown();
        }
      }
      band[bandIdx].currentFreq = si4735.getFrequency();
    }
    if (!PRESbut and !RETRObut and !VOLbut and !AGCgainbut and !bright and !cityRETRObut and !bandRETRObut and !SETUPbut and !MEMObut and !SQUELCHbut) FreqDispl();
    if (SCANbut) {
      DrawSCANind();
      DisplaySCANsignal();
    }
    encoderCount = 0;
  }
}

void encoderButtonCheck()  {

  if (analogRead(ENCODER_SWITCH) < 500 and (ThirdLayer or FirstLayer or SecondLayer)) {
      Beep(1, 0);
      long encTime = millis() + 1000;
      while (analogRead(ENCODER_SWITCH) < 500 and encTime > millis());
      if (analogRead(ENCODER_SWITCH) < 500) {

       Beep(1, 0);
       pressed = tft.getTouch(&x, &y);
       if (pressed) {

        Beep(1, 0);
        x = y = 0;
        bool tmp;
        if (SCANbut) {
          tmp = SCANpause;
          SCANpause = true;
          pauseSCAN();
          deltaScanLine += currentScanLine - 159 + (screenV * 40);
        }
        screenV = !screenV;
        prevscreenV = screenV;
        if (SCANbut) {
          currentScanLine = 159 - (screenV * 40);
          ScanEmpty = true;
          for (int i = 0; i < 320; i++) ScanMark[i] = false;
          drawSCANgraf(true);
          DrawSCANtxt(true);
          posScan = 0;
        }
        screenRotate();
        returnLayer();
        if (SCANbut) {
          SCANpause = false;
          pauseSCAN();
          SCANpause = tmp;
          pauseSCAN();
        }
        delay(500);

       } else if (MEMObut) {

        presetBank = !presetBank;
        if (presetBank) {
          presetLoad();
          presetSort();
          if (lastPreset < 0) presetBank = false; else currentMemo = 0;
        } else currentMemo = 0;
        drawButtons(L_MEMO);
        if (presetBank) {
          drawButton(L_MEMO, 0, B_BLOCK);
          drawButton(L_MEMO, 1, B_BLOCK);
          drawButton(L_MEMO, 2, B_BLOCK);
        }
        displMEMO();

       } else {

        if ((currentMode == LSB || currentMode == USB || currentMode == CW) and !RETRObut) {
          if (bfoOn) bfoOn = false; else bfoOn = true;
          if (FirstLayer) {
            if (bfoOn) {
              drawButton(L_FIRST, B_BFO, B_SELECT);
              drawButton(L_FIRST, B_STEP, B_NORMAL);
            } else {
              drawButton(L_FIRST, B_BFO, B_NORMAL);
              drawButton(L_FIRST, B_STEP, B_BLOCK);
            }
          }
          if (VOLbut) {
            VOLbut = false;
            if (FirstLayer) drawButton(L_FIRST, B_VOL, B_NORMAL);
          }
          bfoTr = true;
          FreqDispl();
          if (SCANbut) {
            if (bfoOn) drawButton(L_SCAN, 2, B_NORMAL); else drawButton(L_SCAN, 2, B_BLOCK);
            if (bfoOn) {
              if (SCANpause == false) {
                SCANpause = true;
                pauseSCAN();
              }
            } else {
              DrawSCANind();
              DisplaySCANsignal();
            }
          }
        } else if (RETRObut and RETROband > 3) {
          if (currentMode == LSB || currentMode == USB) {
            bandHamRetro = 0;
            currentRetroFreq = si4735.getFrequency() - (currentBFO / 1000);
            bandIdx = bandRetro[RETROband].band;
            currentMode = AM;
            BandSet();
            si4735.setFrequency(currentRetroFreq);
            si4735.setFrequencyStep(5);
            band[bandIdx].currentStep = 5;
            currentBFO = 0;
            drawRETROscale();
          } else {
            int i = 1;
            while (currentRetroFreq < band[i].minimumFreq or currentRetroFreq > band[i].maximumFreq) i++;
            if (bandMode[i] == LSB or bandMode[i] == USB) {
              bandIdx = i;
              currentMode = bandMode[i];
              previousMode = currentMode;
              band[bandIdx].prefmod = currentMode;
              BandSet();
              currentBFO = 0;
              currentBFOmanu = 0;
              currentBFOStep = 25;
              si4735.setFrequency(currentRetroFreq);
              bandHamRetro = i;
              drawRETROscale();
            }
          }
          checkAGC();
        } else ErrorBeep();
       }
      } else {
        long encTime = millis() + 400;
        while (analogRead(ENCODER_SWITCH) > 500 and encTime > millis());
        if (analogRead(ENCODER_SWITCH) < 500) {
          while (analogRead(ENCODER_SWITCH) < 500);
          long encTime = millis() + 400;
          while (analogRead(ENCODER_SWITCH) > 500 and encTime > millis());
          if (analogRead(ENCODER_SWITCH) < 500) {

            if (RETRObut) scrollRetro = -1;
            if ((currentMode != LSB) and (currentMode != USB) and (currentMode != CW) and (ThirdLayer or FirstLayer))   {
              SEEK = true;
              SEEKdispl(1);
              if (ThirdLayer) drawButton(L_THIRD, B_SEEKDN, B_SELECT);
              if (currentMode != FM) {
                if (band[bandIdx].bandType == MW_BAND_TYPE || band[bandIdx].bandType == LW_BAND_TYPE) {
                  si4735.setSeekAmSpacing(1);
                  si4735.setSeekAmLimits(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq);
                }
                else {
                  bandIdx = 29;
                  si4735.setSeekAmSpacing(1);
                  si4735.setSeekAmLimits(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq);
                }
              }
              si4735.seekStationProgress(SeekFreq, checkStopSeeking,  SEEK_DOWN);
              delay(300);
              SEEK = false;
              if (ThirdLayer) drawButton(L_THIRD, B_SEEKDN, B_NORMAL);
              currentFrequency = si4735.getFrequency();
              band[bandIdx].currentFreq = currentFrequency ;
              previousFrequency = currentFrequency;
              FreqDraw(currentFrequency, 0);
              delay(300);
            }
          } else {

            if (RETRObut) scrollRetro = 1;
            if ((currentMode != LSB) and (currentMode != USB) and (currentMode != CW) and (ThirdLayer or FirstLayer))   {
              SEEK = true;
              SEEKdispl(0);
              if (ThirdLayer) drawButton(L_THIRD, B_SEEKUP, B_SELECT);
              if (currentMode != FM) {
                if (band[bandIdx].bandType == MW_BAND_TYPE || band[bandIdx].bandType == LW_BAND_TYPE) {
                  si4735.setSeekAmSpacing(1);
                  si4735.setSeekAmLimits(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq);
                } else {
                  bandIdx = 29;
                  si4735.setSeekAmSpacing(1);
                  si4735.setSeekAmLimits(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq);
                }
              }
              si4735.seekStationProgress(SeekFreq, checkStopSeeking,  SEEK_UP);
              delay(300);
              SEEK = false;
              if (ThirdLayer) drawButton(L_THIRD, B_SEEKUP, B_NORMAL);
              currentFrequency = si4735.getFrequency();
              band[bandIdx].currentFreq = currentFrequency ;
              previousFrequency = currentFrequency;
              FreqDraw(currentFrequency, 0);
              delay(300);
            }
          }
        } else {

         bool mainpurp = true;

         if (scrollRetro) {
            scrollRetro = 0;
            mainpurp = false;
         }

         if (cityRETRObut) {
            cityRETRObut = false;
            RETRObut = true;
            if (prevPresetId != PresetId) {
              prevPresetId = PresetId;
              currentPRES = 0;
              presetLoad();
              presetSort();
              presetSetPos();
            }
            drawRETRO();
            currentRetroFreq = 0;
            mainpurp = false;
         }

         if (PRESbut) {
            PRESbut = false;
            drawButton(L_THIRD, B_FM, B_NORMAL);
            DrawDispl();
            mainpurp = false;
         }

         if (bright) {
            bright = false;
            drawButton(L_THIRD, B_LIGHT, B_NORMAL);
            mainpurp = false;
         }

         if (AGCgainbut) {
            AGCgainbut = false;
            FreqDispl();
            drawButton(L_THIRD, B_ATT, B_NORMAL);
            mainpurp = false;
         }

         if (SETUPbut) mainpurp = false;

         if (MEMObut) {
            if (MEMOadd) {
              posMemoName++;
              if (posMemoName < 20) charMemoName = addMemoName[posMemoName];
            } else {
              if (presetBank) {
                int tmpBand = bandFreq(preset[currentMemo].presetIdx);
                if (tmpBand == 29 and (preset[currentMemo].presetIdx < 153 or preset[currentMemo].presetIdx > 30000)) {
                  ErrorBeep();
                } else {
                  Beep(1, 0);
                  if (bandIdx != tmpBand or currentMode != bandMode[tmpBand]) {
                    bandIdx = tmpBand;
                    currentMode = bandMode[tmpBand];
                    BandSet();
                  }
                  if (tmpBand) si4735.setFrequency(preset[currentMemo].presetIdx); else si4735.setFrequency(preset[currentMemo].presetIdx * 100);
                  band[bandIdx].currentFreq = si4735.getFrequency();
                  FreqDispl();
                  displMEMO();
                }
              } else {
                if (MemoBank[currentMemo].freq < 153 or MemoBank[currentMemo].freq > 30000) {
                  ErrorBeep();
                } else {
                  if (bandIdx != (MemoBank[currentMemo].band & 0x1F) or currentMode != trunc(MemoBank[currentMemo].band / 32)) {
                    bandIdx = MemoBank[currentMemo].band & 0x1F;
                    currentMode = trunc(MemoBank[currentMemo].band / 32);
                    BandSet();
                  }
                  si4735.setFrequency(MemoBank[currentMemo].freq);
                  band[bandIdx].currentFreq = MemoBank[currentMemo].freq;
                  FreqDispl();
                }
              }
              checkAGC();
            }
            mainpurp = false;
         }

#ifdef IhaveSI5351
         if (FirstLayer) {
            if ((bfoOn) and (calibratSI5351 == false)) {
              calibratSI5351 = true;
              calibratvalSI5351 = 3276800 - FreqSI5351;
              FreqDispl();
              mainpurp = false;
            }
         }
#endif
         if (mainpurp) {

          if (encoderBtnState == 0) {
            SQUELCHbut = false;
            squelchDecay = 0;
            if (VOLbut) VOLbut = false; else VOLbut = true;
          }
          if (encoderBtnState == 1) {
            VOLbut = false;
            if (SQUELCHbut) SQUELCHbut = false; else SQUELCHbut = true;
          }
          if (encoderBtnState == 2) {
            VOLbut = false;
            SQUELCHbut = false;
            squelchDecay = 0;
          }
          encoderBtnState = encoderBtnState + 1;
          if (encoderBtnState == 3) encoderBtnState = 0;

          if (FirstLayer) {
            if (VOLbut) drawButton(L_FIRST, B_VOL, B_SELECT); else drawButton(L_FIRST, B_VOL, B_NORMAL);
          }
          if (RETRObut) {
            if (VOLbut) drawButton(L_RETRO, 2, B_SELECT); else drawButton(L_RETRO, 2, B_NORMAL);
          }
          if (bfoOn) {
            bfoOn = false;
            if (FirstLayer) drawButton(L_FIRST, B_BFO, B_NORMAL);
            if (RETRObut) drawRETROscale();
          }
          if (RETRObut) drawRetroVol(); else FreqDispl();
          if (SCANbut) {
            if (VOLbut) {
              if (SCANpause == false) {
                SCANpause = true;
                pauseSCAN();
              }
            } else {
              DrawSCANind();
              DisplaySCANsignal();
            }
          }
         }
        }
      }
    while (analogRead(ENCODER_SWITCH) < 500);
  }
}

void setStep()  {

  if (bfoOn && (currentMode == LSB || currentMode == USB || currentMode == CW))
  {
    if (currentBFOStep == 1) currentBFOStep = 10;
    else if (currentBFOStep == 10) currentBFOStep = 25;
    else currentBFOStep = 1;
  }
  if (SCANbut == false) {
    useBand();
    checkAGC();
    DrawDispl();
  }
}

void Beep(int cnt, int tlb) {

  if (beeperOn) {
    int tla = 100;
    for (int i = 0; i < cnt; i++) {
      digitalWrite(BEEPER, 1);
      delay(tla);
      digitalWrite(BEEPER, 0);
      delay(tlb);
    }
  }
}

void DrawFila()   {

  FirstLayer = true;
  SecondLayer = false;
  tft.fillScreen(TFT_BLACK);
  DrawButFila();
  DrawDispl();
  DrawSmeter();
  DrawVolumeIndicator();
  DrawBatteryIndicator();
  elapsedBat = 0;
}

void DrawThla()  {

  ThirdLayer = true;
  ForthLayer = false;
  tft.fillScreen(TFT_BLACK);
  DrawButThla();
  DrawDispl();
  DrawSmeter();
  DrawVolumeIndicator();
  DrawBatteryIndicator();
  elapsedBat = 0;
}

void DrawButFila() {

  drawButtons(L_FIRST);

  if (currentMode != LSB && currentMode != USB && currentMode != CW) drawButton(L_FIRST, B_BFO, B_BLOCK);
  if (bfoOn) drawButton(L_FIRST, B_BFO, B_SELECT);

  si4735.getAutomaticGainControl();
  if (si4735.isAgcEnabled()) drawButton(L_FIRST, B_AGC, B_JAM);

  if (Mutestat) drawButton(L_FIRST, B_MUTE, B_JAM);

  if (currentMode == FM) drawButton(L_FIRST, B_MODE, B_BLOCK);

  if (currentMode == LSB || currentMode == USB || currentMode == CW) drawButton(L_FIRST, B_STEP, B_BLOCK);
}

void DrawButThla() {

  drawButtons(L_THIRD);

  if (currentMode == LSB || currentMode == USB || currentMode == CW) {
    drawButton(L_THIRD, B_SEEKUP, B_BLOCK);
    drawButton(L_THIRD, B_SEEKDN, B_BLOCK);
  }

  if (RDS) drawButton(L_THIRD, B_RDS, B_JAM);

  if (displayPower) drawButton(L_THIRD, B_LIGHT, B_BLOCK);
}

void DrawVolumeIndicator()  {

  tft.setTextSize(1);
  tft.fillRect(XVolInd + 2, YVolInd - 1, 157, 28, TFT_GREY);
  tft.setTextColor(TFT_WHITE, TFT_GREY);
  tft.setCursor(XVolInd +  11, YVolInd + 7);
  tft.print("0%");
  tft.setCursor(XVolInd + 126, YVolInd + 7);
  tft.print("100%");
}

void DrawBatteryIndicator()  {

  tft.fillRect(XVolInd + 161, YVolInd - 1, 77, 28, TFT_GREY);
  if (batShow) tft.fillRect(XVolInd + 176, YVolInd + 5, 46, 16, TFT_NAVY);
}

void DrawSmeter()  {

  if (cwDecoderOn) { drawCWDecoderView(); return; }
  if (analogMeterOn) { drawAnalogMeter(); return; }
  if (rssiHistoryOn) { SmeterHistory(); return; }
  String IStr;
  tft.setTextSize(1);
  tft.fillRect(Xsmtr + 2, Ysmtr + 6, 236, 46, TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(BC_DATUM);
  for (int i = 0; i < 10; i++) {
    tft.fillRect(Xsmtr + 15 + (i * 12), Ysmtr + 24, 2, 8, TFT_WHITE);
    IStr = String(i);
    tft.setCursor((Xsmtr + 14 + (i * 12)), Ysmtr + 13);
    tft.print(i);
  }
  for (int i = 1; i < 7; i++) {
    tft.fillRect((Xsmtr + 123 + (i * 16)), Ysmtr + 24, 3, 8, TFT_RED);
    IStr = String(i * 10);
    tft.setCursor((Xsmtr + 117 + (i * 16)), Ysmtr + 13);
    if ((i == 2) or (i == 4) or (i == 6))  {
      tft.print("+");
      tft.print(i * 10);
    }
  }
  tft.fillRect(Xsmtr + 15, Ysmtr + 32 , 112, 3, TFT_WHITE);
  tft.fillRect(Xsmtr + 127, Ysmtr + 32 , 100, 3, TFT_RED);

}

void SEEKdispl (int dir)  {

  tft.setTextSize(2);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextDatum(BL_DATUM);
  tft.drawString("SEEK", XFreqDispl, YFreqDispl + 40);
  if (dir) tft.drawString("<<<<", XFreqDispl, YFreqDispl + 60); else tft.drawString(">>>>", XFreqDispl, YFreqDispl + 60);
}

void drawList(uint8_t lay, String text) {

  tft.fillScreen(COLOR_BACKGROUND);
  FreqDispl();
  int d = 0;
  if (!screenV and !BroadBand and !FREQbut){
    d = 40;
    for (int n = 1; n <= 20; n++) {
      tft.fillRect(40 - (n * 2), 0, 2, 240, ((int(n / 2) * 4096) + (n * 32)));
      tft.fillRect((n * 2) + 278, 0, 2, 240, ((int(n / 2) * 4096) + (n * 32)));
    }
  }
  tft.fillRect(d, 0, 240, 20, TFT_GREY);
  tftPlSetSize(1);
  tftPlSetStyle(REG_T);
  tftPlSetFont(T1012_T);
  tftPlSetDatum(BC_T);
  tftPlSetColor(TFT_CYAN, TFT_GREY);
  tftPlPrint(text, 120 + d, 20);
  if (lay == L_SETUP) {
    spr.createSprite(265, 120);
    spr.fillScreen(COLOR_BACKGROUND);
    spr.pushImage(0, 0, 265, 120, (uint16_t *)logo);
    if (screenV) spr.pushSprite(-25, 80); else spr.pushSprite(27, 80);
    spr.deleteSprite();
  }
  drawListBut(lay);
}

void drawListBut(uint8_t lay) {

  drawButtons(lay);
  if (BroadBand || HamBand) {
    drawButton(lay, bandIdx, B_JAM);
  } else if (BandWidth) {
    if (currentMode == AM) drawButton(lay, bwIdxAM, B_JAM);
    else if (currentMode == FM) drawButton(lay, bwIdxFM, B_JAM);
    else drawButton(lay, bwIdxSSB, B_JAM);
  } else if (STEPbut) {
    if (band[bandIdx].bandType == MW_BAND_TYPE or band[bandIdx].bandType == LW_BAND_TYPE) drawButton(lay, ssIdxMW, B_JAM);
    else if (currentMode == FM) drawButton(lay, ssIdxFM, B_JAM);
    else drawButton(lay, ssIdxAM, B_JAM);
  } else if (Modebut) drawButton(lay, currentMode, B_JAM);
}

void subrstatus() {

  tft.fillScreen(TFT_BLACK);

  spr.createSprite(265, 120);
  spr.fillScreen(COLOR_BACKGROUND);
  spr.pushImage(0, 0, 265, 120, (uint16_t *)logo);
  if (screenV) spr.pushSprite(-25, 0); else spr.pushSprite(27, 0);
  spr.deleteSprite();

  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Software version V5.3b  17-09-2022 IU4ALH Mod", 5, 0);
  tft.drawString("Mode     : " + String(bandModeDesc[band[bandIdx].prefmod]), 5, 10);
  if ( currentMode != FM)  tft.drawString("Freq.    : " + String(currentFrequency, 0) + " KHz", 5, 20);
  else tft.drawString("Freq.    : " + String(currentFrequency / 100, 1) + " MHz", 5, 20);
  si4735.getCurrentReceivedSignalQuality();
  tft.drawString("RSSI     : " + String(si4735.getCurrentRSSI()) + "dBuV", 5, 30);
  tft.drawString("SNR      : " + String(si4735.getCurrentSNR()) + "uV", 5, 40);
  if (  currentMode == FM ) {
    sprintf(buffer, "%s", (si4735.getCurrentPilot()) ? "STEREO" : "MONO");
    tft.drawString("         : " + String(buffer), 5, 50);
  }
  si4735.getAutomaticGainControl();
  si4735.getCurrentReceivedSignalQuality();
  tft.drawString("LNA GAIN index: " + String(si4735.getAgcGainIndex()) + "/" + String(currentAGCgain), 5, 60);
  tft.drawString("Volume   : )" + String(si4735.getVolume()), 5, 70);
  sprintf(buffer, "%s", (si4735.isAgcEnabled()) ? "AGC ON " : "AGC OFF");
  tft.drawString(buffer, 5, 80);
  if (bfoOn) tft.drawString("BFO ON  ", 5, 90);
  else tft.drawString("BFO OFF ", 5, 90);
  tft.drawString("AVC max GAIN  : " + String(si4735.getCurrentAvcAmMaxGain()), 5, 100);
  tft.drawString("Ant. Cap = " + String(si4735.getAntennaTuningCapacitor()) , 5, 110);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Band          :  " + String(bandIdx) + "  " + String(band[bandIdx].bandName) , 5, 120);
  tft.drawString("Bandwidth SSB : " + String(bandwidthSSB[bwIdxSSB]) + " KHz", 5, 130);
  tft.drawString("Bandwidth AM  : " + String(bandwidthAM[bwIdxAM]) + " KHz", 5, 140);
  tft.drawString("Bandwidth FM  : " + String(bandwidthFM[bwIdxFM]) + " KHz", 5, 150);
  tft.drawString("Stepsize  MW  :   " + String(ssIdxMW) + " KHz", 5, 160);
  tft.drawString("Stepsize  AM  :   " + String(ssIdxAM) + " KHz", 5, 170);
  tft.drawString("Stepsize SSB  :   " "1 KHz fixed", 5, 180);
  tft.drawString("Stepsize  FM  : " + String(ssIdxFM * 10) + " KHz", 5, 190);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  float vsupply;
  if (batShow) vsupply = readVsupply(); else vsupply = ((1.66 / 1850) * analogRead(ENCODER_SWITCH)) * 2;
  tft.drawString("Power Supply   : " + String(vsupply, 2) + " V", 5, 200);
  tft.drawString("EEPROM SIZE    : " + String(EEPROM_SIZE) + " byte | FREE: " + String(EEPROM_SIZE - offsetEEPROM - sizeof(MemoBank) - sizeof(storage)) + " byte", 5, 210);
  tft.drawString("EEPROM storage : " + String(sizeof(storage)) + " byte. Offset: " + String(offsetEEPROM), 5, 220);
  tft.drawString("EEPROM MemoBank: " + String(sizeof(MemoBank)) + " byte. Offset: " + String(offsetMemoEEPROM), 5, 230);

  while (x == 0 and (elapsedSaver + 120000) > millis()) {
    presStat = tft.getTouch(&x, &y);
  }
  x = y = 0;
  Beep(1, 0);
  delay(400);
  elapsedSaver = millis();
}

void showRDSStation() {

  if ((FirstLayer) or (ThirdLayer)) {
    tft.setCursor(XFreqDispl + 80, YFreqDispl + 58);
    tft.print(stationName);
  }
  delay(150);
}

void printWrapped(String text, int x, int y, int maxWidth, int lineHeight) {
  int start = 0;
  int lineY = y;
  int len = text.length();
  while (start < len) {
    int end = start;
    int lastSpace = -1;
    while (end < len) {
      if (tft.textWidth(text.substring(start, end + 1)) > maxWidth and end > start) break;
      if (text.charAt(end) == ' ') lastSpace = end;
      end++;
    }
    int breakPoint;
    int skip = 0;
    if (end >= len) {
      breakPoint = len;
    } else if (lastSpace > start) {
      breakPoint = lastSpace;
      skip = 1;
    } else {
      breakPoint = end;
    }
    tft.setCursor(x, lineY);
    tft.print(text.substring(start, breakPoint));
    start = breakPoint + skip;
    lineY += lineHeight;
  }
}

void showrdsMsg() {

  if ((FirstLayer) or (ThirdLayer)) {
    printWrapped(rdsMsg, 0, YFreqDispl + 75, 235, 8);
  }
  delay(100);
}

void showRDSTime() {

if ((FirstLayer) or (ThirdLayer)) {
    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
    tft.setTextDatum(BC_DATUM);
    tft.setCursor(0, YFreqDispl + 59);
    calcRDSTime();
    tft.print(rdsTime);
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
    tft.setTextDatum(BC_DATUM);
    tft.setCursor(0, YFreqDispl + 50);
    tft.print(" Time");
       delay(100);
  }
 }

void calcRDSTime() {

  if (strlen(rdsTime) > 10) {
    int gmtHour = 0;
    int gmtMinute = 0;
    int gmtHourOffset = 0;
    int gmtMinuteOffset = 0;
    gmtHour = gmtHour + ((rdsTime[0] - 48) * 10);
    gmtHour = gmtHour + (rdsTime[1] - 48);
    gmtMinute = gmtMinute + ((rdsTime[3] - 48) * 10);
    gmtMinute = gmtMinute + (rdsTime[4] - 48);
    gmtHourOffset = gmtHourOffset + ((rdsTime[7] - 48) * 10);
    gmtHourOffset = gmtHourOffset + (rdsTime[8] - 48);
    gmtMinuteOffset = gmtMinuteOffset + ((rdsTime[10] - 48) * 10);
    gmtMinuteOffset = gmtMinuteOffset + (rdsTime[11] - 48);
    if (rdsTime[6] == '-') {
      gmtHour = gmtHour + gmtHourOffset;
      if (gmtHour > 23) gmtHour = gmtHour - 24;
      gmtMinute = gmtMinute + gmtMinuteOffset;
    } else {
      gmtHour = gmtHour - gmtHourOffset;
      if (gmtHour < 0) gmtHour = gmtHour + 24;
      gmtMinute = gmtMinute - gmtMinuteOffset;
    }
    rdsTime[0] = (trunc(gmtHour / 10)) + 48;
    rdsTime[1] = gmtHour - (trunc(gmtHour / 10) * 10) + 48;
    rdsTime[3] = (trunc(gmtMinute / 10)) + 48;
    rdsTime[4] = gmtMinute - (trunc(gmtMinute / 10) * 10) + 48;
    rdsTime[5] = '\0';
  }
}

void checkRDS() {

  si4735.getRdsStatus();
  if (si4735.getRdsReceived()) {
    if (si4735.getRdsSync() && si4735.getRdsSyncFound() ) {
      stationName = si4735.getRdsText0A();
      rdsMsg = si4735.getRdsText2A();
      rdsTime = si4735.getRdsTime();
      tft.setTextSize(2);
      if (PRESbut) tft.setTextColor(TFT_CYAN, TFT_BLACK); else tft.setTextColor(TFT_CYAN, TFT_BLACK);
      tft.setTextDatum(BC_DATUM);
      if ( stationName != NULL )   showRDSStation();

     tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextDatum(BC_DATUM);

      if ( rdsMsg != NULL )   showrdsMsg();

       tft.setTextSize(1);
      tft.setTextColor(TFT_BLACK, TFT_BLACK);
      tft.setTextDatum(BC_DATUM);

    }
  }
}

void Segment(String freq, String mask, int d) {

 if (Saver) {
  spr.createSprite(140, 38);
  spr.fillScreen(COLOR_BACKGROUND);
  spr.setTextSize(1);
  spr.setTextPadding(0);
  spr.setFreeFont(&DSEG7_Classic_Mini_Regular_34);
  spr.setTextDatum(BR_DATUM);
  spr.setTextColor(saverColor);
  spr.drawString(freq, 140, 38);
  spr.pushSprite(saverX, saverY);
 } else {
  if (!digitLigth) mask = "";
  if (SEEK) {
    spr.createSprite(194, 38);
    d = 46;
  } else {
    if (RETRObut) {
      if (RETROband) spr.createSprite(150, 38); else spr.createSprite(110, 38);
    } else {
#ifdef IhaveSI5351
      spr.createSprite(240, 38);
#endif
#ifdef IhaveCrystal
      if (bfoOn) spr.createSprite(110, 38); else spr.createSprite(240, 38);
#endif
    }
  }
  spr.fillScreen(COLOR_BACKGROUND);
  spr.setTextSize(1);
  spr.setTextPadding(0);
  spr.setFreeFont(&DSEG7_Classic_Mini_Regular_34);
  spr.setTextDatum(BR_DATUM);
  int x = 222;
  if (bfoOn and !RETRObut) {
#ifdef IhaveCrystal
    x = 110;
#endif
    spr.setTextColor(TFT_BROWN);
    spr.drawString(mask, x, 38);
    spr.setTextColor(TFT_ORANGE);
    spr.drawString(freq, x, 38);
  } else {
    if ((currentMode == AM || currentMode == FM) and !RETRObut) x = 190;
    if (SEEK) x = 144;
    if (RETRObut) {
      if (RETROband) x = 150; else x = 110;
    }
    if (bfoOn) spr.setTextColor(TFT_BROWN); else spr.setTextColor(TFT_DARKCYAN);
    spr.drawString(mask, x, 38);
    if (PREtap) {
      spr.setTextColor(TFT_LIGTHYELLOW);
    } else {
      if (bfoOn) spr.setTextColor(TFT_ORANGE); else spr.setTextColor(COLOR_INDICATOR_FREQ);
    }
    spr.drawString(freq, x, 38);
  }
  if (RETRObut) {
    if (screenV) {
      if (RETROband) spr.pushSprite(15, 200); else spr.pushSprite(50, 200);
    } else spr.pushSprite(125, -3);
  } else spr.pushSprite(XFreqDispl + d, YFreqDispl + 20);
 }
 spr.setFreeFont(NULL);
 spr.deleteSprite();
}

void FreqDispl() {

  if (FirstLayer or ThirdLayer or SecondLayer) {
    int d = 0;
    if ((SCANbut or HamBand or Modebut or STEPbut or BandWidth or MEMObut) and !screenV) d = 40;
    currentFrequency = si4735.getFrequency();
    if (!FREQbut and !HamBand and !Modebut and !BandWidth and !BroadBand and !SCANbut and !MEMObut and !STEPbut) {
      AGCfreqdisp();
      BFOStepdisp();
    }
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(4);
    tft.setTextDatum(BC_DATUM);
    if ((VOLbut) or (AGCgainbut) or (SQUELCHbut)) {
      int y = 40;
      if ((currentMode == LSB || currentMode == USB  || currentMode == CW ) and (FirstLayer or ThirdLayer)) y = 48;
      if (volDisp) tft.fillRect( XFreqDispl + d + 40, YFreqDispl + 20 , 55, y, TFT_BLACK); else tft.fillRect( XFreqDispl + d, YFreqDispl + 20 , 240, y, TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(3);

      if (SQUELCHbut) {
        tft.drawString(String(currentSquelch), XFreqDispl + 80 + d, YFreqDispl + 53);
        tft.setTextSize(2);
        tft.drawString(" SQUELCH dB", XFreqDispl + 160 + d, YFreqDispl + 53);
      }

      if (VOLbut) {
        tft.drawString(String(currentVOL), XFreqDispl + 84 + d, YFreqDispl + 53);
        tft.setTextSize(2);
        tft.drawString("VOLUME", XFreqDispl + 160 + d, YFreqDispl + 53);
      }
      if (AGCgainbut) {
        tft.drawString(String(currentAGCgain), XFreqDispl + 60, YFreqDispl + 53);
        tft.setTextSize(2);
        tft.drawString("RF ATT", XFreqDispl + 160, YFreqDispl + 53);
      }
      volDisp = true;
    } else {
      volDisp = false;
      if (currentMode == LSB || currentMode == USB  || currentMode == CW ) {

       Displayfreq = (currentFrequency * 1000) - (band[bandIdx].lastBFO);
       if (CWShift) Displayfreq = Displayfreq + 700;
       int mhz = trunc(Displayfreq / 1000000);
       int khz = Displayfreq - (mhz * 1000000);
       khz = trunc(khz / 1000);
       int hz = Displayfreq - (mhz * 1000000) - (khz * 1000);
       char s[12] = {'\0'};
       if (mhz > 0) sprintf(s, "%i %03i.%02i", mhz, khz, hz / 10); else sprintf(s, "%i.%02i", khz, hz / 10);

       if (!bfoOn or bfoTr) {
        tft.setTextDatum(BR_DATUM);
        tft.setTextColor(COLOR_INDICATOR_FREQ, COLOR_BACKGROUND);
        if (bfoTr) {
          bfoTr = false;
          for (int i = 4; i > 1; i--) {
            if (bfoOn) tft.setTextSize(i); else tft.setTextSize(6 - i);
            tft.fillRect( XFreqDispl + d, YFreqDispl + 20 , 240, 48, TFT_BLACK);
            tft.drawString(String(s), XFreqDispl + 230 + d, YFreqDispl + 62);
            delay(100);
          }
        }
        if (!bfoOn) Segment(String(s), "88 888.88", d);
        tft.setTextSize(2);
        if (FREQbut or HamBand or Modebut or BandWidth or BroadBand or SCANbut or MEMObut or STEPbut) {
          tft.fillRect(XFreqDispl + d, YFreqDispl + 60, 240, 20, TFT_GREY);
          tft.setTextColor(TFT_YELLOW, TFT_GREY);
          tft.drawString("KHz", XFreqDispl + 234 + d, YFreqDispl + 78);
        } else {
          tft.setTextColor(TFT_YELLOW, TFT_BLACK);
          tft.drawString("KHz", XFreqDispl + 229 + d, YFreqDispl + 84);
        }
        if (bfoOn == false and (FREQbut or HamBand or Modebut or BandWidth or BroadBand or SCANbut or MEMObut) == false) {
          tft.fillRect(XFreqDispl + 141 + d, YFreqDispl + 60, 81, 5, TFT_BLACK);
          if (freqstepnr == 0)  tft.fillRect(XFreqDispl + 141 + d, YFreqDispl + 60, 21, 5, TFT_ORANGE);
          if (freqstepnr == 1)  tft.fillRect(XFreqDispl + 171 + d, YFreqDispl + 60, 21, 5, TFT_ORANGE);
          if (freqstepnr == 2)  tft.fillRect(XFreqDispl + 200 + d, YFreqDispl + 60, 21, 5, TFT_ORANGE);
        }
       }
        if (bfoOn) {
#ifdef IhaveCrystal
          Segment(String(currentBFOmanu), "-888", d);
          tft.setTextSize(2);
          tft.setTextDatum(BL_DATUM);
          tft.setTextColor(TFT_ORANGE, TFT_BLACK);
          tft.drawString("Hz", XFreqDispl + 120 + d, YFreqDispl + 40);
          tft.setTextColor(TFT_BLACK, TFT_ORANGE);
          tft.fillRect(XFreqDispl + 156 + d, YFreqDispl + 21, 42, 20, TFT_ORANGE);
          tft.drawString("BFO", XFreqDispl + 160 + d, YFreqDispl + 40);
          tft.setTextDatum(BR_DATUM);
          if (PREtap) tft.setTextColor(TFT_LIGTHYELLOW, COLOR_BACKGROUND); else tft.setTextColor(COLOR_INDICATOR_FREQ, COLOR_BACKGROUND);
          tft.drawString(String(s), XFreqDispl + 230 + d, YFreqDispl + 62);
#endif
#ifdef IhaveSI5351
          float temp = FreqSI5351 + calibratvalSI5351;
          Segment(String((temp / 100), 2), "88 888.88", d);
          if (stepsizesynth == 10)  tft.fillRect(XFreqDispl + 171 + d, YFreqDispl + 60, 21, 5, TFT_ORANGE);
          if (stepsizesynth ==  1)  tft.fillRect(XFreqDispl + 200 + d, YFreqDispl + 60, 21, 5, TFT_ORANGE);
#endif
        }
        tft.setTextDatum(BC_DATUM);
      } else {
        FreqDraw(currentFrequency, d);
        if (FREQbut or HamBand or Modebut or BandWidth or BroadBand or SCANbut or MEMObut or STEPbut) tft.fillRect(XFreqDispl + d, YFreqDispl + 60, 240, 20, TFT_GREY);
      }
    }
  }
}

void FreqDraw (float freq, int d)  {

  tft.fillRect( XFreqDispl + 46 + d, YFreqDispl + 20 , 194, 48, TFT_BLACK);
  if (currentMode == FM) {
    Displayfreq =  freq / 100;
    Segment(String(Displayfreq, 2), "188.88", d);
    tft.setTextDatum(BC_DATUM);
    tft.setTextSize(2);
    if (Saver) {
      tft.setTextColor(TFT_DARKCYAN, TFT_BLACK);
      tft.drawString("MHz", saverX + 165, saverY + 38);
    } else {
      tft.setTextColor(TFT_YELLOW, TFT_BLACK);
      tft.drawString("MHz", XFreqDispl + 215 + d, YFreqDispl + 60);
    }
  } else {
    if (band[bandIdx].bandType == MW_BAND_TYPE || band[bandIdx].bandType == LW_BAND_TYPE) {
      Displayfreq =  freq;
      Segment(String(Displayfreq, 0), "1888", d);
      tft.setTextDatum(BC_DATUM);
      tft.setTextSize(2);
      if (Saver) {
        tft.setTextColor(TFT_DARKCYAN, TFT_BLACK);
        tft.drawString("MHz", saverX + 165, saverY + 38);
      } else {
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawString("KHz", XFreqDispl + 215 + d, YFreqDispl + 60);
      }
    } else {
      Displayfreq =  freq / 1000;
      Segment(String(Displayfreq, 3), "88.888", d);
      tft.setTextDatum(BC_DATUM);
      tft.setTextSize(2);
      if (Saver) {
        tft.setTextColor(TFT_DARKCYAN, TFT_BLACK);
      tft.drawString("MHz", saverX + 165, saverY + 38);
      } else {
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawString("MHz", XFreqDispl + 215 + d, YFreqDispl + 60);
      }
    }
  }
}

bool checkStopSeeking() {

  return (bool) encoderCount || tft.getTouch(&x, &y) || analogRead(ENCODER_SWITCH) < 500;
}

void SeekFreq (uint16_t freq)  {

  FreqDraw(float(freq), 0);
}

void DrawDispl() {

  tft.fillRect(XFreqDispl, YFreqDispl, 240, 86, TFT_BLACK);

  tft.setTextSize(1);
  tft.setTextDatum(BC_DATUM);

  tft.setTextColor(2031, TFT_BLACK);
  tft.drawString(band[bandIdx].bandName, XFreqDispl + 180, YFreqDispl + 15);
  tft.drawRect(XFreqDispl + 160, YFreqDispl + 2, 39, 16, 2031);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  Modtext = bandModeDesc[currentMode];
  if ((Modtext == "USB") and (CWShift == true)) Modtext = "CW";
  tft.drawString(Modtext, XFreqDispl + 95, YFreqDispl + 15);
  tft.drawRect(XFreqDispl + 80, YFreqDispl + 2, 29, 16, TFT_YELLOW);

  if (currentMode == AM) BWtext = bandwidthAM[bwIdxAM];
  if (currentMode == LSB || currentMode == USB || currentMode == CW) BWtext = bandwidthSSB[bwIdxSSB];
  if (currentMode == FM) BWtext = bandwidthFM[bwIdxFM];
  tft.setTextColor(64799, TFT_BLACK);
  if (BWtext == "AUTO") {
    tft.drawString("F AUTO", XFreqDispl + 135, YFreqDispl + 15);
  } else tft.drawString("F" + BWtext + "KHz", XFreqDispl + 135, YFreqDispl + 15);
  tft.drawRect(XFreqDispl + 110, YFreqDispl + 2, 49, 16, 64799);

  tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  if (currentMode == FM) {
    tft.drawString(String((band[bandIdx].currentStep) * 10) + "KHz", XFreqDispl + 220, YFreqDispl + 15);
  } else  tft.drawString(String(band[bandIdx].currentStep) + "KHz", XFreqDispl + 220, YFreqDispl + 15);
  tft.drawRect(XFreqDispl + 200, YFreqDispl + 2, 39, 16, TFT_SKYBLUE);

  FreqDispl();
}

void AGCfreqdisp() {

  uint16_t col = TFT_SILVER;
  if (AGCgain) col = 64528;
  tft.setTextSize(1);
  tft.setTextColor(col, TFT_BLACK);
  tft.setTextDatum(BC_DATUM);
  if (AGCgain > 1) {
    if (currentAGCgain > 9) tft.drawString("ATT" + String(currentAGCgain), XFreqDispl + 60, YFreqDispl + 15); else tft.drawString("ATT " + String(currentAGCgain), XFreqDispl + 60, YFreqDispl + 15);
  } else tft.drawString(" AGC ", XFreqDispl + 60, YFreqDispl + 15);
  tft.drawRect(XFreqDispl + 40, YFreqDispl + 2, 39, 16, col);
}

void BFOStepdisp() {

  uint16_t col = TFT_SILVER;
  if ((currentMode == LSB || currentMode == USB || currentMode == CW) and currentBFOmanu) col = TFT_ORANGE;
  tft.setTextSize(1);
  tft.setTextColor(col, TFT_BLACK);
  tft.setTextDatum(BC_DATUM);
  if (bfoOn) {
#ifdef IhaveCrystal
    tft.drawString(String(currentBFOStep) + " Hz", XFreqDispl + 20, YFreqDispl + 15);
#endif
  } else {
    tft.drawString(" BFO ", XFreqDispl + 20, YFreqDispl + 15);
  }
  tft.drawRect(XFreqDispl, YFreqDispl + 2, 39, 16, col);
}

void ErrorBeep()  {

  Beep(2, 100);
}

void MuteAudOn() {

  si4735.setHardwareAudioMute(1);
  AudioMut = true;
  elapsedAudMut = millis();
}

void MuteAud() {

  if (((millis() - elapsedAudMut) > MIN_ELAPSED_AudMut_TIME ) and (AudioMut = true))
  {
    AudioMut = false;
    si4735.setHardwareAudioMute(0);
  }
}

void showFirmwareInformation() {

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  int xp;
  int yp;
  if (screenV) {
    tft.setCursor(0, 20);
    tft.println("Firmware Information");
    tft.setCursor(0, 35);
    tft.println("====================");
    xp = 35;
    yp = 50;
  } else {
    tft.setCursor(40, 20);
    tft.println("Firmware Information");
    tft.setCursor(40, 35);
    tft.println("====================");
    xp = 75;
    yp = 50;
  }
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(xp, yp + 20);
  tft.print("Part Number (HEX)........: ");
  tft.println(si4735.getFirmwarePN(), HEX);
  tft.setCursor(xp, yp + 40);
  tft.print("Firmware Major Revision..: ");
  tft.println(si4735.getFirmwareFWMAJOR());
  tft.setCursor(xp, yp + 60);
  tft.print("Firmware Minor Revision..: ");
  tft.println(si4735.getFirmwareFWMINOR());
  tft.setCursor(xp, yp + 80);
  tft.print("Patch ID ................: ");
  tft.print(si4735.getFirmwarePATCHH(), HEX);
  tft.println(si4735.getFirmwarePATCHL(), HEX);
  tft.setCursor(xp, yp + 100);
  tft.print("Component Major Revision.: ");
  tft.println(si4735.getFirmwareCMPMAJOR());
  tft.setCursor(xp, yp + 120);
  tft.print("Component Minor Revision.: ");
  tft.println(si4735.getFirmwareCMPMINOR());
  tft.setCursor(xp, yp + 140);
  tft.print("Chip Revision............: ");
  tft.println(si4735.getFirmwareCHIPREV());

  while (x == 0 and (elapsedSaver + 120000) > millis()) {
    presStat = tft.getTouch(&x, &y);
  }
  x = y = 0;
  Beep(1, 0);
  delay(400);
  elapsedSaver = millis();
}

void pauseSCAN() {

  scanStopUntil = 0;
  int d = screenV * 40;
  if (SCANpause) {
    si4735.setAudioMute(audioMuteOff);
    if (band[bandIdx].bandType == MW_BAND_TYPE or band[bandIdx].bandType == LW_BAND_TYPE) si4735.setFrequencyStep(ssIdxMW);
    else if (currentMode == FM) si4735.setFrequencyStep(ssIdxFM);
    else si4735.setFrequencyStep(ssIdxAM);
    setFreq(currentScanFreq + int((currentScanLine - 159 + d + deltaScanLine) * SCANstep));
    AGCgain = ScanAGC;
    checkAGC();
    drawButton(L_SCAN, 1, B_JAM);
  } else {
    si4735.setAudioMute(audioMuteOn);
    si4735.setFrequencyStep(1);
    posScanFreq = currentScanFreq + int((deltaScanLine - 159 + d + posScan) * SCANstep);
    setFreq(posScanFreq);
    if (currentMode == LSB or currentMode == USB or currentMode == CW) band[bandIdx].lastBFO = currentBFO = 0;
    AGCgain = 0;
    checkAGC();
    drawButton(L_SCAN, 1, B_NORMAL);
  }
}

void DisplaySCANsignal() {

  int d = screenV * 40;
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_GREY);
  tft.setTextDatum(BL_DATUM);
  if (SCANpause) tft.drawString("RSSI " + String(si4735.getCurrentRSSI()) + " dBuV ", 130 - d, 70); else tft.drawString("RSSI " + String((198 - ScanValueRSSI[int(currentScanLine)]) / signalScale, 0) + " dBuV ", 130 - d, 70);
  tft.setTextColor(TFT_ORANGE, TFT_GREY);
  if (SCANpause) tft.drawString(" SNR " + String(si4735.getCurrentSNR()) + " dB ", 130 - d, 80); else tft.drawString(" SNR " + String(ScanValueSNR[int(currentScanLine)]) + " dB ", 130 - d, 80);
}

void drawSCAN() {

  setFreq(currentScanFreq + int((currentScanLine - 159 + (screenV * 40) + deltaScanLine) * SCANstep));
  FreqDispl();
  drawList(L_SCAN, String(band[bandIdx].bandName) + " BAND SCANNER");
  if ((currentMode == LSB || currentMode == USB || currentMode == CW) and bfoOn == false) drawButton(L_SCAN, 2, B_BLOCK);

  DrawSCANind();
}

uint16_t heatColor565(uint8_t v) {
  uint8_t r, g, b;
  if (v < 64) {
    r = 0; g = 0; b = v;
  } else if (v < 128) {
    uint8_t f = v - 64;
    r = 0; g = f * 2; b = 64 - f / 2;
  } else if (v < 192) {
    uint8_t f = v - 128;
    r = f * 4; g = 128 + f / 2; b = 0;
  } else {
    uint8_t f = v - 192;
    r = 255; g = 255 - f * 4; b = 0;
  }
  return tft.color565(r, g, b);
}

void commitWaterfallRow() {
  waterfallHead = (waterfallHead + 1) % WF_ROWS;
  for (int x = 0; x < 320; x++) {
    int v = ScanValueSNR[x] * 6;
    if (v < 0) v = 0; if (v > 255) v = 255;
    waterfallBuf[waterfallHead][x] = (uint8_t) v;
  }
}

void drawSCANwaterfall() {
  int d = screenV * 40;
  int width = 320 - (screenV * 80);
  int top = scanSplitY(d) + 2, bottom = 198 + d;
  int rowH = 3;
  int rows = (bottom - top) / rowH;
  if (rows > WF_ROWS) rows = WF_ROWS;
  static uint16_t rowBuf[320];
  for (int r = 0; r < rows; r++) {
    int bufIdx = (waterfallHead - r + WF_ROWS) % WF_ROWS;
    for (int x = 0; x < width; x++) rowBuf[x] = heatColor565(waterfallBuf[bufIdx][x]);
    int y = top + r * rowH;
    for (int yy = 0; yy < rowH and (y + yy) < bottom; yy++) tft.pushImage(0, y + yy, width, 1, rowBuf);
  }
}

int scanSplitY(int d40) {
  return 81 + ((198 + d40) - 81) / 2;
}

void drawSCANgraf(bool erase) {

  int d = screenV * 80;
  if (erase) {
    ScanEmpty = true;
    posScanFreq = currentScanFreq + int((deltaScanLine - 159 + d) * SCANstep);
    for (int i = 0; i < 320; i++) ScanMark[i] = false;
    if (scanWaterfallOn) {
      for (int r = 0; r < WF_ROWS; r++) for (int x = 0; x < 320; x++) waterfallBuf[r][x] = 0;
      waterfallHead = 0;
      lastWaterfallCommit = millis();
    }
  }
  ScanBeginBand = -1;
  ScanEndBand = 320 - d;
  int d40 = screenV * 40;
  int splitY = scanSplitY(d40);
  for (int n = 0; n < (320 - d); n++) {
    if (erase) {
      ScanValueRSSI[n] = 198 + (d / 2);
      ScanPeakRSSI[n] = 198 + (d / 2);
      ScanValueSNR[n] = 0;
    }
    ScanScaleLine[n] = 0;
    if (scanWaterfallOn) drawSCANlineCompact(n, d40, 81, splitY);
    else drawSCANline(n);
  }
  if (scanWaterfallOn) drawSCANwaterfall();
}

void drawSCANlineCompact(int n, int d, int graphTop, int graphBottom) {
  updateScanBandEdges(n, d);
  int fullBottom = 198 + d;
  int fullTop = 81;
  int hFull = fullBottom - fullTop;
  int hHalf = graphBottom - graphTop;

  if (n == int(currentScanLine)) {
    tft.drawLine(n, graphTop, n, graphBottom, TFT_RED);
    DisplaySCANsignal();
    return;
  }

  int16_t colf = TFT_NAVY;
  if (ScanValueSNR[n] > 0) {
    colf = TFT_NAVY + 0x8000;
    if (ScanValueSNR[n] < 16) colf += (ScanValueSNR[n] * 2048);
    else { colf = 0xF810; if (ScanValueSNR[n] < 24) colf += ((ScanValueSNR[n] - 16) * 258); else colf = 0xFF1E; }
  }
  long frq = currentScanFreq + ((n - 159 + d + deltaScanLine) * SCANstep);
  if (frq > band[bandIdx].maximumFreq or frq < band[bandIdx].minimumFreq) colf = TFT_GREY;

  int rawV = ScanValueRSSI[n];
  if (rawV < fullTop) rawV = fullTop;
  int y = graphTop + (long)(rawV - fullTop) * hHalf / hFull;
  tft.drawLine(n, y + 1, n, graphBottom, colf);
  tft.drawLine(n, graphTop, n, y - 1, TFT_BLACK);
  tft.drawPixel(n, y, TFT_SILVER);

  int rawP = ScanPeakRSSI[n];
  if (rawP < fullBottom and rawP < rawV - 1) {
    int py = graphTop + (long)(rawP - fullTop) * hHalf / hFull;
    tft.drawPixel(n, py, TFT_DARKGREY);
  }

  if (ScanMark[n]) tftTransRect(n, graphBottom - 6, 1, 5, TFT_YELLOW);

  if (showHamSegments) {
    int8_t seg = hamSegmentType(frq);
    if (seg == 0) tft.drawPixel(n, graphTop, TFT_CYAN);
    else if (seg == 1) tft.drawPixel(n, graphTop, TFT_MAGENTA);
    else if (seg == 2) tft.drawPixel(n, graphTop, TFT_GREEN);
  }
}

void updateScanBandEdges(int n, int d) {
  long frqCheck = currentScanFreq + ((n - 159 + d + deltaScanLine) * SCANstep);
  if (frqCheck > band[bandIdx].maximumFreq) {
    if (ScanEndBand == (320 - (d * 2))) ScanEndBand = n;
  }
  if (frqCheck < band[bandIdx].minimumFreq) {
    if (ScanBeginBand < n) ScanBeginBand = n;
  }
}

void drawSCANline(int n) {

  int d = screenV * 40;

  int frq = currentScanFreq + ((n - 159 + d + deltaScanLine) * SCANstep);
  bool tmpLine = false;

  if (n == int(currentScanLine)) {
    tft.drawLine(int(currentScanLine), 81, int(currentScanLine), 198 + d, TFT_RED);
    DisplaySCANsignal();
  } else {
    int16_t colf = TFT_NAVY;
    int16_t colb = TFT_BLACK;

    if (!ScanScaleLine[n]) {
      if (SCANstep > 4) {
        if ((frq - (int(frq / 1000) * 1000)) < SCANstep) ScanScaleLine[n] = 2;
      } else {
        if ((frq - (int(frq / 100) * 100)) < SCANstep) {
          if (!prevScaleLine) ScanScaleLine[n] = 2;
          if (SCANstep < 2) tmpLine = true;
        }
      }
    }
    if (ScanScaleLine[n] == 2) {
      colf = TFT_BLACK;
      colb = TFT_OLIVE;
    }

    if (ScanValueSNR[n] > 0) {
      colf = TFT_NAVY + 0x8000;
      if (ScanValueSNR[n] < 16) {
        colf += (ScanValueSNR[n] * 2048);
      } else {
        colf = 0xF810;
        if (ScanValueSNR[n] < 24) colf += ((ScanValueSNR[n] - 16) * 258); else colf = 0xFF1E;
      }
    }

    updateScanBandEdges(n, d);
    if ((currentScanFreq + ((n - 159 + d + deltaScanLine) * SCANstep)) > band[bandIdx].maximumFreq) {
      colf = TFT_GREY;
    }
    if ((currentScanFreq + ((n - 159 + d + deltaScanLine) * SCANstep)) < band[bandIdx].minimumFreq) {
      colf = TFT_GREY;
    }

    int tmpValue = ScanValueRSSI[n];
    if (tmpValue < 82) tmpValue = 82;
    tft.drawLine(n, tmpValue + 1, n, 198 + d, colf);
    tft.drawLine(n, 81, n, tmpValue - 1, colb);

    if (ScanPeakRSSI[n] < (198 + d) and ScanPeakRSSI[n] < tmpValue - 1) {
      tft.drawPixel(n, ScanPeakRSSI[n], TFT_DARKGREY);
    }

    if (!ScanScaleLine[n]) {
      if (SCANstep <= 2){
        if (SCANstep == 2) {
          if ((frq + 50 - (int((frq + 50) / 100) * 100)) < 2) ScanScaleLine[n] = 3;
        } else {
          if ((frq + 50 - (int((frq + 50) / 100) * 100)) == 0) {
            if (!prevScaleLine) ScanScaleLine[n] = 3;
            tmpLine = true;
          }
        }
      }
    }
    if (ScanScaleLine[n] == 3) tft.drawLine(n, 81, n, 95, TFT_OLIVE);

    if (!ScanScaleLine[n]) {
      if (SCANstep < 2){
        for (int i = 10; i < 50; i +=10) {
          if ((frq + i - (int((frq + i) / 100) * 100)) == 0) {
            if (!prevScaleLine) ScanScaleLine[n] = 4;
            tmpLine = true;
          }
          if ((frq + i + 50 - (int((frq + i + 50) / 100) * 100)) == 0) {
            if (!prevScaleLine) ScanScaleLine[n] = 4;
            tmpLine = true;
          }
        }
      }
    }
    if (ScanScaleLine[n] == 4) tft.drawLine(n, 81, n, 88, TFT_OLIVE);

    prevScaleLine = tmpLine;
    if (!ScanScaleLine[n]) ScanScaleLine[n] = 1;

    int tmpValuePrev = ScanValueRSSI[n - 1];
    if (tmpValuePrev < 82) tmpValuePrev = 82;
    if (n == 0 or n == int(currentScanLine) + 1) tft.drawPixel(n, tmpValue, TFT_SILVER); else tft.drawLine(n - 1, tmpValuePrev, n, tmpValue, TFT_SILVER);
  }

  if (ScanMark[n]) tftTransRect(n, 95, 1, 5, TFT_YELLOW);

  if (showHamSegments) {
    int8_t seg = hamSegmentType(frq);
    if (seg == 0) tft.drawLine(n, 81, n, 83, TFT_CYAN);
    else if (seg == 1) tft.drawLine(n, 81, n, 83, TFT_MAGENTA);
    else if (seg == 2) tft.drawLine(n, 81, n, 83, TFT_GREEN);
  }
}

void DisplaySCAN() {

  int d = screenV * 40;
  bool setf = false;
  posScan = (int(posScanFreq - currentScanFreq) / SCANstep) + 159 - d - deltaScanLine;
  if (posScan < 0) posScan = 0;
  if (posScan >= ScanEndBand and !ScanEmpty) {
    posScan = 0;
    setf = true;
  }
  if (posScan > (319 - (d * 2))) {
    posScan = 0;
    setf = true;
    ScanEmpty = false;
  }
  if (posScan < 0) {
    posScan = 0;
    setf = true;
  }
  if (posScan <= ScanBeginBand and !ScanEmpty) {
    posScan = ScanBeginBand +1;
    setf = true;
  }
  if (scanWaterfallOn and !Saver) {
    if (millis() - lastWaterfallCommit > WF_COMMIT_MS) {
      lastWaterfallCommit = millis();
      commitWaterfallRow();
      drawSCANwaterfall();
      DrawSCANtxt(true);
    }
  }
  if (setf) {
    bool closeCallHandled = false;
    if (closeCallOn and !scanStopUntil) {
      int bestIdx = -1, bestVal = 9999;
      for (int i = ScanBeginBand + 1; i < ScanEndBand; i++) {
        long fq = currentScanFreq + (long)((deltaScanLine - 159 + d + i) * SCANstep);
        if (ScanPeakRSSI[i] < bestVal and !isFreqSkipped(fq)) { bestVal = ScanPeakRSSI[i]; bestIdx = i; }
      }
      if (bestIdx >= 0 and bestVal < (198 + d)) {
        currentScanLine = bestIdx;
        posScanFreq = currentScanFreq + int((deltaScanLine - 159 + d + bestIdx) * SCANstep);
        setFreq(posScanFreq);
        si4735.setAudioMute(audioMuteOff);
        SCANpause = true;
        pauseSCAN();
        logDiscovery(posScanFreq, ScanValueSNR[bestIdx]);
        closeCallHandled = true;
      }
    }
    if (!closeCallHandled) setFreq(currentScanFreq + int((deltaScanLine - 159 + d + posScan) * SCANstep));
  } else {
    if (posScan == posScanLast) ScanValueRSSI[posScan] = (ScanValueRSSI[posScan] + getSignal(true)) / 2; else ScanValueRSSI[posScan] = getSignal(true);
    if (ScanValueRSSI[posScan] < ScanPeakRSSI[posScan]) ScanPeakRSSI[posScan] = ScanValueRSSI[posScan];
    if (posScan == posScanLast) ScanValueSNR[posScan] = (ScanValueSNR[posScan] + getSignal(false)) / 2; else ScanValueSNR[posScan] = getSignal(false);
    long freqAtPos = currentScanFreq + (long)((deltaScanLine - 159 + d + posScan) * SCANstep);
    if (ScanValueSNR[posScan] >= ScanMarkSNR and posScan > ScanBeginBand and posScan < ScanEndBand and !isFreqSkipped(freqAtPos)) ScanMark[posScan] = true;
    if (!Saver) {
      if (scanWaterfallOn) drawSCANlineCompact(posScan, d, 81, scanSplitY(d));
      else drawSCANline(posScan);
      DrawSCANtxt(false);
    }
    if (scanStopOnSignal and !scanStopUntil and ScanValueSNR[posScan] >= ScanMarkSNR and posScan > ScanBeginBand and posScan < ScanEndBand and !isFreqSkipped(freqAtPos)) {
      si4735.setAudioMute(audioMuteOff);
      posScanFreq = currentScanFreq + int((deltaScanLine - 159 + d + posScan) * SCANstep);
      setFreq(posScanFreq);
      AGCgain = ScanAGC;
      checkAGC();
      scanStopUntil = millis() + ((unsigned long)scanStopSeconds * 1000UL);
      logDiscovery(freqAtPos, ScanValueSNR[posScan]);
      return;
    }
    if (SCANstep < 1) {
      for (int i = 1; i < 1 / SCANstep; i++) {
        posScan++;
        if (posScan < (320 - (d * 2))) {
          if ((currentMode == LSB or currentMode == USB or currentMode == CW) and !ScanEmpty) {
            si4735.setSSBBfo(1000 * SCANstep * i);
            ScanValueRSSI[posScan] = getSignal(true);
            ScanValueSNR[posScan] = getSignal(false);
          } else {
            ScanValueRSSI[posScan] = ScanValueRSSI[posScan - 1];
            ScanValueSNR[posScan] = ScanValueSNR[posScan - 1];
          }
          if (ScanValueRSSI[posScan] < ScanPeakRSSI[posScan]) ScanPeakRSSI[posScan] = ScanValueRSSI[posScan];
          if (ScanValueSNR[posScan] >= ScanMarkSNR) ScanMark[posScan] = true;
          if (!Saver) {
            if (scanWaterfallOn) drawSCANlineCompact(posScan, d, 81, scanSplitY(d));
            else drawSCANline(posScan);
            DrawSCANtxt(false);
          }
        }
      }
      if ((currentMode == LSB or currentMode == USB or currentMode == CW) and !ScanEmpty) si4735.setSSBBfo(0);
    }
    posScanLast = posScan;

    if (ScanEmpty) setFreq(currentScanFreq + int((deltaScanLine - 159 + d + posScan + 1) * SCANstep)); else freqUp();
  }
}

int getSignal(bool rssi) {

  int res = 0;
  for (int i = 0; i < countScanSignal; i++) {
    si4735.getCurrentReceivedSignalQuality();
    if (rssi) res += si4735.getCurrentRSSI(); else res += si4735.getCurrentSNR();
  }
  if (rssi) res = 198 + (screenV * 40) - ((res / countScanSignal) * signalScale); else res /= countScanSignal;
  return (int) res;
}

void setFreq(float f) {

  posScanFreq = f;
  si4735.setFrequency(f);
  if (currentMode == LSB or currentMode == USB or currentMode == CW) si4735.setAutomaticGainControl(1, 0);
}

void freqUp() {

  posScanFreq++;
  si4735.frequencyUp();
  if (currentMode == LSB or currentMode == USB or currentMode == CW) si4735.setAutomaticGainControl(1, 0);
}

void drawOutlinedString(String text, int x, int y, uint16_t color) {
  tft.setTextColor(TFT_BLACK);
  tft.drawString(text, x - 1, y - 1);
  tft.drawString(text, x + 1, y - 1);
  tft.drawString(text, x - 1, y + 1);
  tft.drawString(text, x + 1, y + 1);
  tft.drawString(text, x, y - 1);
  tft.drawString(text, x, y + 1);
  tft.drawString(text, x - 1, y);
  tft.drawString(text, x + 1, y);
  tft.setTextColor(color);
  tft.drawString(text, x, y);
}

void DrawSCANtxt(bool all) {

  int d = screenV * 80;
  tft.setTextSize(1);
  if ((ScanEndBand < (315 - d)) and ((posScan > (ScanEndBand + 5)) and (posScan < (ScanEndBand + 45))) or all) {
    tft.setTextDatum(BL_DATUM);
    drawOutlinedString("END OF", ScanEndBand + 5, 120, TFT_SILVER);
    drawOutlinedString("BAND", ScanEndBand + 5, 130, TFT_SILVER);
    drawOutlinedString(band[bandIdx].bandName, ScanEndBand + 5, 140, TFT_SILVER);
  }
  if ((ScanBeginBand > 5) and ((posScan > (ScanBeginBand - 43)) and (posScan < (ScanBeginBand - 3))) or all) {
    tft.setTextDatum(BR_DATUM);
    drawOutlinedString("BEGIN", ScanBeginBand - 5, 120, TFT_SILVER);
    drawOutlinedString("BAND", ScanBeginBand - 5, 130, TFT_SILVER);
    drawOutlinedString(band[bandIdx].bandName, ScanBeginBand - 5, 140, TFT_SILVER);
  }
  if (posScan < 60 or all) {

    tft.setTextDatum(BL_DATUM);
    if (currentMode == FM) drawOutlinedString(String((currentScanFreq + (SCANstep * (deltaScanLine - 159 + (d / 2)))) / 100) + " MHz ", 0, 90, TFT_GREEN); else drawOutlinedString(String(int(currentScanFreq + (SCANstep * (deltaScanLine - 159 + (d / 2))))) + " KHz ", 0, 90, TFT_GREEN);

    if (currentMode == FM and SCANstep > 4) drawOutlinedString("10 MHz", 0, 195 + (d / 2), TFT_YELLOW);
    if ((currentMode == FM and SCANstep == 4) or (currentMode != FM and SCANstep > 4)) drawOutlinedString("1 MHz", 0, 195 + (d / 2), TFT_YELLOW);
    if (currentMode == FM and SCANstep == 2) drawOutlinedString("500 KHz", 0, 195 + (d / 2), TFT_YELLOW);
    if ((currentMode == FM and SCANstep < 2) or (currentMode != FM and SCANstep == 4)) drawOutlinedString("100 KHz", 0, 195 + (d / 2), TFT_YELLOW);
    if (currentMode != FM and SCANstep == 2) drawOutlinedString("50 KHz", 0, 195 + (d / 2), TFT_YELLOW);
    if (currentMode != FM and SCANstep < 2) drawOutlinedString("10 KHz", 0, 195 + (d / 2), TFT_YELLOW);
  }
  if (posScan > (240 - d) or all) {

    tft.setTextDatum(BR_DATUM);
    if (currentMode == FM) drawOutlinedString(" " + String((currentScanFreq + (SCANstep * (160 - (d / 2) + deltaScanLine))) / 100) + " MHz", 319 - d, 90, TFT_GREEN); else drawOutlinedString(" " + String(int(currentScanFreq + (SCANstep * (160 - (d / 2) + deltaScanLine)))) + " KHz", 319 - d, 90, TFT_GREEN);

    if (SCANstep >= 1) drawOutlinedString("1:" + String(int(SCANstep)), 319 - d, 195 + (d / 2), TFT_YELLOW); else drawOutlinedString("x" + String(int(1 / SCANstep)), 319 - d, 195 + (d / 2), TFT_YELLOW);
  }
}

void DrawSCANind() {

  int d = !screenV * 40;
  tft.fillRect(d, 60, 80, 20, TFT_GREY);
  tft.setTextSize(1);
  tft.setTextDatum(BC_DATUM);

  tft.drawRect(d, 64, 49, 13, TFT_SKYBLUE);
  tft.setTextColor(TFT_SKYBLUE, TFT_GREY);
  if (currentMode == FM) tft.drawString(String((band[bandIdx].currentStep) * 10) + " KHz", d + 25, 75); else  tft.drawString(String(band[bandIdx].currentStep) + " KHz", d + 25, 75);

  tft.drawRect(d + 50, 64, 29, 13, TFT_YELLOW);
  tft.setTextColor(TFT_YELLOW, TFT_GREY);
  Modtext = bandModeDesc[currentMode];
  if ((Modtext == "USB") and (CWShift == true)) Modtext = "CW";
  tft.drawString(Modtext, d + 65, 75);
}

void initRetro() {

  drawRETRO();
  if (bandIdx != bandRetro[RETROband].band or currentMode != bandMode[bandIdx]) {
    bandIdx = bandRetro[RETROband].band;
    currentMode = bandMode[bandIdx];
    BandSet();
  }
  if (bandRetro[RETROband].currentFreq < bandRetro[RETROband].minimumFreq or bandRetro[RETROband].currentFreq > bandRetro[RETROband].maximumFreq) bandRetro[RETROband].currentFreq = bandRetro[RETROband].minimumFreq;
  float freq = bandRetro[RETROband].currentFreq;
  if (bandIdx == 0) freq *= 100;
  si4735.setFrequency(freq);
  si4735.setFrequencyStep(bandRetro[RETROband].hardStep);
  band[bandIdx].currentStep = bandRetro[RETROband].hardStep;
  band[bandIdx].currentFreq = si4735.getFrequency();
}

void drawRETRO() {

  tft.fillScreen(COLOR_BACKGROUND);
  int d = screenV * 80;
  if (screenV) {
    for (int n = 1; n <= 20; n++) {
      tft.fillRect(40 - (n * 2), 200, 2, 40, ((int(n / 2) * 4096) + (n * 32)));
      tft.fillRect((n * 2) + 198, 200, 2, 40, ((int(n / 2) * 4096) + (n * 32)));
    }
  }

  tft.fillRect(0, 36, 320 - d, 4, TFT_LIGTHYELLOW);

  tftPlSetFont(T1516_T);
  tftPlSetSize(1);
  tftPlSetColor(TFT_OLIVE, TFT_TRANS);
  tftPlSetDatum(BL_T);
  tftPlSetStyle(NCB_T);
  tftPlSetCut(0, 10 + (d / 10));
  tftPlPrint(String(presetNameLoad()), 0, 18);
  tftPlSetCut(0, 0);
  if (tftPlLength(String(presetNameLoad())) > (10 + (d / 10))) {
    textScroll = 0;
    elapsedScroll = millis() + 3000;
  }

  tftPlSetColor(TFT_RED, TFT_TRANS);
  tftPlSetStyle(NBL_T);
  if (RETROband or screenV) {
    x = 284 - (d * 1.375);
    y = 16 + (d * 2.525);
  } else {
    x = 293;
    y = 18;
  }
  if (langRetroEN) tftPlPrint(String(bandRetro[RETROband].BandName), x, y); else tftPlPrint(String(bandRetro[RETROband].BandNamePL), x, y);

  tft.setTextSize(1);
  tft.setTextDatum(BL_DATUM);
  tft.setTextColor(TFT_LIGTHYELLOW, TFT_BLACK);
  if (!RETROband or screenV) {
    tft.drawString("STEREO", 250 - d, 32);
    tft.drawRect(295 - d, 22, 20, 11, TFT_LIGTHYELLOW);
  }

  tft.drawString("VOLUME", 0, 32);
  tft.drawLine(46, 21, 127, 21, TFT_DARKCYAN);
  drawRetroVol();

  tft.setTextSize(2);
  if (RETROband or screenV) {
    x = 284 - (d * 1.375);
    y = 35 + (d * 2.5);
  } else {
    x = 250;
    y = 20;
  }
  if (RETROband > 1) tft.drawString("KHz", x, y); else tft.drawString("MHz", x, y);

  for (int i = 0; i < 6; i++) {
    tft.fillRect(0, i * 12 + 51, 320 - d, 1, TFT_LIGTHYELLOW);
    tft.fillRect(0, i * 12 + 139, 320 - d, 1, TFT_LIGTHYELLOW);
  }

  drawButtons(L_RETRO);
}

void drawRetroVol() {

  int vol = map(si4735.getVolume(), MinVOL, MaxVOL, 0, 19);
  for (int i = 0; i < 20; i++) {
    int color = (31 - (abs(10 - i) * 2)) * 2113;
    if (i == vol) color = TFT_RED;
    tft.drawRect((i * 4) + 47, 24, 2, 7, color);
  }
}

void drawRETROscale() {

  int d = screenV * 80;

  Displayfreq = si4735.getFrequency();
  if (bandHamRetro) Displayfreq -= (currentBFO / 1000);
  if (bandIdx == 0) Displayfreq /= 100;
  if (RETROband == 0) Segment(String(Displayfreq, 1), "188.8", 0);
  else
  if (RETROband == 1) Segment(String(Displayfreq, 2), "88.88", 0);
  else
  if (RETROband == 2) Segment(String(Displayfreq, 0), "888", 0);
  else
  if (RETROband == 3) Segment(String(Displayfreq, 0), "1888", 0);
  else Segment(String(Displayfreq, 0), "88888", 0);

  int color = TFT_LIGTHYELLOW;
  if (band[bandIdx].currentStep == bandRetro[RETROband].softStep and bandRetro[RETROband].hardStep != bandRetro[RETROband].softStep) color = 0xFE10;
  if (band[bandIdx].currentStep != bandRetro[RETROband].softStep and band[bandIdx].currentStep != bandRetro[RETROband].hardStep) color = 0xFF14;
  tftPlSetFont(T1012_T);
  tftPlSetSize(1);
  tftPlSetColor(TFT_BLACK, TFT_TRANS);
  tftPlSetDatum(BC_T);
  tftPlSetStyle(NBL_T);

  float tmp = 160 - (d / 2);
  float tmpMark = currentRetroScale * bandRetro[RETROband].mark;
  while (tmp >= tmpMark) tmp -= tmpMark;
  float freq = (Displayfreq - (trunc(Displayfreq / bandRetro[RETROband].mark) * bandRetro[RETROband].mark)) * currentRetroScale;
  tft.fillRect(0, 112, 320 - d, 16, color);

  for (float i = tmp - freq - tmpMark; i < (320 - d + (tmpMark / 2)); i += tmpMark) {
    freq = Displayfreq + ((i - 160 + (d / 2)) / currentRetroScale);
    if (freq > bandRetro[RETROband].maximumFreq or freq < bandRetro[RETROband].minimumFreq) {
      for (int n = 0; n < (tmpMark / 2); n += 4) tft.fillRect(i + n, 112, 2, 15, TFT_BLACK);
    } else {
      tftPlPrint(String(freq, 0), i, 126);
    }
    freq = Displayfreq + ((i + (tmpMark / 2) - 160 + (d / 2)) / currentRetroScale);
    if (freq > bandRetro[RETROband].maximumFreq or (freq + (bandRetro[RETROband].mark / 2)) < bandRetro[RETROband].minimumFreq) {
      for (int n = 0; n < (tmpMark / 2); n += 4) tft.fillRect(i + n + (tmpMark / 2), 112, 2, 15, TFT_BLACK);
    } else {
      tft.fillRect(i + (tmpMark / 2), 115, 2, 10, TFT_BLACK);
    }
  }

  if (RETROband > 3) {
    int i = 1;
    tft.fillRect(0, RetroStationPos[5] - 2, 319 - d, 9, TFT_BLACK);
    while (i < 29) {
      if (bandMode[i] == LSB or bandMode[i] == USB) {
        if (band[i].currentFreq >= bandRetro[RETROband].minimumFreq and band[i].currentFreq <= bandRetro[RETROband].maximumFreq) {
          int minPos = ((band[i].minimumFreq - bandRetro[RETROband].currentFreq) * currentRetroScale) + 159 - (d / 2);
          int maxPos = ((band[i].maximumFreq - bandRetro[RETROband].currentFreq) * currentRetroScale) + 159 - (d / 2);
          if (maxPos > -(currentRetroScale * 10) and minPos < (319 - d + (currentRetroScale * 10))) {
            if (minPos < -11) minPos = -11;
            if (maxPos > 330) maxPos = 330 - d;
            tft.fillRect(minPos, RetroStationPos[5] - 2, maxPos - minPos + 1, 9, TFT_BROWN);
          }
        }
      }
      i++;
    }
  }

  tftPlSetFont(T1012_T);
  tftPlSetSize(1);
  tftPlSetDatum(BR_T);
  tftPlSetStyle(NBL_T);
  tftPlBottomCut = true;
  for (int i = 0; i <= lastPreset; i++) {
    if (preset[i].presetIdx >= bandRetro[RETROband].minimumFreq and preset[i].presetIdx <= bandRetro[RETROband].maximumFreq) {
      freq = (((preset[i].presetIdx) - Displayfreq) * currentRetroScale) + 160 - (d / 2);
      int tmplen = tftPlLength(preset[i].PresetName) * 8;
      if (freq > -20 and freq < tmplen + 350 - d) {
        tft.fillRect(freq - tmplen - 40, preset[i].presetPos - 3, tmplen + 60, 11, TFT_BLACK);
        if ((preset[i].presetIdx) == Displayfreq) {
          tftPlSetColor(TFT_WHITE, TFT_TRANS);
          tft.fillRect(freq - 10, preset[i].presetPos, 20, 5, TFT_LIGTHYELLOW);
        } else {
          tftPlSetColor(TFT_SILVER, TFT_TRANS);
          tft.fillRect(freq - 10, preset[i].presetPos, 20, 5, TFT_OLIVE);
        }
        tftPlPrint(String(preset[i].PresetName), freq - 20, preset[i].presetPos + 9);
      }
    }
  }
  tftPlBottomCut = false;

  for (int i = 0; i < 6; i++) {
    tft.fillRect(158 - (d / 2), (i * 12) + 40, 4, 11, TFT_RED);
    tft.fillRect(158 - (d / 2), (i * 12) + 128, 4, 11, TFT_RED);
  }

}

void drawRetroCity() {

  int d = !screenV * 40;
  if (!cityRetroRotary) {
    tft.fillScreen(COLOR_BACKGROUND);
    tft.fillRect(d, 0, 240, 20, TFT_GREY);
    tft.setTextSize(2);
    tft.setTextColor(TFT_CYAN, TFT_GREY);
    tft.setTextDatum(BC_DATUM);
    tft.drawString("CITY", 120 + d, 20);
    if (!screenV) {
      for (int n = 1; n <= 20; n++) {
        tft.fillRect(40 - (n * 2), 0, 2, 240, ((int(n / 2) * 4096) + (n * 32)));
        tft.fillRect((n * 2) + 278, 0, 2, 240, ((int(n / 2) * 4096) + (n * 32)));
      }
    }
  }

  int id;
  for (int i = 0; i <= lastGroup; i++) if (group[i].groupIdx == PresetId) id = i;

  id += cityRetroRotary;
  if (id < 0 or id > lastGroup) {
    id -= cityRetroRotary;
  } else {
    PresetId = group[id].groupIdx;

    uint16_t col;
    tftPlSetFont(T1012_T);
    tftPlSetSize(2);
    tftPlSetDatum(BC_T);
    tftPlSetStyle(NRG_T);

    for (int i = 0; i < 7; i++) {
      if (!screenV) {
        for (int n = 1; n <= 20; n++) {
          tft.fillRect(40 - (n * 2), (i * 30) + 20, 2, 30, ((int(n / 2) * 4096) + (n * 32)));
          tft.fillRect((n * 2) + 278, (i * 30) + 20, 2, 30, ((int(n / 2) * 4096) + (n * 32)));
        }
      }
      tft.fillRect(d, (i * 30) + 20, 240, 30, TFT_BLACK);
      if (i == 3) {
        tft.fillRect(d, (i * 30) + 20, 240, 1, TFT_CYAN);
        tft.fillRect(d, (i * 30) + 49, 240, 1, TFT_CYAN);
      }
      if ((id + i - 3) >= 0 and (id + i - 3) <= lastGroup) {
        switch (i) {
          case 0:
          case 6:
            col = 520;
            break;
          case 1:
          case 5:
            col = 780;
            break;
          case 2:
          case 4:
            col = TFT_DARKCYAN;
            break;
          default:
            col = TFT_WHITE;
            break;
        }
        tftPlSetColor(col, TFT_TRANS);
        tftPlPrint(String(group[id + i - 3].PresetName), 120 + d, (i * 30) + 45);
      }
    }
  }
  cityRetroRotary = 0;
}

void drawRetroBand() {

  int d = !screenV * 40;
  if (!screenV) {
    for (int n = 1; n <= 20; n++) {
      tft.fillRect(40 - (n * 2), 0, 2, 40, ((int(n / 2) * 4096) + (n * 32)));
      tft.fillRect((n * 2) + 278, 0, 2, 40, ((int(n / 2) * 4096) + (n * 32)));
    }
  }
  tft.fillScreen(COLOR_BACKGROUND);
  tft.fillRect(d, 0, 240, 20, TFT_GREY);
  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN, TFT_GREY);
  tft.setTextDatum(BC_DATUM);
  tft.drawString("BAND", 120 + d, 20);

  drawRetroBandBut();
}

void drawRetroBandBut() {

  spr.createSprite(But_Width, But_Height);
  tftPlSetSize(1);
  for (int n = 0 ; n <= lastBandRetro; n++) {
    spr.fillScreen(COLOR_BACKGROUND);
    if (n == RETROband) {
      spr.fillRoundRect(0, 0, But_Width, But_Height, 8, TFT_GOLD);
      spr.drawRoundRect(0, 0, But_Width, But_Height, 8, TFT_GOLD);
    } else {
      if (VHFon or n != 1) {
        spr.fillRoundRect(0, 0, But_Width, But_Height, 8, TFT_DARKGREY);
        spr.drawRoundRect(0, 0, But_Width, But_Height, 8, TFT_GOLD);
      } else {
        spr.fillRoundRect(0, 0, But_Width, But_Height, 8, TFT_BLACK);
        spr.drawRoundRect(0, 0, But_Width, But_Height, 8, TFT_DARKGREY);
      }
    }
    if (screenV) spr.pushSprite(bandRetro[n].xPosV, bandRetro[n].yPosV); else  spr.pushSprite(bandRetro[n].xPosH, bandRetro[n].yPosH);
    tftPlSetFont(T1516_T);
    tftPlSetStyle(NBL_T);
    tftPlSetDatum(BC_T);
    if (n == RETROband) {
      tftPlSetColor(TFT_LIGTHYELLOW, TFT_TRANS);
    } else {
      tftPlSetColor(TFT_WHITE, TFT_TRANS);
      if (langRetroEN) {
        if (screenV) tftPlPrint(String(bandRetro[n].BandName), bandRetro[n].xPosV + (But_Width / 2) + 3, bandRetro[n].yPosV + (But_Height / 2) + 9); else tftPlPrint(String(bandRetro[n].BandName), bandRetro[n].xPosH + (But_Width / 2) + 3, bandRetro[n].yPosH + (But_Height / 2) + 9);
      } else {
        if (screenV) tftPlPrint(String(bandRetro[n].BandNamePL), bandRetro[n].xPosV + (But_Width / 2) + 3, bandRetro[n].yPosV + (But_Height / 2) + 9); else tftPlPrint(String(bandRetro[n].BandNamePL), bandRetro[n].xPosH + (But_Width / 2) + 3, bandRetro[n].yPosH + (But_Height / 2) + 9);
      }
      tftPlSetColor(TFT_OLIVE, TFT_TRANS);
    }
    if (langRetroEN) {
      if (screenV) tftPlPrint(String(bandRetro[n].BandName), bandRetro[n].xPosV + (But_Width / 2) + 2, bandRetro[n].yPosV + (But_Height / 2) + 8); else tftPlPrint(String(bandRetro[n].BandName), bandRetro[n].xPosH + (But_Width / 2) + 2, bandRetro[n].yPosH + (But_Height / 2) + 8);
    } else {
      if (screenV) tftPlPrint(String(bandRetro[n].BandNamePL), bandRetro[n].xPosV + (But_Width / 2) + 2, bandRetro[n].yPosV + (But_Height / 2) + 8); else tftPlPrint(String(bandRetro[n].BandNamePL), bandRetro[n].xPosH + (But_Width / 2) + 2, bandRetro[n].yPosH + (But_Height / 2) + 8);
    }
    if ((langRetroEN and bandRetro[n].RetroBandTime != "") or (!langRetroEN and bandRetro[n].RetroBandTimePL != "")) {
      if (screenV) tft.drawRect(bandRetro[n].xPosV, bandRetro[n].yPosV, 220, 40, TFT_OLIVE); else tft.drawRect(bandRetro[n].xPosH, bandRetro[n].yPosH, 220, 40, TFT_OLIVE);
    }
    tftPlSetFont(T1012_T);
    tftPlSetStyle(NRG_T);
    tftPlSetDatum(BL_T);
    tftPlSetColor(TFT_LIGTHYELLOW, TFT_TRANS);
    if (langRetroEN) {
      if (screenV) tftPlPrint(String(bandRetro[n].RetroBandTime), bandRetro[n].xPosV + 90, bandRetro[n].yPosV + 27); else tftPlPrint(String(bandRetro[n].RetroBandTime), bandRetro[n].xPosH + 90, bandRetro[n].yPosH + 27);
    } else {
      if (screenV) tftPlPrint(String(bandRetro[n].RetroBandTimePL), bandRetro[n].xPosV + 90, bandRetro[n].yPosV + 27); else tftPlPrint(String(bandRetro[n].RetroBandTimePL), bandRetro[n].xPosH + 90, bandRetro[n].yPosH + 27);
    }
  }
  spr.deleteSprite();
}

void displMEMO() {

  int d = !screenV * 40;
  tft.setTextSize(2);
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_GREY);
  if (presetBank) tft.drawString("PRESETS", XFreqDispl + d + 120, YFreqDispl + 78); else tft.drawString("       ", XFreqDispl + d + 120, YFreqDispl + 78);
  tft.setTextSize(1);
  tftPlSetSize(1);
  tftPlSetStyle(NBL_T);
  for (int i = -1; i < (2 + screenV); i++) {
    if (MEMOdel) {
      tftTransRect(d, 120, 240, 40, TFT_RED);
    } else {
      if (i) {
        if (presetBank) tft.fillRect(d, (i * 40) + 120, 240, 40, TFT_NAVY); else tft.fillRect(d, (i * 40) + 120, 240, 40, TFT_BLACK);
      } else tft.fillRect(d, 120, 240, 40, TFT_DARKCYAN);
    }
    if (presetBank) {
      if ((currentMemo + i) >= 0 and (currentMemo + i) <= lastPreset ) {
        tftPlSetDatum(BL_T);
        tftPlSetFont(T1516_T);
        tftPlSetColor(TFT_YELLOW, TFT_TRANS);
        tftPlPrint(String(currentMemo + i + 1), d, (i * 40) + 141);
        tftPlSetColor(TFT_WHITE, TFT_TRANS);
        tftPlSetCut(0, 20);
        tftPlWidth = 12;
        tftPlPrint(String(preset[currentMemo + i].PresetName), d, (i * 40) + 157);
        tftPlSetCut(0, 0);
        tftPlSetDatum(BR_T);
        tftPlSetColor(TFT_CYAN, TFT_TRANS);
        if (preset[currentMemo + i].presetIdx < 109) tftPlPrint(String(preset[currentMemo + i].presetIdx, 2) + " MHz", d + 155, (i * 40) + 141); else tftPlPrint(String(preset[currentMemo + i].presetIdx, 0) + " KHz", d + 155, (i * 40) + 141);

        if (i) tft.setTextColor(2031, TFT_NAVY); else tft.setTextColor(2031, TFT_DARKCYAN);
        tft.drawString(band[bandFreq(preset[currentMemo + i].presetIdx)].bandName, d + 175, (i * 40) + 137);
        tft.drawRect(d + 155, (i * 40) + 124, 39, 16, 2031);

        if (i) tft.setTextColor(TFT_YELLOW, TFT_NAVY); else tft.setTextColor(TFT_YELLOW, TFT_DARKCYAN);
        tft.drawString(bandModeDesc[bandMode[bandFreq(preset[currentMemo + i].presetIdx)]], d + 215, (i * 40) + 137);
        tft.drawRect(d + 195, (i * 40) + 124, 39, 16, TFT_YELLOW);
      }
    } else {
      if ((currentMemo + i) >= 0 and (currentMemo + i) <= lastMemoBank ) {
        tftPlSetDatum(BL_T);
        tftPlSetFont(T1516_T);
        tftPlSetColor(TFT_YELLOW, TFT_TRANS);
        tftPlPrint(String(currentMemo + i + 1), d, (i * 40) + 141);
        if ((MemoBank[currentMemo + i].freq < 153 or MemoBank[currentMemo + i].freq > 30000) and !(!i and MEMOadd))  {
          if (!i and MEMOdel) {
            MEMOdel = false;
            ErrorBeep();
          }
          tftPlSetColor(TFT_WHITE, TFT_TRANS);
          tftPlPrint("empty", d, (i * 40) + 157);
        } else {
          tftPlSetColor(TFT_WHITE, TFT_TRANS);
          tftPlSetCut(0, 20);
          tftPlWidth = 12;
          if (MEMOadd and !i) tftPlPrint(String(addMemoName), d, 157); else tftPlPrint(String(MemoBank[currentMemo + i].Name), d, (i * 40) + 157);
          tftPlSetCut(0, 0);
          tftPlSetDatum(BR_T);
          tftPlSetColor(TFT_CYAN, TFT_TRANS);
          if (MEMOadd and !i) {
            if (addMemoBand == FM) tftPlPrint(String(float(addMemoFreq) / 100, 2) + " MHz", d + 155, 141); else tftPlPrint(String(addMemoFreq) + " KHz", d + 155, 141);
          } else {
            if (MemoBank[currentMemo + i].band == FM) tftPlPrint(String(float(MemoBank[currentMemo + i].freq) / 100, 2) + " MHz", d + 155, (i * 40) + 141); else tftPlPrint(String(MemoBank[currentMemo + i].freq) + " KHz", d + 155, (i * 40) + 141);
          }

          if (i) tft.setTextColor(2031, TFT_BLACK); else tft.setTextColor(2031, TFT_DARKCYAN);
          if (MEMOadd and !i) tft.drawString(band[addMemoBand].bandName, d + 175, 137); else tft.drawString(band[(MemoBank[currentMemo + i].band & 0x1F)].bandName, d + 175, (i * 40) + 137);
          tft.drawRect(d + 155, (i * 40) + 124, 39, 16, 2031);

          if (i) tft.setTextColor(TFT_YELLOW, TFT_BLACK); else tft.setTextColor(TFT_YELLOW, TFT_DARKCYAN);
          if (MEMOadd and !i) tft.drawString(bandModeDesc[addMemoMode], d + 215, 137); else tft.drawString(bandModeDesc[int(MemoBank[currentMemo + i].band / 32)], d + 215, (i * 40) + 137);
          tft.drawRect(d + 195, (i * 40) + 124, 39, 16, TFT_YELLOW);
        }
      }
    }
  }
}

int bandFreq(float freq) {

  int n = 0;
  if (freq < 64 or freq > 108) {
    n = 1;
    bool flag = false;
    while (n < 29 and !flag) if (freq >= band[n].minimumFreq and freq <= band[n].maximumFreq) flag = true; else n++;
  }
  return n;
}

void displSETUP() {

  int d = !screenV * 5;
  tft.fillRect(d, 20, 280, 178, TFT_BLACK);
  if (!screenV) {
    for (int n = 1; n <= 20; n++) {
      tft.fillRect(40 - (n * 2), 40, 2, 160, ((int(n / 2) * 4096) + (n * 32)));
      tft.fillRect((n * 2) + 278, 40, 2, 160, ((int(n / 2) * 4096) + (n * 32)));
    }
  }

  spr.createSprite(265, 120);
  spr.fillScreen(COLOR_BACKGROUND);

  if (screenV) spr.pushSprite(-25, 80); else spr.pushSprite(27, 80);
  spr.deleteSprite();

	tftPlSetSize(1);
	tftPlSetStyle(NBL_T);
	tftPlSetDatum(BL_T);
	tftPlSetFont(T1516_T);
	tftPlSetColor(TFT_ORANGE, TFT_BLACK);

  switch (pageSetup) {
    case 0:
      tftPlPrint("USTAWIENIA - SI473X      ", 2 , 18);
      displSETUPitem     ("FM od 64 MHz      ", 20,  prevVHFon, (VHFon != prevVHFon));
      displSETUPitem     ("Szukaj w AM 1 KHz ", 52,  prevseekAccuracy, (seekAccuracy != prevseekAccuracy));
      displSETUPitem     ("Wykres trendu RSSI", 84,  prevrssiHistoryOn, (rssiHistoryOn != prevrssiHistoryOn));
      displSETUPitemValue("Prog wykrycia SNR ", 116, String(prevScanMarkSNR) + " dB", (ScanMarkSNR != prevScanMarkSNR));
      displSETUPitem     ("Stop na sygnale   ", 148, prevscanStopOnSignal, (scanStopOnSignal != prevscanStopOnSignal));
      break;
    case 1:
      tftPlPrint("USTAWIENIA - UŻYTKOWE    ", 2 , 18);
      displSETUPitem     ("RDS tylko w FM    ", 20,  !prevRDSalways, (RDSalways != prevRDSalways));
      displSETUPitem     ("Podświetl cyfry   ", 52,  prevdigitLigth, (digitLigth != prevdigitLigth));
      displSETUPitem     ("Memo in preset    ", 84, prevmemoPreset, (memoPreset != prevmemoPreset));
      displSETUPitem     ("ANG. nazwy pasm   ", 116, !prevlangRetroEN, (langRetroEN != prevlangRetroEN));
      break;
    case 2:
      tftPlPrint("USTAWIENIA - WYŚWIETLACZ ", 2 , 18);
      displSETUPitem     ("Wygaszacz ekranu  ", 20,  prevsaverOn, (saverOn != prevsaverOn));
      displSETUPitem     ("Wygaś całkiem     ", 52,  prevdisplayOff, (displayOff != prevdisplayOff));
      displSETUPitemValue("Czas do wygaszacza", 84, saverTimeText(prevsaverTime), (saverTime != prevsaverTime));
      displSETUPitem     ("Orientacja pionowa", 116, prevscreenV, (screenV != prevscreenV));
      displSETUPitem     ("Waterfall w SCAN  ", 148, prevscanWaterfallOn, (scanWaterfallOn != prevscanWaterfallOn));
      break;
    case 3:
      tftPlPrint("USTAWIENIA - SKANOWANIE  ", 2 , 18);
      displSETUPitemValue("Min skali         ", 20,  String("x" + String(int(1 / prevminSCANstep))), (minSCANstep != prevminSCANstep));
      displSETUPitemValue("Max skali         ", 52,  String("1:" + String(int(prevmaxSCANstep))), (maxSCANstep != prevmaxSCANstep));
      displSETUPitem     ("Auto skala        ", 84, prevautoSCANstep, (autoSCANstep != prevautoSCANstep));
      displSETUPitem     ("Dokładność skanu  ", 116, prevSCANaccuracy, (SCANaccuracy != prevSCANaccuracy));
	  displSETUPitem     ("Wygaszacz w SCAN  ", 148, !prevsaverDisableOnScan, (saverDisableOnScan != prevsaverDisableOnScan));
      break;
    case 4:
      tftPlPrint("USTAWIENIA - SPRZĘT      ", 2 , 18);
      displSETUPitemValue("Bateria min (0%)  ", 20,  batVText(prevbatMinV), (batMinV != prevbatMinV));
      displSETUPitem     ("Pokaż baterię     ", 52,  prevbatShow, (batShow != prevbatShow));
      displSETUPitem     ("Brzęczyk          ", 84, prevbeeperOn, (beeperOn != prevbeeperOn));
      displSETUPitem     ("Jasność           ", 116, prevdisplayPower, (displayPower != prevdisplayPower));
      displSETUPitemValue("Bateria max (100%)", 148, batVText(prevbatMaxV), (batMaxV != prevbatMaxV));
      break;
    case 5:
      tftPlPrint("USTAWIENIA - DOMYŚLNE    ", 2 , 18);
      displSETUPitem     ("Wyczyść ustawienia", 20,  prevloadMemory, (loadMemory != prevloadMemory));
      displSETUPitem     ("Reset fabrycznych ", 52, prevloadDefault, (loadDefault != prevloadDefault));
      displSETUPitem     ("Dekoder CW        ", 84, prevcwDecoderOn, (cwDecoderOn != prevcwDecoderOn));
      displSETUPitem     ("Serwer WWW        ", 116, prevwebServerOn, (webServerOn != prevwebServerOn));
      displSETUPitem     ("Analogowy S-metr  ", 148, prevanalogMeterOn, (analogMeterOn != prevanalogMeterOn));
      break;
    case 6:
      tftPlPrint("USTAWIENIA - WIFI        ", 2 , 18);
      displSETUPitem     ("WiFi włączone     ", 20,  prevwifiEnable, (wifiEnable != prevwifiEnable));
      displSETUPwifiStatus(52, wifiStatusText());
      displSETUPitem     ("Konfiguruj    ", 84, prevwifiConfigureNow, (wifiConfigureNow != prevwifiConfigureNow));
      displSETUPitem     ("Resetuj zapisaną sieć. ", 116, prevresetWifiConfig, (resetWifiConfig != prevresetWifiConfig));
      displSETUPitem     ("Tryb nocny (NTP)  ", 148, prevnightModeOn, (nightModeOn != prevnightModeOn));
      break;
	case 7:
	  tftPlPrint("USTAWIENIA - TEST        ", 2 , 18);
		int startX = 20 + d;
		int startY = 40;
		int size = 40;
		int gap = 10;

		  tft.fillRect(startX, startY, size, size, TFT_RED);
		  tftPlSetColor(TFT_WHITE, TFT_TRANS);
		  tftPlSetDatum(BC_T);
		  tftPlPrint("R", startX + size/2, startY + size + 10);

		  tft.fillRect(startX + size + gap, startY, size, size, TFT_GREEN);
		  tftPlPrint("G", startX + size + gap + size/2, startY + size + 10);

		  tft.fillRect(startX + 2*(size + gap), startY, size, size, TFT_BLUE);
		  tftPlPrint("B", startX + 2*(size + gap) + size/2, startY + size + 10);

		  tftPlSetDatum(BL_T);
		  tftPlSetColor(TFT_YELLOW, TFT_TRANS);
		  tftPlPrint("ĄąĆćĘęÓóŁłŻżŹźŃńŚś", 25, startY + size + 30);

		  tftPlSetColor(TFT_WHITE, TFT_TRANS);
		  tftPlPrint("Wolna pamięć: " + String(ESP.getFreeHeap() / 1024) + " KB", 20, startY + size + 62);
		  tftPlPrint("Wolny blok: " + String(ESP.getMaxAllocHeap() / 1024) + " KB", 20, startY + size + 82);

		  tft.fillRect(d + 20, 180, 90, 16, TFT_DARKGREY);
		  tft.drawRect(d + 20, 180, 90, 16, TFT_CYAN);
		  tftPlSetDatum(BC_T);
		  tftPlSetColor(TFT_CYAN, TFT_TRANS);
		  tftPlPrint("USTAW DOTYK", 65 + d, 194);

		  tft.fillRect(d + 130, 180, 90, 16, TFT_DARKGREY);
		  tft.drawRect(d + 130, 180, 90, 16, TFT_CYAN);
		  tftPlPrint("FW INFO", 175 + d, 194);
	break;
  }
}

void displSETUPitem(String itemName, int pos, bool state, bool changed) {

  int d = !screenV * 5;
  if (changed) tftTransRect(d, pos + 2, 240, 36, 0xC000);
  tft.drawRect(d, pos + 5, 30, 30, TFT_WHITE);
  if (state) tft.fillRect(d + 5, pos + 10, 20, 20, TFT_GREENYELLOW);
  tftPlSetSize(1);
  tftPlSetStyle(NBL_T);
  tftPlSetDatum(BL_T);
  tftPlSetFont(T1516_T);
  tftPlSetColor(TFT_GREENYELLOW, TFT_TRANS);
  tftPlWidth = 12;
  tftPlPrint(itemName, d + 40, pos + 30);
}

void displSETUPitemValue(String itemName, int pos, String state, bool changed) {

  int d = !screenV * 5;
  if (changed) tftTransRect(d, pos + 2, 280, 36, 0xC000);
  tft.drawRect(d, pos + 5, 30, 30, TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK);
  tft.drawString(state, d + 15, pos + 25);
  tftPlSetSize(1);
  tftPlSetStyle(NBL_T);
  tftPlSetDatum(BL_T);
  tftPlSetFont(T1516_T);
  tftPlSetColor(TFT_GREENYELLOW, TFT_TRANS);
  tftPlWidth = 12;
  tftPlPrint(itemName, d + 40, pos + 30);
}

void displSETUPwifiStatus(int pos, String status) {

  int d = !screenV * 5;
  tftPlSetSize(1);
  tftPlSetStyle(NRG_T);
  tftPlSetDatum(BL_T);
  tftPlSetFont(T1012_T);
  tftPlSetColor(TFT_WHITE, TFT_TRANS);
  tftPlPrint("Stan WiFi: " + status, d, pos + 30);
}

void defaultSETUP() {

  if (confirm("ZAŁADOWAĆ DOMYŚLNE?") == 1) {
    prevVHFon = true;
    prevseekAccuracy = false;

    prevRDSalways = false;
    prevdigitLigth = true;
    prevmemoPreset = false;
    prevlangRetroEN = true;

    prevsaverOn = true;
    prevdisplayOff = false;
    prevsaverTime = 600;
    prevscreenV = false;

    prevminSCANstep = 0.125;
    prevmaxSCANstep = 8;
    prevautoSCANstep = true;
    prevSCANaccuracy = true;
	prevsaverDisableOnScan = true;
    prevbatMinV = 270;
    prevbatMaxV = 405;
    prevrssiHistoryOn = false;
    prevScanMarkSNR = 3;
    prevscanStopOnSignal = false;
    prevscanWaterfallOn = false;
    prevcwDecoderOn = false;
    prevwebServerOn = false;
    prevanalogMeterOn = false;
    prevnightModeOn = false;

    prevbatShow = false;
    prevbeeperOn = true;
    prevdisplayPower = false;

    prevloadMemory = false;
    prevloadDefault = false;

    prevwifiEnable = true;
    prevwifiConfigureNow = false;
    prevresetWifiConfig = false;
  }
  drawList(L_SETUP,"USTAWIENIA");
  if (!pageSetup) drawButton(L_SETUP, 0, B_BLOCK);
  if (pageSetup == maxPageSetup) drawButton(L_SETUP, 1, B_BLOCK);
}

void changeSETUP(int pos) {

  switch (pageSetup) {
    case 0:
      switch (pos) {
        case 0:
          prevVHFon = !prevVHFon;
          break;
        case 1:
          prevseekAccuracy = !prevseekAccuracy;
          break;
        case 2:
          prevrssiHistoryOn = !prevrssiHistoryOn;
          break;
        case 3:
          prevScanMarkSNR = nextScanSnr(prevScanMarkSNR);
          break;
        case 4:
          prevscanStopOnSignal = !prevscanStopOnSignal;
          break;
      }
      break;
    case 1:
      switch (pos) {
        case 0:
          prevRDSalways = !prevRDSalways;
          break;
        case 1:
          prevdigitLigth = !prevdigitLigth;
          break;
        case 2:
          prevmemoPreset = !prevmemoPreset;
          break;
        case 3:
          prevlangRetroEN = !prevlangRetroEN;
          break;
      }
      break;
    case 2:
      switch (pos) {
        case 0:
          prevsaverOn = !prevsaverOn;
          break;
        case 1:
          prevdisplayOff = !prevdisplayOff;
          break;
        case 2:
          prevsaverTime = nextSaverTime(prevsaverTime);
          break;
        case 3:
          prevscreenV = !prevscreenV;
          break;
        case 4:
          prevscanWaterfallOn = !prevscanWaterfallOn;
          break;
      }
      break;
    case 3:
      switch (pos) {
        case 0:
          prevminSCANstep *= 2;
          if (prevminSCANstep == 1) prevminSCANstep = 0.125;
          break;
        case 1:
          prevmaxSCANstep *= 2;
          if (prevmaxSCANstep == 16) prevmaxSCANstep = 1;
          break;
        case 2:
          prevautoSCANstep = !prevautoSCANstep;
          break;
        case 3:
          prevSCANaccuracy = !prevSCANaccuracy;
          break;
		case 4:
          prevsaverDisableOnScan = !prevsaverDisableOnScan;
          break;
      }
      break;
    case 4:
      switch (pos) {
        case 0:
          prevbatMinV = nextBatMinV(prevbatMinV);
          break;
        case 1:
          prevbatShow = !prevbatShow;
          break;
        case 2:
          prevbeeperOn = !prevbeeperOn;
          break;
        case 3:
          prevdisplayPower = !prevdisplayPower;
          break;
        case 4:
          prevbatMaxV = nextBatMaxV(prevbatMaxV);
          break;
      }
      break;
    case 5:
      switch (pos) {
        case 0:
          prevloadMemory = !prevloadMemory;
          if (prevloadMemory) prevloadDefault = false;
          break;
        case 1:
          prevloadDefault = !prevloadDefault;
          if (prevloadDefault) prevloadMemory = false;
          break;
        case 2:
          prevcwDecoderOn = !prevcwDecoderOn;
          break;
        case 3:
          prevwebServerOn = !prevwebServerOn;
          break;
        case 4:
          prevanalogMeterOn = !prevanalogMeterOn;
          break;
      }
      break;
    case 6:
      switch (pos) {
        case 0:
          prevwifiEnable = !prevwifiEnable;
          break;
        case 1:
          break;
        case 2:
          prevwifiConfigureNow = !prevwifiConfigureNow;
          if (prevwifiConfigureNow) prevresetWifiConfig = false;
          break;
        case 3:
          prevresetWifiConfig = !prevresetWifiConfig;
          if (prevresetWifiConfig) prevwifiConfigureNow = false;
          break;
        case 4:
          prevnightModeOn = !prevnightModeOn;
          break;
      }
      break;
    case 7:
      switch (pos) {
        case 5:
          if (x < 120) {
            tft.fillScreen(TFT_BLACK);
            tftPlSetSize(1);
            tftPlSetStyle(NRG_T);
            tftPlSetDatum(TC_T);
            tftPlSetColor(TFT_YELLOW, TFT_TRANS);
            tftPlPrint("Dotknij kolejno znaczniki", 120, 8);
            tft.calibrateTouch(touchCalData, TFT_WHITE, TFT_RED, 15);
            for (int i = 0; i < 5; i++) storage.touchCalData[i] = touchCalData[i];
            saveConfig();
            tft.setTouch(touchCalData);
          } else {
            showFirmwareInformation();
          }
          displSETUP();
          break;
      }
      break;
  }
}

void saveSETUP() {

  if (VHFon != prevVHFon or langRetroEN != prevlangRetroEN or beeperOn != prevbeeperOn or digitLigth != prevdigitLigth or loadMemory != prevloadMemory or
      batShow != prevbatShow or memoPreset != prevmemoPreset or loadDefault != prevloadDefault or saverOn != prevsaverOn or saverTime != prevsaverTime or
      screenV != prevscreenV or displayOff != prevdisplayOff or minSCANstep != prevminSCANstep or maxSCANstep != prevmaxSCANstep or
      autoSCANstep != prevautoSCANstep or SCANaccuracy != prevSCANaccuracy or displayPower != prevdisplayPower or RDSalways != prevRDSalways or
      seekAccuracy != prevseekAccuracy or wifiEnable != prevwifiEnable or wifiConfigureNow != prevwifiConfigureNow or
	  saverDisableOnScan != prevsaverDisableOnScan or
      batMinV != prevbatMinV or batMaxV != prevbatMaxV or rssiHistoryOn != prevrssiHistoryOn or
      ScanMarkSNR != prevScanMarkSNR or scanStopOnSignal != prevscanStopOnSignal or scanWaterfallOn != prevscanWaterfallOn or cwDecoderOn != prevcwDecoderOn or webServerOn != prevwebServerOn or analogMeterOn != prevanalogMeterOn or nightModeOn != prevnightModeOn or
      resetWifiConfig != prevresetWifiConfig) {
    int n = confirm("ZAPISAĆ ZMIANY?");
    if (n == 1) {
      if (VHFon != prevVHFon) {
        VHFon = prevVHFon;
        if (VHFon) band[0].minimumFreq = 6400; else band[0].minimumFreq = 8750;
        si4735.setSeekFmLimits(band[0].minimumFreq, band[0].maximumFreq);
        if (!bandIdx) si4735.setFM(band[0].minimumFreq, band[0].maximumFreq, band[0].currentFreq, band[0].currentStep);
      }
      if (memoPreset != prevmemoPreset) {
        memoPreset = prevmemoPreset;
        presetLoad();
        presetSort();
      }
      if (loadDefault != prevloadDefault) {
        loadDefault = prevloadDefault;
        if (loadDefault) storage.chkDigit = 32; else storage.chkDigit = 64;
      }
      if (screenV != prevscreenV) {
        screenV = prevscreenV;
        screenRotate();
      }
      langRetroEN = prevlangRetroEN;
      beeperOn = prevbeeperOn;
      digitLigth = prevdigitLigth;
      loadMemory = prevloadMemory;
      batShow = prevbatShow;
      saverOn = prevsaverOn;
      saverTime = prevsaverTime;
      displayOff = prevdisplayOff;
      minSCANstep = prevminSCANstep;
      maxSCANstep = prevmaxSCANstep;
      autoSCANstep = prevautoSCANstep;
      SCANaccuracy = prevSCANaccuracy;
	  saverDisableOnScan = prevsaverDisableOnScan;
      batMinV = prevbatMinV;
      batMaxV = prevbatMaxV;
      rssiHistoryOn = prevrssiHistoryOn;
      ScanMarkSNR = prevScanMarkSNR;
      scanStopOnSignal = prevscanStopOnSignal;
      scanWaterfallOn = prevscanWaterfallOn;
      cwDecoderOn = prevcwDecoderOn;
      webServerOn = prevwebServerOn;
      if (webServerOn) enableWebServer(); else disableWebServer();
      analogMeterOn = prevanalogMeterOn;
      nightModeOn = prevnightModeOn;
      updateCWDecoderPin();
      displayPower = prevdisplayPower;
      RDSalways = prevRDSalways;
      seekAccuracy = prevseekAccuracy;
      wifiEnable = prevwifiEnable;
      if (SCANaccuracy) countScanSignal = 3; else countScanSignal = 1;

      bool wifiActionTaken = false;
      if (prevwifiConfigureNow) {
        configureWifiNow();
        wifiActionTaken = true;
      }
      if (prevresetWifiConfig) {
        if (confirm("USUNĄĆ SIEĆ WIFI?") == 1) {
          wifiManager.resetSettings();
          WiFi.disconnect(true);
          WiFi.mode(WIFI_OFF);
          storage.wifiSSID[0] = '\0';
          storage.wifiPassword[0] = '\0';
          Serial.println("Zapisane sieci WiFi usunięto.");
          wifiActionTaken = true;
        } else {
          Serial.println("Anulowano reset sieci WiFi.");
        }
      }
      wifiConfigureNow = prevwifiConfigureNow = false;
      resetWifiConfig  = prevresetWifiConfig  = false;
      if (wifiActionTaken) {
        drawList(L_SETUP, "USTAWIENIA");
        if (!pageSetup) drawButton(L_SETUP, 0, B_BLOCK);
        if (pageSetup == maxPageSetup) drawButton(L_SETUP, 1, B_BLOCK);
      }

      if (loadMemory or loadDefault) {
        if (confirm("RESTART?") == 1) {
          tft.fillRect(!screenV * 40, 40, 240, 120, TFT_BLACK);
          tft.setTextSize(2);
          tft.setTextDatum(BC_DATUM);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.drawString("RESTARTOWANIE...", 160 - (screenV * 40), 100);
          delay(5000);
          ESP.restart();
        }
      }
    } else if (n == -1) {
      drawList(L_SETUP,"USTAWIENIA");
      if (!pageSetup) drawButton(L_SETUP, 0, B_BLOCK);
      if (pageSetup == maxPageSetup) drawButton(L_SETUP, 1, B_BLOCK);
      SETUPbut = true;
    }
  }
}

int confirm(String text) {

  int d = !screenV * 40;
  if (!screenV) tftTransRect(0, 0, 320, 240, TFT_MAROON); else tftTransRect(0, 0, 240, 320, TFT_MAROON);
  tft.fillRect(d, 40, 240, 120, TFT_BLACK);
  tftPlSetSize(1);
  tftPlSetStyle(REG_T);
  tftPlSetFont(T1012_T);
  tftPlSetDatum(BC_T);
  tftPlSetColor(TFT_WHITE, TFT_BLACK);
  tftPlPrint(text, 120 + d, 80);
  drawButtons(L_CONFIRM);
  int n = -1;
  while (n < 0 and (elapsedSaver + 120000) > millis()) {
    presStat = tft.getTouch(&x, &y);
    n = jamButton(L_CONFIRM);
  }
  if (n >= 0) {
    elapsedSaver = millis();
    x = y = 0;
  }
  return n;
}

void tftTransRect(int x, int y, int w, int h, uint16_t c) {

  bool z = 0;
  for (int i = x; i < x + w; i++) {
    for (int j = y; j < y + h; j += 2) {
      tft.drawPixel(i, j + z, c);
    }
    z = !z;
  }
}

String presetNameLoad() {

  String ret = "";
  for (int i = 0; i <= lastGroup; i++) if (group[i].groupIdx == PresetId) ret = String(group[i].PresetName);
  return (String) ret;
}

void presetLoad() {

  int count = 0;
  for (int i = 0; i <= lastMemory; i++) {
    if (memory[i].memoryGroup == "" and presetBank) {
        preset[count].presetIdx = memory[i].memoryIdx;
        preset[count].PresetName = memory[i].MemoryName;
        count++;
    } else {
      int n = strlen(memory[i].memoryGroup) + 1;
      char p[n];
      char *pp = memory[i].memoryGroup;
      for (int j = 0; j < n; j++) p[j] = pp[j];
      char *str = strtok(p, " ");
      while (str != NULL) {
        if (String(str) == String(PresetId) or String(str) == "ALL") {
          preset[count].presetIdx = memory[i].memoryIdx;
          preset[count].PresetName = memory[i].MemoryName;
          preset[count].presetPos = 0;
          count++;
        }
        str = strtok(NULL, " ");
      }
    }
  }

  if (memoPreset and !presetBank) {
    for (int i = 0; i <= lastMemoBank; i++) {
      if (MemoBank[i].freq >= 153 and MemoBank[i].freq <= 30000) {
        if (MemoBank[i].band) preset[count].presetIdx = MemoBank[i].freq; else preset[count].presetIdx = float(MemoBank[i].freq) / 100;
        preset[count].PresetName = MemoBank[i].Name;
        int j = 19;
        while (j > 0 and (MemoBank[i].Name[j] == char(32) or MemoBank[i].Name[j] == char(0))) j--;
        preset[count].PresetName[j + 1] = NULL;
        preset[count].presetPos = 0;
        count++;
      }
    }
  }

  lastPreset = count - 1;
}

void presetSort() {

  for (int i = 0; i < lastPreset; i++) {
    for (int j = i + 1; j <= lastPreset; j++) {
      if (preset[j].presetIdx < preset[i].presetIdx) {
        float f = preset[i].presetIdx;
        char *n = preset[i].PresetName;
        preset[i].presetIdx = preset[j].presetIdx;
        preset[i].PresetName = preset[j].PresetName;
        preset[j].presetIdx = f;
        preset[j].PresetName = n;
      }
    }
  }
}

void presetSetPos() {

  currentRetroScale = bandRetro[RETROband].scale;
  int y = random(3);
  float tmpPos[12];
  for (int i = 0; i <= lastPreset; i++) {
    if (preset[i].presetIdx >= bandRetro[RETROband].minimumFreq and preset[i].presetIdx <= bandRetro[RETROband].maximumFreq) {
      int j = 0;
      while (j < 12) {
        if (tmpPos[j] <= (((preset[i].presetIdx) * currentRetroScale) - ((tftPlLength(preset[i].PresetName) * 8) + 60))) j = 12;
        j++;
      }
      if (j == 12) {
        for (int n = 0; n < 12; n++) tmpPos[n] = 0;
        currentRetroScale++;
        i = -1;
      } else {
        while (tmpPos[y] > (((preset[i].presetIdx) * currentRetroScale) - ((tftPlLength(preset[i].PresetName) * 8) + 60))) {
          y += random(1, 3);
          if (y > 11) y = random(3);
        }
        tmpPos[y] = (preset[i].presetIdx) * currentRetroScale;
        preset[i].presetPos = RetroStationPos[y];
      }
    }
  }
}

void tftPlSetSize(float siz) {

  tftPlSize = siz;
}

void tftPlSetColor(uint16_t color, int32_t back) {

  tftPlColor = color;
  tftPlBack = back;
}

void tftPlSetDatum(int datum) {

  tftPlDatum = datum;
}

void tftPlSetStyle(int style) {

  tftPlStyle = style;
  tftPlWidth = 8 + (tftPlFont * 4) + (2 * !(tftPlStyle & 0x02)) + tftPlFont;
}

void tftPlSetFont(int font) {

  tftPlFont = font;
  tftPlWidth = 8 + (tftPlFont * 4) + (2 * !(tftPlStyle & 0x02)) + tftPlFont;
}

void tftPlSetCut(int beg, int con) {

  tftPlBeginChar = beg;
  tftPlContChar = con;
}

int tftPlGlyphIndex(String text, int &i) {

  uint8_t first = (uint8_t) text[i];
  if (first >= 32 and first <= 126) return first - 32;

  if (i + 1 >= text.length()) return 0;

  uint8_t second = (uint8_t) text[i + 1];
  i++;

  if (first == 0xC4) {
    if (second == 0x84) return 95;
    if (second == 0x86) return 96;
    if (second == 0x98) return 97;
    if (second == 0x85) return 104;
    if (second == 0x87) return 105;
    if (second == 0x99) return 106;
  }
  if (first == 0xC5) {
    if (second == 0x81) return 98;
    if (second == 0x83) return 99;
    if (second == 0x9A) return 101;
    if (second == 0xB9) return 102;
    if (second == 0xBB) return 103;
    if (second == 0x82) return 107;
    if (second == 0x84) return 108;
    if (second == 0x9B) return 110;
    if (second == 0xBA) return 111;
    if (second == 0xBC) return 112;
  }
  if (first == 0xC3) {
    if (second == 0x93) return 100;
    if (second == 0xB3) return 109;
  }

  return 0;
}

void tftPlPrint(String text, int x, int y) {

  int curcount;
  int cd = 0;
  int tmpLen = tftPlLength(text);
  if (tmpLen > tftPlContChar and tftPlContChar) tmpLen = tftPlContChar;
  uint8_t ascii[tmpLen];
  int textLength = 0;

  for (int i = 0; i < text.length(); i++) {
    int glyphIndex = tftPlGlyphIndex(text, i);
    if (textLength >= tftPlBeginChar and ((textLength - tftPlBeginChar) < tftPlContChar or !tftPlContChar)) ascii[textLength - tftPlBeginChar] = glyphIndex;
    textLength++;
  }
  textLength -= tftPlBeginChar;
  if (textLength > tftPlContChar and tftPlContChar) textLength = tftPlContChar;

  if (textLength and (tftPlColor >= 0 or tftPlBack >= 0)) {

    bool tftPlTopAnchor = (tftPlDatum == TL_T or tftPlDatum == TC_T or tftPlDatum == TR_T);
    int tftPlHAlign = tftPlDatum;
    if (tftPlDatum == TL_T) tftPlHAlign = -1;
    if (tftPlDatum == TC_T) tftPlHAlign = 0;
    if (tftPlDatum == TR_T) tftPlHAlign = 1;

    if (!tftPlTopAnchor) y -= (tftPlSize * (12 + (tftPlFont * 4)));
    if (tftPlHAlign > 0) x -= tftPlSize * tftPlWidth * textLength;
    if (tftPlHAlign == 0) x -= tftPlSize * tftPlWidth * textLength / 2;

    for (int i = 0; i < textLength; i++) {
      for (int xx = 0; xx < tftPlWidth; xx++) {
        if (tftPlStyle > 3) {
          cd = (2 + tftPlFont) * tftPlSize;
          curcount = tftPlCursiveLevel - 1;
        }
        for (int yb = 0; yb < 2; yb++ ) {

          unsigned short by;
          unsigned short byp;
          if (tftPlFont) {
            if (xx > 10) {
              by = 0x00;
            } else {
              by = Tahoma15x16[(ascii[i] * 22) + (xx * 2) + yb];
            }
            if ((tftPlStyle & 0x01) and xx > 0 and xx < 12) byp = Tahoma15x16[(ascii[i] * 22) + ((xx - 1) * 2) + yb];
          } else {
            if (xx > 6) {
              by = 0x00;
            } else {
              by = Tahoma10x12[(ascii[i] * 14) + (xx * 2) + yb];
            }
            if ((tftPlStyle & 0x01) and xx > 0 and xx < 8) byp = Tahoma10x12[(ascii[i] * 14) + ((xx - 1) * 2) + yb];
          }

          for (int yy = 0; yy < (8 - (yb * 4 * (1 - tftPlFont)) - (tftPlBottomCut * (yb * (2 + tftPlFont)))); yy++ ) {
            bool bi = (by >> yy) & 0x01;
            if ((tftPlStyle & 0x01) and xx > 0 and xx < (8 + (4 * tftPlFont))) bi = bi | ((byp >> yy) & 0x01);
            for (int sy = 0; sy < tftPlSize; sy++) {
              for (int sx = 0; sx < tftPlSize; sx++) {

                if (bi) {
                  if (tftPlColor >= 0) tft.drawPixel(x + (i * tftPlSize * tftPlWidth) + (xx * tftPlSize) + sx + cd, y + (yb * tftPlSize * 8) + (yy * tftPlSize) + sy, tftPlColor);
                } else {
                  if (tftPlBack >= 0) tft.drawPixel(x + (i * tftPlSize * tftPlWidth) + (xx * tftPlSize) + sx + cd, y + (yb * tftPlSize * 8) + (yy * tftPlSize) + sy, tftPlBack);
                }

              }
              if (tftPlStyle > 3) {
                if (curcount) {
                  curcount--;
                } else {
                  curcount = tftPlCursiveLevel - 1;
                  cd--;
                }
              }
            }
          }
        }
      }
    }
  }
}

int tftPlLength(String text) {

  int textsize = 0;
  for (int i = 0; i < text.length(); i++) {
    tftPlGlyphIndex(text, i);
    textsize++;
  }
  return (int) textsize;
}

int tftPlTextWidth(String text) {

  return (int) (tftPlLength(text) * tftPlWidth * tftPlSize) + (trunc(tftPlStyle / 4) * tftPlCursiveLevel * tftPlSize);
}

void drawButtons(uint8_t lay) {

  for (int n = 0 ; n <= lastBut; n++) if (but[n].layout == lay) drawBut(n, B_NORMAL, "");
}

void drawButton(uint8_t lay, uint8_t num, uint8_t state) {

  int n = 0;
  bool flag = false;
  while (n <= lastBut && !flag) {
    if (but[n].layout == lay && but[n].num == num) {
      flag = true;
      drawBut(n, state, "");
    }
    n++;
  }
}

void drawButton(uint8_t lay, uint8_t num, uint8_t state, String altText) {

  int n = 0;
  bool flag = false;
  while (n <= lastBut && !flag) {
    if (but[n].layout == lay && but[n].num == num) {
      flag = true;
      drawBut(n, state, altText);
    }
    n++;
  }
}

int jamButton(uint8_t lay) {

  int res = -1;
  int n = 0;
  bool flag = false;
  while (n <= lastBut && !flag) {
    if (but[n].layout == lay) {
      if (screenV) {
        if (x >= (but[n].xPosV) && x < (but[n].xPosV + But_Width) && y >= (but[n].yPosV) && y < (but[n].yPosV + But_Height)) flag = true;
      } else {
        if (x >= (but[n].xPosH) && x < (but[n].xPosH + But_Width) && y >= (but[n].yPosH) && y < (but[n].yPosH + But_Height)) flag = true;
      }
      if (flag and but[n].Name != "" and but[n].Name != " ") {
        res = but[n].num;
        if (!butBlock[n]) {
          drawBut(n, B_JAM, "");
          delay(200);
          Beep(1, 0);
          x = y = 0;
        }
      }
    }
    n++;
  }
  return res;
}

void drawBut(uint8_t id, uint8_t state, String alt) {
  int bx = screenV ? but[id].xPosV : but[id].xPosH;
  int by = screenV ? but[id].yPosV : but[id].yPosH;

  if (alt == "") alt = but[id].Name;
  int type = but[id].type;
  if (alt == "") state = B_BLOCK;
  if (state == B_BLOCK) butBlock[id] = true; else butBlock[id] = false;

  uint16_t accent;
  if (type == B_GREEN) accent = TFT_GREEN;
  else if (type == B_GOLD) accent = TFT_GOLD;
  else accent = TFT_CYAN;

  uint16_t fillColor, borderColor, mainColor;
  const int radius = 8;

  if (state == B_BLOCK) {
    fillColor = TFT_BLACK;
    borderColor = TFT_DARKGREY;
    mainColor = TFT_DARKGREY;
  } else if (state == B_JAM) {
    fillColor = accent;
    borderColor = accent;
    mainColor = TFT_BLACK;
  } else if (state == B_SELECT or (type == B_GOLD and state != B_NORMAL)) {
    fillColor = accent;
    borderColor = accent;
    mainColor = TFT_BLACK;
  } else {
    fillColor = TFT_DARKGREY;
    borderColor = accent;
    mainColor = accent;
  }

  tft.fillRoundRect(bx, by, But_Width, But_Height, radius, fillColor);
  tft.drawRoundRect(bx, by, But_Width, But_Height, radius, borderColor);

  tftPlSetSize(1);
  tftPlSetStyle(REG_T);
  tftPlSetFont(T1516_T);
  int chosenHeight = 16;
  if (tftPlTextWidth(alt) > But_Width - 10) {
    tftPlSetFont(T1012_T);
    chosenHeight = 12;
  }
  tftPlSetDatum(BC_T);
  int cx = bx + (But_Width / 2);
  int cy = by + (But_Height + chosenHeight) / 2;

  tftPlSetColor(mainColor, TFT_TRANS);
  tftPlPrint(alt, cx, cy);
}

void screenRotate() {

  if (screenV) {
    tft.setRotation(0);
    tft.setTouch(calDataV);
  } else {
    tft.setRotation(1);
    tft.setTouch(calDataH);
  }
}

void view(String text, int x, int y) {

  tftPlSetFont(T1012_T);
  tftPlSetSize(1);
  tftPlSetColor(TFT_WHITE, TFT_BLACK);
  tftPlSetDatum(BL_T);
  tftPlSetStyle(BOL_T);
  tftPlPrint(text, x, y);
}
