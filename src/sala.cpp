// puxando o "salas.h"
#include "../include/salas.h" 
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

// --- Métodos da classe Reserva ---
int Reserva::converterParaMinutos(const string& hora) {
    if (hora.length() < 5) return 0;
    int h = stoi(hora.substr(0, 2));
    int m = stoi(hora.substr(3, 2));
    return h * 60 + m;
}

Reserva::Reserva(string d, string hi, string hf) {
    dia = d;
    hora_inicio = hi;
    hora_fim = hf;
    inicio_minutos = converterParaMinutos(hi);
    fim_minutos = converterParaMinutos(hf);
}

string Reserva::getDia() const { return dia; }
string Reserva::getHoraInicio() const { return hora_inicio; }
string Reserva::getHoraFim() const { return hora_fim; }
int Reserva::getInicioMinutos() const { return inicio_minutos; }
int Reserva::getFimMinutos() const { return fim_minutos; }

bool Reserva::temConflito(const string& d, const string& hi, const string& hf) const {
    if (dia != d) return false;
    int outro_inicio = converterParaMinutos(hi);
    int outro_fim = converterParaMinutos(hf);
    return (inicio_minutos < outro_fim) && (outro_inicio < fim_minutos);
}

string Reserva::paraCsv(const string& codigoSala) const {
    return codigoSala + "," + dia + "," + hora_inicio + "," + hora_fim;
}

// --- Métodos da classe mãe Sala ---
Sala::Sala(string c, int cap) {
    codigo = c;
    capacidade = cap;
}
string Sala::getCodigo() const { return codigo; }
void Sala::setCodigo(string c) { codigo = c; }
int Sala::getCapacidade() const { return capacidade; }
void Sala::setCapacidade(int cap) { capacidade = cap; }

bool Sala::verificarConflito(const string& dia, const string& h_inicio, const string& h_fim) const {
    for (const auto& r : reservas) {
        if (r.temConflito(dia, h_inicio, h_fim)) {
            return true;
        }
    }
    return false;
}

void Sala::adicionarReserva(const Reserva& nova_reserva) {
    reservas.push_back(nova_reserva);
}

const vector<Reserva>& Sala::getReservas() const {
    return reservas;
}

void Sala::listarReservas() const {
    if (reservas.empty()) {
        cout << "  [Sem reservas cadastradas]" << endl;
    } else {
        cout << "  [Reservas de Alocacao]:" << endl;
        for (const auto& r : reservas) {
            cout << "    - Dia: " << r.getDia() 
                 << " | Horario: " << r.getHoraInicio() << " as " << r.getHoraFim() << endl;
        }
    }
}

// --- Métodos da classe filha SalaTeorica ---
SalaTeorica::SalaTeorica(string c, int cap, bool projetor) : Sala(c, cap) { 
    tem_projetor = projetor;
}
bool SalaTeorica::getTemProjetor() const { return tem_projetor; }
void SalaTeorica::setTemProjetor(bool p) { tem_projetor = p; }
void SalaTeorica::exibirDetalhes() const {
    cout << "Sala Teorica: " << codigo << " | Capacidade: " << capacidade 
         << " | Projetor: " << (tem_projetor ? "Sim" : "Nao") << endl;
    listarReservas();
}
string SalaTeorica::paraCsv() const {
    return "T," + codigo + "," + to_string(capacidade) + "," + (tem_projetor ? "1" : "0");
}

// --- Métodos da classe filha Laboratorio ---
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
    listarReservas();
}
string Laboratorio::paraCsv() const {
    return "L," + codigo + "," + to_string(capacidade) + "," + tipo_lab + "," + to_string(qtd_computadores);
}

// --- Métodos da classe SistemaAlocacao ---
SistemaAlocacao::~SistemaAlocacao() {
    for (auto& par : tabela_salas) {
        delete par.second;
    }
}

void SistemaAlocacao::adicionarSala(Sala* nova_sala) {
    string codigo = nova_sala->getCodigo();
    if (tabela_salas.find(codigo) != tabela_salas.end()) {
        cout << "Erro: Sala " << codigo << " ja existe no sistema!" << endl;
        delete nova_sala;
    } else {
        tabela_salas[codigo] = nova_sala;
        cout << "Sala " << codigo << " adicionada com sucesso!" << endl;
        if (!carregando) salvarSalasNoArquivo();
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
        salvarSalasNoArquivo();
        salvarReservasNoArquivo(); // Atualiza as reservas pois a sala removida continha reservas
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

int SistemaAlocacao::carregarDoArquivo(const string& nomeArquivo) {
    arquivo_salas = nomeArquivo;

    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Arquivo " << nomeArquivo << " nao encontrado, iniciando sem salas." << endl;
        return 0;
    }

    carregando = true;
    int quantidadeAntes = tabela_salas.size();
    string linha;
    int numeroLinha = 0;

    while (getline(arquivo, linha)) {
        numeroLinha++;
        if (!linha.empty() && linha.back() == '\r') linha.pop_back();
        if (linha.empty() || linha[0] == '#') continue;

        stringstream fluxoLinha(linha);
        string tipo, codigo, capacidadeTexto, extra1, extra2;
        getline(fluxoLinha, tipo, ',');
        getline(fluxoLinha, codigo, ',');
        getline(fluxoLinha, capacidadeTexto, ',');
        getline(fluxoLinha, extra1, ',');
        getline(fluxoLinha, extra2, ',');

        try {
            int capacidade = stoi(capacidadeTexto);
            if (tipo == "T") {
                adicionarSala(new SalaTeorica(codigo, capacidade, stoi(extra1) == 1));
            } else if (tipo == "L") {
                adicionarSala(new Laboratorio(codigo, capacidade, extra1, stoi(extra2)));
            } else {
                cout << "Linha " << numeroLinha << ": tipo invalido, ignorando." << endl;
            }
        } catch (...) {
            cout << "Linha " << numeroLinha << ": formato invalido, ignorando." << endl;
        }
    }

    arquivo.close();
    carregando = false;
    return tabela_salas.size() - quantidadeAntes;
}

void SistemaAlocacao::salvarSalasNoArquivo() const {
    if (arquivo_salas.empty()) return;

    ofstream arquivo(arquivo_salas);
    if (!arquivo.is_open()) {
        cout << "Erro: nao foi possivel salvar em " << arquivo_salas << endl;
        return;
    }

    arquivo << "# tipo,codigo,capacidade,extra1,extra2" << endl;
    arquivo << "# T = teorica (extra1: projetor 1/0) | L = laboratorio (extra1: Hardware/Software, extra2: qtd computadores)" << endl;

    vector<string> codigos;
    for (const auto& par : tabela_salas) codigos.push_back(par.first);
    sort(codigos.begin(), codigos.end());

    for (const string& codigo : codigos) {
        arquivo << tabela_salas.at(codigo)->paraCsv() << endl;
    }
}

void SistemaAlocacao::realizarReserva() {
    string codigo;
    Sala* salaEncontrada = nullptr;

    // --- ETAPA 1: Busca e validação do código da sala na Tabela Hash ---
    while (true) {
        cout << "\n=== ALOCACAO / RESERVA DE SALA ===" << endl;
        cout << "Digite o codigo da sala (ou '0' / 'CANCELAR' para voltar): ";
        cin >> codigo;

        if (codigo == "0" || codigo == "CANCELAR" || codigo == "cancelar") {
            cout << "Operacao de reserva cancelada." << endl;
            return;
        }

        auto it = tabela_salas.find(codigo);
        if (it != tabela_salas.end()) {
            salaEncontrada = it->second;
            cout << "Sala " << codigo << " localizada com sucesso!" << endl;
            break;
        }

        cout << "Erro: Sala '" << codigo << "' nao encontrada no sistema. Tente novamente." << endl;
    }

    // --- ETAPA 2: Validação de Dia, Horários e Checagem de Conflitos ---
    string dia, hora_inicio, hora_fim;

    while (true) {
        cout << "\nDigite o dia da reserva (ex: 15/10/2026) ou '0' para cancelar: ";
        cin >> dia;

        if (dia == "0" || dia == "CANCELAR" || dia == "cancelar") {
            cout << "Operacao de reserva cancelada." << endl;
            return;
        }

        cout << "Digite o horario de inicio (formato HH:MM, ex: 14:00): ";
        cin >> hora_inicio;

        cout << "Digite o horario de fim (formato HH:MM, ex: 16:00): ";
        cin >> hora_fim;

        if (salaEncontrada->verificarConflito(dia, hora_inicio, hora_fim)) {
            cout << "\n[ERRO DE CONFLITO] A sala " << codigo 
                 << " ja possui reserva no dia " << dia 
                 << " no intervalo de " << hora_inicio << " as " << hora_fim << "." << endl;
            cout << "Por favor, tente outro dia/horario ou digite '0' para cancelar." << endl;
        } else {
            Reserva nova_reserva(dia, hora_inicio, hora_fim);
            salaEncontrada->adicionarReserva(nova_reserva);
            cout << "\n[SUCESSO] Reserva realizada com sucesso para a sala " << codigo 
                 << " no dia " << dia << " (" << hora_inicio << " as " << hora_fim << ")!" << endl;
            
            salvarReservasNoArquivo(); // Persiste a nova reserva no arquivo CSV
            break;
        }
    }
}

int SistemaAlocacao::carregarReservasDoArquivo(const string& nomeArquivo) {
    arquivo_reservas = nomeArquivo;

    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Arquivo " << nomeArquivo << " nao encontrado, iniciando sem reservas." << endl;
        return 0;
    }

    string linha;
    int numeroLinha = 0;
    int reservasCarregadas = 0;

    while (getline(arquivo, linha)) {
        numeroLinha++;
        if (!linha.empty() && linha.back() == '\r') linha.pop_back();
        if (linha.empty() || linha[0] == '#') continue;

        stringstream fluxoLinha(linha);
        string codigoSala, dia, horaInicio, horaFim;
        getline(fluxoLinha, codigoSala, ',');
        getline(fluxoLinha, dia, ',');
        getline(fluxoLinha, horaInicio, ',');
        getline(fluxoLinha, horaFim, ',');

        auto it = tabela_salas.find(codigoSala);
        if (it != tabela_salas.end()) {
            Reserva novaReserva(dia, horaInicio, horaFim);
            it->second->adicionarReserva(novaReserva);
            reservasCarregadas++;
        } else {
            cout << "Linha " << numeroLinha << ": Sala '" << codigoSala << "' nao existe, reserva ignorada." << endl;
        }
    }

    arquivo.close();
    return reservasCarregadas;
}

void SistemaAlocacao::salvarReservasNoArquivo() const {
    if (arquivo_reservas.empty()) return;

    ofstream arquivo(arquivo_reservas);
    if (!arquivo.is_open()) {
        cout << "Erro: nao foi possivel salvar em " << arquivo_reservas << endl;
        return;
    }

    arquivo << "# codigo_sala,dia,hora_inicio,hora_fim" << endl;

    vector<string> codigos;
    for (const auto& par : tabela_salas) codigos.push_back(par.first);
    sort(codigos.begin(), codigos.end());

    for (const string& codigo : codigos) {
        const Sala* sala = tabela_salas.at(codigo);
        for (const auto& r : sala->getReservas()) {
            arquivo << r.paraCsv(codigo) << endl;
        }
    }
}