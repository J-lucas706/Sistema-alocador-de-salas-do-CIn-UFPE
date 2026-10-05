#ifndef SISTEMAALOCACAO_H
#define SISTEMAALOCACAO_H

#include <unordered_map>
#include <string>
#include <vector>
#include "Sala.h"
using namespace std;

// Apenas regra de negocio: sem cin/cout e sem arquivos
class SistemaAlocacao {
private:
    unordered_map<string, Sala*> tabela_salas;

public:
    SistemaAlocacao() = default;
    ~SistemaAlocacao();
    // evita copia (o sistema e dono dos ponteiros)
    SistemaAlocacao(const SistemaAlocacao&) = delete;
    SistemaAlocacao& operator=(const SistemaAlocacao&) = delete;

    bool adicionarSala(Sala* nova);               // false se o codigo ja existe
    Sala* buscarSala(const string& codigo) const; // nullptr se nao existe
    bool removerSala(const string& codigo);
    bool reservar(const string& codigo, const string& dia,
                  const string& hi, const string& hf); // false se sala nao existe ou ha conflito

    vector<Sala*> getSalasOrdenadas() const;      // ordenadas por codigo
};

#endif