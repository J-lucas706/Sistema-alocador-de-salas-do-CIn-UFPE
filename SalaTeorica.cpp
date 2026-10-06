#include "../include/SalaTeorica.h"
#include <iostream>

// Chama o construtor do pai (Sala) na lista de inicializacao e depois inicia o atributo proprio
SalaTeorica::SalaTeorica(string c, int cap, bool projetor) : Sala(c, cap) {
    tem_projetor = projetor;
}

bool SalaTeorica::getTemProjetor() const { return tem_projetor; }
void SalaTeorica::setTemProjetor(bool p) { tem_projetor = p; }

// override: mostra os dados da sala teorica e reaproveita listarReservas() do pai
void SalaTeorica::exibirDetalhes() const {
    cout << "Sala Teorica: " << codigo << " | Capacidade: " << capacidade
         << " | Projetor: " << (tem_projetor ? "Sim" : "Nao") << endl;
    listarReservas();
}

// override: formato de uma linha do salas.csv (T = teorica)
string SalaTeorica::paraCsv() const {
    return "T," + codigo + "," + to_string(capacidade) + "," + (tem_projetor ? "1" : "0");
}