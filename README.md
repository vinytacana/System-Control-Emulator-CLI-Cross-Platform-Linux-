
### DACC Station - System Control Emulator

  
DACC Station is a specialized middleware designed to abstract the complexity of Linux hardware management for embedded gaming consoles. Developed as part of an extension project at the Federal University of Rondônia (UNIR), this software provides a unified CLI (Command Line Interface) bridge between the user and low-level kernel drivers, enabling seamless control over Audio, Video, Network, and Bluetooth peripherals without requiring direct shell interaction. This project serves as the core system controller for the DACC Station console, ensuring that students' games and applications can run in a controlled, "plug-and-play" environment.

---

### Architecture & Technical Overview

The system is built as a monolithic C++ application acting as a wrapper for various Linux subsystems. It replaces complex shell scripts with optimized C++ system calls, utilizing fork(), exec(), and pipes for process management.
#### Key Modules
1. Hybrid Video Management (X11 & Wayland)
The system implements a session-aware display manager. It automatically detects the active display server via environment variables (XDG_SESSION_TYPE) and dispatches the appropriate backend commands:

- X11/Xorg: Utilizes xrandr for resolution switching and scaling.
- **Wayland:** Integrates with `wlr-randr` (for wlroots-based compositors) and `gsettings` (for GNOME), ensuring cross-compatibility.
- **Scaling Engine:** Includes a logic layer to handle fractional scaling calculations for pixel-perfect rendering on modern displays.

2. Advanced Bluetooth Automation (Virtual Terminals)
Standard Linux tools like `bluetoothctl` are designed for interactive human use, making automation difficult. This project implements a custom **Pseudo-Terminal (PTY) Wrapper**:
- **Mechanism:** Uses `forkpty()` to spawn a child process that mimics a real terminal
- **Stream Parsing:** A dedicated C++ parser reads the raw stdout stream in real-time, filtering out ANSI escape codes and debug logs.
- **Result:** Enables the system to programmatically scan, pair, trust, and connect devices (like PS4/Xbox controllers) purely via code, handling complex authentication handshakes automatically.

3. Network & Audio Stack
   - **Wi-Fi:** Wraps `nmcli` (NetworkManager) to handle scanning and WPA2 secure connections, abstracting the complexity of SSID parsing and signal strength calculation.
   - **Audio:** Directly interfaces with **PipeWire** via `wpctl`. It implements safety limiters (soft-clipping) to prevent volume gain from exceeding hardware limits (100%), protecting physical speakers from distortion.

---
### Installation & Build
#### Prerequisites

Ensure your Linux distribution has the following dependencies installed:

- **Compiler:** `g++` (supporting C++17 standard)
- **System Tools:** `bluetoothctl` (BlueZ), `nmcli` (NetworkManager), `wpctl` (WireWire/PipeWire), `xrandr` / `wlr-randr`.
- **Libraries:** Standard C++ STL.
#### Compilation

Clone the repository and compile using the provided Makefile or manually via g++:

```bash
# Clone the repository
git clone https://github.com/vinytacana/System-Control-Emulator-CLI-Cross-Platform-Linux-.git
cd System-Control-Emulator-CLI-Cross-Platform-Linux-

# Manual Compilation
g++ -o sys.exe main.cpp emulador.cpp -lutil -std=c++17
```
(Note: The `-lutil` flag is essential for linking the PTY functions used in the Bluetooth module).

---
### Usage

The compiled binary (`sys.exe`) accepts arguments to trigger specific hardware functions. This design allows it to be easily integrated into other front-ends (like EmulationStation or a custom GUI).

**Basic Syntax:**
```bash
./sys.exe <command> [arguments]
```

### Examples

**Audio Control:**

```bash
./sys.exe aumentar_volume      # Increases volume by 5%
./sys.exe definir_volume 0.5   # Sets volume to 50%
```

**Network Management:**

```bash
./sys.exe listar_wifi
./sys.exe conectar_wifi "MyNetwork" "MyPassword123"
```

**Bluetooth (The Hard Stuff Made Easy):**

```bash
# Scans for devices for 10 seconds and returns a clean list
./sys.exe scan_dispositivos_bluetooth 10 

# Automatically pairs and connects to a controller
./sys.exe conectar_bluetooth AA:BB:CC:11:22:33

```

---

### Roadmap & Future Updates
This project is under active development. While the CLI backend is stable, future iterations aim to expand user autonomy:

- **[Planned] File Manager Module:** A secure, permission-based system for managing ROMs and configuration files directly on the console.
- **[Planned] Hardware Telemetry:** Real-time monitoring of CPU temperature and RAM usage.
- **[Planned] Video Presets:** Pre-configured scaling profiles (e.g., "Pixel Perfect 3x", "4:3 Stretch") for retro gaming.
- ---
###  License

This project is open-source and available under the [![License: MIT](https://img.shields.io)](https://opensource.org). Contributions are welcome!

---

---

# DACC Station - Emulador de Controle de Sistema 

**DACC Station** é um _middleware_ especializado, projetado para abstrair a complexidade do gerenciamento de _hardware_ no Linux para consoles de jogos embarcados. Desenvolvido como parte de um projeto de extensão da Universidade Federal de Rondônia (UNIR), este software fornece uma ponte unificada via CLI (Interface de Linha de Comando) entre o usuário e os _drivers_ de baixo nível do _kernel_, permitindo controle total sobre Áudio, Vídeo, Rede e Bluetooth sem a necessidade de interação direta com o _shell_. Este projeto atua como o controlador central do console DACC Station, garantindo que os jogos e aplicações desenvolvidos pelos alunos possam rodar em um ambiente controlado e amigável (_plug-and-play_).

---

### Arquitetura e Visão Técnica

O sistema foi construído como uma aplicação monolítica em C++ que atua como um _wrapper_ (envoltório) para vários subsistemas do Linux. Ele substitui _scripts_ de shell complexos por chamadas de sistema otimizadas em C++, utilizando `fork()`, `exec()` e `pipes` para gerenciamento robusto de processos.
#### Módulos Principais

##### 1. Gerenciamento de Vídeo Híbrido (X11 & Wayland)

O sistema implementa um gerenciador de exibição "consciente da sessão". Ele detecta automaticamente o servidor gráfico ativo através de variáveis de ambiente (`XDG_SESSION_TYPE`) e despacha os comandos de _backend_ apropriados:

- **X11/Xorg:** Utiliza `xrandr` para troca de resolução e escala.
    
- **Wayland:** Integra-se com `wlr-randr` (para compositores baseados em wlroots) e `gsettings` (para GNOME), garantindo compatibilidade cruzada.
    
- **Motor de Escala:** Inclui uma camada lógica para lidar com cálculos de escala fracionária, essencial para renderização nítida em monitores modernos.
    

##### 2. Automação Avançada de Bluetooth (Terminais Virtuais)

Ferramentas padrão do Linux, como `bluetoothctl`, são projetadas para uso humano interativo, dificultando a automação. Este projeto implementa um **Wrapper de Pseudo-Terminal (PTY)** customizado:

- **Mecanismo:** Usa `forkpty()` para criar um processo filho que simula um terminal real.
- **Parsing de Stream:** Um _parser_ dedicado em C++ lê o fluxo de saída (_stdout_) em tempo real, filtrando códigos de escape ANSI e logs de depuração irrelevantes.
- **Resultado:** Permite que o sistema escaneie, pareie, confie e conecte dispositivos (como controles de PS4/Xbox) puramente via código, lidando automaticamente com os complexos "apertos de mão" (_handshakes_) de autenticação.
##### 3. Pilha de Rede e Áudio

- **Wi-Fi:** Encapsula o `nmcli` (NetworkManager) para lidar com varredura e conexões seguras WPA2, abstraindo a complexidade do _parsing_ de SSIDs e cálculo de força de sinal.
- **Áudio:** Interfaceia diretamente com o **PipeWire** via `wpctl`. Implementa limitadores de segurança (_soft-clipping_) para impedir que o ganho de volume exceda os limites do hardware (100%), protegendo os alto-falantes físicos contra distorção.
---
### Instalação e Compilação
#### Pré-requisitos

Certifique-se de que sua distribuição Linux possui as seguintes dependências instaladas:

- **Compilador:** `g++` (com suporte ao padrão C++17)
    
- **Ferramentas de Sistema:** `bluetoothctl` (BlueZ), `nmcli` (NetworkManager), `wpctl` (WireWire/PipeWire), `xrandr` / `wlr-randr`.
    
- **Bibliotecas:** STL C++ padrão.
    

### Compilação

Clone o repositório e compile manualmente via g++:
```bash
# Clonar o repositório
git clone https://github.com/vinytacana/System-Control-Emulator-CLI-Cross-Platform-Linux-.git
cd System-Control-Emulator-CLI-Cross-Platform-Linux-

# Compilação Manual
g++ -o sys.exe main.cpp emulador.cpp -lutil -std=c++17
```
(Nota: A flag `-lutil` é essencial para vincular as funções de PTY usadas no módulo Bluetooth).

---
### Como usar
O binário compilado (`sys.exe`) aceita argumentos para acionar funções específicas de hardware. Esse design permite que ele seja facilmente integrado a outros _front-ends_ (como EmulationStation ou uma GUI personalizada).

**Sintaxe Básica:**
```bash
./sys.exe <comando> [argumentos]
```

#### Exemplos de Uso

**Controle de Áudio:**
```bash
./sys.exe aumentar_volume      # Aumenta o volume em 5%
./sys.exe definir_volume 0.5   # Define o volume para 50%
```
**Gerenciamento de Rede:**
```bash
./sys.exe listar_wifi
./sys.exe conectar_wifi "MinhaRede" "SenhaSegura123"
```

**Bluetooth (O Difícil Tornado Fácil):**
```bash
# Escaneia dispositivos por 10 segundos e retorna uma lista limpa
./sys.exe scan_dispositivos_bluetooth 10 

# Pareia e conecta automaticamente a um controle
./sys.exe conectar_bluetooth AA:BB:CC:11:22:33
```
---
### Roadmap e Atualizações Futuras

Este projeto está em desenvolvimento ativo. Embora o _backend_ CLI esteja estável, iterações futuras visam expandir a autonomia do usuário:    
- **[Planejado] Módulo Gerenciador de Arquivos:** Um sistema seguro, baseado em permissões, para gerenciar ROMs e arquivos de configuração diretamente no console.
- **[Planejado] Telemetria de Hardware:** Monitoramento em tempo real da temperatura da CPU e uso de RAM.
- **[Planejado] Presets de Vídeo:** Perfis de escala pré-configurados (ex: "Pixel Perfect 3x", "Esticar 4:3") para jogos retrô.

- ---
### Licença

Este projeto é open-source e está disponível sob a [![License: MIT](https://img.shields.io)](https://opensource.org). Contribuições são bem-vindas!