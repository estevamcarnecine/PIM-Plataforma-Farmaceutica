#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int timestamp;
    float ValorSinal;
} RegistroSinal;

#define NUM_AMOSTRAS 20
#define TAMANHO_JANELA 5 

void gravar_dados_binarios(RegistroSinal *dados, int numRegistros, const char *nomeArquivo);
int ler_dados_binarios(RegistroSinal **dados, const char *nomeArquivo);
void aplicar_filtro_digital(RegistroSinal *dadosEntrada, RegistroSinal *dadosSaida, int numRegistros);

#endif // REGISTRO_H