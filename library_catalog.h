#ifndef LIBRARY_CATALOG_H
#define LIBRARY_CATALOG_H

#define MAX_TITLE_LENGTH 50
#define MAX_AUTHOR_LENGTH 30
#define MAX_AREA_LENGTH 30
#define MAX_PUBLISHER_LENGTH 30

/* Estrutura base de um livro no acervo. */
struct Book {
    int code;
    char title[MAX_TITLE_LENGTH];
    char author[MAX_AUTHOR_LENGTH];
    char area[MAX_AREA_LENGTH];
    int year;
    char publisher[MAX_PUBLISHER_LENGTH];
};

/* Cadastra um ou mais livros no catálogo em memória. */
void registerBooks(struct Book **catalog, int *catalogSize, int *capacity);
/* Imprime todos os livros cadastrados. */
void printBooks(const struct Book catalog[], int catalogSize);
/* Retorna índice do livro por código; -1 quando não encontrado. */
int searchBookByCode(const struct Book catalog[], int catalogSize, int targetCode);
/* Ordena os livros por ano de publicação (e código em empate). */
void sortBooksByYear(struct Book catalog[], int catalogSize);
/* Edita os dados de um livro pelo código informado. */
void editBookByCode(struct Book catalog[], int catalogSize);
/* Remove um livro do catálogo pelo código informado. */
void removeBookByCode(struct Book catalog[], int *catalogSize);
/* Salva o acervo em arquivo binário. */
int saveCatalog(const struct Book catalog[], int catalogSize);
/* Carrega o acervo de arquivo binário para memória. */
int loadCatalog(struct Book **catalog, int *catalogSize, int *capacity);

#endif
