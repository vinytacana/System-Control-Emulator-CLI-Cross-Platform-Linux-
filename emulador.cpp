#include "functions.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <thread>
#include <chrono>
#include <pty.h>
#include <sys/select.h>
#include <unordered_map>


using namespace std;

void aumentar_volume()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        execlp("sh", "sh", "-c", "wpctl set-volume @DEFAULT_AUDIO_SINK@ 50%+", nullptr);
        perror("execlp falhou");
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(pid, NULL, 0);
        cout << "Volume aumentado com sucesso\n";
    }
    else
    {
        perror("fork falhou");
    }
}

void diminuir_volume()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        execlp("sh", "sh", "-c", "wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%-", nullptr);
        perror("execlp falhou");
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(pid, NULL, 0);
        cout << "Volume diminuído com sucesso\n";
    }
    else
    {
        perror("fork falhou");
    }
}

void definir_volume(float valor)
{
    pid_t pid = fork();
    string comando = "wpctl set-volume @DEFAULT_AUDIO_SINK@ " + to_string(valor);
    if (pid == 0)
    {
        execlp("sh", "sh", "-c", comando.c_str(), nullptr);
        perror("Execlp falhou");
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(pid, nullptr, 0);
        cout << "Volume definido para " << valor * 100 << "%\n";
    }
    else
    {
        perror("fork falhou");
    }
}

void aumentar_brilho()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        execlp("sh", "sh", "-c", "brightnessctl set +10%", nullptr);
        perror("execlp falhou");
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(pid, NULL, 0);
        cout << "Brilho aumentado com sucesso\n";
    }
    else
    {
        perror("fork falhou");
    }
}

void diminuir_brilho()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        execlp("sh", "sh", "-c", "brightnessctl set 10%-", nullptr);
        perror("execlp falhou");
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(pid, NULL, 0);
        cout << "Brilho diminuído com sucesso\n";
    }
    else
    {
        perror("fork falhou");
    }
}

void verificarSessao()
{
    const char *sessao = getenv("XDG_SESSION_TYPE");
    const char *compositor = getenv("XDG_SESSION_DESKTOP");
    if (!sessao)
    {
        cout << "Não foi possível detectar o tipo de sessão\n";
        return;
    }
    string sessaoStr(sessao);
    cout << "Sessão atual: " << sessaoStr << "\n";
    if (sessaoStr == "wayland")
    {
        cout << " Você está em Wayland. O xrandr pode não funcionar para mudar resolução.\n";
    }
    else if (sessaoStr == "x11")
    {
        cout << "Sessão Xorg detectada. xrandr deve funcionar normalmente.\n";
    }
    else
    {
        cout << "Tipo de sessão não reconhecido\n";
    }

    string comp(compositor);
    cout << "Compositor atual: " << comp << "\n";

    if (comp == "gnome")
    {
        cout << " O compositor gnome nao e compativel com o xrand nem com wlr_randr, mude de compositor para mudar configuracoes de resolucao" << "\n";
    }
    else
    {
        cout << "Compositor nao reconhecido" << "\n";
    }
}
void listar_resolucao()
{
    cout << "Lista de saidas e resolucoes suportadas: \n";
    pid_t pid = fork();
    if (pid == 0)
    {
        execlp("xrandr", "xrandr", nullptr);
        perror("execlp falhou");
        exit(1);
    }
    else if (pid > 1)
    {
        waitpid(pid, NULL, 0);
    }
    else
    {
        perror("execlp falhou!!");
    }
}
void alterarEscala(const string &saida, float escalaX, float escalaY)
{
    const char *sessao = getenv("XDG_SESSION_TYPE");
    if (!sessao)
    {
        cerr << "Não foi possível detectar a sessão\n";
        return;
    }

    string sessaoStr(sessao);
    string comando;

    if (sessaoStr == "x11")
    {
        if (escalaY < 0)
            escalaY = escalaX;
        comando = "xrandr --output " + saida + " --scale " +
                  to_string(escalaX) + "x" + to_string(escalaY);
    }
    else if (sessaoStr == "wayland")
    {
        const char *compositor = getenv("XDG_SESSION_DESKTOP");
        string comp(compositor ? compositor : "");

        if (comp == "gnome")
        {
            int escalaInt = static_cast<int>(escalaX + 0.5);
            comando = "gsettings set org.gnome.desktop.interface scaling-factor " + to_string(escalaInt);
        }
        else
        {
            comando = "wlr-randr --output " + saida + " --scale " + to_string(escalaX);
        }
    }
    else
    {
        cerr << "Sessão não reconhecida.\n";
        return;
    }

    cout << "Executando: " << comando << endl;
    int ret = system(comando.c_str());
    if (ret != 0)
    {
        cerr << "Falha ao alterar escala\n";
    }
}

void alterarResolucao(const string &saida, const string &modo)
{
    const char *sessao = getenv("XDG_SESSION_TYPE");
    if (!sessao)
    {
        cerr << "Não foi possível detectar a sessão\n";
        return;
    }

    string sessaoStr(sessao);
    string comando;

    if (sessaoStr == "x11")
    {
        comando = "xrandr --output " + saida + " --mode " + modo;
    }
    else if (sessaoStr == "wayland")
    {
        const char *compositor = getenv("XDG_SESSION_DESKTOP");
        string comp(compositor ? compositor : "");

        if (comp == "gnome")
        {
            cerr << "GNOME Wayland não suporta mudar resolução via xrandr/wlr-randr.\n";
            cerr << "Use a interface gráfica ou configure escalonamento.\n";
            return;
        }
        else
        {
            comando = "wlr-randr --output " + saida + " --mode " + modo;
        }
    }
    else
    {
        cerr << "Sessão não reconhecida.\n";
        return;
    }

    cout << "Executando: " << comando << endl;
    int ret = system(comando.c_str());
    if (ret != 0)
    {
        cerr << "Falha ao alterar resolução\n";
    }
}

void listar_wifi()
{
    cout << "Executando...\n";
    pid_t pid = fork();
    if (pid == 0)
    {
        execlp("nmcli", "nmcli", "device", "wifi", "list", nullptr);
        perror("execlp falhou");
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(pid, NULL, 0);
    }
    else
    {
        perror("fork falhou");
    }
}

void conectar_wifi(const string &ssid, const string &senha)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        string comando = "nmcli device wifi connect \"" + ssid + "\" password \"" + senha + "\"";
        execlp("sh", "sh", "-c", comando.c_str(), nullptr);
        perror("execlp falhou");
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(pid, NULL, 0);
        cout << "Tentando conectar na rede: " << ssid << endl;
    }
    else
    {
        perror("fork falhou");
    }
}

void desconectar_wifi(const string &id)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        string comando = "nmcli connection down id \"" + id + "\"";
        execlp("sh", "sh", "-c", comando.c_str(), nullptr);
        perror("execlp falhou");
        exit(1);
    }
    else if (pid > 0)
    {
        waitpid(pid, nullptr, 0);
        cout << "Rede \"" << id << "\" desconectada.\n";
    }
    else
    {
        perror("fork falhou");
    }
}

std::vector<device_bt> scan_dispositivos_bluetooth(int segundos)
{
   std::unordered_map<std::string, device_bt> mapa;

    int master_fd;
    pid_t pid = forkpty(&master_fd, nullptr, nullptr, nullptr);

    if (pid == 0)
    {
        execlp("bluetoothctl", "bluetoothctl", nullptr);
        _exit(1);
    }

    write(master_fd, "power on\n", 9);
    write(master_fd, "agent on\ndefault-agent\n", 24);
    write(master_fd, "scan on\n", 8);

    auto inicio = std::chrono::steady_clock::now();
    char buffer[512];

    while (true)
    {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(master_fd, &fds);

        struct timeval tv{1, 0};
        int ret = select(master_fd + 1, &fds, nullptr, nullptr, &tv);

        if (ret > 0 && FD_ISSET(master_fd, &fds))
        {
            int n = read(master_fd, buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = '\0';
                std::stringstream ss(buffer);

                // agora só atualiza o MAPA
                parsing_bluetooth_stream(ss, mapa);
            }
        }

        auto agora = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(agora - inicio).count() >= segundos)
            break;
    }

    write(master_fd, "scan off\nquit\n", 14);
    close(master_fd);

    // converte mapa → vector
    std::vector<device_bt> lista;
    for (auto &[_, d] : mapa)
        lista.push_back(d);

    return lista;
}

bool conectar_bluetooth(const string &mac)
{
    string comando = "bluetoothctl connect " + mac;
    int resultado = system(comando.c_str());

    if (resultado == 0)
    {
        cout << "Conectado com sucesso ao dispositivo " << mac << endl;
        return true;
    }
    else
    {
        cerr << "Falha ao conectar ao dispositivo " << mac << endl;
        return false;
    }
}

bool desconectar_bluetooth(const string &mac)
{
    string comando = "bluetoothctl disconnect " + mac;
    int resultado = system(comando.c_str());

    if (resultado == 0)
    {
        cout << "Desconectado com sucesso do dispositivo " << mac << endl;
        return true;
    }
    else
    {
        cerr << "Falha ao desconectar do dispositivo " << mac << endl;
        return false;
    }
}

void listar_dispositivos_bluetooth(const std::vector<device_bt> &dispositivos)
{
    if (dispositivos.empty())
    {
        std::cout << "Nenhum dispositivo encontrado.\n";
        return;
    }

    int i = 1;
    for (const auto &d : dispositivos)
    {
        std::cout << i++ << ") "
                  << d.nome << " - "
                  << d.mac << '\n';
    }
}void parsing_bluetooth_stream(
    std::istream &input,
    std::unordered_map<std::string, device_bt> &mapa
)
{
    std::string linha;
    std::string ultimo_mac; // ← chave para discovery

    while (std::getline(input, linha))
    {
        // Caso 1: linha "Device <MAC> <NOME>"
        if (linha.find("Device ") != std::string::npos)
        {
            std::stringstream ss(linha);
            std::string device_kw, mac;
            ss >> device_kw >> mac;

            if (device_kw != "Device" || mac.empty())
                continue;

            // DEL → remover
            if (linha.find("[DEL]") != std::string::npos)
            {
                mapa.erase(mac);
                continue;
            }

            auto &dev = mapa[mac];
            dev.mac = mac;
            ultimo_mac = mac;

            // tenta capturar nome na mesma linha
            std::string resto;
            std::getline(ss, resto);

            size_t first = resto.find_first_not_of(" \t");
            if (first != std::string::npos)
            {
                std::string nome = resto.substr(first);
                if (!nome.empty())
                    dev.nome = nome;
            }

            continue;
        }

        // Caso 2: discovery → "Name:"
        if (!ultimo_mac.empty() && linha.find("Name:") != std::string::npos)
        {
            auto &dev = mapa[ultimo_mac];

            std::string nome = linha.substr(linha.find("Name:") + 5);
            size_t first = nome.find_first_not_of(" \t");
            size_t last  = nome.find_last_not_of(" \t\r\n");

            if (first != std::string::npos && last != std::string::npos)
            {
                nome = nome.substr(first, last - first + 1);
                if (nome.size() >= 2)
                    dev.nome = nome;
            }
        }
    }
}

std::vector<device_bt> get_list_device()
{
    std::unordered_map<std::string, device_bt> mapa;

    int ret = system("bluetoothctl devices > /tmp/bt_list.txt");
    if (ret != 0)
    {
        std::cerr << "Aviso: Falha ao executar bluetoothctl.\n";
    }

    std::ifstream arquivo("/tmp/bt_list.txt");
    if (!arquivo.is_open())
    {
        std::cerr << "Erro: Não foi possível abrir o arquivo de lista Bluetooth.\n";
        return {};
    }

    parsing_bluetooth_stream(arquivo, mapa);

    std::vector<device_bt> lista;
    for (auto &[_, d] : mapa)
        lista.push_back(d);

    return lista;
}


void gerenciar_bluetooth()
{
    std::cout << "Carregando lista de dispositivos...\n";
    std::vector<device_bt> dispositivos = get_list_device();

    if (dispositivos.empty())
    {
        std::cout << "Nenhum dispositivo pareado encontrado.\n";
        std::cout << "Dica: Use 'scan_dispositivos_bluetooth' para encontrar novos.\n";
        return;
    }

    std::cout << "\n=== Dispositivos Pareados ===\n";
    for (size_t i = 0; i < dispositivos.size(); ++i)
    {
        std::cout << "[" << i + 1 << "] " << dispositivos[i].nome
                  << " \t(" << dispositivos[i].mac << ")\n";
    }
    std::cout << "=============================\n";

    std::cout << "Escolha um dispositivo (0 para sair): ";
    int escolha;
    std::cin >> escolha;

    if (std::cin.fail() || escolha < 0 || escolha > static_cast<int>(dispositivos.size()))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Opção inválida.\n";
        return;
    }

    if (escolha == 0)
        return;

    const auto &device = dispositivos[escolha - 1];

    std::cout << "\nDispositivo selecionado: " << device.nome << "\n";
    std::cout << "[1] Conectar\n";
    std::cout << "[2] Desconectar\n";
    std::cout << "Opção: ";

    int acao;
    std::cin >> acao;

    if (acao == 1)
    {
        conectar_bluetooth(device.mac);
    }
    else if (acao == 2)
    {
        desconectar_bluetooth(device.mac);
    }
    else
    {
        std::cout << "Ação desconhecida.\n";
    }
}

int obter_bateria()
{
    vector<string> caminhos = {
        "/sys/class/power_supply/BAT0/capacity",
        "/sys/class/power_supply/BAT1/capacity"
    };

    for (const auto &path : caminhos)
    {
        ifstream arquivo(path);
        if (arquivo.is_open())
        {
            int porcentagem;
            arquivo >> porcentagem;
            return porcentagem;
        }
    }

    return -1;
}
long long obter_tempo_ms()
{
    
    auto agora = std::chrono::system_clock::now();
    
    
    auto duracao = agora.time_since_epoch();
    
    
    return std::chrono::duration_cast<std::chrono::milliseconds>(duracao).count();
}
