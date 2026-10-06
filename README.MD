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
* **C++** (compilado com `g++`)
* **Dependências:** Nenhuma biblioteca externa. Utiliza apenas a biblioteca padrão do C++ e as APIs nativas do sistema operacional para soquetes/rede (Winsock no Windows).

### Frontend
* **HTML5 / CSS3 / JavaScript (Vanilla)**
* Executado diretamente no navegador através do servidor local.

---

## Como Rodar o Projeto

Existem duas formas de usar o sistema:

1. **Servidor web (recomendado):** interface gráfica no navegador.
2. **Menu de terminal:** interface em texto, direto no console.

> ⚠️ **Importante:** execute **todos os comandos na raiz do projeto**, ou seja, a pasta que contém
> `src/`, `include/`, `data/`, `frontend/` e `servidor/`. Se o programa for executado de outra pasta,
> ele não encontra os arquivos da interface (`frontend/`) nem os dados (`data/`).

---

### Windows 10/11 (passo a passo)

#### 1. Instalar o compilador g++ (MinGW-w64)

O script de compilação usa o `g++`. Se você ainda não o tem:

1. Baixe e instale o **MSYS2** em <https://www.msys2.org>.
2. Abra o terminal **"MSYS2 UCRT64"** (menu Iniciar) e execute:
   ```
   pacman -S mingw-w64-ucrt-x86_64-gcc
   ```
3. Adicione `C:\msys64\ucrt64\bin` ao **PATH** do Windows
   (Painel de Controle → Sistema → Configurações avançadas → Variáveis de Ambiente → `Path` → Novo).
4. **Feche e abra um novo terminal** (ou reinicie o VS Code) e confira:
   ```
   g++ --version
   ```
   Se aparecer a versão do compilador, está pronto.

#### 2. Extrair o projeto

Extraia o `.rar`/`.zip` com o 7-Zip ou WinRAR (botão direito → *Extrair aqui*). **Não execute de dentro do arquivo compactado.**

Atenção: dependendo de como for extraído, pode ficar **uma pasta dentro da outra**
(por exemplo, `Projeto EDOO\projeto-de-EDOO-estrutura-de-dados-orientado-a-objetos-do-CIn-UFPE\`).
A raiz correta é a pasta **interna**, a que contém `src`, `data`, `frontend`, `include` e `servidor`.

#### 3. Abrir um terminal na raiz do projeto

No Prompt de Comando, PowerShell ou no terminal do VS Code, vá até a raiz:

```
cd "caminho\para\projeto-de-EDOO-estrutura-de-dados-orientado-a-objetos-do-CIn-UFPE"
dir
```

O `dir` deve listar as pastas `data`, `frontend`, `include`, `servidor` e `src`.
No VS Code, o jeito mais simples é abrir direto essa pasta (*Arquivo → Abrir Pasta*), assim o terminal já inicia na raiz.

#### 4. Compilar o servidor (Opção A: script `.bat`)

```
.\compilar_servidor.bat
```

(no Prompt de Comando, pode ser apenas `compilar_servidor.bat`; no PowerShell, o prefixo `.\` é obrigatório)

Saída esperada:

```
Compilado com sucesso! Para iniciar: servidor_web.exe
Pressione qualquer tecla para continuar. . .
```

Aperte qualquer tecla para continuar. Foi gerado o arquivo `servidor_web.exe` na raiz do projeto.

> O script apenas **compila**. Ele não inicia o servidor — isso é feito no próximo passo.

#### 4-B. Compilar manualmente (Opção B: se o `.bat` não funcionar)

Se o `.bat` não rodar no seu terminal, execute diretamente o mesmo comando que ele usa (tudo em uma única linha, na raiz do projeto):

```
g++ -std=c++11 -Wall servidor/ServidorWeb.cpp src/Reserva.cpp src/Sala.cpp src/SalaTeorica.cpp src/Laboratorio.cpp src/SistemaAlocacao.cpp src/RepositorioCsv.cpp -o servidor_web.exe -lws2_32
```

O resultado é o mesmo `servidor_web.exe`. Se não aparecer nenhuma mensagem de erro, a compilação deu certo.

#### 5. Iniciar o servidor

```
.\servidor_web.exe
```

Saída esperada:

```
0 sala(s) carregada(s) do arquivo.
0 reserva(s) carregada(s) do arquivo.

Servidor no ar! Abra no navegador: http://localhost:8080
Para encerrar: Ctrl+C
```

> **"0 sala(s)" é normal na primeira execução.** Os arquivos `data/salas.csv` e `data/reservas.csv`
> começam apenas com linhas de comentário, então o sistema inicia vazio. As salas e reservas são
> cadastradas pela própria interface.

Com o servidor no ar, abra no navegador:

**<http://localhost:8080>**

Mantenha o terminal aberto enquanto usar o sistema. Para encerrar o servidor, pressione `Ctrl+C`.

Para usar outra porta (caso a 8080 esteja ocupada):

```
.\servidor_web.exe 8081
```

e acesse `http://localhost:8081`.

> Se o Firewall do Windows perguntar sobre acesso à rede, pode permitir em redes privadas.

---

### Menu de terminal (Windows)

Para usar a versão em console, compile todos os arquivos de `src/` **listando-os explicitamente**:

```
g++ -std=c++17 -Wall src/Main.cpp src/Menu.cpp src/Reserva.cpp src/Sala.cpp src/SalaTeorica.cpp src/Laboratorio.cpp src/SistemaAlocacao.cpp src/RepositorioCsv.cpp -o menu.exe
```

e execute:

```
menu.exe
```

(no PowerShell: `.\menu.exe`)

> No **PowerShell**, não use `src/*.cpp`: ele não expande o `*` para o `g++` e dá o erro
> `src/*.cpp: No such file or directory`. Liste os arquivos como acima.
> No Prompt de Comando (`cmd`) o `*` funciona normalmente.

---

### Linux / macOS

No terminal, navegue até a raiz do projeto e execute:

```bash
chmod +x compilar_servidor.sh
./compilar_servidor.sh
./servidor_web
```

Depois acesse <http://localhost:8080>.

> O servidor foi desenvolvido e testado para **Windows** (usa Winsock). O script para Linux/macOS está disponível,
> mas a compatibilidade do código do servidor com soquetes POSIX não foi verificada.

---

### Problemas comuns

| Mensagem / sintoma | Causa e solução |
|---|---|
| `g++` não é reconhecido como comando | Compilador não instalado ou fora do PATH. Refaça o passo 1 e **abra um novo terminal**. |
| `src/Main.cpp: No such file or directory` | O terminal não está na raiz do projeto (veja se há pasta dentro de pasta). Use `cd` até a pasta correta e confirme com `dir`. |
| `src/*.cpp: No such file or directory` | Você está no PowerShell. Liste os arquivos um a um (veja "Menu de terminal"). |
| `compilar_servidor.bat` não é reconhecido (PowerShell) | Use `.\compilar_servidor.bat`. |
| "Arquivo frontend/... não encontrado" | O servidor foi iniciado fora da raiz do projeto. Rode `servidor_web.exe` da pasta que contém `frontend/`. |
| "não foi possível usar a porta 8080" | A porta está em uso. Rode `servidor_web.exe 8081` e acesse `http://localhost:8081`. |
| O navegador não abre a página | Confirme que o terminal mostra "Servidor no ar!" e que o endereço é `http://localhost:8080` (não `https`). |

---

## Estrutura de Arquivos

```
.
├── data/                  # Arquivos de dados (salas.csv e reservas.csv)
├── frontend/              # Arquivos da interface (HTML, CSS e JS)
├── include/               # Arquivos de cabeçalho (.h)
├── servidor/              # Servidor web C++ (ServidorWeb.cpp)
├── src/                   # Implementação da lógica de negócio em C++ puro
├── compilar_servidor.sh   # Script de compilação (Linux/macOS)
├── compilar_servidor.bat  # Script de compilação (Windows)
└── README.md              # Documentação do projeto
```
