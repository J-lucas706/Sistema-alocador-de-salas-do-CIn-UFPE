#include "../include/SalaTeorica.h"
#include <iostream>

SalaTeorica::SalaTeorica(string c, int cap, bool projetor) : Sala(c, cap) {
    tem_projetor = projetor;
}

bool SalaTeorica::getTemProjetor() const { return tem_projetor; }
void SalaTeorica::setTemProjetor(bool p) { tem_projetor = p; }

void SalaTeorica::exibirDetalhes() const {
    cout << "Sala Teorica: " << codigo << " | Capacidade: " << capacidade
         << " | Projetor: " << (tem_projetor ? "Sim" : "Nao") << endl;
    listarReservas();
}

string SalaTeorica::paraCsv() const {
    return "T," + codigo + "," + to_string(capacidade) + "," + (tem_projetor ? "1" : "0");
}