#include <iostream>
#include <cstdlib>
#include "functions.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        cout << "Uso:\n";
        cout << argv[0] << " aumentar_volume\n";
        cout << argv[0] << " diminuir_volume\n";
        cout << argv[0] << " definir_volume <valor>\n";
        cout << argv[0] << " aumentar_brilho\n";
        cout << argv[0] << " diminuir_brilho\n";
        cout << argv[0] << " verificar_sessao\n";
        cout << argv[0] << " alterar_resolucao <saida> <modo>\n";
        cout << argv[0] << " listar_resolucao\n";
        cout << argv[0] << " alterar_escala <saida> <escalaX> [escalaY]\n";
        cout << argv[0] << " listar_wifi\n";
        cout << argv[0] << " conectar_wifi <SSID> <senha>\n";
        cout << argv[0] << " desconectar_wifi <SSID>\n";
        cout << argv[0] << " scan_dispositivos_bluetooth\n";
        cout << argv[0] << " conectar_bluetooth <MAC>\n";
        cout << argv[0] << " desconectar_bluetooth <MAC>\n";
        cout << argv[0] << " listar_dispositivos_bluetooth\n";
        cout << argv[0] << " gerenciar_bluetooth\n";

        return 1;
    }

    string_view cmd{argv[1]};

    if (cmd == "aumentar_volume")
    {
        aumentar_volume();
    }
    else if (cmd == "diminuir_volume")
    {
        diminuir_volume();
    }
    else if (cmd == "definir_volume")
    {
        if (argc < 3)
        {
            cout << "Uso: " << argv[0] << " definir_volume <valor>\n";
            return 1;
        }
        float valor = atof(argv[2]);
        definir_volume(valor);
    }
    else if (cmd == "aumentar_brilho")
    {
        aumentar_brilho();
    }
    else if (cmd == "diminuir_brilho")
    {
        diminuir_brilho();
    }
    else if (cmd == "verificar_sessao")
    {
        verificarSessao();
    }
    else if (cmd == "alterar_resolucao")
    {
        if (argc < 3)
        {
            listar_resolucao();
            return 0;
        }
        else if (argc < 4)
        {
            cout << "Uso: " << argv[0] << " alterar_resolucao <saida> <modo>\n";
            return 1;
        }
        string saida = argv[2];
        string modo = argv[3];
        alterarResolucao(saida, modo);
    }
    else if (cmd == "listar_resolucao")
    {
        listar_resolucao();
    }
    else if (cmd == "alterar_escala")
    {
        if (argc < 3)
        {
            cout << "Uso: " << argv[0] << " alterar_escala <saida> <escalaX> [escalaY]\n";
            return 1;
        }
        string saida = argv[2];
        float escalaX = atof(argv[3]);
        float escalaY = -1;
        if (argc >= 5)
        {
            escalaY = atof(argv[4]);
        }
        alterarEscala(saida, escalaX, escalaY);
    }
    else if (cmd == "listar_wifi")
    {
        listar_wifi();
    }
    else if (cmd == "conectar_wifi")
    {
        if (argc < 4)
        {
            cout << "Uso: " << argv[0] << " conectar_wifi <SSID> <senha>\n";
            return 1;
        }
        string ssid = argv[2];
        string senha = argv[3];
        conectar_wifi(ssid, senha);
    }
    else if (cmd == "desconectar_wifi")
    {
        if (argc < 3)
        {
            cout << "Uso: " << argv[0] << " desconectar_wifi <SSID>\n";
            return 1;
        }
        string id = argv[2];
        desconectar_wifi(id);
    }
    else if (cmd == "scan_dispositivos_bluetooth")
    {
        scan_dispositivos_bluetooth();
    }
    else if (cmd == "conectar_bluetooth")
    {
        if (argc < 2)
        {
            cout << "Uso: " << argv[0] << "conectar_bluetooth <MAC>\n";
        }
        string mac = argv[2];
        conectar_bluetooth(mac);
    }
    else if (cmd == "desconectar_bluetooth")
    {
        if (argc < 2)
        {
            cout << "Uso: " << argv[0] << "desconectar_bluetooth <MAC>";
        }

        string mac = argv[2];
        desconectar_bluetooth(mac);
    }
    else if (cmd == "listar_dispositivos_bluetooth")
    {
        listar_dispositivos_bluetooth();
    }
    else if (cmd == "gerenciar_bluetooth")
    {
        gerenciar_bluetooth();
    }
    else
    {
        cout << "Comando desconhecido: " << cmd << "\n";
    }

    return 0;
}
