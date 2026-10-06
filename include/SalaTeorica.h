#ifndef SALATEORICA_H
#define SALATEORICA_H

#include "Sala.h"

class SalaTeorica : public Sala {
private:
    bool tem_projetor;

public:
    SalaTeorica(string c, int cap, bool projetor);

    bool getTemProjetor() const;
    void setTemProjetor(bool p);

    void exibirDetalhes() const override;
    string paraCsv() const override;
};

#endif