# Documentação Técnica do Sistema (DACC Station Middleware)

## Visão Geral da Arquitetura
O **DACC Station Middleware** (`sys.exe`) é um wrapper em C++ projetado para abstrair chamadas de sistema Linux. Ele atua como uma camada intermediária entre a interface do usuário (Frontend/CLI) e os drivers do Kernel/Userspace (BlueZ, NetworkManager, PipeWire, X11/Wayland).

O objetivo principal deste código é evitar o uso de scripts Shell instáveis, trazendo tipagem forte, tratamento de exceções e manipulação de processos via chamadas de sistema Unix (`fork`, `exec`, `pipe`, `pty`).

---

## 1. Módulo Bluetooth (O Desafio do PTY)
**Arquivo:** `emulador.cpp` | **Função:** `scan_dispositivos_bluetooth`

O driver padrão do Linux (`bluetoothctl`) é interativo e não foi feito para automação via script. Para contornar isso, implementamos uma técnica de **Terminal Virtual**.

### Como Funciona:
1.  **ForkPTY:** Utilizamos a biblioteca `pty.h` para criar um par mestre-escravo de pseudo-terminal.
2.  **Processo Filho:** Executa o `bluetoothctl` dentro desse terminal falso. O `bluetoothctl` "pensa" que está sendo operado por um humano.
3.  **Processo Pai:**
    * Escreve comandos (`scan on`, `pair`, `connect`) no descritor de arquivo mestre.
    * Lê a saída bruta (`stdout`) usando `select()` para não bloquear a execução.
4.  **Parser Customizado:** A função `parsing_bluetooth_stream` recebe o fluxo de texto sujo (com códigos ANSI e logs) e extrai apenas o que importa: `MAC Address` e `Alias` (Nome do dispositivo).

**Nota para Mantenedores:** Se o `bluetoothctl` mudar o formato de saída em futuras versões do BlueZ, a lógica de *parsing* (linhas de leitura de string) precisará ser ajustada.

---

## 2. Módulo de Vídeo (Híbrido X11/Wayland)
**Arquivo:** `emulador.cpp` | **Funções:** `alterarResolucao`, `alterarEscala`

O sistema detecta automaticamente o servidor gráfico através da variável de ambiente `XDG_SESSION_TYPE`.

* **Sessão X11:** Utiliza o comando `xrandr`. O sistema calcula escalas baseadas em float (ex: 1.5x) e aplica via `--scale`.
* **Sessão Wayland:**
    * **GNOME:** Utiliza `gsettings` para alterar o fator de escala da interface (`scaling-factor`).
    * **wlroots (Sway/Hyprland):** Utiliza `wlr-randr`.

**Atenção:** O GNOME Wayland não suporta troca de resolução arbitrária via linha de comando da mesma forma que o X11. O código atual prioriza o escalonamento (Scaling) em vez da troca de resolução bruta para garantir compatibilidade.

---

## 3. Módulo de Áudio (PipeWire)
**Arquivo:** `emulador.cpp` | **Funções:** `aumentar_volume`, `diminuir_volume`

Optou-se pelo uso do `wpctl` (WirePlumber) em vez do antigo ALSA ou PulseAudio puro, garantindo compatibilidade com distribuições Linux modernas.

* **Segurança:** O código utiliza a flag `5%+` e `5%-`. Diferente de scripts simples, o comando `wpctl` já gerencia limites de hardware, mas o wrapper C++ garante que comandos inválidos (ex: definir volume para "200%") sejam descartados antes de chegar ao driver.

---

## 4. Compilação
Para compilar este projeto, é necessário linkar a biblioteca de utilitários do sistema (`libutil`) devido ao uso de `forkpty`.

**Comando de Build:**
```bash
g++ -o sys.exe main.cpp emulador.cpp -lutil -std=c++17
``