// ============================================================================
// PALETA KOLORÓW - styl klasycznej profesjonalnej radiostacji krótkofalarskiej
// (bursztynowy odczyt VFD + chłodny "instrument teal" na czarnym tle,
//  inspirowane Yaesu / Collins / Drake)
//
// Ten plik ładuje się PO bibliotece TFT_eSPI.h, więc #undef + #define poniżej
// nadpisuje domyślne kolory biblioteki w CAŁYM projekcie - bez dotykania
// pliku .ino. Oryginalne (domyślne) wartości TFT_eSPI zostały zachowane
// w komentarzach obok każdej definicji, dla porównania / łatwego powrotu.
// ============================================================================

#undef TFT_BLACK
#define TFT_BLACK       0x0000      /*   0,   0,   0   - bez zmian, czysta czern tla */

#undef TFT_NAVY
#define TFT_NAVY        0x0864      /*   8,  12,  32   - bylo 0,0,128 - ciemniejszy, bardziej "panelowy" granat */

#undef TFT_DARKGREEN
#define TFT_DARKGREEN   0x0AC5      /*   8,  89,  41   - bylo 0,128,0 */

#undef TFT_DARKCYAN
#define TFT_DARKCYAN    0x0B6F      /*   8, 109, 123   - bylo 0,128,128 - obrys S-metra/wygaszacza */

#undef TFT_MAROON
#define TFT_MAROON      0x5882      /*  90,  16,  16   - bylo 128,0,0 - tlo alertow/potwierdzen */

#undef TFT_PURPLE
#define TFT_PURPLE      0x7952      /* 123,  40, 148   - bylo 128,0,128 */

#undef TFT_OLIVE
#define TFT_OLIVE       0x7AE4      /* 123,  93,  32   - bylo 128,128,0 - przygaszony bursztyn zamiast oliwki (podzialka skali) */

#undef TFT_LIGHTGREY
#define TFT_LIGHTGREY   0xC67A      /* 197, 206, 213   - bylo 211,211,211 - lekko chlodniejszy stalowy odcien */

#undef TFT_DARKGREY
#define TFT_DARKGREY    0x39E8      /*  57,  60,  65   - bylo 128,128,128 - ciemna "obudowa" przyciskow/naglowkow */

#undef TFT_BLUE
#define TFT_BLUE        0x11F8      /*  16,  60, 197   - bylo 0,0,255 */

#undef TFT_GREEN
#define TFT_GREEN       0x3F2D      /*  57, 230, 106   - bylo 0,255,0 - "fosfor CRT", RDS/aktywnosc */

#undef TFT_CYAN
#define TFT_CYAN        0x2E3B      /*  41, 198, 222   - bylo 0,255,255 - glowny chlodny akcent (S-metr, wskazniki wtorne) */

#undef TFT_RED
#define TFT_RED         0xE145      /* 230,  40,  41   - bylo 255,0,0 - kursor/alarm, lekko przygaszona czerwien */

#undef TFT_MAGENTA
#define TFT_MAGENTA     0xD953      /* 222,  40, 156   - bylo 255,0,255 */

#undef TFT_YELLOW
#define TFT_YELLOW      0xFD60      /* 255, 174,   0   - bylo 255,255,0 - GLOWNY bursztynowy odczyt czestotliwosci (VFD) */

#undef TFT_WHITE
#define TFT_WHITE       0xF7DE      /* 246, 250, 246   - bylo 255,255,255 - lekko cieplejsza biel */

#undef TFT_ORANGE
#define TFT_ORANGE      0xFC02      /* 255, 129,  16   - bylo 255,180,0 - akcent ostrzegawczy */

#undef TFT_GREENYELLOW
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0   - bez zmian */

#undef TFT_PINK
#define TFT_PINK        0xE3D2      /* 230, 121, 148   - bylo 255,192,203 */

#undef TFT_LIGHTPINK
#define TFT_LIGHTPINK   0xFC9F      /* bez zmian */

#undef TFT_BROWN
#define TFT_BROWN       0x6A24      /* 106,  68,  32   - bylo 150,75,0 */

#undef TFT_GOLD
#define TFT_GOLD        0xFE27      /* 255, 198,  57   - bylo 255,215,0 - drugorzedny akcent bursztynowy */

#undef TFT_SILVER
#define TFT_SILVER      0xAD97      /* 172, 178, 189   - bylo 192,192,192 - chlodny stalowy (tekst przyciskow) */

#undef TFT_SKYBLUE
#define TFT_SKYBLUE     0x5D5C      /*  90, 170, 230   - bylo 135,206,235 - zimny akcent (np. pole STEP) */

#undef TFT_VIOLET
#define TFT_VIOLET      0x89F8      /* 139,  60, 197   - bylo 180,46,226 */

// ============================================================================
// Semantyczne aliasy (obecnie w wiekszosci nieuzywane bezposrednio w kodzie,
// zachowane na przyszlosc / do ew. dalszej rozbudowy UI)
// ============================================================================
#define COLOR_BACKGROUND      TFT_BLACK
#define COLOR_FRAME           TFT_BLACK
#define COLOR_BUTTON_BG       TFT_DARKGREY
#define COLOR_BUTTON_ON1      TFT_PINK
#define COLOR_BUTTON_ON2      TFT_DARKCYAN
#define COLOR_BUTTON_TEXT     TFT_BLACK
#define COLOR_BUTTON_TEMP     TFT_RED
#define COLOR_BUTTON_SEND     TFT_ORANGE
#define COLOR_INDICATOR_FREQ  TFT_YELLOW
#define COLOR_INDICATOR_TXT1  TFT_GREEN
#define COLOR_INDICATOR_TXT2  TFT_RED
#define COLOR_INDICATOR_TXT3  TFT_ORANGE
#define COLOR_PANEL_TEXT      TFT_WHITE
#define COLOR_RDS_TEXT        TFT_GREEN
#define COLOR_RSMTRF          TFT_RED
#define COLOR_RSMTRB          TFT_BLACK
#define COLOR_SNRMTRF         TFT_SKYBLUE
#define COLOR_SNRMTRB         TFT_BLACK
#define COLOR_MULTMTRF        TFT_GREENYELLOW
#define COLOR_MULTMTRB        TFT_BLACK
#define COLOR_BLNDMTRF        TFT_BLUE
#define COLOR_BLNDMTRB        TFT_BLACK
#define COLOR_VOLF            TFT_ORANGE
#define COLOR_VOLB            TFT_BLACK
#define COLOR_BFO             TFT_ORANGE
