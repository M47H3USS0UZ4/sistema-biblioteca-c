# 📚 Sistema de Biblioteca em C

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Terminal](https://img.shields.io/badge/Terminal-%234D4D4D.svg?style=for-the-badge&logo=windows-terminal&logoColor=white)

Este projeto implementa um sistema robusto e interativo de gerenciamento de biblioteca direto no console, construído inteiramente em C. O foco principal é a demonstração de conceitos fundamentais como alocação dinâmica de memória, manipulação de ponteiros, modularização e persistência de dados em arquivos binários.

## 📸 Demonstração

<img src="https://github.com/user-attachments/assets/9383a0ed-2e9f-45a0-b1eb-c2f5ae185ab1" width="300">

<img src="https://github.com/user-attachments/assets/cc1a224c-9b78-46d1-83f1-a3b5757183c1" width="400">

## 🚀 Funcionalidades

- **CRUD Completo:** Cadastrar, listar, editar e remover livros por código único.
- **Busca e Ordenação:** Pesquisar livros por código e ordenar o acervo por ano de publicação (usando o algoritmo `qsort` do C).
- **Persistência de Dados Segura:** Salvamento e carregamento automático em arquivo binário (`acervo.dat`).
- **Alocação Dinâmica:** O sistema não possui limite fixo de livros; a memória cresce dinamicamente conforme a necessidade.
- **Prevenção de Corrupção:** Verificação de integridade avançada (Magic Number e checagem de bytes órfãos com `fgetc` no carregamento) para evitar *Segmentation Faults* e corrupção de memória.
- **Tratamento de Entrada:** Blindagem contra entradas inválidas (ex: digitar letras em campos numéricos) com loops de validação e suporte a caracteres UTF-8 no console do Windows.

## 📂 Estrutura do Projeto

A arquitetura foi dividida para respeitar o princípio de responsabilidade única:

* `library_system.c`: Ponto de entrada (`main`) e orquestração do menu interativo.
* `library_catalog.h` / `.c`: Módulo contendo as regras de negócio, manipulação do acervo e persistência.
* `io_utils.h` / `.c`: Utilitários para entrada/saída limpa no console, limpeza de buffer e compatibilidade de codificação.
* `build.ps1`: Script de automação em PowerShell para facilitar a compilação e execução.

## 🛠️ Como Executar

### Pré-requisitos
* Compilador GCC instalado (MinGW no Windows).
* Terminal com suporte a PowerShell (opcional, para usar o script de build).

### Opção 1: Usando o Script de Build (Recomendado)
No terminal PowerShell, na raiz do projeto, execute o script para compilar e rodar automaticamente:
```powershell
.\build.ps1 -Run
