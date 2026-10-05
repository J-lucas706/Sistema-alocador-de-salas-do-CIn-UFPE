#include "../include/RepositorioCsv.h"
#include "../include/SalaTeorica.h"
#include "../include/Laboratorio.h"
#include <fstream>
#include <sstream>
#include <iostream>

RepositorioCsv::RepositorioCsv(string salas, string reservas) {
    arquivo_salas = salas;
    arquivo_reservas = reservas;
}

int RepositorioCsv::carregarSalas(SistemaAlocacao& sistema) {
    ifstream arquivo(arquivo_salas);
    if (!arquivo.is_open()) {
        cout << "Arquivo " << arquivo_salas << " nao encontrado, iniciando sem salas." << endl;
        return 0;
    }

    string linha;
    int numeroLinha = 0, carregadas = 0;

    while (getline(arquivo, linha)) {
        numeroLinha++;
        if (!linha.empty() && linha.back() == '\r') linha.pop_back();
        if (linha.empty() || linha[0] == '#') continue;

        stringstream fluxo(linha);
        string tipo, codigo, capTexto, extra1, extra2;
        getline(fluxo, tipo, ',');
        getline(fluxo, codigo, ',');
        getline(fluxo, capTexto, ',');
        getline(fluxo, extra1, ',');
        getline(fluxo, extra2, ',');

        try {
            int cap = stoi(capTexto);
            Sala* sala = nullptr;
            if (tipo == "T") {
                sala = new SalaTeorica(codigo, cap, stoi(extra1) == 1);
            } else if (tipo == "L") {
                sala = new Laboratorio(codigo, cap, extra1, stoi(extra2));
            } else {
                cout << "Linha " << numeroLinha << ": tipo invalido, ignorando." << endl;
                continue;
            }
            if (sistema.adicionarSala(sala)) carregadas++;
            else cout << "Linha " << numeroLinha << ": sala duplicada, ignorando." << endl;
        } catch (...) {
            cout << "Linha " << numeroLinha << ": formato invalido, ignorando." << endl;
        }
    }
    return carregadas;
}

int RepositorioCsv::carregarReservas(SistemaAlocacao& sistema) {
    ifstream arquivo(arquivo_reservas);
    if (!arquivo.is_open()) {
        cout << "Arquivo " << arquivo_reservas << " nao encontrado, iniciando sem reservas." << endl;
        return 0;
    }

    string linha;
    int numeroLinha = 0, carregadas = 0;

    while (getline(arquivo, linha)) {
        numeroLinha++;
        if (!linha.empty() && linha.back() == '\r') linha.pop_back();
        if (linha.empty() || linha[0] == '#') continue;

        stringstream fluxo(linha);
        string codigo, dia, hi, hf;
        getline(fluxo, codigo, ',');
        getline(fluxo, dia, ',');
        getline(fluxo, hi, ',');
        getline(fluxo, hf, ',');

        if (!Reserva::intervaloValido(hi, hf)) {
            cout << "Linha " << numeroLinha << ": horario invalido, reserva ignorada." << endl;
            continue;
        }
        if (sistema.reservar(codigo, dia, hi, hf)) {
            carregadas++;
        } else {
            cout << "Linha " << numeroLinha << ": sala '" << codigo
                 << "' inexistente ou conflito, reserva ignorada." << endl;
        }
    }
    return carregadas;
}

void RepositorioCsv::salvarSalas(const SistemaAlocacao& sistema) const {
    ofstream arquivo(arquivo_salas);
    if (!arquivo.is_open()) {
        cout << "Erro: nao foi possivel salvar em " << arquivo_salas << endl;
        return;
    }
    arquivo << "# tipo,codigo,capacidade,extra1,extra2" << endl;
    arquivo << "# T = teorica (extra1: projetor 1/0) | L = laboratorio (extra1: Hardware/Software, extra2: qtd computadores)" << endl;
    for (const Sala* sala : sistema.getSalasOrdenadas()) {
        arquivo << sala->paraCsv() << endl;
    }
}

void RepositorioCsv::salvarReservas(const SistemaAlocacao& sistema) const {
    ofstream arquivo(arquivo_reservas);
    if (!arquivo.is_open()) {
        cout << "Erro: nao foi possivel salvar em " << arquivo_reservas << endl;
        return;
    }
    arquivo << "# codigo_sala,dia,hora_inicio,hora_fim" << endl;
    for (const Sala* sala : sistema.getSalasOrdenadas()) {
        for (const auto& r : sala->getReservas()) {
            arquivo << r.paraCsv(sala->getCodigo()) << endl;
        }
    }
}