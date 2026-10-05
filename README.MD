# Sistema de Alocação de Salas

> Projeto desenvolvido para a disciplina de **Estruturas de Dados Orientadas a Objetos (EDOO)** — CIn/UFPE.

**Semestre:** 2026.2  

---

## Equipe
* **Caio Cesar Souza de Lira** (`ccsl@cin.ufpe.br`)
* **João Lucas Bezerra Silva** (`jlbs2@cin.ufpe.br`)
* **Matheus Luiz Teixeira da Silva** (`mlts@cin.ufpe.br`)
* **Vicente Martins Pereira Loureiro** (`vmpl@cin.ufpe.br`)

---

## Visão Geral
O **Sistema de Alocação de Salas** é uma aplicação voltada para o gerenciamento, reserva e organização do uso de salas.

O backend foi desenvolvido **100% em C++ puro** (sem bibliotecas externas), sendo responsável por gerenciar a lógica de negócio, a persistência de dados e a execução do servidor web local. A interface gráfica é renderizada no navegador por meio de um frontend em HTML5, CSS3 e JavaScript (Vanilla).

---

## Conceitos da Disciplina Aplicados (EDOO)
O backend da aplicação foi projetado aplicando integralmente os pilares do paradigma de **Programação Orientada a Objetos (POO)**:

* **Classes e Abstração:** Modelagem de entidades do domínio do problema (salas, reservas, usuários, alocações).
* **Encapsulamento:** Proteção do estado interno dos objetos através de atributos privados e métodos de acesso seguros.
* **Herança:** Criação de hierarquias de classes para reaproveitamento de código e especialização de comportamentos.
* **Polimorfismo:** Utilização de métodos virtuais e sobrescrita de comportamentos para tratamento genérico de diferentes tipos de objetos.
* **Estruturas de Dados:** Organização e manipulação eficiente dos dados em memória e em arquivos sem dependência de bibliotecas de terceiros.

---

## 🛠 Tecnologias e Dependências

### Backend
* **C++** (C++17 ou superior)
* **Dependências:** Nenhuma biblioteca externa. Utiliza apenas a biblioteca padrão do C++ e as APIs nativas do sistema operacional para soquetes/rede.

### Frontend
* **HTML5 / CSS3 / JavaScript (Vanilla)**
* Executado diretamente no navegador através do servidor local.

---

## Como Rodar o Projeto

Os scripts de execução automatizam a compilação do código e inicialização do servidor.

### Linux / macOS

No terminal, navegue até a raiz do projeto e execute:

```bash
chmod +x compilar_servidor.sh
./compilar_servidor.sh
./servidor_web

Windows

No Prompt de Comando ou PowerShell, navegue até a raiz do projeto e execute:

compilar_servidor.bat

Estrutura de Arquivos

.
├── data/                 # Arquivos de dados e armazenamento
├── frontend/             # Arquivos da interface (HTML, CSS e JS)
├── include/              # Arquivos de cabeçalho (.h / .hpp)
├── servidor/             # Implementação do servidor web C++ (servidor.cpp)
├── src/                  # Implementação da lógica de negócio em C++ puro
├── compilar_servidor.sh  # Script de compilação e execução (Linux/macOS)
├── compilar_servidor.bat # Script de compilação e execução (Windows)
└── README.md             # Documentação do projeto