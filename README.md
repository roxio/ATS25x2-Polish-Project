# ATS25X2 — Instrukcja obsługi

**SI4735 2.8" TFT WiFi — Wersja Polska**

Wersja firmware: **0.17PL**
Data wydania: 12 sierpnia 2026
Projekt: ATS25X2 Polish Project (RoX10 PL MOD)

---

## Spis treści

1. [Wprowadzenie](#1-wprowadzenie)
2. [Pierwsze uruchomienie](#2-pierwsze-uruchomienie)
3. [Ekran główny](#3-ekran-główny)
4. [Gesty dotykowe na wyświetlaczu częstotliwości](#4-gesty-dotykowe-na-wyświetlaczu-częstotliwości)
5. [S-metr i wskaźniki siły sygnału](#5-s-metr-i-wskaźniki-siły-sygnału)
6. [Skanowanie pasma](#6-skanowanie-pasma)
7. [Pamięć stacji](#7-pamięć-stacji)
8. [RDS (Radio Data System)](#8-rds-radio-data-system)
9. [Tryb nocny](#9-tryb-nocny)
10. [Menu ustawień](#10-menu-ustawień)
11. [Panel sterowania WWW](#11-panel-sterowania-www)
12. [Rozwiązywanie problemów](#12-rozwiązywanie-problemów)

---

## 1. Wprowadzenie

ATS25X2 to odbiornik radiowy oparty na układzie SI4735, wyposażony w 2,8-calowy dotykowy wyświetlacz TFT oraz moduł WiFi. Niniejsza polska wersja projektu (ATS25X2 Polish Project) rozbudowuje oryginalne oprogramowanie o pełną polonizację interfejsu, zaawansowany skaner pasma z waterfallem, rozbudowany system pamięci stacji, tryb nocny, kalibrowany S-metr, oraz — od wersji 0.17PL — pełny panel sterowania dostępny przez przeglądarkę internetową.

Odbiornik obsługuje pasma FM, MW, LW, SW oraz zakresy amatorskie w trybach AM, LSB, USB i CW.

### 1.1 Zawartość niniejszej instrukcji

Instrukcja opisuje wszystkie funkcje dostępne w wersji firmware 0.17PL: obsługę podstawową, gesty dotykowe, skanowanie pasma, pamięć stacji, menu ustawień oraz panel sterowania WWW. Rozdziały ułożone są od podstaw obsługi do funkcji zaawansowanych.

> **Uwaga:** Niektóre funkcje (np. dekoder CW) wymagają dodatkowego, opcjonalnego sprzętu podłączonego do GPIO odbiornika i mogą nie działać na standardowej, niemodyfikowanej płytce.

---

## 2. Pierwsze uruchomienie

Po włączeniu urządzenia na ekranie pojawia się logo powitalne z delikatną animacją rozjaśniania, a następnie ekran z informacjami o wersji oprogramowania i wykrytym module SI4735.

### 2.1 Reset do ustawień fabrycznych

Aby przywrócić ustawienia fabryczne, przytrzymaj wciśnięty przycisk enkodera od momentu włączenia urządzenia — nieprzerwanie, aż do zakończenia sekwencji startowej (ok. 6-7 sekund). Urządzenie automatycznie nadpisze zapisaną konfigurację wartościami domyślnymi.

### 2.2 Obrót ekranu (orientacja pozioma/pionowa)

Aby obrócić ekran, przytrzymaj wciśnięty przycisk enkodera przez około 1 sekundę, a następnie — wciąż go trzymając — dotknij ekranu. Samo przytrzymanie enkodera bez dotknięcia ekranu uruchamia inną funkcję (patrz rozdz. 4.6).

### 2.3 Konfiguracja WiFi

Jeśli urządzenie nie ma zapisanej sieci WiFi, po starcie automatycznie uruchamia się własny kreator konfiguracji: skanuje dostępne sieci, wyświetla je na liście (posortowane, z oznaczeniem sieci otwartych i zabezpieczonych), a po wybraniu sieci zabezpieczonej pokazuje ekranową klawiaturę do wpisania hasła.

> **Wskazówka:** Konfigurację WiFi można też w każdej chwili uruchomić ponownie lub zresetować z poziomu USTAWIENIA → WIFI.

---

## 3. Ekran główny

Ekran główny wyświetla bieżącą częstotliwość, pasmo, tryb pracy, S-metr, zegar oraz wskaźnik baterii. W dolnej części znajdują się przyciski szybkiego dostępu do najczęściej używanych funkcji (pasmo, tryb, głośność, strojenie, skanowanie, pamięć, ustawienia).

W lewym dolnym rogu wyświetlacza częstotliwości, w miarę korzystania z funkcji opisanych w rozdziale 4, mogą pojawiać się małe wskaźniki:

- **LOCK** — czerwony napis, gdy ekran jest zablokowany
- **VFO A / VFO B** — które VFO jest aktualnie aktywne
- **PRIO / PRIO!** — status kanału priorytetowego
- **MM:SS** — stoper nasłuchu, jeśli był uruchomiony

---

## 4. Gesty dotykowe na wyświetlaczu częstotliwości

Poza standardowymi przyciskami ekranowymi, wyświetlacz częstotliwości reaguje na dodatkowe gesty dotykowe. Wszystkie działają wyłącznie na ekranie głównym (nie w menu, skanerze itd.) i są nieaktywne, gdy ekran jest zablokowany — z wyjątkiem samego odblokowania.

| Strefa ekranu | Gest | Funkcja |
|---|---|---|
| Lewa część (poza SSB) | Krótkie dotknięcie | Historia ostatnich częstotliwości |
| Lewa część | Przytrzymanie ~600 ms | Blokada / odblokowanie ekranu |
| Środek (poza SSB/CW) | Krótkie dotknięcie | Zmiana kroku strojenia |
| Prawa część | Przytrzymanie ~600 ms | Przełączenie VFO A / VFO B |
| Obszar S-metra | Krótkie dotknięcie | Zapisz bieżącą częstotliwość jako priorytetową |
| Obszar S-metra | Przytrzymanie ~600 ms | Włącz / wyłącz kanał priorytetowy |
| Obok wskaźnika PRIO | Krótkie dotknięcie | Start / stop stopera |
| Obok wskaźnika PRIO | Przytrzymanie ~600 ms | Reset stopera |

> **Uwaga:** W trybie SSB/CW środkowa część wyświetlacza służy do wyboru cyfry przy precyzyjnym strojeniu ręcznym — gest zmiany kroku jest tam wyłączony, żeby nie kolidować z tą funkcją.

### 4.1 Blokada ekranu (LOCK)

Przytrzymanie lewej części wyświetlacza częstotliwości (~600 ms) blokuje dotyk i obrót enkodera — przydatne np. podczas przenoszenia radia w torbie, żeby przypadkowo nie zmienić częstotliwości. Odbiornik nadal normalnie odbiera i wyświetla dane (zegar, S-metr) — blokada dotyczy wyłącznie wejścia użytkownika. Ten sam gest odblokowuje ekran.

### 4.2 Dwa VFO (A/B)

Odbiornik obsługuje dwa niezależne VFO, każde pamiętające własne pasmo, tryb, częstotliwość, krok strojenia i offset BFO. Przytrzymanie prawej części wyświetlacza częstotliwości (~600 ms) przełącza między VFO A i VFO B.

Przy pierwszym przełączeniu na VFO B, zaczyna ono jako kopia aktualnego stanu VFO A — dopiero od tego momentu można przestroić je niezależnie.

### 4.3 Historia częstotliwości

Krótkie dotknięcie lewej części wyświetlacza (gdy ekran nie jest zablokowany) otwiera listę do 10 ostatnio używanych częstotliwości. Częstotliwość trafia na listę dopiero, gdy jest stabilna (bez zmian) przez 4 sekundy — dzięki temu lista nie zapełnia się przypadkowymi wartościami podczas przekręcania gałki.

Dotknięcie pozycji na liście przywraca ją; dotknięcie poza listą zamyka ją bez zmian.

### 4.4 Kanał priorytetowy

Funkcja pozwala monitorować wybraną częstotliwość w tle, podczas gdy słuchasz innej stacji.

- Krótkie dotknięcie obszaru S-metra zapisuje bieżącą częstotliwość jako priorytetową i włącza monitorowanie.
- Co ok. 8 sekund odbiornik na chwilę sprawdza kanał priorytetowy.
- Jeśli wykryje tam sygnał — zostaje na nim (wskaźnik zmienia się na „PRIO!”), automatyczny powrót nastąpi po 15 sekundach ciszy.
- Jeśli sygnału nie ma — natychmiastowy powrót do poprzedniej częstotliwości.

Długie przytrzymanie obszaru S-metra włącza lub wyłącza monitorowanie bez zmiany zapisanego celu.

### 4.5 Stoper nasłuchu / QSO

Prosty stoper wyświetlany w formacie MM:SS. Krótkie dotknięcie startuje/zatrzymuje, długie przytrzymanie resetuje do zera.

### 4.6 Szybka zmiana kroku strojenia

Dotknięcie środkowej części wyświetlacza częstotliwości (poza trybem SSB/CW) cyklicznie zmienia krok strojenia bez konieczności wchodzenia w menu STEP:

- **AM/SW:** 1 → 5 → 9 → 10 kHz → z powrotem
- **FM:** 10 kHz → 100 kHz → z powrotem
- **MW/LW:** 1 → 5 → 9 → 10 kHz → z powrotem

---

## 5. S-metr i wskaźniki siły sygnału

W obszarze S-metra odbiornik może wyświetlać kilka różnych widoków. Wybór trybu odbywa się w ustawieniach (USTAWIENIA → DOMYŚLNE oraz USTAWIENIA → SI473X) lub zdalnie przez panel WWW.

### 5.1 Klasyczny pasek S-metra

Domyślny widok — pozioma belka rosnąca wraz z siłą sygnału. W prawym górnym rogu wyświetlana jest etykieta S-punktu (np. „S7”, „S9+20”), wyliczana z dokładnych progów dBuV, osobno skalibrowanych dla trybu FM i pozostałych.

Na pasku widoczna jest też biała kreska Peak Hold — pokazuje niedawny szczyt sygnału i powoli opada, gdy sygnał słabnie.

### 5.2 Wykres trendu RSSI

Włączany w USTAWIENIA → SI473X. Zamiast pojedynczego paska pokazuje historię zmian siły sygnału w czasie.

### 5.3 Analogowy S-metr

Włączany w USTAWIENIA → DOMYŚLNE. Klasyczny, wychylający się wskaźnik na łuku ze skalą, w stylu tradycyjnych radiostacji.

### 5.4 Przełączanie trybu metera

Tryb wyświetlania S-metra (klasyczny / trend RSSI / analogowy) można cyklicznie przełączać jednym przyciskiem w panelu sterowania WWW (patrz rozdz. 11), bez konieczności wchodzenia w menu urządzenia.

---

## 6. Skanowanie pasma

Tryb skanowania (przycisk SCAN) automatycznie przemiata wybrane pasmo, pokazując graficzny wykres siły sygnału.

### 6.1 Waterfall

Włączany w USTAWIENIA → WYŚWIETLACZ. W trybie łączonym górna połowa ekranu pokazuje kompaktowy, bieżący wykres siły sygnału, a dolna — historię kolejnych przebiegów jako kolorową mapę czasowo-częstotliwościową (im cieplejszy kolor, tym silniejszy sygnał). Bufor obejmuje 38 ostatnich przebiegów.

### 6.2 Plan pasm amatorskich

Na wykresie skanera widoczne są kolorowe znaczniki segmentów pasm amatorskich (CW / cyfrowe / fonia) zgodnie z planem pasm IARU Region 1, dla zakresów 160–10 m.

### 6.3 Próg wykrycia i zatrzymanie na sygnale

Próg SNR, powyżej którego sygnał jest oznaczany jako wykryty, można ustawić w USTAWIENIA → SI473X („Prog wykrycia SNR”). Po włączeniu opcji „Stop na sygnale” skaner automatycznie wstrzymuje przemiatanie na kilka sekund po wykryciu sygnału powyżej progu, po czym samoczynnie wznawia.

### 6.4 Tryb Close Call

Włączany wyłącznie przez panel WWW (menu opcji). Po zakończeniu pełnego przebiegu skanowania odbiornik automatycznie przeskakuje na najsilniejszy punkt zarejestrowany podczas tego przebiegu (na bazie danych Peak Hold) i zatrzymuje się tam.

### 6.5 Lista wykluczeń (skip list)

Częstotliwości dodane do listy wykluczeń są pomijane zarówno przy zwykłym oznaczaniu sygnału, jak i przy funkcjach „Stop na sygnale” oraz Close Call. Lista zarządzana jest wyłącznie przez panel WWW — z możliwością importu i eksportu jako plik .txt (patrz rozdz. 11.5).

### 6.6 Log Discovery

Każde zatrzymanie skanera na sygnale (czy to przez „Stop na sygnale”, czy Close Call) jest zapisywane w logu wraz z czasem, częstotliwością i wartością SNR. Log dostępny jest w panelu WWW, z możliwością eksportu do pliku .txt.

---

## 7. Pamięć stacji

Odbiornik posiada bank pamięci na zapis ulubionych stacji wraz z nazwami. Podstawowy zapis, odczyt i przeglądanie banku pamięci odbywa się z poziomu ekranu głównego (przycisk pamięci).

### 7.1 Zarządzanie pamięcią przez panel WWW

Panel sterowania WWW oferuje znacznie rozszerzone możliwości zarządzania bankiem pamięci:

- **Wyszukiwarka** — filtrowanie listy po fragmencie nazwy stacji
- **Sortowanie** — według nazwy, częstotliwości lub pasma
- **Grupowanie według pasma** — stacje pogrupowane jak w bankach
- **Ulubione** — oznaczanie gwiazdką i filtrowanie tylko ulubionych (oznaczenie działa w ramach sesji, nie jest zapisywane trwale)
- **Masowy import z pliku .txt** — format: `częstotliwość;nazwa;pasmo;tryb`, jedna stacja na linię, np. `7175;HAM RADIO;40M;LSB`

---

## 8. RDS (Radio Data System)

W trybie FM odbiornik dekoduje dane RDS: nazwę stacji oraz tekst radiowy (Radio Text). Domyślnie RDS wyświetla się tylko po aktywacji przycisku FM (opcję tę można zmienić w USTAWIENIA → UŻYTKOWE: „RDS tylko w FM”).

W panelu WWW nazwa stacji i tekst RDS wyświetlają się obok siebie, w sposób stabilny — raz odebrana wartość pozostaje widoczna nawet, gdy pojedyncze odpytanie akurat nie zwróci danych (RDS naturalnie „milczy” chwilami między kolejnymi grupami danych).

---

## 9. Tryb nocny

Włączany w USTAWIENIA → WIFI („Tryb nocny (NTP)”). Gdy aktywny, odbiornik automatycznie przyciemnia podświetlenie ekranu w oknie 22:00–6:00 czasu lokalnego (wymaga zsynchronizowanego czasu przez NTP — patrz rozdz. 2.3), a po zakończeniu tego okna przywraca poprzedni poziom jasności.

---

## 10. Menu ustawień

Menu ustawień podzielone jest na osiem zakładek, przełączanych przyciskami «« / »» u dołu ekranu.

### 10.1 SI473X

| Pozycja | Opis |
|---|---|
| FM od 64 MHz | Rozszerza dolną granicę pasma FM (zgodność ze starszym standardem OIRT) |
| Szukaj w AM 1 KHz | Dokładność automatycznego wyszukiwania stacji w paśmie AM |
| Wykres trendu RSSI | Przełącza S-metr na widok wykresu historii siły sygnału |
| Prog wykrycia SNR | Próg SNR (dB) uznawany za wykryty sygnał podczas skanowania |
| Stop na sygnale | Automatyczne wstrzymanie skanera po wykryciu sygnału |

### 10.2 UŻYTKOWE

| Pozycja | Opis |
|---|---|
| RDS tylko w FM | Czy dane RDS pokazują się zawsze, czy tylko po aktywacji FM |
| Podświetl cyfry | Podświetlenie cyfr wyświetlacza częstotliwości |
| Memo in preset | Sposób łączenia banku pamięci z presetami |
| ANG. nazwy pasm | Język nazw pasm w trybie retro (polski/angielski) |

### 10.3 WYŚWIETLACZ

| Pozycja | Opis |
|---|---|
| Wygaszacz ekranu | Włącza/wyłącza wygaszacz z animowanym zegarem |
| Wygaś całkiem | Po czasie wygaszacza całkowicie wyłącza podświetlenie |
| Czas do wygaszacza | Regulowany czas aktywacji (30 s – 30 min) |
| Orientacja pionowa | Ustawia orientację ekranu bez potrzeby gestu obrotu |
| Waterfall w SCAN | Włącza łączony widok wykres/waterfall w skanerze |

### 10.4 SKANOWANIE

| Pozycja | Opis |
|---|---|
| Min skali / Max skali | Zakres skali powiększenia wykresu skanera |
| Auto skala | Automatyczny dobór skali podczas skanowania |
| Dokładność skanu | Precyzja przemiatania pasma |
| Wygaszacz w SCAN | Czy wygaszacz może się aktywować podczas skanowania |

### 10.5 SPRZĘT

| Pozycja | Opis |
|---|---|
| Bateria min / max (0%/100%) | Kalibracja wskaźnika naładowania baterii |
| Pokaż baterię | Widoczność wskaźnika baterii na ekranie głównym |
| Brzęczyk | Dźwięki potwierdzenia przycisków |
| Jasność | Poziom podświetlenia wyświetlacza |

### 10.6 DOMYŚLNE

| Pozycja | Opis |
|---|---|
| Wyczyść ustawienia | Kasuje zapisaną konfigurację |
| Reset fabrycznych | Przywraca wartości domyślne z firmware |
| Dekoder CW | Eksperymentalny dekoder Morse'a (wymaga dodatkowego sprzętu na GPIO) |
| Serwer WWW | Włącza panel sterowania przez przeglądarkę (patrz rozdz. 11) |
| Analogowy S-metr | Przełącza S-metr na widok analogowej wskazówki |

### 10.7 WIFI

| Pozycja | Opis |
|---|---|
| WiFi włączone | Główny przełącznik modułu WiFi |
| Status | Informacja o stanie połączenia / zapisanej sieci |
| Konfiguruj | Uruchamia kreator konfiguracji sieci |
| Resetuj zapisaną sieć | Kasuje zapisane dane logowania WiFi |
| Tryb nocny (NTP) | Patrz rozdz. 9 |

### 10.8 TEST

Ekran diagnostyczny: test kolorów RGB, test wyświetlania polskich znaków, informacja o wolnej pamięci RAM, oraz dwa przyciski:

- **USTAW DOTYK** — uruchamia kalibrację ekranu dotykowego (dotknij kolejno wyświetlane znaczniki); wynik zapisywany jest trwale i automatycznie stosowany przy każdym starcie.
- **FW INFO** — pokazuje szczegółowe informacje o firmware chipu SI4735 (numer części, wersje firmware i komponentów, chip revision).

---

## 11. Panel sterowania WWW

Od wersji 0.17PL odbiornik udostępnia pełny panel sterowania dostępny z dowolnej przeglądarki w tej samej sieci lokalnej.

### 11.1 Włączenie i dostęp

Panel włącza się automatycznie, jeśli w USTAWIENIA → DOMYŚLNE aktywna jest opcja „Serwer WWW” (domyślnie włączona od wersji 0.17PL). Po udanym połączeniu z zapisaną siecią WiFi, odbiornik NIE usypia modułu WiFi (w przeciwieństwie do trybu bez serwera WWW, gdzie WiFi wyłącza się po synchronizacji czasu dla oszczędności energii), tylko od razu udostępnia panel.

Adres dostępu:

- `http://ats25x2.local` — działa w większości sieci domowych (mDNS)
- Adres IP urządzenia — widoczny w USTAWIENIA → WIFI, jako rozwiązanie zapasowe

### 11.2 Wygląd i układ

Interfejs stylizowany jest na panel przedni profesjonalnej radiostacji: podwójny wyświetlacz VFO A/B, dwa metery (SIGNAL i SNR), pasek boczny z ośmioma przyciskami funkcyjnymi, siatka przycisków pasm i trybów.

Dostępny jest tryb kompaktowy (przycisk „[kompakt]” w nagłówku) — mniejsze elementy, więcej informacji jednocześnie widocznych. Wybór trybu zapamiętywany jest w przeglądarce.

### 11.3 Sterowanie

| Funkcja | Opis |
|---|---|
| Strojenie | Wpisanie częstotliwości ręcznie (w obrębie aktualnego pasma) |
| Pasmo / Tryb | Siatka przycisków — natychmiastowa zmiana |
| VFO A/B | Przełączanie przyciskiem ⇄ |
| Głośność | Suwak, zmiana na żywo |
| Wyciszenie | Przycisk MUTE |
| AGC / Tłumik | Wyłączony / automatyczny / ręczny z suwakiem poziomu |
| Kanał priorytetowy | Zapis celu, włączenie/wyłączenie monitorowania |
| Stoper | Start / stop / reset |
| Blokada ekranu | Zdalne blokowanie/odblokowanie urządzenia |
| Skaner | Pauza / wznowienie (gdy skaner aktywny na urządzeniu) |

> **Uwaga:** Zmiana pasma lub trybu — z poziomu urządzenia lub panelu WWW — zawsze wycisza dźwięk na czas przestrajania, eliminując słyszalny trzask.

### 11.4 Telemetria na żywo

Panel odświeża stan co 2 sekundy, pokazując: częstotliwość, pasmo, tryb, metry SIGNAL/SNR, dane RDS, napięcie baterii oraz status skanera.

### 11.5 Lista wykluczeń (skip list)

Dostępna w sekcji „Lista wykluczeń” panelu opcji. Umożliwia dodawanie i usuwanie częstotliwości pomijanych przez skaner, a także:

- **Pobierz .txt** — eksportuje bieżącą listę do pliku
- **Wczytaj .txt** — importuje listę z pliku (jedna częstotliwość w kHz na linię)

### 11.6 Log Discovery

Sekcja „Discovery” pokazuje historię zatrzymań skanera (czas, częstotliwość, SNR) z możliwością eksportu do pliku .txt (format CSV ze średnikiem).

### 11.7 Eksport i import konfiguracji

W menu opcji dostępna jest funkcja pełnego eksportu konfiguracji urządzenia do pliku .txt oraz importu takiego pliku z powrotem. Eksport zawiera dokładną, bit-w-bit kopię wewnętrznej struktury ustawień.

> **Uwaga:** Import konfiguracji działa wyłącznie między urządzeniami na identycznej wersji firmware. Plik niepasującej wersji zostanie odrzucony z komunikatem błędu, bez ryzyka uszkodzenia bieżącej konfiguracji.

---

## 12. Rozwiązywanie problemów

**Panel WWW jest niedostępny**
- Sprawdź, czy w USTAWIENIA → DOMYŚLNE opcja „Serwer WWW” jest włączona.
- Sprawdź w USTAWIENIA → WIFI, czy urządzenie jest połączone z siecią i jaki ma adres IP.
- Jeśli `http://ats25x2.local` nie działa, spróbuj bezpośrednio adresu IP.

**Dekoder CW nie działa**
- Funkcja wymaga dodatkowego układu (np. LM567) podłączonego do dedykowanego wejścia GPIO lub łączenia przez rezystor. Na standardowej, niemodyfikowanej płytce nie zadziała.

**Ekran nie reaguje na dotyk**
- Sprawdź, czy ekran nie jest zablokowany (patrz rozdz. 4.1).
- Jeśli dotyk jest systematycznie przesunięty względem miejsca dotknięcia, wykonaj ponowną kalibrację: USTAWIENIA → TEST → „USTAW DOTYK”.

**Trzask lub pyknięcie dźwięku przy zmianie pasma**
- Od wersji 0.17PL zmiana pasma/trybu jest wyciszana automatycznie. Jeśli trzask nadal występuje, sprawdź czy urządzenie ma aktualną wersję firmware.

**Import stacji/konfiguracji przez WWW się nie udaje**
- Sprawdź format pliku — dla stacji: `częstotliwość;nazwa;pasmo;tryb`, jedna linia na stację.
- Dla konfiguracji: plik musi pochodzić z urządzenia na tej samej wersji firmware.

---

*ATS25X2 Polish Project — RoX10 PL MOD*
