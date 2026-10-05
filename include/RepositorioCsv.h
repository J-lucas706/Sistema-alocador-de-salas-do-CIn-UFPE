#ifndef REPOSITORIOCSV_H
#define REPOSITORIOCSV_H

#include <string>
#include "SistemaAlocacao.h"
using namespace std;

// Cuida apenas da persistencia em arquivos CSV
class RepositorioCsv {
private:
    string arquivo_salas;
    string arquivo_reservas;

public:
    RepositorioCsv(string salas, string reservas);

    int carregarSalas(SistemaAlocacao& sistema);      // retorna qtd carregada
    int carregarReservas(SistemaAlocacao& sistema);   // retorna qtd carregada
    void salvarSalas(const SistemaAlocacao& sistema) const;
    void salvarReservas(const SistemaAlocacao& sistema) const;
};

#endif