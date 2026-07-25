// V.0.11PL  25.07.2026 Wi-FI Managing and timeout fix;
// 	ZMIANA: biblioteka "WiFiManager" by tzapu
// 	ZMIANA: usunięto dane logowania WiFi na sztywno w kodzie (WIFI_SSID / WIFI_PASS).
// 		- Konfiguracja sieci odbywa się teraz przez portal WiFiManager (patrz connectWifi())
// 		i jest zapisywana automatycznie w pamięci flash ESP32 - bez potrzeby rekompilacji.
// 	ZMIANA: opcja w SETUP - reset zapisanej sieci WiFi (otwiera portal konfiguracyjny)
// 	ZMIANA: connectWifi() nie blokuje już urządzenia w nieskończoność.
// 		- Jeśli jest zapisana sieć, WiFiManager próbuje się połączyć maks. connectTimeout sekund.
// 		- Jeśli połączenie się nie uda (lub nie ma zapisanej sieci), otwiera na krótko
//   		(configPortalTimeout sekund) własny punkt dostępowy "ATS25X2-Setup" z portalem
//   		konfiguracyjnym (wystarczy podłączyć się telefonem i wybrać sieć/hasło z listy).
// 		- Jeśli i to się nie powiedzie w wyznaczonym czasie, funkcja zwraca false
//   		i urządzenie startuje dalej w trybie offline (radio działa normalnie bez WiFi).
// 	ZMIANA: initTime() zakłada teraz, że WiFi jest już połączone (o to dba connectWifi()
// 		wywoływane wcześniej w setup()) - usunięto zduplikowane, blokujące WiFi.begin().
// 		Dodano limit prób (maxTries), więc funkcja nie zawiesi się, jeśli serwer NTP nie odpowiada.
// 	ZMIANA: connectWifi() zwraca teraz status (true/false) zamiast blokować urządzenie
// 		w nieskończoność. initTime() wywoływane jest tylko gdy WiFi faktycznie się połączyło.
// 		Po zakończeniu synchronizacji WiFi jest wyłączane - radio nie potrzebuje go
// 		do normalnej pracy, a wyłączenie oszczędza energię (istotne przy zasilaniu z baterii).
// 	ZMIANA: migracja / normalizacja ustawienia "WiFi enabled at boot"
// 	ZMIANA: reset ustawień radia NIE kasuje zapisanej sieci WiFi
// 	ZMIANA: reset/konfiguracja WiFi już nie wymaga restartu

// V.0.1PL  06.06.2026 REBRANDING - POLISH PROJECT ATS25X2;

//  V.5.3b  17.09.2022 Added Squelch Function, updated By  Lyle Hancoch Sr. from USA;
//  V.5.3a  27.08.2022 Date & Time  via WI-FI & charge initial screen updated By  Emphi Erte from Indonesia;
//  V.5.2a  15.05.2022 Fully RDS services - RDS Station  name - RDS News & info - RDS Time station Mod.by IU4ALH ;
//                     led colors on S-Meter bar

//  V4.0    23-02-2022 Binns MOD full rebuilding design and functionaly. See owerview on youtube channel:
//  V3.4    24-11-2021 Bug support.
//  V3.4    24-11-2021 Memory added for BFO in each band when crystal is in use.
//  V3.3    02-11-2021
//  V3.2.6b 29-10-2021  5351 calibration
//  V3.2.4  03-10-2021 100 Hz & 10 Hz added in SSB. Many changes in control interface. Bug support.
//                    SI5351 added, replacing crystal and also used as BFO. All steps and bandwidth per modulation
//                    are now stored in memory.
//  V3.2.3  29-09-2021 Added Sprite buttons from Jim Yasuda.
//  This sketch is based on the si4735 Library of Ricardo PU2CLR. Thanks for the very nice work.

//  This sketch uses  a 2.8 inch 240*320 touch-screen with ILI9341, ESP32 WROOM-32 and Rotary Encoder.
//  The radio is fully controlled by the (Touch)Screen and Rotary Encoder
//  This sketch uses the Rotary Encoder Class implementation from Ben Buxton (the source code is included
//  together with this sketch).
//  For the touch-screen the library TFT_eSPI is used. The configuration setup-file: setup1_ILI9341 is also
//  included.
//  Also a schematic drawing is available.

//  ABOUT SSB PATCH:
//  First of all, it is important to say that the SSB patch content is not part of this library. The paches used here were made available by Mr.
//  Vadim Afonkin on his Dropbox repository. It is important to note that the author of the SI473x library does not encourage anyone to use the SSB patches
//  content for commercial purposes. In other words, this library only supports SSB patches, the patches themselves are not part of this library.
//  This sketch will download a SSB patch to your SI4735 device (patch_init.h). It will take about 8KB of the Arduino memory.
//  In this context, a patch is a piece of software used to change the behavior of the SI4735 device.
//  There is little information available about patching the SI4735. The following information is the understanding of the author of
//  this project and it is not necessarily correct. A patch is executed internally (run by internal MCU) of the device.
//  Usually, patches are used to fixes bugs or add improvements and new features of the firmware installed in the internal ROM of the device.
//  Patches to the SI4735 are distributed in binary form and have to be transferred to the internal RAM of the device by
//  the host MCU (in this case Arduino). Since the RAM is volatile memory, the patch stored into the device gets lost when you turn off the system.
//  Consequently, the content of the patch has to be transferred again to the device each time after turn on the system or reset the device.

//  ATTENTION: The author of this project does not guarantee that procedures shown here will work in your development environment.
//  Given this, it is at your own risk to continue with the procedures suggested here.
//  This library works with the I2C communication protocol and it is designed to apply a SSB extension PATCH to CI SI4735-D60.
//  Once again, the author disclaims any liability for any damage this procedure may cause to your SI4735 or other devices, like an ATS-25 that you are using.
//  This sketch SHOULD work with the Chinese KIT ATS-25 sold on AliExpress, eBay etc.
//  The author of this sketch and Arduino Library does not know the seller of this kit and does not have a commercial relationship with any commercial product that uses the Arduino Library.
//  It is important you understand that there is no guarantee that this sketch will work correctly in your current product.
//  SO, DO NOT TRY IT IF YOU DON'T KNOW WHAT ARE YOU DOING. YOU MUST BE ABLE TO GO BACK TO THE PREVIOUS VERSION IF THIS SKETCH DOES NOT WORK FOR YOU.

//  Library TFT_eSPI you may download from here : https://github.com/Bodmer/TFT_eSPI
//  Library Rotary is provided with the program
//  Library SI4735 you may download from here   : https://github.com/pu2clr/SI4735
//
//  *********************************
//  **   Display connections etc.  **
//  *********************************
//  |------------|------------------|------------|------------|------------|
//  |Display 2.8 |      ESP32       |   Si4735   |  Encoder   |  Beeper    |
//  |  ILI9341   |                  |            |            |            |        Encoder        1,2,3
//  |------------|------------------|------------|------------|------------|        Encoder switch 4,5
//  |   Vcc      |     3V3     | 01 |    Vcc     |            |            |        pin 33 with 18K to 3.3 volt and 18K to ground.
//  |   GND      |     GND     | 02 |    GND     |     2,4    |            |        pin 32 (Beeper) via 2K to base V1  BC547
//  |   CS       |     15      | 03 |            |            |            |        Collector via beeper to 5v
//  |   Reset    |      4      | 04 |            |            |            |        Emmitor to ground
//  |   D/C      |      2      | 05 |            |            |            |
//  |   SDI      |     23      | 06 |            |            |            |        Encoder        1,2,3
//  |   SCK      |     18      | 07 |            |            |            |        Encoder switch 4,5
//  |   LED Coll.|     14 2K   | 08 |            |            |            |        Display LED
//  |   SDO      |             | 09 |            |            |            |        Emmitor  V2 BC557 to 3.3 V
//  |   T_CLK    |     18      | 10 |            |            |            |        Base with 2K to pin 14 (Display_Led)
//  |   T_CS     |      5      | 11 |            |            |            |        Collector to led pin display
//  |   T_DIN    |     23      | 12 |            |            |            |
//  |   T_DO     |     19      | 13 |            |            |            |
//  |   T_IRQ    |     34      | 14 |            |            |            |
//  |            |     12      |    |   Reset    |            |            |
//  |            |     21      |    |    SDA     |            |            |
//  |            |     22      |    |    SCL     |            |            |
//  |            |     16      |    |            |      1     |            |
//  |            |     17      |    |            |      3     |            |
//  |            |     33      |    |            |      5     |            |
//  |            |     32 2K   |    |            |            |     In     |
//  |            |     27 Mute |    |see schematics           |            |
//  |------------|-------------|----|------------|------------|------------|
