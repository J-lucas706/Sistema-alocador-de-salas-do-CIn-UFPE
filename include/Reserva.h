#ifndef RESERVA_H
#define RESERVA_H

#include <string>
using namespace std;

// Representa uma reserva (dia + intervalo de horario)
class Reserva {
private:
    string dia;
    string hora_inicio;
    string hora_fim;
    int inicio_minutos;
    int fim_minutos;

    static int converterParaMinutos(const string& hora);

public:
    Reserva(string d, string hi, string hf);

    string getDia() const;
    string getHoraInicio() const;
    string getHoraFim() const;

    bool temConflito(const string& d, const string& hi, const string& hf) const;
    string paraCsv(const string& codigoSala) const;

    // Valida o formato HH:MM
    static bool horarioValido(const string& hora);
    // Valida se inicio < fim (ambos no formato HH:MM)
    static bool intervaloValido(const string& hi, const string& hf);
};

#endif