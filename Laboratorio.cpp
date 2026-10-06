#include "../include/Laboratorio.h"
#include <iostream>

// Chama o construtor do pai (Sala) na lista de inicializacao e depois inicia os atributos proprios
Laboratorio::Laboratorio(string c, int cap, string tipo, int qtd_pcs) : Sala(c, cap) {
    tipo_lab = tipo;
    qtd_computadores = qtd_pcs;
}

string Laboratorio::getTipoLab() const { return tipo_lab; }
void Laboratorio::setTipoLab(string t) { tipo_lab = t; }
int Laboratorio::getQtdComputadores() const { return qtd_computadores; }
void Laboratorio::setQtdComputadores(int qtd) { qtd_computadores = qtd; }

// override: mostra os dados do laboratorio e reaproveita listarReservas() do pai
void Laboratorio::exibirDetalhes() const {
    cout << "Laboratorio de " << tipo_lab << ": " << codigo
         << " | Capacidade: " << capacidade
         << " | Computadores: " << qtd_computadores << endl;
    listarReservas();
}

// override: formato de uma linha do salas.csv (L = laboratorio)
string Laboratorio::paraCsv() const {
    return "L," + codigo + "," + to_string(capacidade) + "," + tipo_lab + "," + to_string(qtd_computadores);
}