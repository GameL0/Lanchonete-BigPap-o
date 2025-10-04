@echo off
echo Tentando compilar o projeto Big Papao...

REM Tentar diferentes compiladores
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe" (
    echo Usando MSVC...
    "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe" main.c funcoes.c /Fe:main.exe
    goto :end
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe" (
    echo Usando MSVC 2019...
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe" main.c funcoes.c /Fe:main.exe
    goto :end
)

where gcc >nul 2>&1
if %errorlevel% == 0 (
    echo Usando GCC...
    gcc -o main.exe main.c funcoes.c -Wall -Wextra
    goto :end
)

where clang >nul 2>&1
if %errorlevel% == 0 (
    echo Usando Clang...
    clang -o main.exe main.c funcoes.c -Wall -Wextra
    goto :end
)

where tcc >nul 2>&1
if %errorlevel% == 0 (
    echo Usando TCC...
    tcc -o main.exe main.c funcoes.c
    goto :end
)

echo Nenhum compilador C encontrado!
echo Por favor, instale um dos seguintes:
echo - Microsoft Visual Studio (com C++)
echo - MinGW-w64
echo - Clang
echo - TCC (Tiny C Compiler)
echo.
echo Ou use um compilador online como:
echo - https://onlinegdb.com/
echo - https://replit.com/
echo - https://www.programiz.com/c-programming/online-compiler/

:end
if exist main.exe (
    echo.
    echo Compilacao bem-sucedida! Executavel criado: main.exe
    echo Para executar: main.exe
) else (
    echo.
    echo Falha na compilacao.
)
pause

