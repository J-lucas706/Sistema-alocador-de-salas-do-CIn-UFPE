#include "../include/Menu.h"
#include "../include/SalaTeorica.h"
#include "../include/Laboratorio.h"
#include <iostream>
#include <limits>

Menu::Menu(SistemaAlocacao& s, RepositorioCsv& r) : sistema(s), repo(r) {}

// Le um inteiro e repete enquanto a entrada for invalida
int Menu::lerInt(const string& mensagem) {
    int valor;
    while (true) {
        cout << mensagem;
        if (cin >> valor) return valor;
        if (cin.eof()) exit(0);
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida, digite um numero." << endl;
    }
}

void Menu::exibirOpcoes() const {
    cout << "\n[1] Adicionar Sala" << endl;
    cout << "[2] Buscar Sala" << endl;
    cout << "[3] Remover Sala" << endl;
    cout << "[4] Listar Todas as Salas" << endl;
    cout << "[5] Reservar Sala" << endl;
    cout << "[0] Sair do Sistema" << endl;
}

void Menu::executar() {
    int opcao = -1;
    while (opcao != 0) {
        exibirOpcoes();
        opcao = lerInt("Escolha uma opcao: ");

        switch (opcao) {
            case 1: adicionarSala(); break;
            case 2: buscarSala();    break;
            case 3: removerSala();   break;
            case 4: listarSalas();   break;
            case 5: reservarSala();  break;
            case 0: cout << "\nEncerrando o sistema. Ate logo!" << endl; break;
            default: cout << "\nOpcao invalida. Tente novamente." << endl;
        }
    }
}

void Menu::adicionarSala() {
    int tipo = lerInt("\nQual o tipo de sala?\n[1] Teorica\n[2] Laboratorio\nEscolha: ");
    if (tipo != 1 && tipo != 2) {
        cout << "Tipo de sala invalido!" << endl;
        return;
    }

    string codigo;
    cout << "Digite o codigo da sala (Ex: E6): ";
    cin >> codigo;
    int capacidade = lerInt("Digite a capacidade de alunos: ");

    Sala* nova = nullptr;
    if (tipo == 1) {
        int proj = lerInt("A sala tem projetor? (1 para Sim, 0 para Nao): ");
        nova = new SalaTeorica(codigo, capacidade, proj == 1);
    } else {
        string tipoLab;
        cout << "Qual o tipo do laboratorio? (Hardware ou Software): ";
        cin >> tipoLab;
        int qtd = lerInt("Quantidade de computadores: ");
        nova = new Laboratorio(codigo, capacidade, tipoLab, qtd);
    }

    if (sistema.adicionarSala(nova)) {
        cout << "Sala " << codigo << " adicionada com sucesso!" << endl;
        repo.salvarSalas(sistema);
    } else {
        cout << "Erro: Sala " << codigo << " ja existe no sistema!" << endl;
    }
}

void Menu::buscarSala() {
    string codigo;
    cout << "\nDigite o codigo da sala que deseja buscar: ";
    cin >> codigo;

    Sala* sala = sistema.buscarSala(codigo);
    if (sala != nullptr) {
        cout << "Sala encontrada:" << endl;
        sala->exibirDetalhes();
    } else {
        cout << "Sala " << codigo << " nao encontrada." << endl;
    }
}

void Menu::removerSala() {
    string codigo;
    cout << "\nDigite o codigo da sala que deseja remover: ";
    cin >> codigo;

    if (sistema.removerSala(codigo)) {
        cout << "Sala " << codigo << " removida com sucesso!" << endl;
        repo.salvarSalas(sistema);
        repo.salvarReservas(sistema); // a sala removida tinha reservas
    } else {
        cout << "Erro ao remover: Sala " << codigo << " nao encontrada." << endl;
    }
}

void Menu::listarSalas() {
    cout << "\n=== Lista de Todas as Salas ===" << endl;
    auto salas = sistema.getSalasOrdenadas();
    if (salas.empty()) {
        cout << "Nenhuma sala cadastrada." << endl;
        return;
    }
    for (const Sala* s : salas) s->exibirDetalhes();
    cout << "===============================" << endl;
}

void Menu::reservarSala() {
    string codigo;
    cout << "\n=== ALOCACAO / RESERVA DE SALA ===" << endl;

    // Etapa 1: escolher a sala
    while (true) {
        cout << "Digite o codigo da sala (ou '0' para voltar): ";
        cin >> codigo;
        if (codigo == "0") {
            cout << "Operacao de reserva cancelada." << endl;
            return;
        }
        if (sistema.buscarSala(codigo) != nullptr) break;
        cout << "Erro: Sala '" << codigo << "' nao encontrada. Tente novamente." << endl;
    }

    // Etapa 2: dia e horarios
    string dia, hi, hf;
    while (true) {
        cout << "\nDigite o dia da reserva (ex: 15/10/2026) ou '0' para cancelar: ";
        cin >> dia;
        if (dia == "0") {
            cout << "Operacao de reserva cancelada." << endl;
            return;
        }
        cout << "Horario de inicio (HH:MM): ";
        cin >> hi;
        cout << "Horario de fim (HH:MM): ";
        cin >> hf;

        if (!Reserva::intervaloValido(hi, hf)) {
            cout << "Horario invalido! Use HH:MM e o inicio deve ser antes do fim." << endl;
            continue;
        }

        if (sistema.reservar(codigo, dia, hi, hf)) {
            cout << "\n[SUCESSO] Sala " << codigo << " reservada em " << dia
                 << " (" << hi << " as " << hf << ")!" << endl;
            repo.salvarReservas(sistema);
            return;
        }
        cout << "\n[ERRO DE CONFLITO] A sala " << codigo << " ja possui reserva em "
             << dia << " nesse intervalo. Tente outro horario." << endl;
    }
}