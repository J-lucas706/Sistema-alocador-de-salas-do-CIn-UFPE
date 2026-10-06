#include <iostream>
#include "../include/SistemaAlocacao.h"
#include "../include/RepositorioCsv.h"
#include "../include/Menu.h"

int main() {
    cout << "========================================\n";
    cout << "   SISTEMA DE ALOCACAO - CIn UFPE\n";
    cout << "========================================\n";

    // Ordem: cria o sistema, carrega os dados dos CSVs e entrega o controle ao menu
    SistemaAlocacao sistema;
    RepositorioCsv repo("data/salas.csv", "data/reservas.csv");

    cout << repo.carregarSalas(sistema) << " sala(s) carregada(s) do arquivo." << endl;
    cout << repo.carregarReservas(sistema) << " reserva(s) carregada(s) do arquivo." << endl;

    Menu menu(sistema, repo);
    menu.executar();
    return 0;
}