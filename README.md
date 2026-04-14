# TradeNote
A simple C++23 desktop application using Qt6.

## Requirements
- C++23 compiler
- Qt6 (Widgets, Network)
- nlohmann_json

## Build
```bash
mkdir build && cd build
cmake ..
make
```

## Debian Package

To create a Debian package for the project, run the provided script:

```bash
./scripts/create_deb.sh
```

The package will be generated in the `build-deb` directory.

## Windows Installer

To create a Windows installer (NSIS), run the provided script in a Windows environment with NSIS installed:

```bash
./scripts/create_win_installer.sh
```

Alternatively, you can generate a ZIP package using CPack:

```bash
cd build-win
cpack -G ZIP
```