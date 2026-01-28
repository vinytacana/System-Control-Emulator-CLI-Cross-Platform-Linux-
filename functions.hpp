#ifndef FUNCOES_H
#define FUNCOES_H

#include <string>
#include <vector>
#include <unordered_map>
#include <istream>

struct device_bt {
    std::string mac;
    std::string nome;
};

// Sistema
void aumentar_volume();
void diminuir_volume();
void definir_volume(float valor);
void aumentar_brilho();
void diminuir_brilho();
void verificarSessao();
void alterarResolucao(const std::string &saida, const std::string &modo);
void listar_resolucao();
void alterarEscala(const std::string &saida, float escalaX, float escalaY = -1);

// Wi-Fi
void listar_wifi();
void conectar_wifi(const std::string &ssid, const std::string &senha);
void desconectar_wifi(const std::string &id);

// Bluetooth
std::vector<device_bt> scan_dispositivos_bluetooth(int segundos = 10);
bool conectar_bluetooth(const std::string &mac);
bool desconectar_bluetooth(const std::string &mac);
void listar_dispositivos_bluetooth(const std::vector<device_bt> &dispositivos);
void gerenciar_bluetooth();

// Parsing
void parsing_bluetooth_stream(
    std::istream &input,
    std::unordered_map<std::string, device_bt> &mapa
);

#endif
