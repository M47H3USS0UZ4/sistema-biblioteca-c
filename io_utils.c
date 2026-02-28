#include "io_utils.h"

#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

/* Lê uma linha do usuário com suporte a UTF-8 no Windows e fallback padrão. */
void read_line(char *buf, size_t size) {
    if (buf == NULL || size == 0) return;

#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(hStdin, &mode)) {
        wchar_t wbuf[1024];
        DWORD read = 0;
         
        if (ReadConsoleW(hStdin, wbuf, 1023, &read, NULL)) {
    
            while (read > 0 && (wbuf[read - 1] == L'\n' || wbuf[read - 1] == L'\r')) {
                read--;
            }
            wbuf[read] = L'\0';
            
            WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, buf, (int)size, NULL, NULL);
            return;
        }
    }
#endif

    if (fgets(buf, (int)size, stdin) == NULL) {
        buf[0] = '\0';
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    } else {
        size_t len = strcspn(buf, "\n");
        if (buf[len] == '\n') {
            buf[len] = '\0';
        } else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }
}

/* Lê e valida inteiro em intervalo [min, max], repetindo em caso de erro. */
int read_int(const char *prompt, int min, int max, int *out) {
    char buf[128];
    char *endptr;
    long val;
    while (1) {
        printf("%s", prompt);
        
        read_line(buf, sizeof(buf)); 
        if (buf[0] == '\0') return 0;
        
        errno = 0;
        val = strtol(buf, &endptr, 10);
        if (endptr == buf) {
            printf("Entrada inválida. Tente novamente.\n");
            continue;
        }
        while (*endptr == ' ' || *endptr == '\t' || *endptr == '\n' || *endptr == '\r') {
            endptr++;
        }
        if (*endptr != '\0') {
            printf("Entrada inválida. Digite apenas números inteiros.\n");
            continue;
        }
        if (errno == ERANGE || val < min || val > max) {
            printf("Valor fora do intervalo. Tente novamente.\n");
            continue;
        }
        *out = (int)val;
        return 1;
    }
}

/* Inicializa locale e code pages para exibição correta de acentos no console. */
void init_console_output(void) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF-8");
#else
    setlocale(LC_ALL, "");
#endif
}

/* Limpa a tela do terminal com fallback ANSI caso o comando falhe. */
void clear_screen(void) {
#ifdef _WIN32
    int result = system("cls");
#else
    int result = system("clear");
#endif
    if (result != 0) {
        printf("\x1b[2J\x1b[H");
    }
}

/* Pausa o fluxo até ENTER para facilitar leitura de mensagens no console. */
void pause_console(void) {
    int c;
    printf("Pressione ENTER para continuar...");
    fflush(stdout);
    c = getchar();
    if (c != '\n' && c != EOF) {
        while ((c = getchar()) != '\n' && c != EOF);
    }
}
