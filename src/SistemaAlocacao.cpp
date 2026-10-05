#include "../include/SistemaAlocacao.h"
#include <algorithm>

SistemaAlocacao::~SistemaAlocacao() {
    for (auto& par : tabela_salas) {
        delete par.second;
    }
}

bool SistemaAlocacao::adicionarSala(Sala* nova) {
    if (tabela_salas.find(nova->getCodigo()) != tabela_salas.end()) {
        delete nova;
        return false;
    }
    tabela_salas[nova->getCodigo()] = nova;
    return true;
}

Sala* SistemaAlocacao::buscarSala(const string& codigo) const {
    auto it = tabela_salas.find(codigo);
    return (it != tabela_salas.end()) ? it->second : nullptr;
}

bool SistemaAlocacao::removerSala(const string& codigo) {
    auto it = tabela_salas.find(codigo);
    if (it == tabela_salas.end()) return false;
    delete it->second;
    tabela_salas.erase(it);
    return true;
}

bool SistemaAlocacao::reservar(const string& codigo, const string& dia,
                               const string& hi, const string& hf) {
    Sala* sala = buscarSala(codigo);
    if (sala == nullptr) return false;
    if (sala->verificarConflito(dia, hi, hf)) return false;
    sala->adicionarReserva(Reserva(dia, hi, hf));
    return true;
}

vector<Sala*> SistemaAlocacao::getSalasOrdenadas() const {
    vector<Sala*> lista;
    for (const auto& par : tabela_salas) lista.push_back(par.second);
    sort(lista.begin(), lista.end(), [](const Sala* a, const Sala* b) {
        return a->getCodigo() < b->getCodigo();
    });
    return lista;
}