//puxando o "salas.h"
#include "../include/salas.h" 

//usando a classe mãe
Sala::Sala(string c, int cap) {
    codigo = c;
    capacidade = cap;
}
string Sala::getCodigo() const { return codigo; }
void Sala::setCodigo(string c) { codigo = c; }
int Sala::getCapacidade() const { return capacidade; }
void Sala::setCapacidade(int cap) { capacidade = cap; }

//usando a classe filha das salas teórica
SalaTeorica::SalaTeorica(string c, int cap, bool projetor) : Sala(c, cap) { 
    tem_projetor = projetor;
}
bool SalaTeorica::getTemProjetor() const { return tem_projetor; }
void SalaTeorica::setTemProjetor(bool p) { tem_projetor = p; }
void SalaTeorica::exibirDetalhes() const {
    cout << "Sala Teorica: " << codigo << " | Capacidade: " << capacidade 
         << " | Projetor: " << (tem_projetor ? "Sim" : "Nao") << endl;
}

//usando a classe filha dos laboratórios
Laboratorio::Laboratorio(string c, int cap, string tipo, int qtd_pcs) : Sala(c, cap) { 
    tipo_lab = tipo;
    qtd_computadores = qtd_pcs;
}
string Laboratorio::getTipoLab() const { return tipo_lab; }
void Laboratorio::setTipoLab(string t) { tipo_lab = t; }
int Laboratorio::getQtdComputadores() const { return qtd_computadores; }
void Laboratorio::setQtdComputadores(int qtd) { qtd_computadores = qtd; }
void Laboratorio::exibirDetalhes() const {
    cout << "Laboratorio de " << tipo_lab << ": " << codigo 
         << " | Capacidade: " << capacidade 
         << " | Computadores: " << qtd_computadores << endl;
}

SistemaAlocacao::~SistemaAlocacao() {
    for (auto& par : tabela_salas) {
        delete par.second;
    }
}

void SistemaAlocacao::adicionarSala(Sala* nova_sala) {
    string codigo = nova_sala->getCodigo();
    if (tabela_salas.find(codigo) != tabela_salas.end()) {
        cout << "Erro: Sala " << codigo << " ja existe no sistema!" << endl;
    } else {
        tabela_salas[codigo] = nova_sala;
        cout << "Sala " << codigo << " adicionada com sucesso!" << endl;
    }
}

void SistemaAlocacao::buscarSala(string codigo) const {
    auto it = tabela_salas.find(codigo);
    if (it != tabela_salas.end()) {
        cout << "Sala encontrada: " << endl;
        it->second->exibirDetalhes();
    } else {
        cout << "Sala " << codigo << " nao encontrada." << endl;
    }
}

void SistemaAlocacao::removerSala(string codigo) {
    auto it = tabela_salas.find(codigo);
    if (it != tabela_salas.end()) {
        delete it->second;
        tabela_salas.erase(it);
        cout << "Sala " << codigo << " removida com sucesso!" << endl;
    } else {
        cout << "Erro ao remover: Sala " << codigo << " nao encontrada." << endl;
    }
}

void SistemaAlocacao::listarTodas() const {
    cout << "\n=== Lista de Todas as Salas ===" << endl;
    if (tabela_salas.empty()) {
        cout << "Nenhuma sala cadastrada." << endl;
        return;
    }
    for (const auto& par : tabela_salas) {
        par.second->exibirDetalhes();
    }
    cout << "===============================\n" << endl;
}