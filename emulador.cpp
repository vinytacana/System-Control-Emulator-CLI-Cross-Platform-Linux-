#include "functions.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>

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