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
    string responsavel_nome;   // quem alugou a sala
    string responsavel_id;     // ID (login) do CIn/UFPE, ex: fln1

    static int converterParaMinutos(const string& hora);

public:
    // nome e id sao opcionais para continuar lendo reservas antigas (sem responsavel)
    Reserva(string d, string hi, string hf, string nome = "", string id = "");

    string getDia() const;
    string getHoraInicio() const;
    string getHoraFim() const;
    string getResponsavelNome() const;
    string getResponsavelId() const;

    bool temConflito(const string& d, const string& hi, const string& hf) const;
    string paraCsv(const string& codigoSala) const;

    // Valida o formato HH:MM
    static bool horarioValido(const string& hora);
    // Valida se inicio < fim (ambos no formato HH:MM)
    static bool intervaloValido(const string& hi, const string& hf);
    // Valida o ID do CIn: so letras e numeros, de 2 a 10 caracteres (ex: fln1)
    static bool idValido(const string& id);
    // Troca virgulas e quebras de linha por espaco para nao quebrar o CSV
    static string limparCampoCsv(const string& texto);
};

#endif