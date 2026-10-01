#ifndef SALAS_H
#define SALAS_H 

#include <string>
#include <iostream>

//usando para a tabela hash para ir direto a sala que desejo,
//para não ter que usar um laço para ver a sala um por um
#include <unordered_map> 



using namespace std;

//classe pai
class Sala {
protected:
    string codigo;
    int capacidade;

public:
    Sala(string c, int cap);
    virtual ~Sala() = default;

    string getCodigo() const;
    void setCodigo(string c);
    
    int getCapacidade() const;
    void setCapacidade(int cap);

    virtual void exibirDetalhes() const = 0; 
};

//classe filha das salas teóricas
class SalaTeorica : public Sala {
private:
    bool tem_projetor;

public:
    SalaTeorica(string c, int cap, bool projetor);
    
    bool getTemProjetor() const;
    void setTemProjetor(bool p);

    void exibirDetalhes() const override;
};

//classe filha dos laboratórios
class Laboratorio : public Sala {
private:
    string tipo_lab; //hardware ou software
    int qtd_computadores;

public:
    Laboratorio(string c, int cap, string tipo, int qtd_pcs);
    
    string getTipoLab() const;
    void setTipoLab(string t);
    
    int getQtdComputadores() const;
    void setQtdComputadores(int qtd);

    void exibirDetalhes() const override;
};


class SistemaAlocacao {
private:
    
    unordered_map<string, Sala*> tabela_salas;

public:
    ~SistemaAlocacao(); //criando o destrutor

    // Funções do CRUD
    void adicionarSala(Sala* nova_sala);
    void buscarSala(string codigo) const;
    void removerSala(string codigo);
    void listarTodas() const;
    int carregarDoArquivo(const string& nomeArquivo); // retorna quantas salas novas entraram
};

#endif