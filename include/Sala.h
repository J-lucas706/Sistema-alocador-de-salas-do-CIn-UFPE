#ifndef SALA_H
#define SALA_H

#include <string>
#include <vector>
#include "Reserva.h"
using namespace std;

// Classe abstrata base de todas as salas
class Sala {
protected:
    string codigo;
    int capacidade;
    vector<Reserva> reservas;

public:
    Sala(string c, int cap);
    virtual ~Sala() = default;

    string getCodigo() const;
    void setCodigo(string c);
    int getCapacidade() const;
    void setCapacidade(int cap);

    bool verificarConflito(const string& dia, const string& hi, const string& hf) const;
    void adicionarReserva(const Reserva& nova);
    const vector<Reserva>& getReservas() const;
    void listarReservas() const;

    virtual void exibirDetalhes() const = 0;
    virtual string paraCsv() const = 0;
};

#endif