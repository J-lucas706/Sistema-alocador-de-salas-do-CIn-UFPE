#include "../include/Laboratorio.h"
#include <iostream>

Laboratorio::Laboratorio(string c, int cap, string tipo, int qtd_pcs) : Sala(c, cap) {
    tipo_lab = tipo;
    qtd_computadores = qtd_pcs;
}

string Laboratorio::getTipoLab() const { return tipo_lab; }
void Laboratorio::setTipoLab(string t) { tipo_lab = t; }
int Laboratorio::getQtdComputadores() const { return qtd_computadores; }
void Laboratorio::setQtdComputadores(int qtd) { qtd_computadores = qtd; }

void Laboratorio::exibirDetalhes() const {
    cout << "Laboratorio de " << tipo_lab << ": " << codigo
         << " | Capacidade: " << capacidade
         << " | Computadores: " << qtd_computadores << endl;
    listarReservas();
}

string Laboratorio::paraCsv() const {
    return "L," + codigo + "," + to_string(capacidade) + "," + tipo_lab + "," + to_string(qtd_computadores);
}