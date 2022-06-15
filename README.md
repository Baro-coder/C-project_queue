# C-project - queue


## Opis [^desc]
Zestaw programów typu 'producent', 'konsument' prezentujący
możliwości synchronizacji i współbieżnego działania kilku
procesów zdolnych do obróbki i przekazywania sobie danych.
___
**System operacyjny:** *Linux*

**Język:** *C*

**Mechanizmy synchronizacji**: 
  - *kolejki komnikatów (Message Queue),*
  - *pamięć współdzielona (shmem).*

## Uruchomienie projektu: [^run] 

Przejście do katalogu projektu:
> cd *existing project folder*

Wywołanie głównego skryptu bash: *project.sh*:
> ./project.sh

Skrypt samodzielnie przeprowadzi kompilację wszystkich
plików źródłowych *.c* i w przypadku nie wystąpienia
błędów kompilacji uruchomi zestaw programów.

W przypadku wystąpienia błędów kompilacji skrypt wyświetli
zidentyfikowane błędy i zakończy działanie.


## Procesy [^proc]

### Proces główny - MAIN
  Z procesu głównego powoływane są 3 procesy potomne.
  Po ich powołaniu proces główny wstrzymuje pracę, aż do 
  ich zakończenia. Wówczas proces główny zamyka wszystkie
  uprzednio utworzone struktury synchronizujące procesy
  i kończy działanie.

### Proces potomny 1
  Czyta dane z *stdin* (standardowego wejścia) lub z *file* (pliku) oraz przekazuje
  je do procesu 2 w niezmienionej formie poprzez **kolejkę komunikatów** (pipe).

### Proces potomny 2
  Pobiera dane przesłane przez proces 1, oblicza ilość znaków w każdej linii i wyznaczoną
  liczbę przekazuje do procesu 3.

### Proces potomny 3
  Pobiera wyprodukowane przez proces 2 dane i umieszcza je w *stdout* (standardowym wyjściu).
  Każda jednostka danych powinna zostać wyprowadzona w osobnym wierszu.


## Sygnały [^sig]

### Scenariusz powiadamiania się procesów o swoim stanie:
  1. Do procesu 3 wysyłane są sygnały. 
  2. Proces 3 przesyłą otrzymany sygnał do procesu macierzystego.
  3. Proces macierzysty zapisuje wartość sygnału do pamięci dzielonej
     oraz wysyła powiadomienie do procesu 1 o odczytaniu zawartości
     pamięci dzielonej.
  4. Proces 1 po odczytaniu wartości sygnału, wysyła powiadomienie
     do procesu 2 o odczytaniu zawartości pamięci dzielonej.
  5. Proces 2 po odczytaniu wartości sygnału, wysyła powiadomienie
     do procesu 3 o odczytaniu zawartości pamięci dzielonej.


[^desc]: Opis
[^run]: Uruchomienie projektu
[^proc]: Procesy
[^sig]: Sygnały
