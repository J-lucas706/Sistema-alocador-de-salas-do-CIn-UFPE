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
                  const string& hi, const string& hf,
                  const string& nome = "", const string& id = ""); // false se sala nao existe ou ha conflito

    // UPDATE (o codigo da sala e a identidade e nao muda). Para teorica use 'projetor';
    // para laboratorio use 'tipoLab' e 'qtd'. false se a sala nao existe ou os dados sao invalidos.
    bool atualizarSala(const string& codigo, int capacidade, bool projetor,
                       const string& tipoLab, int qtd);
    // UPDATE de uma reserva (indice na lista da sala); false se sala/indice nao existe ou ha conflito
    bool atualizarReserva(const string& codigo, size_t indice, const string& dia,
                          const string& hi, const string& hf,
                          const string& nome, const string& id);

    // DELETE de uma reserva (indice na lista da sala); false se sala/indice nao existe
    bool removerReserva(const string& codigo, size_t indice);

    vector<Sala*> getSalasOrdenadas() const;      // ordenadas por codigo
};

#endif