@echo off
REM Compila o servidor web (backend C++ + ponte HTTP). Rode na raiz do projeto.
g++ -std=c++11 -Wall servidor/ServidorWeb.cpp src/Reserva.cpp src/Sala.cpp src/SalaTeorica.cpp src/Laboratorio.cpp src/SistemaAlocacao.cpp src/RepositorioCsv.cpp -o servidor_web.exe -lws2_32
if %errorlevel%==0 echo Compilado com sucesso! Para iniciar: servidor_web.exe
pause
