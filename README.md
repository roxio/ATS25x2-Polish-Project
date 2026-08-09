# Historia zmian

## v0.16PL (09.08.2026)

### Waterfall, plan pasm amatorskich oraz poprawki stabilności

W tej wersji rozbudowano skaner pasma o waterfall, plan pasm amatorskich, dodano tryb automatycznego zatrzymywania na wykrytym sygnale oraz naprawiono szereg błędów dotyczących Wi-Fi, audio i interfejsu ustawień.

### Dodano

- **Waterfall w skanerze pasma**
  - tryb łączony: górna połowa ekranu — kompaktowy wykres siły sygnału, dolna połowa — historia kolejnych przebiegów jako kolorowa mapa czasowo-częstotliwościowa,
  - bufor 38 wierszy historii, aktualizacja co ok. 2 sekundy,
  - **peak hold** — przygaszony ślad najsilniejszego dotychczas zarejestrowanego sygnału w każdym punkcie wykresu,
  - **znacznik segmentów pasm amatorskich** (CW / cyfrowe / fonia) wg band planu IARU Region 1, dla pasm 160–10m,
  - konfigurowalny próg wykrycia sygnału (dawniej na sztywno w kodzie),
  - tryb **"stop na sygnale"** — automatyczne zamiatanie wstrzymuje się na kilka sekund po wykryciu sygnału powyżej progu, po czym samo wznawia skanowanie,
  - włączane/wyłączane w ustawieniach.

- **Nowe opcje konfiguracji**
  - możliwość wyłączenia wygaszacza podczas skanowania pasma,
  - zapis nowych ustawień w pamięci EEPROM.

- **Diagnostyka pamięci**
  - licznik wolnej pamięci RAM na ekranie testowym, pomocny przy wykrywaniu fragmentacji pamięci podczas długiej pracy urządzenia.

### Zmieniono

- `EEPROM.commit()` w autozapisie i zapisie banku pamięci wykonuje się teraz tylko wtedy, gdy dane faktycznie się zmieniły (dawniej zapisywał bezwarunkowo co 5 sekund, co przy ciągłej pracy skracało żywotność pamięci flash),
- `WiFi.scanNetworks()` zwalnia teraz swój wewnętrzny bufor wyników po każdym skanowaniu sieci,
- teksty informacyjne na ekranie skanera (granice pasma, częstotliwości, skala) rysowane z czarnym obrysem zamiast na nieprzezroczystym tle — nie zasłaniają już danych wykresu/waterfalla,
- poprawiono układ i marginesy ekranu ustawień (mniejszy margines od krawędzi, większy obszar czyszczenia przy przełączaniu zakładek, poprawna kolejność rysowania dekoracji),
- po każdym przełączeniu zakładki ustawień odświeżane są wszystkie przyciski, nie tylko wybrane.

### Poprawiono

- **status Wi-Fi w ustawieniach** błędnie pokazywał "brak zapisanej sieci" mimo prawidłowo zapisanej i działającej sieci,
- **rejestrację pinu wyciszenia audio**, która była nieaktywna w kodzie, przez co wyciszanie (start urządzenia, skanowanie, squelch) nie miało żadnego efektu,
- **pyknięcie w głośniku przy starcie urządzenia** — dodano ponowne wyciszenie po inicjalizacji chipu radiowego oraz płynne narastanie głośności od zera zamiast skoku na pełną wartość,
- wyciek pamięci w wygaszaczu ekranu, powodujący psucie wyglądu przycisków po powrocie z wygaszacza,
- przeliczanie pozycji dotyku na pozycję w menu ustawień (niezgodność z aktualnym układem wierszy).

---

## v0.15PL (06.08.2026)

### Kalibracja akumulatora, wykres RSSI oraz kolejne usprawnienia interfejsu

W tej wersji dodano możliwość kalibracji wskaźnika baterii, wykres historii siły sygnału RSSI oraz kolejne usprawnienia interfejsu użytkownika. Rozszerzono konfigurację EEPROM, poprawiono obsługę Wi-Fi i uporządkowano kod odpowiedzialny za konfigurację oraz wyświetlanie danych.

### Dodano

- **Kalibrację akumulatora**

  - konfigurowalne minimalne i maksymalne napięcie,
  - dokładniejsze wyznaczanie poziomu naładowania.

- **Wykres historii RSSI**

  - bieżący wykres zmian siły sygnału,
  - możliwość włączenia lub wyłączenia funkcji.

- **Nowe opcje konfiguracji**

  - możliwość wyłączenia wygaszacza podczas skanowania pasma,
  - zapis nowych ustawień w pamięci EEPROM.

### Zmieniono

- przebudowano sposób wyświetlania tekstu i przycisków,
- odświeżono ekran startowy oraz wyświetlanie logo,
- usprawniono konfigurację i obsługę sieci Wi-Fi,
- rozszerzono strukturę danych zapisywanych w EEPROM,
- zaktualizowano informacje o wersji programu.

### Poprawiono

- dokładność wskazań poziomu naładowania akumulatora,
- zgodność z wcześniejszymi wersjami danych zapisanych w EEPROM,
- inicjalizację urządzenia i obsługę wyświetlacza podczas uruchamiania,
- organizację kodu oraz stabilność działania programu.

## v0.14PL (03.08.2026)
### Wygaszacz ekranu oraz usprawnienia zarządzania konfiguracją Wi-Fi

W tej wersji dodano konfigurowalny wygaszacz ekranu oraz przebudowano sposób przechowywania konfiguracji sieci Wi-Fi. Rozszerzono mechanizm migracji ustawień, zwiększono niezawodność obsługi pamięci EEPROM i usprawniono zapis oraz odczyt konfiguracji urządzenia.

### Dodano

- **Konfigurowalny wygaszacz ekranu**
  - regulowany czas aktywacji (30 s – 30 min),
  - animowany zegar odbijający się od krawędzi ekranu,
  - automatyczną zmianę kolorów.

- **Trwały zapis konfiguracji Wi-Fi**
  - zapisywanie nazwy sieci (SSID) i hasła,
  - automatyczne odtwarzanie konfiguracji po uruchomieniu urządzenia.

- **Dynamiczny tekst**
  - automatyczne dopasowanie tekstu do elementów interfejsu,
  - dalsze poprawki polskiej czcionki.

### Zmieniono

- przebudowano mechanizm zapisu i odczytu konfiguracji Wi-Fi,
- urządzenie korzysta w pierwszej kolejności z danych zapisanych w EEPROM,
- konfiguracja sieci jest automatycznie aktualizowana po połączeniu z Wi-Fi,
- rozszerzono mechanizm migracji ustawień.

### Poprawiono

- obsługę i walidację danych EEPROM,
- obsługę błędów odczytu konfiguracji Wi-Fi,
- organizację kodu oraz stabilność aktualizacji ustawień.
- zwiekszono ilość pamięci do 2304

---

## v0.13PL (30.07.2026)
### Własny kreator konfiguracji Wi-Fi oraz nowe funkcje interfejsu

Całkowicie przebudowano konfigurację sieci Wi-Fi. Zamiast portalu WiFiManager użytkownik może skonfigurować połączenie bezpośrednio z poziomu radia. Dodano ekranową klawiaturę, ekran testowy oraz szereg usprawnień interfejsu.

### Dodano

- **Własny kreator konfiguracji Wi-Fi**
  - automatyczne skanowanie i sortowanie sieci,
  - przewijaną listę dostępnych sieci,
  - oznaczanie sieci otwartych i zabezpieczonych,
  - możliwość pominięcia konfiguracji.

- **Ekranową klawiaturę**
  - wpisywanie hasła z poziomu radia,
  - obsługę liter, cyfr i znaków specjalnych,
  - Shift, Backspace i Spację,
  - podgląd wpisywanego hasła.

- **Ekran testowy**
  - test kolorów RGB,
  - test wyświetlania polskich znaków.

### Zmieniono

- przebudowano sposób konfiguracji Wi-Fi,
- urządzenie najpierw próbuje połączyć się z zapisaną siecią,
- po nieudanej próbie uruchamiany jest kreator konfiguracji,
- WiFiManager służy wyłącznie do zapisu konfiguracji,
- po synchronizacji czasu moduł Wi-Fi jest automatycznie wyłączany.

### Poprawiono

- synchronizację czasu NTP,
- zarządzanie modułem Wi-Fi,
- responsywność interfejsu,
- komunikaty ekranowe,
- obsługę pracy offline,
- wyświetlanie polskich znaków.

---

## v0.12PL (26.07.2026)
### Polonizacja interfejsu oraz usprawnienia konfiguracji Wi-Fi

### Dodano

- pełną polonizację menu i komunikatów,
- poprawioną obsługę polskich znaków,
- nowe komunikaty diagnostyczne,
- stronę Wi-Fi w ustawieniach.

### Zmieniono

- przetłumaczono większość interfejsu,
- przebudowano ekran ustawień,
- poprawiono wygląd menu,
- zmieniono nazwę punktu dostępowego WiFiManager,
- zabezpieczono portal konfiguracyjny hasłem,
- zaktualizowano ekran startowy i informacje o wersji.

### Poprawiono

- inicjalizację EEPROM,
- synchronizację czasu,
- obsługę Wi-Fi,
- komunikaty diagnostyczne,
- organizację kodu.

---

## v0.11PL (25.07.2026)
### Zarządzanie siecią Wi-Fi oraz poprawa mechanizmów timeout

Całkowicie przebudowano obsługę sieci Wi-Fi, zastępując dotychczasowe rozwiązanie biblioteką **WiFiManager**.

### Zmiany

- wykorzystano bibliotekę **WiFiManager** do konfiguracji Wi-Fi,
- usunięto zapisane w kodzie dane logowania (`WIFI_SSID` i `WIFI_PASS`),
- konfiguracja sieci odbywa się przez portal WiFiManager z zapisem danych w pamięci ESP32,
- dodano możliwość resetowania konfiguracji Wi-Fi z poziomu menu,
- funkcja `connectWifi()` działa nieblokująco i umożliwia uruchomienie radia w trybie offline,
- dodano limity czasu połączenia i synchronizacji NTP,
- synchronizacja czasu wykonywana jest wyłącznie po udanym połączeniu z siecią,
- po synchronizacji moduł Wi-Fi jest automatycznie wyłączany,
- dodano mechanizm migracji ustawień,
- przywracanie ustawień fabrycznych nie usuwa zapisanej konfiguracji Wi-Fi,
- ponowna konfiguracja Wi-Fi nie wymaga restartu urządzenia.

---

## v0.1PL (06.06.2026)
### Polski projekt ATS25X2

- Rebranding projektu - Powstanie tej wersji oprogramowania ATS25X2.

---
## v5.3b (17.09.2022)
Autor zmian: **Lyle Hancock Sr. (USA)**
WERSJA BAZOWA od której projekt jest aktualizowany
