#include "../include/Reserva.h"
#include <cctype>

// Converte "HH:MM" em minutos desde 00:00 para facilitar a comparacao de horarios
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

bool Reserva::idValido(const string& id) {
    if (id.length() < 2 || id.length() > 10) return false;
    for (char c : id) {
        if (!isalnum(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

string Reserva::limparCampoCsv(const string& texto) {
    string limpo = texto;
    for (char& c : limpo) {
        if (c == ',' || c == '\n' || c == '\r') c = ' ';
    }
    return limpo;
}

// Guarda tambem os horarios em minutos, calculados uma unica vez
Reserva::Reserva(string d, string hi, string hf, string nome, string id) {
    dia = d;
    hora_inicio = hi;
    hora_fim = hf;
    inicio_minutos = converterParaMinutos(hi);
    fim_minutos = converterParaMinutos(hf);
    responsavel_nome = limparCampoCsv(nome);
    responsavel_id = limparCampoCsv(id);
}

string Reserva::getDia() const { return dia; }
string Reserva::getHoraInicio() const { return hora_inicio; }
string Reserva::getHoraFim() const { return hora_fim; }
string Reserva::getResponsavelNome() const { return responsavel_nome; }
string Reserva::getResponsavelId() const { return responsavel_id; }

// Regra principal do sistema: so ha conflito no MESMO dia e se os intervalos se sobrepoem
// (reservas que apenas se encostam, ex: 08:00-10:00 e 10:00-12:00, NAO conflitam)
bool Reserva::temConflito(const string& d, const string& hi, const string& hf) const {
    if (dia != d) return false;
    int outro_inicio = converterParaMinutos(hi);
    int outro_fim = converterParaMinutos(hf);
    return (inicio_minutos < outro_fim) && (outro_inicio < fim_minutos);
}

string Reserva::paraCsv(const string& codigoSala) const {
    return codigoSala + "," + dia + "," + hora_inicio + "," + hora_fim
           + "," + responsavel_nome + "," + responsavel_id;
}