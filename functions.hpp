#ifndef FUNCOES_H
#define FUNCOES_H
#include <string>
#include <vector>

struct device_bt {
    std::string mac;
    std::string nome;
};

void aumentar_volume();
void diminuir_volume();
void definir_volume(float valor);
void aumentar_brilho();
void diminuir_brilho();
void verificarSessao();
void alterarResolucao(const std::string &saida, const std::string &modo);
void listar_resolucao();
void alterarEscala(const std::string &saida, float escalaX, float escalaY = -1);
void listar_wifi();
void conectar_wifi(const std::string &ssid, const std::string &senha);
void desconectar_wifi(const std::string &id);

void scan_dispositivos_bluetooth();
bool conectar_bluetooth(const std::string &mac);
bool desconectar_bluetooth(const std::string &mac);
void listar_dispositivos_bluetooth();
void gerenciar_bluetooth();


#endif
