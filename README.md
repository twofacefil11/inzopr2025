# inzopr2025

Niewielka alikacja do nakładania efektów oraz eksportu obrazów. Projekt realizowany w ramach przedmiotu Inżynieria Oprogramowania.

## Getting started

Clone this repo. Run this in your preferred shell:

```shell
git clone https://github.com/twofacefil11/inzopr2025.git
```


## Building

Ze względu na wykorzystanie API Windowsa, korzystamy z kompilatora `MSVC`. Jest on zapewniany przez Visual Studio.

W katalogu głównym repozytorium znajdują się dwa skrypty:

```shell
/inzopr2025
├── build.bat
└── build.sh
```

- Jeżeli korzystasz z `cmd` lub `pwsh`, odpalenie `build.bat` powinno skompilować wszytsko co mamy do tej pory.
- Jeżeli siedzisz w `bashu` poprzez `WSL` - używaj `build.sh`, który odpala tylko `cmd.exe` i wykonuje `build.bat`.

### Requirements

- Visual Studio 2022 (Enterprise or Community edition);
- `windows.h` is required _somewhere_

### Compilation

W skrypcie `build.bat` jest ścieżka do konsoli deweloperskiej VisualStudio22.
Jest to środowisko, w którym znajdują się wszstkie niezbędnę do kompilacji składniki, z których korzysta VS22.
Powyższe skrypty przywołują owe środowisko i wywołują instrukcje kompilacji MSVC na odpowiednich plikach tak, jakbyśmy korzystali z Visual Studio.

Skrypty powinny być w stanie oszacować, czy VS22 jest zainstalowane w **domyślnej lokalizacji** oraz w jakiej edycji (od niej zależy ścieżka do `Developer Command Prompt for VS 2022`).

W przyszłości skrypty z **pewnością** zmienią się, gdy zdefiniujemy tryby kompilacji dla **DEBUG**, **TESTING**, oraz obsługe **LOGGING**.

Po kompilacji na chwile obecną, program i output znajdziesz w:

```shell
/inzopr2025
└── build
    ├── out.exe
    └── /output
        └── out.*

```

Jeżeli ktoś umie i chce, to niech ogarnie `.sln` dla VS22. Ja tego nie potrzebuje do szczęścia.

## Platform

This software is written using `Windows API` and will only run on modern Windows systems starting from **Windows 8**.

- tested only on Windows 10/11

## Dependencies

| Use | Library       |
| ------------- |:-------------:|
| So much stuff... | `windows.h` |
| Loading images, parsing headers       | [`stb_image.h`](https://github.com/nothings/stb/blob/master/stb_image.h)     |
| Writting and export       | [`stb_image_write.h`](https://github.com/nothings/stb/blob/master/stb_image_write.h)     |

Możliwe, że wykorzystamy więcej resourców z projektu [stb](https://github.com/nothings/stb).
