#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stddef.h>

/* Lê uma linha de texto do console com limite de buffer. */
void read_line(char *buf, size_t size);
/* Lê inteiro validado no intervalo informado. */
int read_int(const char *prompt, int min, int max, int *out);
/* Limpa o terminal atual. */
void clear_screen(void);
/* Aguarda ENTER do usuário. */
void pause_console(void);
/* Configura o console para saída adequada (UTF-8/locale). */
void init_console_output(void);

#endif
