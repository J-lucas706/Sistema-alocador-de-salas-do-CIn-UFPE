#ifndef SALAS_H
#define SALAS_H 

#include <string>
#include <iostream>
#include <unordered_map> 
#include <vector>

using namespace std;

// Classe para gerir as reservas individuais de cada sala
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
    int getInicioMinutos() const;
    int getFimMinutos() const;

    bool temConflito(const string& d, const string& hi, const string& hf) const;
    string paraCsv(const string& codigoSala) const; // Converte a reserva para o formato CSV
};

// classe pai
class Sala {
protected:
    string codigo;
    int capacidade;
    vector<Reserva> reservas; // Vetor que armazena as reservas da sala

public:
    Sala(string c, int cap);
    virtual ~Sala() = default;

    string getCodigo() const;
    void setCodigo(string c);
    
    int getCapacidade() const;
    void setCapacidade(int cap);

    // Métodos para gestão de reservas
    bool verificarConflito(const string& dia, const string& h_inicio, const string& h_fim) const;
    void adicionarReserva(const Reserva& nova_reserva);
    const vector<Reserva>& getReservas() const;
    void listarReservas() const;

    virtual void exibirDetalhes() const = 0; 
    virtual string paraCsv() const = 0; // converte a sala em uma linha do arquivo csv
};

// classe filha das salas teóricas
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

// classe filha dos laboratórios
class Laboratorio : public Sala {
private:
    string tipo_lab; // hardware ou software
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


class SistemaAlocacao {
private:
    unordered_map<string, Sala*> tabela_salas;
    string arquivo_salas;      // arquivo onde as salas sao salvas
    string arquivo_reservas;   // arquivo onde as reservas sao salvas
    bool carregando = false;   // true enquanto le o arquivo, pra n salvar no meio da leitura

    void salvarSalasNoArquivo() const;
    void salvarReservasNoArquivo() const;

public:
    ~SistemaAlocacao(); // criando o destrutor

    // Funções do CRUD de Salas
    void adicionarSala(Sala* nova_sala);
    void buscarSala(string codigo) const;
    void removerSala(string codigo);
    void listarTodas() const;
    int carregarDoArquivo(const string& nomeArquivo); // retorna quantas salas novas entraram

    // Funções de Reserva e Persistência de Reservas
    void realizarReserva();
    int carregarReservasDoArquivo(const string& nomeArquivo); // carrega reservas do CSV
};

#endif