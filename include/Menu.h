#ifndef MENU_H
#define MENU_H

#include <string>
#include "SistemaAlocacao.h"
#include "RepositorioCsv.h"
using namespace std;

// Toda a interacao com o usuario (cin/cout) fica aqui
class Menu {
private:
    SistemaAlocacao& sistema;
    RepositorioCsv& repo;

    int lerInt(const string& mensagem);
    void exibirOpcoes() const;

    void adicionarSala();
    void buscarSala();
    void removerSala();
    void listarSalas();
    void reservarSala();

public:
    Menu(SistemaAlocacao& s, RepositorioCsv& r);
    void executar();
};

#endif