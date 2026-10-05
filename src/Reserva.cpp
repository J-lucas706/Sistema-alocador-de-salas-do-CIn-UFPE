#include "../include/Reserva.h"
#include <cctype>

int Reserva::converterParaMinutos(const string& hora) {
    int h = stoi(hora.substr(0, 2));
    int m = stoi(hora.substr(3, 2));
    return h * 60 + m;
}

bool Reserva::horarioValido(const string& hora) {
    if (hora.length() != 5 || hora[2] != ':') return false;
    for (int i : {0, 1, 3, 4}) {
        if (!isdigit(hora[i])) return false;
    }
    return stoi(hora.substr(0, 2)) < 24 && stoi(hora.substr(3, 2)) < 60;
}

bool Reserva::intervaloValido(const string& hi, const string& hf) {
    if (!horarioValido(hi) || !horarioValido(hf)) return false;
    return converterParaMinutos(hi) < converterParaMinutos(hf);
}

Reserva::Reserva(string d, string hi, string hf) {
    dia = d;
    hora_inicio = hi;
    hora_fim = hf;
    inicio_minutos = converterParaMinutos(hi);
    fim_minutos = converterParaMinutos(hf);
}

string Reserva::getDia() const { return dia; }
string Reserva::getHoraInicio() const { return hora_inicio; }
string Reserva::getHoraFim() const { return hora_fim; }

bool Reserva::temConflito(const string& d, const string& hi, const string& hf) const {
    if (dia != d) return false;
    int outro_inicio = converterParaMinutos(hi);
    int outro_fim = converterParaMinutos(hf);
    return (inicio_minutos < outro_fim) && (outro_inicio < fim_minutos);
}

string Reserva::paraCsv(const string& codigoSala) const {
    return codigoSala + "," + dia + "," + hora_inicio + "," + hora_fim;
}