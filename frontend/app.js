// Frontend: so interface. Regras de negocio (conflitos, validacoes, gravacao nos CSV)
// ficam no backend C++; aqui apenas enviamos formularios e apresentamos os dados.
let salas = [], salaAtual = null, editSala = null, editRes = null, filtroTipo = "", ordem = "codigo";

const $ = s => document.querySelector(s);
const $$ = s => document.querySelectorAll(s);
const esc = t => String(t).replace(/[&<>"']/g, c => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c]));
const p2 = n => String(n).padStart(2, "0");
const agora = new Date();
const HOJE = `${p2(agora.getDate())}/${p2(agora.getMonth() + 1)}/${agora.getFullYear()}`;
const chave = d => d.split("/").reverse().join("");
const cmp = (a, b) => (chave(a.dia) + a.inicio).localeCompare(chave(b.dia) + b.inicio);
const DIAS = ["domingo", "segunda-feira", "terça-feira", "quarta-feira", "quinta-feira", "sexta-feira", "sábado"];
const nomeDia = d => { const [dd, mm, aa] = d.split("/"); const n = DIAS[new Date(aa, mm - 1, dd).getDay()]; return n[0].toUpperCase() + n.slice(1); };

async function api(metodo, url, dados) {
  const op = { method: metodo, headers: { "X-Frontend": "1" } };
  if (dados) op.body = new URLSearchParams(dados);
  let resp;
  try { resp = await fetch(url, op); }
  catch { status(false); throw new Error("Sem conexão com o servidor C++. Ele está rodando?"); }
  const corpo = await resp.json().catch(() => ({}));
  if (!resp.ok) throw new Error(corpo.erro || "Erro " + resp.status);
  return corpo;
}
function status(ok) { const s = $("#status"); s.textContent = ok ? "Backend C++ conectado" : "Servidor C++ offline"; s.className = "pill " + (ok ? "ok" : "off"); }
function toast(texto, erro) {
  const t = document.createElement("div"); t.className = "toast" + (erro ? " erro" : ""); t.textContent = texto;
  $("#toasts").append(t); setTimeout(() => t.remove(), 4500);
}
async function carregar() {
  try { salas = await api("GET", "/api/salas"); status(true); }
  catch (e) { salas = []; toast(e.message, true); }
  render();
}

// ---------- renderizacao ----------
function render() { renderNumeros(); renderSalas(); renderAgenda(); }

function renderNumeros() {
  const reservasHoje = salas.reduce((n, s) => n + s.reservas.filter(r => r.dia === HOJE).length, 0);
  const itens = [[salas.length, "salas cadastradas"], [salas.filter(s => s.tipo === "L").length, "laboratórios"],
    [salas.reduce((n, s) => n + s.capacidade, 0), "lugares no total"], [reservasHoje, "reservas para hoje"]];
  $("#numeros").innerHTML = itens.map(([n, t]) => `<div class="num"><b>${n}</b><span>${t}</span></div>`).join("");
}

function renderSalas() {
  const q = $("#busca").value.trim().toLowerCase();
  let lista = salas.filter(s => (!filtroTipo || s.tipo === filtroTipo) && (!q || s.codigo.toLowerCase().includes(q)));
  if (ordem === "cap") lista = [...lista].sort((a, b) => b.capacidade - a.capacidade);
  if (!lista.length) {
    $("#salas").innerHTML = `<p class="nada">${salas.length ? "Nenhuma sala encontrada com esse filtro." : "Nenhuma sala cadastrada ainda. Clique em “Adicionar sala” para começar."}</p>`;
    return;
  }
  $("#salas").innerHTML = lista.map(s => {
    const hoje = s.reservas.filter(r => r.dia === HOJE).sort(cmp);
    const futuras = s.reservas.filter(r => chave(r.dia) >= chave(HOJE)).sort(cmp);
    const antigas = s.reservas.length - futuras.length;
    const st = hoje.length ? `<span class="pill busy">Ocupada hoje · ${esc(hoje[0].inicio)}–${esc(hoje[0].fim)}</span>` : `<span class="pill free">Livre hoje</span>`;
    const det = s.tipo === "T" ? `${s.capacidade} alunos · Projetor: ${s.projetor ? "sim" : "não"}` : `${s.capacidade} alunos · ${s.computadores} computadores`;
    const itens = futuras.slice(0, 3).map(r => `<li><time>${esc(r.dia)} · ${esc(r.inicio)}–${esc(r.fim)}</time><br>` +
      (r.id ? `Alugada por ${esc(r.nome)} &lt;${esc(r.id)}&gt;` : "Responsável não informado") + ` <button class="link" data-act="editres" data-cod="${esc(s.codigo)}" data-i="${r.indice}">editar</button> <button class="link" data-act="cancelres" data-cod="${esc(s.codigo)}" data-i="${r.indice}">cancelar</button></li>`).join("");
    const mais = futuras.length > 3 ? `<button class="link" data-act="agenda">+ ${futuras.length - 3} na Agenda</button>` : "";
    const vazio = !futuras.length ? `<p class="vazio">Sem reservas futuras${antigas ? ` (${antigas} anterior${antigas > 1 ? "es" : ""})` : ""}.</p>` : "";
    return `<article class="sala ${s.tipo}">
      <div class="placa"><span class="cod">${esc(s.codigo)}</span><div><div class="tipo">${s.tipo === "T" ? "Sala teórica" : "Laboratório de " + esc(s.tipoLab)}</div><div class="det">${det}</div></div></div>
      <div class="corpo">${st}<h3>Próximas reservas</h3>${itens ? `<ul class="res">${itens}</ul>` : ""}${vazio}${mais}
      <div class="rodape"><button data-act="reservar" data-cod="${esc(s.codigo)}">Reservar</button>
      <button class="sec" data-act="editar" data-cod="${esc(s.codigo)}">Editar</button>
      <button class="sec" data-act="remover" data-cod="${esc(s.codigo)}">Remover</button></div></div></article>`;
  }).join("");
}

function renderAgenda() {
  const todas = [];
  salas.forEach(s => s.reservas.forEach(r => todas.push({ ...r, sala: s })));
  const lista = todas.filter(r => $("#passadas").checked || chave(r.dia) >= chave(HOJE)).sort(cmp);
  if (!lista.length) { $("#agenda").innerHTML = `<p class="nada">Nenhuma reserva ${$("#passadas").checked ? "cadastrada" : "a partir de hoje"}.</p>`; return; }
  const dias = [...new Set(lista.map(r => r.dia))];
  $("#agenda").innerHTML = dias.map(d => `<div class="dia"><h3>${nomeDia(d)}, ${esc(d)}${d === HOJE ? '<span class="pill">Hoje</span>' : ""}</h3>` +
    lista.filter(r => r.dia === d).map(r => `<div class="linha-ag ${r.sala.tipo}"><time>${esc(r.inicio)}–${esc(r.fim)}</time><b>${esc(r.sala.codigo)}</b>` +
      `<span>${r.id ? `${esc(r.nome)} &lt;${esc(r.id)}&gt;` : "Responsável não informado"}</span><button class="link" data-act="editres" data-cod="${esc(r.sala.codigo)}" data-i="${r.indice}">editar</button><button class="link" data-act="cancelres" data-cod="${esc(r.sala.codigo)}" data-i="${r.indice}">cancelar</button></div>`).join("") + `</div>`).join("");
}

// ---------- navegacao e filtros ----------
function aba(nome) {
  $$(".abas button").forEach(b => b.classList.toggle("ativa", b.dataset.aba === nome));
  ["salas", "agenda", "ajuda"].forEach(a => $("#aba-" + a).hidden = a !== nome);
}
$$(".abas button").forEach(b => b.addEventListener("click", () => aba(b.dataset.aba)));
$("#busca").addEventListener("input", renderSalas);
$("#ordem").addEventListener("change", e => { ordem = e.target.value; renderSalas(); });
$("#passadas").addEventListener("change", renderAgenda);
$("#chips").addEventListener("click", e => {
  const c = e.target.closest(".chip"); if (!c) return;
  filtroTipo = c.dataset.tipo; $$("#chips .chip").forEach(x => x.classList.toggle("on", x === c)); renderSalas();
});
$$("[data-fechar]").forEach(b => b.addEventListener("click", () => b.closest("dialog").close()));

// ---------- acoes nos cartoes ----------
$("#salas").addEventListener("click", async e => {
  const b = e.target.closest("button[data-act]"); if (!b) return;
  if (b.dataset.act === "agenda") return aba("agenda");
  const sala = salas.find(s => s.codigo === b.dataset.cod); if (!sala) return;
  if (b.dataset.act === "reservar") {
    salaAtual = sala.codigo; editRes = null; $("#btnSalvarReserva").textContent = "Reservar"; $("#formReserva").reset(); $("#erroReserva").textContent = "";
    $("#rDia").value = `${agora.getFullYear()}-${p2(agora.getMonth() + 1)}-${p2(agora.getDate())}`;
    $("#tituloReserva").textContent = `Reservar sala ${sala.codigo}`;
    $("#resumoReserva").textContent = sala.tipo === "T" ? `Sala teórica · ${sala.capacidade} alunos` : `Laboratório de ${sala.tipoLab} · ${sala.computadores} computadores`;
    $("#dlgReserva").showModal();
  } else if (b.dataset.act === "editar") {
    abrirEdicaoSala(sala);
  } else if (b.dataset.act === "remover" && confirm(`Remover a sala ${sala.codigo} e todas as suas reservas?`)) {
    try { await api("DELETE", "/api/salas/" + encodeURIComponent(sala.codigo)); await carregar(); toast(`Sala ${sala.codigo} removida com sucesso!`); }
    catch (err) { toast(err.message, true); }
  }
});

// ---------- adicionar sala ----------
function trocaTipo() { const t = $("#sTipo").value === "T"; $("#boxProj").hidden = !t; $("#boxLab").hidden = t; }
function modoSala(edicao) {   // alterna o mesmo dialogo entre "adicionar" e "editar"
  $("#sTipo").disabled = edicao; $("#sCodigo").readOnly = edicao;
  $("#btnSalvarSala").textContent = edicao ? "Salvar alterações" : "Salvar sala";
  [...$("#sTipoLab").options].forEach(o => { if (!["Hardware", "Software"].includes(o.value)) o.remove(); });
}
$("#btnNovaSala").addEventListener("click", () => {
  editSala = null; $("#formSala").reset(); modoSala(false); trocaTipo();
  $("#tituloSala").textContent = "Adicionar sala"; $("#erroSala").textContent = ""; $("#dlgSala").showModal();
});
function abrirEdicaoSala(s) {
  editSala = s.codigo; $("#formSala").reset(); modoSala(true);
  $("#sTipo").value = s.tipo; trocaTipo(); $("#sCodigo").value = s.codigo; $("#sCap").value = s.capacidade;
  if (s.tipo === "T") $("#sProj").checked = s.projetor;
  else {
    if (![...$("#sTipoLab").options].some(o => o.value === s.tipoLab)) $("#sTipoLab").append(new Option(s.tipoLab));
    $("#sTipoLab").value = s.tipoLab; $("#sQtd").value = s.computadores;
  }
  $("#tituloSala").textContent = `Editar sala ${s.codigo}`; $("#erroSala").textContent = ""; $("#dlgSala").showModal();
}
$("#sTipo").addEventListener("change", trocaTipo);
$("#formSala").addEventListener("submit", async e => {
  e.preventDefault();
  const dados = { tipo: $("#sTipo").value, codigo: $("#sCodigo").value, capacidade: $("#sCap").value,
    projetor: $("#sProj").checked ? "1" : "0", tipoLab: $("#sTipoLab").value, computadores: $("#sQtd").value };
  try {
    const nova = editSala ? await api("PUT", "/api/salas/" + encodeURIComponent(editSala), dados) : await api("POST", "/api/salas", dados);
    $("#dlgSala").close(); await carregar(); toast(`Sala ${nova.codigo} ${editSala ? "atualizada" : "adicionada"} com sucesso!`);
  }
  catch (err) { $("#erroSala").textContent = err.message; }
});

// ---------- editar reserva ----------
function abrirEdicaoReserva(cod, i) {
  const s = salas.find(x => x.codigo === cod), r = s && s.reservas.find(x => x.indice === i); if (!r) return;
  salaAtual = cod; editRes = i; $("#formReserva").reset(); $("#erroReserva").textContent = "";
  $("#rNome").value = r.nome; $("#rId").value = r.id; $("#rDia").value = r.dia.split("/").reverse().join("-");
  $("#rIni").value = r.inicio; $("#rFim").value = r.fim;
  $("#tituloReserva").textContent = `Editar reserva da sala ${cod}`;
  $("#resumoReserva").textContent = `Reserva atual: ${r.dia} · ${r.inicio}–${r.fim}`;
  $("#btnSalvarReserva").textContent = "Salvar alterações"; $("#dlgReserva").showModal();
}
document.addEventListener("click", e => { const b = e.target.closest("button[data-act=editres]"); if (b) abrirEdicaoReserva(b.dataset.cod, +b.dataset.i); });

// ---------- cancelar reserva ----------
document.addEventListener("click", async e => {
  const b = e.target.closest("button[data-act=cancelres]"); if (!b) return;
  const s = salas.find(x => x.codigo === b.dataset.cod), r = s && s.reservas.find(x => x.indice === +b.dataset.i); if (!r) return;
  if (!confirm(`Cancelar a reserva da sala ${s.codigo} em ${r.dia}, ${r.inicio}–${r.fim}?`)) return;
  try { await api("DELETE", `/api/salas/${encodeURIComponent(s.codigo)}/reservas/${r.indice}`); await carregar(); toast(`Reserva da sala ${s.codigo} cancelada.`); }
  catch (err) { toast(err.message, true); }
});

// ---------- reservar ----------
$$(".atalhos .chip").forEach(c => c.addEventListener("click", () => { $("#rIni").value = c.dataset.ini; $("#rFim").value = c.dataset.fim; }));
$("#formReserva").addEventListener("submit", async e => {
  e.preventDefault();
  const dia = $("#rDia").value.split("-").reverse().join("/");   // yyyy-mm-dd -> dd/mm/aaaa
  const dados = { codigo: salaAtual, dia, inicio: $("#rIni").value, fim: $("#rFim").value, nome: $("#rNome").value, id: $("#rId").value };
  try {
    const edit = editRes;
    if (edit !== null) await api("PUT", `/api/salas/${encodeURIComponent(salaAtual)}/reservas/${edit}`, dados);
    else await api("POST", "/api/reservas", dados);
    $("#dlgReserva").close(); await carregar();
    toast(edit !== null ? `Reserva da sala ${salaAtual} atualizada: ${dia}, ${dados.inicio}–${dados.fim}.`
                        : `Sala ${salaAtual} alugada por ${dados.nome.trim()} <${dados.id.trim()}> em ${dia}.`);
  } catch (err) { $("#erroReserva").textContent = err.message; }
});

// ---------- tema ----------
function tema(t) { document.documentElement.dataset.theme = t; try { localStorage.setItem("tema", t); } catch {} }
let salvo = null; try { salvo = localStorage.getItem("tema"); } catch {}
tema(salvo || (matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light"));
$("#tema").addEventListener("click", () => tema(document.documentElement.dataset.theme === "dark" ? "light" : "dark"));

$("#hoje").textContent = `${nomeDia(HOJE)}, ${HOJE}`;
carregar();