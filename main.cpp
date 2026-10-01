#include <iostream>
#include <string>
#include "include/salas.h"

using namespace std;

int main() {
    SistemaAlocacao meuSistema;
    int opcao = -1;

    cout << "========================================\n";
    cout << "   SISTEMA DE ALOCACAO - CIn UFPE\n";
    cout << "========================================\n";

    int salasCarregadas = meuSistema.carregarDoArquivo("salas.csv");
    cout << salasCarregadas << " sala(s) carregada(s) do arquivo." << endl;

    //mostrando as opções para o usuário
    while (opcao != 0) {
        cout << "\n[1] Adicionar Sala" << endl;
        cout << "[2] Buscar Sala" << endl;
        cout << "[3] Remover Sala" << endl;
        cout << "[4] Listar Todas as Salas" << endl;
        cout << "[0] Sair do Sistema" << endl;
        cout << "Escolha uma opcao: ";
        
        //o input do usuário
        cin >> opcao;

        if (opcao == 1) {
            int tipoSala;
            cout << "\nQual o tipo de sala?\n[1] Teorica\n[2] Laboratorio\nEscolha: ";
            cin >> tipoSala;

            string codigo;
            int capacidade;
            
            cout << "Digite o codigo da sala (Ex: E6): ";
            cin >> codigo;
            cout << "Digite a capacidade de alunos: ";
            cin >> capacidade;

            //perguntando sobre o que mais o aluno deseja na sala teorica
            if (tipoSala == 1) {
                int respProjetor;
                cout << "A sala tem projetor? (1 para Sim, 0 para Nao): ";
                cin >> respProjetor;
                bool temProjetor = (respProjetor == 1);
                
                meuSistema.adicionarSala(new SalaTeorica(codigo, capacidade, temProjetor));
            } 
            ////perguntando sobre o que mais o aluno deseja no laboratório
            else if (tipoSala == 2) {
                string tipoLab;
                int qtdComputadores;
                cout << "Qual o tipo do laboratorio? (Hardware ou Software): ";
                cin >> tipoLab;
                cout << "Quantidade de computadores: ";
                cin >> qtdComputadores;
                
                meuSistema.adicionarSala(new Laboratorio(codigo, capacidade, tipoLab, qtdComputadores));
            } else {
                cout << "Tipo de sala invalido!" << endl;
            }
        } 
        else if (opcao == 2) {
            string codigoBusca;
            cout << "\nDigite o codigo da sala que deseja buscar: ";
            cin >> codigoBusca;
            meuSistema.buscarSala(codigoBusca);
        } 
        else if (opcao == 3) {
            string codigoRemover;
            cout << "\nDigite o codigo da sala que deseja remover: ";
            cin >> codigoRemover;
            meuSistema.removerSala(codigoRemover);
        } 
        else if (opcao == 4) {
            meuSistema.listarTodas();
        } 
        else if (opcao == 0) {
            cout << "\nEncerrando o sistema. Ate logo!" << endl;
        } 
        else {
            cout << "\nOpcao invalida. Tente novamente." << endl;
        }
    }

    return 0;
}