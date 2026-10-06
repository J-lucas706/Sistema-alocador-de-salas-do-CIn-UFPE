// Ponte HTTP local entre o frontend (HTML/CSS/JS) e o backend C++.
// Nao altera nenhuma classe do projeto: apenas reutiliza SistemaAlocacao,
// RepositorioCsv, Sala, Reserva etc. Toda regra de negocio continua no C++.
// Escuta somente em 127.0.0.1 (nao fica exposto na rede).
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include "../include/SistemaAlocacao.h"
#include "../include/RepositorioCsv.h"
#include "../include/SalaTeorica.h"
#include "../include/Laboratorio.h"
#include "../include/Reserva.h"

#ifdef _WIN32
  #include <winsock2.h>
  typedef SOCKET sock_t;
  #define FECHAR closesocket
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <unistd.h>
  typedef int sock_t;
  #define FECHAR close
  #define INVALID_SOCKET (-1)
#endif

struct Resp { int status; string tipo; string corpo; };

// ---------- utilidades ----------
static string jsonStr(const string& s) {
    string o = "\"";
    for (unsigned char c : s) {
        switch (c) {
            case '"':  o += "\\\""; break;
            case '\\': o += "\\\\"; break;
            case '\n': o += "\\n";  break;
            case '\r': o += "\\r";  break;
            case '\t': o += "\\t";  break;
            default:
                if (c < 0x20) { char b[8]; snprintf(b, sizeof b, "\\u%04x", c); o += b; }
                else o += (char)c;
        }
    }
    return o + "\"";
}

static string urlDecode(const string& s) {
    string o;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '+') o += ' ';
        else if (s[i] == '%' && i + 2 < s.size() && isxdigit((unsigned char)s[i + 1]) && isxdigit((unsigned char)s[i + 2])) {
            o += (char)stoi(s.substr(i + 1, 2), nullptr, 16);
            i += 2;
        } else o += s[i];
    }
    return o;
}

static map<string, string> parseForm(const string& corpo) {
    map<string, string> m;
    stringstream ss(corpo);
    string par;
    while (getline(ss, par, '&')) {
        size_t p = par.find('=');
        if (p == string::npos) continue;
        m[urlDecode(par.substr(0, p))] = urlDecode(par.substr(p + 1));
    }
    return m;
}

static string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n"), b = s.find_last_not_of(" \t\r\n");
    return a == string::npos ? "" : s.substr(a, b - a + 1);
}

static bool lerInteiro(const string& t, int& v) {
    try { size_t pos; v = stoi(t, &pos); return pos == t.size(); } catch (...) { return false; }
}

static bool diaValido(const string& d) {  // dd/mm/aaaa
    if (d.size() != 10 || d[2] != '/' || d[5] != '/') return false;
    for (int i : {0, 1, 3, 4, 6, 7, 8, 9}) if (!isdigit((unsigned char)d[i])) return false;
    int dd = stoi(d.substr(0, 2)), mm = stoi(d.substr(3, 2));
    return dd >= 1 && dd <= 31 && mm >= 1 && mm <= 12;
}

static Resp erro(int st, const string& msg) { return {st, "application/json; charset=utf-8", "{\"erro\":" + jsonStr(msg) + "}"}; }
static Resp json(int st, const string& corpo) { return {st, "application/json; charset=utf-8", corpo}; }

static string salaJson(const Sala* s) {
    string j = "{\"codigo\":" + jsonStr(s->getCodigo()) + ",\"capacidade\":" + to_string(s->getCapacidade());
    if (auto t = dynamic_cast<const SalaTeorica*>(s))
        j += string(",\"tipo\":\"T\",\"projetor\":") + (t->getTemProjetor() ? "true" : "false");
    else if (auto l = dynamic_cast<const Laboratorio*>(s))
        j += ",\"tipo\":\"L\",\"tipoLab\":" + jsonStr(l->getTipoLab()) + ",\"computadores\":" + to_string(l->getQtdComputadores());
    j += ",\"reservas\":[";
    bool primeiro = true;
    int idx = 0;
    for (const auto& r : s->getReservas()) {
        if (!primeiro) j += ",";
        primeiro = false;
        j += "{\"indice\":" + to_string(idx++) + ",\"dia\":" + jsonStr(r.getDia()) + ",\"inicio\":" + jsonStr(r.getHoraInicio()) +
             ",\"fim\":" + jsonStr(r.getHoraFim()) + ",\"nome\":" + jsonStr(r.getResponsavelNome()) +
             ",\"id\":" + jsonStr(r.getResponsavelId()) + "}";
    }
    return j + "]}";
}

// Retorna "" se os dados da reserva sao validos, senao a mensagem de erro
static string validarReserva(const string& nome, const string& id, const string& dia, const string& hi, const string& hf) {
    if (nome.empty()) return "O nome nao pode ficar vazio.";
    if (!Reserva::idValido(id)) return "ID invalido: use apenas letras e numeros (2 a 10 caracteres).";
    if (!diaValido(dia)) return "Dia invalido (use dd/mm/aaaa).";
    if (!Reserva::intervaloValido(hi, hf)) return "Horario invalido! Use HH:MM e o inicio deve ser antes do fim.";
    return "";
}

// Mensagem de conflito citando quem ja esta com a sala ('ignorar' = indice da propria reserva na edicao)
static string msgConflito(const Sala* sala, const string& codigo, const string& dia, const string& hi, const string& hf, int ignorar) {
    string msg = "A sala " + codigo + " ja possui reserva em " + dia + " nesse intervalo";
    int i = 0;
    for (const auto& r : sala->getReservas()) {
        if (i++ == ignorar || !r.temConflito(dia, hi, hf)) continue;
        msg += " (" + r.getHoraInicio() + " as " + r.getHoraFim();
        if (!r.getResponsavelId().empty()) msg += ", " + r.getResponsavelNome() + " <" + r.getResponsavelId() + ">";
        msg += ")";
        break;
    }
    return msg + ".";
}

static Resp arquivoEstatico(const string& nome, const string& tipo) {
    ifstream f("frontend/" + nome, ios::binary);
    if (!f.is_open()) return {404, "text/plain; charset=utf-8", "Arquivo frontend/" + nome + " nao encontrado. Rode o servidor na raiz do projeto."};
    stringstream ss; ss << f.rdbuf();
    return {200, tipo, ss.str()};
}

// ---------- rotas ----------
static Resp tratar(const string& metodo, const string& caminho, const string& corpo,
                   bool origemFrontend, SistemaAlocacao& sis, RepositorioCsv& repo) {
    if (metodo == "GET") {
        if (caminho == "/" || caminho == "/index.html") return arquivoEstatico("index.html", "text/html; charset=utf-8");
        if (caminho == "/style.css") return arquivoEstatico("style.css", "text/css; charset=utf-8");
        if (caminho == "/app.js")    return arquivoEstatico("app.js", "text/javascript; charset=utf-8");
        if (caminho == "/api/salas") {
            string j = "[";
            bool primeiro = true;
            for (const Sala* s : sis.getSalasOrdenadas()) {
                if (!primeiro) j += ",";
                primeiro = false;
                j += salaJson(s);
            }
            return json(200, j + "]");
        }
        return erro(404, "Rota nao encontrada.");
    }

    // Protecao: so aceita alteracoes vindas do nosso frontend (o cabecalho custom
    // impede que outro site aberto no navegador mande requisicoes para ca).
    if (!origemFrontend) return erro(403, "Requisicao nao autorizada.");

    if (metodo == "POST" && caminho == "/api/salas") {
        auto f = parseForm(corpo);
        string tipo = f["tipo"], codigo = trim(f["codigo"]);
        if (codigo.empty() || codigo.find_first_of(", \t\r\n") != string::npos)
            return erro(400, "Codigo invalido: nao use espacos nem virgulas.");
        int cap;
        if (!lerInteiro(f["capacidade"], cap) || cap <= 0) return erro(400, "Capacidade invalida.");

        Sala* nova = nullptr;
        if (tipo == "T") {
            nova = new SalaTeorica(codigo, cap, f["projetor"] == "1");
        } else if (tipo == "L") {
            string tl = trim(Reserva::limparCampoCsv(f["tipoLab"]));
            int qtd;
            if (tl.empty()) return erro(400, "Informe o tipo do laboratorio.");
            if (!lerInteiro(f["computadores"], qtd) || qtd < 0) return erro(400, "Quantidade de computadores invalida.");
            nova = new Laboratorio(codigo, cap, tl, qtd);
        } else {
            return erro(400, "Tipo de sala invalido.");
        }
        if (!sis.adicionarSala(nova)) return erro(409, "A sala " + codigo + " ja existe no sistema.");
        repo.salvarSalas(sis);
        return json(201, salaJson(sis.buscarSala(codigo)));
    }

    if (metodo == "DELETE" && caminho.rfind("/api/salas/", 0) == 0) {
        string resto = caminho.substr(11);
        size_t p = resto.find("/reservas/");
        if (p != string::npos) {  // DELETE /api/salas/{codigo}/reservas/{indice}  -> cancelar reserva
            string codigoSala = urlDecode(resto.substr(0, p));
            Sala* salaReserva = sis.buscarSala(codigoSala);
            if (salaReserva == nullptr) return erro(404, "Sala " + codigoSala + " nao encontrada.");
            int indice;
            if (!lerInteiro(resto.substr(p + 10), indice) || indice < 0 || indice >= (int)salaReserva->getReservas().size())
                return erro(404, "Reserva nao encontrada.");
            sis.removerReserva(codigoSala, (size_t)indice);
            repo.salvarReservas(sis);
            return json(200, salaJson(salaReserva));
        }
        string codigo = urlDecode(resto);
        if (!sis.removerSala(codigo)) return erro(404, "Sala " + codigo + " nao encontrada.");
        repo.salvarSalas(sis);
        repo.salvarReservas(sis);  // a sala removida tinha reservas
        return json(200, "{\"ok\":true}");
    }

    if (metodo == "PUT" && caminho.rfind("/api/salas/", 0) == 0) {
        string resto = caminho.substr(11);
        size_t p = resto.find("/reservas/");
        auto f = parseForm(corpo);
        if (p == string::npos) {  // PUT /api/salas/{codigo}  -> editar sala
            string codigo = urlDecode(resto);
            Sala* sala = sis.buscarSala(codigo);
            if (sala == nullptr) return erro(404, "Sala " + codigo + " nao encontrada.");
            int cap, qtd = 0;
            if (!lerInteiro(f["capacidade"], cap) || cap <= 0) return erro(400, "Capacidade invalida.");
            string tl;
            if (dynamic_cast<Laboratorio*>(sala)) {
                tl = trim(Reserva::limparCampoCsv(f["tipoLab"]));
                if (tl.empty()) return erro(400, "Informe o tipo do laboratorio.");
                if (!lerInteiro(f["computadores"], qtd) || qtd < 0) return erro(400, "Quantidade de computadores invalida.");
            }
            if (!sis.atualizarSala(codigo, cap, f["projetor"] == "1", tl, qtd)) return erro(400, "Dados invalidos.");
            repo.salvarSalas(sis);
            return json(200, salaJson(sala));
        }
        // PUT /api/salas/{codigo}/reservas/{indice}  -> editar reserva
        string codigo = urlDecode(resto.substr(0, p));
        Sala* sala = sis.buscarSala(codigo);
        if (sala == nullptr) return erro(404, "Sala " + codigo + " nao encontrada.");
        int indice;
        if (!lerInteiro(resto.substr(p + 10), indice) || indice < 0 || indice >= (int)sala->getReservas().size())
            return erro(404, "Reserva nao encontrada.");
        string dia = f["dia"], hi = f["inicio"], hf = f["fim"], nome = trim(f["nome"]), id = trim(f["id"]);
        string e = validarReserva(nome, id, dia, hi, hf);
        if (!e.empty()) return erro(400, e);
        if (!sis.atualizarReserva(codigo, (size_t)indice, dia, hi, hf, nome, id))
            return erro(409, msgConflito(sala, codigo, dia, hi, hf, indice));
        repo.salvarReservas(sis);
        return json(200, salaJson(sala));
    }

    if (metodo == "POST" && caminho == "/api/reservas") {
        auto f = parseForm(corpo);
        string codigo = f["codigo"], dia = f["dia"], hi = f["inicio"], hf = f["fim"];
        string nome = trim(f["nome"]), id = trim(f["id"]);
        Sala* sala = sis.buscarSala(codigo);
        if (sala == nullptr) return erro(404, "Sala '" + codigo + "' nao encontrada.");
        string e = validarReserva(nome, id, dia, hi, hf);
        if (!e.empty()) return erro(400, e);
        if (!sis.reservar(codigo, dia, hi, hf, nome, id)) {
            return erro(409, msgConflito(sala, codigo, dia, hi, hf, -1));
        }
        repo.salvarReservas(sis);
        return json(201, salaJson(sala));
    }

    return erro(404, "Rota nao encontrada.");
}

// ---------- HTTP minimo ----------
static string minusculas(string s) { for (char& c : s) c = (char)tolower((unsigned char)c); return s; }

// true quando o cabecalho e o corpo (Content-Length) ja chegaram por completo
static bool requisicaoCompleta(const string& buf) {
    size_t fim = buf.find("\r\n\r\n");
    if (fim == string::npos) return buf.size() > 65536;
    string cab = minusculas(buf.substr(0, fim));
    size_t p = cab.find("\r\ncontent-length:");
    size_t tam = 0;
    if (p != string::npos) tam = (size_t)atol(cab.c_str() + p + 17);
    return buf.size() >= fim + 4 + tam;
}

static string montarResposta(const Resp& r) {
    const char* motivo = r.status == 200 ? "OK" : r.status == 201 ? "Created" : r.status == 400 ? "Bad Request" :
                         r.status == 403 ? "Forbidden" : r.status == 404 ? "Not Found" : r.status == 409 ? "Conflict" : "Error";
    return "HTTP/1.1 " + to_string(r.status) + " " + motivo + "\r\nContent-Type: " + r.tipo +
           "\r\nContent-Length: " + to_string(r.corpo.size()) + "\r\nCache-Control: no-store\r\nConnection: close\r\n\r\n" + r.corpo;
}

static void responder(sock_t s, const string& buf, SistemaAlocacao& sis, RepositorioCsv& repo) {
    size_t fim = buf.find("\r\n\r\n");
    Resp r = erro(400, "Requisicao invalida.");
    if (fim != string::npos) {
        stringstream linha(buf.substr(0, buf.find("\r\n")));
        string metodo, caminho;
        linha >> metodo >> caminho;
        caminho = caminho.substr(0, caminho.find('?'));
        bool origem = minusculas(buf.substr(0, fim)).find("\r\nx-frontend:") != string::npos;
        r = tratar(metodo, caminho, buf.substr(fim + 4), origem, sis, repo);
    }
    string saida = montarResposta(r);
    size_t env = 0;
    while (env < saida.size()) {
        int n = (int)send(s, saida.data() + env, (int)(saida.size() - env), 0);
        if (n <= 0) break;
        env += (size_t)n;
    }
}

int main(int argc, char** argv) {
    int porta = argc > 1 ? atoi(argv[1]) : 8080;
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { cout << "Erro ao iniciar Winsock." << endl; return 1; }
#else
    signal(SIGPIPE, SIG_IGN);
#endif

    SistemaAlocacao sistema;
    RepositorioCsv repo("data/salas.csv", "data/reservas.csv");
    cout << repo.carregarSalas(sistema) << " sala(s) carregada(s) do arquivo." << endl;
    cout << repo.carregarReservas(sistema) << " reserva(s) carregada(s) do arquivo." << endl;

    sock_t srv = socket(AF_INET, SOCK_STREAM, 0);
#ifndef _WIN32
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof opt);
#endif
    sockaddr_in end{};
    end.sin_family = AF_INET;
    end.sin_port = htons((unsigned short)porta);
    end.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(srv, (sockaddr*)&end, sizeof end) != 0 || listen(srv, 16) != 0) {
        cout << "Erro: nao foi possivel usar a porta " << porta << " (ja esta em uso?). Tente: servidor <outra_porta>" << endl;
        return 1;
    }
    cout << "\nServidor no ar! Abra no navegador: http://localhost:" << porta << "\nPara encerrar: Ctrl+C" << endl;

    // Laco com select(): atende varios clientes sem threads (navegadores abrem conexoes ociosas)
    struct Cli { sock_t s; string buf; };
    vector<Cli> clis;
    while (true) {
        fd_set rd; FD_ZERO(&rd); FD_SET(srv, &rd);
        sock_t maior = srv;
        for (auto& c : clis) { FD_SET(c.s, &rd); if (c.s > maior) maior = c.s; }
        if (select((int)maior + 1, &rd, nullptr, nullptr, nullptr) < 0) continue;
        if (FD_ISSET(srv, &rd)) {
            sock_t n = accept(srv, nullptr, nullptr);
            if (n != INVALID_SOCKET) {
                if (clis.size() >= 100) { FECHAR(clis.front().s); clis.erase(clis.begin()); }
                clis.push_back({n, ""});
            }
        }
        for (size_t i = 0; i < clis.size();) {
            if (!FD_ISSET(clis[i].s, &rd)) { i++; continue; }
            char b[4096];
            int n = (int)recv(clis[i].s, b, sizeof b, 0);
            if (n <= 0) { FECHAR(clis[i].s); clis.erase(clis.begin() + i); continue; }
            clis[i].buf.append(b, (size_t)n);
            if (requisicaoCompleta(clis[i].buf)) {
                responder(clis[i].s, clis[i].buf, sistema, repo);
                FECHAR(clis[i].s);
                clis.erase(clis.begin() + i);
            } else i++;
        }
    }
}