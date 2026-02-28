#include "library_catalog.h"

#include "io_utils.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define CATALOG_FILE "acervo.dat"
#define MAGIC_NUMBER 0x4249424C /* "BIBL" em ASCII para validar arquivo de acervo. */

/* Lê confirmação simples do usuário (s/n), repetindo até receber valor válido. */
static int readYesNo(const char *prompt) {
    char response[8];
    while (1) {
        printf("%s", prompt);
        read_line(response, sizeof(response));
        if (response[0] == 's' || response[0] == 'S') return 1;
        if (response[0] == 'n' || response[0] == 'N') return 0;
        printf("Resposta inválida. Digite 's' ou 'n'.\n");
    }
}

/* Procura o índice de um livro no catálogo pelo código único. */
static int findBookIndexByCode(const struct Book catalog[], int catalogSize, int code) {
    for (int index = 0; index < catalogSize; ++index) {
        if (catalog[index].code == code) return index;
    }
    return -1;
}

/* Compara livros por ano e, em empate, por código (uso com qsort). */
static int compareBooksByYear(const void *left, const void *right) {
    const struct Book *bookA = (const struct Book *)left;
    const struct Book *bookB = (const struct Book *)right;

    if (bookA->year < bookB->year) return -1;
    if (bookA->year > bookB->year) return 1;

    if (bookA->code < bookB->code) return -1;
    if (bookA->code > bookB->code) return 1;
    return 0;
}

/* Exibe os dados de um livro formatados para a saída de console. */
static void printBook(const struct Book *book, int number) {
    printf("Livro #%d:\n", number);
    printf("Código: %d\n", book->code);
    printf("Título: %s\n", book->title);
    printf("Autor: %s\n", book->author);
    printf("Área: %s\n", book->area);
    printf("Ano: %d\n", book->year);
    printf("Editora: %s\n", book->publisher);
    printf("-----------------\n");
}

/* Garante capacidade mínima do vetor dinâmico, expandindo quando necessário. */
static int ensureCatalogCapacity(struct Book **catalog, int *capacity, int minCapacity) {
    if (*capacity >= minCapacity) {
        return 1;
    }

    int newCapacity = (*capacity == 0) ? 2 : *capacity;
    while (newCapacity < minCapacity) {
        if (newCapacity > INT_MAX / 2) {
            newCapacity = minCapacity;
            break;
        }
        newCapacity *= 2;
    }

    struct Book *resizedCatalog = realloc(*catalog, (size_t)newCapacity * sizeof(struct Book));
    if (!resizedCatalog) {
        return 0;
    }

    *catalog = resizedCatalog;
    *capacity = newCapacity;
    return 1;
}

/* Persiste o catálogo em arquivo binário no disco local, com assinatura de segurança. */
int saveCatalog(const struct Book catalog[], int catalogSize) {
    if (catalogSize < 0) {
        return 0;
    }

    FILE *file = fopen(CATALOG_FILE, "wb");
    if (!file) return 0;

    unsigned int magic = MAGIC_NUMBER;
    if (fwrite(&magic, sizeof(magic), 1, file) != 1) {
        fclose(file);
        return 0;
    }

    if (fwrite(&catalogSize, sizeof(catalogSize), 1, file) != 1) {
        fclose(file); 
        return 0;
    }

    if (catalogSize > 0 && fwrite(catalog, sizeof(struct Book), (size_t)catalogSize, file) != (size_t)catalogSize) {
        fclose(file); 
        return 0;
    }

    fclose(file);
    return 1;
}

/* Carrega o catálogo do arquivo binário, validando a integridade. */
int loadCatalog(struct Book **catalog, int *catalogSize, int *capacity) {
    FILE *file = fopen(CATALOG_FILE, "rb");
    if (!file) {
        *catalog = NULL;
        *catalogSize = 0;
        *capacity = 0;
        return 1; 
    }

    unsigned int magic;
    if (fread(&magic, sizeof(magic), 1, file) != 1 || magic != MAGIC_NUMBER) {
        printf("O arquivo de acervo está corrompido ou é de outro formato.\n");
        fclose(file);
        return 0;
    }

    if (fread(catalogSize, sizeof(*catalogSize), 1, file) != 1 || *catalogSize < 0) {
        fclose(file);
        return 0;
    }

    if (*catalogSize > INT_MAX / (int)sizeof(struct Book)) {
        fclose(file);
        return 0;
    }

    if (*catalogSize == 0) {
        *catalog = NULL;
        *capacity = 0;
        fclose(file);
        return 1;
    }
    
    *catalog = malloc((size_t)*catalogSize * sizeof(struct Book));
    if (!*catalog) {
        fclose(file);
        return 0;
    }
    *capacity = *catalogSize;

    if (fread(*catalog, sizeof(struct Book), (size_t)*catalogSize, file) != (size_t)*catalogSize) {
        free(*catalog);
        *catalog = NULL;
        *catalogSize = 0;
        *capacity = 0;
        fclose(file);
        return 0;
    }

    if (fgetc(file) != EOF) {
        printf("Arquivo possivelmente corrompido. Dados extras detectados no final do arquivo.\n");
        pause_console();
    }

    fclose(file);
    return 1;
}

/* Cadastra um ou mais livros com validação de campos obrigatórios e código único. */
void registerBooks(struct Book **catalog, int *catalogSize, int *capacity) {
    while (1) {
        if (!ensureCatalogCapacity(catalog, capacity, *catalogSize + 1)) {
            printf("Falha ao alocar memória para novos livros.\n");
            pause_console();
            return;
        }

        struct Book newBook;
        printf("CADASTRO DO LIVRO #%d\n", *catalogSize + 1);

        if (!read_int("Código: ", INT_MIN, INT_MAX, &newBook.code)) {
            printf("Erro na leitura do código. Abortando cadastro.\n");
            pause_console();
            return;
        }

        if (findBookIndexByCode(*catalog, *catalogSize, newBook.code) >= 0) {
            printf("Código já existe no acervo. Utilize outro código.\n");
            pause_console();
            continue;
        }

        printf("Título: ");
        read_line(newBook.title, MAX_TITLE_LENGTH);
        if (newBook.title[0] == '\0') {
            printf("Título não pode ficar vazio. Cadastro cancelado.\n");
            pause_console();
            continue;
        }

        printf("Autor: ");
        read_line(newBook.author, MAX_AUTHOR_LENGTH);
        if (newBook.author[0] == '\0') {
            printf("Autor não pode ficar vazio. Cadastro cancelado.\n");
            pause_console();
            continue;
        }

        printf("Área: ");
        read_line(newBook.area, MAX_AREA_LENGTH);
        if (newBook.area[0] == '\0') {
            printf("Área não pode ficar vazia. Cadastro cancelado.\n");
            pause_console();
            continue;
        }

        while (!read_int("Ano de Publicação: ", 0, 9999, &newBook.year)) {
            printf("Entrada inválida! Digite o ano apenas com números.\n");
        }

        printf("Editora: ");
        read_line(newBook.publisher, MAX_PUBLISHER_LENGTH);
        if (newBook.publisher[0] == '\0') {
            printf("Editora não pode ficar vazia. Cadastro cancelado.\n");
            pause_console();
            continue;
        }

        (*catalog)[*catalogSize] = newBook;
        (*catalogSize)++;
        printf(">> Livro '%s' cadastrado com sucesso.\n", newBook.title);
        pause_console();

        if (!readYesNo("Deseja cadastrar outro livro? (s/n): ")) break;
    }
}

/* Imprime todos os livros cadastrados no acervo. */
void printBooks(const struct Book catalog[], int catalogSize) {
    clear_screen();
    printf("Lista de Livros\n");

    if (catalogSize == 0) {
        printf("Nenhum livro cadastrado.\n");
        pause_console();
        return;
    }

    for (int index = 0; index < catalogSize; index++) {
        printBook(&catalog[index], index + 1);
    }
    pause_console();
}

/* Busca livro por código e imprime o resultado quando encontrado. */
int searchBookByCode(const struct Book catalog[], int catalogSize, int targetCode) {
    int index = findBookIndexByCode(catalog, catalogSize, targetCode);
    clear_screen();
    if (index < 0) return -1;

    printf("LIVRO ENCONTRADO\n");
    printBook(&catalog[index], index + 1);
    pause_console();
    return index;
}

/* Ordena o catálogo por ano de publicação usando qsort. */
void sortBooksByYear(struct Book catalog[], int catalogSize) {
    clear_screen();
    printf("ORDENANDO LIVROS POR ANO DE PUBLICAÇÃO\n");

    if (catalogSize <= 1) {
        printf("Acervo já ordenado ou insuficiente.\n");
        pause_console();
        return;
    }

    qsort(catalog, (size_t)catalogSize, sizeof(struct Book), compareBooksByYear);
    printf("Acervo ordenado com sucesso!\n");
    pause_console();
    printBooks(catalog, catalogSize);
}

/* Edita os dados de um livro localizado pelo código informado. */
void editBookByCode(struct Book catalog[], int catalogSize) {
    int targetCode;
    int index;
    struct Book updatedBook;

    clear_screen();
    printf("EDITAR LIVRO\n");

    if (catalogSize == 0) {
        printf("Nenhum livro cadastrado.\n");
        pause_console();
        return;
    }

    if (!read_int("Digite o código do livro para editar: ", INT_MIN, INT_MAX, &targetCode)) {
        printf("Código inválido.\n");
        pause_console();
        return;
    }

    index = findBookIndexByCode(catalog, catalogSize, targetCode);
    if (index < 0) {
        printf("Livro com código %d não encontrado.\n", targetCode);
        pause_console();
        return;
    }

    printf("\nDados atuais:\n");
    printBook(&catalog[index], index + 1);

    updatedBook = catalog[index];

    printf("Novo título: ");
    read_line(updatedBook.title, MAX_TITLE_LENGTH);
    if (updatedBook.title[0] == '\0') {
        printf("Título não pode ficar vazio. Edição cancelada.\n");
        pause_console();
        return;
    }

    printf("Novo autor: ");
    read_line(updatedBook.author, MAX_AUTHOR_LENGTH);
    if (updatedBook.author[0] == '\0') {
        printf("Autor não pode ficar vazio. Edição cancelada.\n");
        pause_console();
        return;
    }

    printf("Nova área: ");
    read_line(updatedBook.area, MAX_AREA_LENGTH);
    if (updatedBook.area[0] == '\0') {
        printf("Área não pode ficar vazia. Edição cancelada.\n");
        pause_console();
        return;
    }

    if (!read_int("Novo ano de publicação: ", 0, 9999, &updatedBook.year)) {
        printf("Ano inválido. Edição cancelada.\n");
        pause_console();
        return;
    }

    printf("Nova editora: ");
    read_line(updatedBook.publisher, MAX_PUBLISHER_LENGTH);
    if (updatedBook.publisher[0] == '\0') {
        printf("Editora não pode ficar vazia. Edição cancelada.\n");
        pause_console();
        return;
    }

    catalog[index] = updatedBook;

    printf("Livro atualizado com sucesso!\n");
    pause_console();
}

/* Remove um livro por código e compacta o vetor após remoção. */
void removeBookByCode(struct Book catalog[], int *catalogSize) {
    int targetCode;
    int index;

    clear_screen();
    printf("REMOVER LIVRO\n");

    if (*catalogSize == 0) {
        printf("Nenhum livro cadastrado.\n");
        pause_console();
        return;
    }

    if (!read_int("Digite o código do livro para remover: ", INT_MIN, INT_MAX, &targetCode)) {
        printf("Código inválido.\n");
        pause_console();
        return;
    }

    index = findBookIndexByCode(catalog, *catalogSize, targetCode);
    if (index < 0) {
        printf("Livro com código %d não encontrado.\n", targetCode);
        pause_console();
        return;
    }

    printf("Livro encontrado:\n");
    printBook(&catalog[index], index + 1);

    if (!readYesNo("Confirma remoção deste livro? (s/n): ")) {
        printf("Remoção cancelada.\n");
        pause_console();
        return;
    }

    for (int i = index; i < *catalogSize - 1; ++i) {
        catalog[i] = catalog[i + 1];
    }
    (*catalogSize)--;

    printf("Livro removido com sucesso!\n");
    pause_console();
}
