#ifndef LABORATORIO_H
#define LABORATORIO_H

#include "Sala.h"

class Laboratorio : public Sala {
private:
    string tipo_lab; // Hardware ou Software
    int qtd_computadores;

public:
    Laboratorio(string c, int cap, string tipo, int qtd_pcs);

    string getTipoLab() const;
    void setTipoLab(string t);
    int getQtdComputadores() const;
    void setQtdComputadores(int qtd);

    void exibirDetalhes() const override;
    string paraCsv() const override;
};

#endif