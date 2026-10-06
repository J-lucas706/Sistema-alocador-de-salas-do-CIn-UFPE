#include "../include/Sala.h"
#include <iostream>

Sala::Sala(string c, int cap) {
    codigo = c;
    capacidade = cap;
}

string Sala::getCodigo() const { return codigo; }
void Sala::setCodigo(string c) { codigo = c; }
int Sala::getCapacidade() const { return capacidade; }
void Sala::setCapacidade(int cap) { capacidade = cap; }

bool Sala::verificarConflito(const string& dia, const string& hi, const string& hf) const {
    for (const auto& r : reservas) {
        if (r.temConflito(dia, hi, hf)) return true;
    }
    return false;
}

void Sala::adicionarReserva(const Reserva& nova) {
    reservas.push_back(nova);
}

bool Sala::atualizarReserva(size_t indice, const Reserva& nova) {
    if (indice >= reservas.size()) return false;
    for (size_t i = 0; i < reservas.size(); i++) {
        if (i == indice) continue;  // a propria reserva nao conta como conflito
        if (reservas[i].temConflito(nova.getDia(), nova.getHoraInicio(), nova.getHoraFim())) return false;
    }
    reservas[indice] = nova;
    return true;
}

const vector<Reserva>& Sala::getReservas() const {
    return reservas;
}

void Sala::listarReservas() const {
    if (reservas.empty()) {
        cout << "  [Sem reservas cadastradas]" << endl;
        return;
    }
    cout << "  [Reservas de Alocacao]:" << endl;
    for (const auto& r : reservas) {
        cout << "    - Dia: " << r.getDia()
             << " | Horario: " << r.getHoraInicio() << " as " << r.getHoraFim();
        if (!r.getResponsavelId().empty()) {
            cout << " | Alugada por " << r.getResponsavelNome()
                 << " <" << r.getResponsavelId() << ">";
        } else {
            cout << " | Responsavel nao informado";  // reserva antiga, sem nome/ID
        }
        cout << endl;
    }
}