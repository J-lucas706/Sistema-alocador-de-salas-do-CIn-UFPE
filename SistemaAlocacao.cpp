#include "../include/SistemaAlocacao.h"
#include "../include/SalaTeorica.h"
#include "../include/Laboratorio.h"
#include <algorithm>

// Libera cada Sala alocada com new (o destrutor virtual de Sala cuida de apagar a filha corretamente)
SistemaAlocacao::~SistemaAlocacao() {
    for (auto& par : tabela_salas) {
        delete par.second;
    }
}

// O sistema assume a posse do ponteiro; se o codigo ja existe, descarta a sala nova
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

// So reserva se a sala existe e nao ha conflito de horario
bool SistemaAlocacao::reservar(const string& codigo, const string& dia,
                               const string& hi, const string& hf,
                               const string& nome, const string& id) {
    Sala* sala = buscarSala(codigo);
    if (sala == nullptr) return false;
    if (sala->verificarConflito(dia, hi, hf)) return false;
    sala->adicionarReserva(Reserva(dia, hi, hf, nome, id));
    return true;
}

// dynamic_cast descobre em tempo de execucao se a Sala* e teorica ou laboratorio
// para alterar so os atributos da classe filha
bool SistemaAlocacao::atualizarSala(const string& codigo, int capacidade, bool projetor,
                                    const string& tipoLab, int qtd) {
    Sala* sala = buscarSala(codigo);
    if (sala == nullptr || capacidade <= 0) return false;
    if (auto t = dynamic_cast<SalaTeorica*>(sala)) {
        t->setTemProjetor(projetor);
    } else if (auto l = dynamic_cast<Laboratorio*>(sala)) {
        if (tipoLab.empty() || qtd < 0) return false;
        l->setTipoLab(tipoLab);
        l->setQtdComputadores(qtd);
    }
    sala->setCapacidade(capacidade);
    return true;
}

bool SistemaAlocacao::atualizarReserva(const string& codigo, size_t indice, const string& dia,
                                       const string& hi, const string& hf,
                                       const string& nome, const string& id) {
    Sala* sala = buscarSala(codigo);
    if (sala == nullptr) return false;
    return sala->atualizarReserva(indice, Reserva(dia, hi, hf, nome, id));
}

bool SistemaAlocacao::removerReserva(const string& codigo, size_t indice) {
    Sala* sala = buscarSala(codigo);
    if (sala == nullptr) return false;
    return sala->removerReserva(indice);
}

// A tabela hash nao guarda ordem; copia para um vector e ordena pelo codigo
vector<Sala*> SistemaAlocacao::getSalasOrdenadas() const {
    vector<Sala*> lista;
    for (const auto& par : tabela_salas) lista.push_back(par.second);
    sort(lista.begin(), lista.end(), [](const Sala* a, const Sala* b) {
        return a->getCodigo() < b->getCodigo();
    });
    return lista;
}