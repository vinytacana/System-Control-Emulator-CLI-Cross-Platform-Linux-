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

void scan_dispositivos_bluetooth()
{
    cout << "Procurando dispositivos Bluetooth... (Ctrl+C para parar)\n";
    system("bluetoothctl scan on");
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

void listar_dispositivos_bluetooth()
{
    cout << "Dispositivos Bluetooth pareados:\n";
    system("bluetoothctl devices");
}
std::vector<device_bt> parsing_bluetooth_stream(std::istream &input)
{
    std::vector<device_bt> lista;
    std::string linha;

    while (std::getline(input, linha))
    {
        std::stringstream ss(linha);
        std::string tag, mac, nome_restante;

        // Tenta ler a primeira palavra ("Device")
        ss >> tag;

        if (tag == "Device")
        {
            ss >> mac; // Lê o MAC

            // Lê o resto da linha como o nome
            std::getline(ss, nome_restante);

            // Limpeza: remove espaços extras no início do nome
            size_t primeiro_char = nome_restante.find_first_not_of(" \t");
            if (primeiro_char != std::string::npos)
            {
                nome_restante = nome_restante.substr(primeiro_char);
            }

            lista.push_back({mac, nome_restante});
        }
    }
    return lista;
}

std::vector<device_bt> get_list_device()
{

    int ret = system("bluetoothctl devices > /tmp/bt_list.txt");
    if (ret != 0)
    {
        std::cerr << "Aviso: Falha ao executar bluetoothctl.\n";
    }

    // Abre o arquivo
    std::ifstream arquivo("/tmp/bt_list.txt");
    if (!arquivo.is_open())
    {
        std::cerr << "Erro: Não foi possível abrir o arquivo de lista Bluetooth.\n";
        return {};
    }

    // Passa o arquivo aberto para o parser
    // O parser não sabe se é um arquivo ou string, ele só lê.
    return parsing_bluetooth_stream(arquivo);
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

    // Validação de entrada
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
