# Historia zmian

## v0.11PL (25.07.2026)
### Zarządzanie siecią Wi-Fi oraz poprawa mechanizmów timeout

W tej wersji całkowicie przebudowano sposób obsługi sieci Wi-Fi, zastępując dotychczasowe rozwiązanie biblioteką **WiFiManager** autorstwa **tzapu**.

### Zmiany

- **Zmieniono bibliotekę odpowiedzialną za konfigurację Wi-Fi**
  - Zastosowano bibliotekę **WiFiManager**.

- **Usunięto zapisane na stałe dane logowania do sieci Wi-Fi**
  - Z kodu usunięto definicje `WIFI_SSID` oraz `WIFI_PASS`.
  - Konfiguracja sieci odbywa się teraz za pomocą portalu konfiguracyjnego WiFiManager (funkcja `connectWifi()`).
  - Dane dostępowe są automatycznie zapisywane w pamięci Flash ESP32 i pozostają zachowane po ponownym uruchomieniu urządzenia.
  - Zmiana sieci Wi-Fi nie wymaga już rekompilacji ani ponownego wgrywania programu.

- **Dodano możliwość resetowania konfiguracji Wi-Fi**
  - W menu **SETUP** pojawiła się opcja usuwania zapisanych danych sieciowych.
  - Po wybraniu tej funkcji uruchamiany jest portal konfiguracyjny umożliwiający ponowną konfigurację połączenia.

- **Funkcja `connectWifi()` nie blokuje już pracy urządzenia**
  - Jeżeli w pamięci znajduje się zapisane połączenie, WiFiManager próbuje połączyć się z siecią przez maksymalnie `connectTimeout` sekund.
  - Jeżeli połączenie nie powiedzie się lub żadna sieć nie jest zapisana, urządzenie uruchamia tymczasowy punkt dostępowy **ATS25X2-Setup** wraz z portalem konfiguracyjnym.
  - Wystarczy połączyć się z tym punktem dostępowym za pomocą telefonu lub komputera i wybrać z listy odpowiednią sieć Wi-Fi oraz podać hasło.
  - Portal działa przez określony czas (`configPortalTimeout` sekund).
  - Jeżeli konfiguracja nie zostanie zakończona w wyznaczonym czasie, funkcja zwraca wartość `false`, a urządzenie uruchamia się w trybie offline.
  - Radio działa wtedy całkowicie normalnie, jedynie funkcje wymagające dostępu do Internetu pozostają niedostępne.

- **Zmodyfikowano działanie funkcji `initTime()`**
  - Funkcja zakłada teraz, że połączenie z siecią zostało już zestawione przez wcześniejsze wywołanie `connectWifi()` w funkcji `setup()`.
  - Usunięto zduplikowane i blokujące wywołanie `WiFi.begin()`.
  - Dodano limit prób synchronizacji (`maxTries`), dzięki czemu urządzenie nie zawiesi się w przypadku niedostępności serwera NTP.

- **Zmieniono sposób synchronizacji czasu**
  - `connectWifi()` zwraca teraz wynik operacji (`true` lub `false`) zamiast blokować działanie programu.
  - Synchronizacja czasu wykonywana jest wyłącznie wtedy, gdy urządzenie rzeczywiście połączyło się z siecią.

- **Automatyczne wyłączanie modułu Wi-Fi**
  - Po zakończeniu synchronizacji czasu moduł Wi-Fi jest automatycznie wyłączany.
  - Radio nie wymaga aktywnego połączenia z siecią podczas normalnej pracy.
  - Wyłączenie Wi-Fi zmniejsza pobór energii, co ma szczególne znaczenie podczas pracy z zasilania bateryjnego.

- **Migracja ustawień**
  - Dodano mechanizm migracji i normalizacji ustawienia **„Wi-Fi włączone przy uruchamianiu”**.

- **Reset ustawień radia**
  - Przywrócenie ustawień fabrycznych radia nie usuwa zapisanych danych sieci Wi-Fi.

- **Konfiguracja Wi-Fi bez restartu**
  - Resetowanie oraz ponowna konfiguracja połączenia Wi-Fi nie wymagają już ponownego uruchamiania urządzenia.

---

## v0.1PL (06.06.2026)
### Polski projekt ATS25X2

- Rebranding projektu.
- Powstanie polskiej wersji oprogramowania ATS25X2.

---
## v5.3b (17.09.2022)
Autor zmian: **Lyle Hancock Sr. (USA)**
### Dodano
- funkcję **Squelch**.
---
## v5.3a (27.08.2022)
Autor zmian: **Emphi Erte (Indonezja)**
### Dodano
- synchronizację daty i czasu przez Wi-Fi,
- nowy ekran startowy podczas ładowania.
---
## v5.2a (15.05.2022)
Modyfikacje: **IU4ALH**
### Dodano

Pełną obsługę systemu **RDS**:

- nazwę stacji,
- informacje tekstowe (RadioText),
- wiadomości,
- czas nadawany przez stację radiową.

### Zmieniono

- kolorowy wskaźnik S-Meter.

---

## v4.0 (23.02.2022)

Autor: **Binns**

### Zmiany

- całkowicie przebudowano wygląd programu,
- przeprojektowano interfejs użytkownika,
- przebudowano funkcjonalność aplikacji.

---

## v3.4 (24.11.2021)

### Dodano

- zapamiętywanie ustawień BFO osobno dla każdego pasma podczas pracy z rezonatorem kwarcowym.

### Poprawiono

- liczne błędy.

---

## v3.3 (02.11.2021)

- aktualizacja wersji.

---

## v3.2.6b (29.10.2021)

### Dodano

- kalibrację generatora SI5351.

---

## v3.2.4 (03.10.2021)

### Dodano

- krok strojenia 100 Hz i 10 Hz dla emisji SSB,
- obsługę układu SI5351 zastępującego rezonator kwarcowy oraz pełniącego funkcję BFO,
- zapamiętywanie kroków strojenia i szerokości pasma dla poszczególnych modulacji.

### Zmieniono

- interfejs użytkownika,
- sposób sterowania.

### Poprawiono

- liczne błędy.

---

## v3.2.3 (29.09.2021)

### Dodano

- przyciski typu Sprite autorstwa **Jim Yasuda**.

---

# O projekcie

Projekt bazuje na bibliotece **SI4735 Library** autorstwa **Ricardo PU2CLR**, której autorowi należą się szczególne podziękowania za udostępnienie swojej pracy.

Radio wykorzystuje:

- wyświetlacz dotykowy **2,8" ILI9341 (240×320)**,
- mikrokontroler **ESP32-WROOM-32**,
- odbiornik **SI4735**,
- enkoder obrotowy.

Całość obsługi odbywa się za pomocą ekranu dotykowego oraz enkodera.

---

# Wykorzystane biblioteki

- SI4735 Library
- TFT_eSPI
- WiFiManager
- Rotary Encoder Library

---

# Informacje o poprawce SSB (SSB Patch)

Projekt umożliwia wgrywanie zewnętrznej poprawki (patcha) rozszerzającej możliwości odbiornika **SI4735** o obsługę emisji **SSB**.

Należy podkreślić, że:

- poprawka **nie jest częścią biblioteki SI4735**,
- autor biblioteki nie jest autorem samego patcha,
- pliki patcha zostały udostępnione przez **Vadima Afonkina**,
- biblioteka jedynie umożliwia przesłanie patcha do pamięci RAM układu podczas uruchamiania.

Patch:

- zajmuje około **8 kB pamięci Arduino**,
- jest ładowany do pamięci RAM odbiornika,
- po wyłączeniu zasilania zostaje utracony,
- dlatego musi zostać ponownie przesłany przy każdym uruchomieniu urządzenia.

Patch służy do zmiany zachowania wewnętrznego oprogramowania układu SI4735 i może dodawać nowe funkcje lub poprawiać błędy producenta.

Autor biblioteki SI4735 nie zachęca do wykorzystywania patchy w zastosowaniach komercyjnych.

---

# Zastrzeżenia

Projekt udostępniany jest **bez jakiejkolwiek gwarancji**.

Autorzy nie ponoszą odpowiedzialności za:

- uszkodzenie układu SI4735,
- uszkodzenie mikrokontrolera ESP32,
- uszkodzenie radia ATS-25,
- ani żadnych innych elementów sprzętowych.

Całość modyfikacji wykonywana jest wyłącznie na własną odpowiedzialność użytkownika.

Jeżeli nie masz możliwości powrotu do poprzedniej wersji oprogramowania lub nie wiesz dokładnie, co robisz, nie zaleca się instalowania tej wersji.

Nie ma również gwarancji, że projekt będzie poprawnie współpracował z każdą wersją sprzętową radia ATS-25 dostępnego na rynku (AliExpress, eBay i inne platformy sprzedażowe).
