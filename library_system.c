#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "library_catalog.h"
#include "io_utils.h"

/* Salva o catálogo e exibe aviso ao usuário em caso de falha de persistência. */
static void save_catalog_or_warn(const struct Book catalog[], int catalogSize) {
    if (!saveCatalog(catalog, catalogSize)) {
        printf("Falha ao salvar o acervo em arquivo.\n");
        pause_console();
    }
}

/* Ponto de entrada: inicializa console, carrega dados, executa menu e finaliza recursos. */
int main(void) {
    struct Book *catalog = NULL;
    int catalogSize = 0;
    int capacity = 0;
    int option = 0;

    init_console_output();

    if (!loadCatalog(&catalog, &catalogSize, &capacity)) {
        printf("Não foi possível carregar o arquivo de acervo. Iniciando acervo vazio.\n");
        catalog = NULL;
        catalogSize = 0;
        capacity = 0;
        pause_console();
    }

    do {
        clear_screen();
        printf("========================================\n");
        printf("        SISTEMA DE BIBLIOTECA\n");
        printf("========================================\n");
        printf("1 - Cadastrar livros\n");
        printf("2 - Imprimir todos os livros\n");
        printf("3 - Pesquisar livro por código\n");
        printf("4 - Ordenar livros por ano\n");
        printf("5 - Editar livro por código\n");
        printf("6 - Remover livro por código\n");
        printf("7 - Sair do programa\n");
        printf("----------------------------------------\n");
        printf("Acervo: %d livro(s)\n", catalogSize);

        if (!read_int("Escolha uma opção: ", 1, 7, &option)) {
            printf("Erro ao ler a opção. Saindo.\n");
            break;
        }

        switch (option) {
            case 1:
                registerBooks(&catalog, &catalogSize, &capacity);
                save_catalog_or_warn(catalog, catalogSize);
                break;
            case 2:
                printBooks(catalog, catalogSize);
                break;
            case 3: {
                int searchCode;
                clear_screen();
                printf("\nPESQUISAR LIVRO\n");
                if (read_int("Digite o código do livro a ser pesquisado: ", INT_MIN, INT_MAX, &searchCode)) {
                    int index = searchBookByCode(catalog, catalogSize, searchCode);
                    if (index < 0) {
                        printf("Livro com código %d não encontrado.\n", searchCode);
                        pause_console();
                    }
                } else {
                    printf("Código inválido.\n");
                    pause_console();
                }
                break;
            }
            case 4:
                sortBooksByYear(catalog, catalogSize);
                save_catalog_or_warn(catalog, catalogSize);
                break;
            case 5:
                editBookByCode(catalog, catalogSize);
                save_catalog_or_warn(catalog, catalogSize);
                break;
            case 6:
                removeBookByCode(catalog, &catalogSize);
                save_catalog_or_warn(catalog, catalogSize);
                break;
            case 7:
                clear_screen();
                save_catalog_or_warn(catalog, catalogSize);
                printf("Saindo do programa. Até mais!\n");
                break;
        }
    } while (option != 7);

    free(catalog);

    return 0;
}