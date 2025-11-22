#include <stdio.h>
#include <stdlib.h>
#include "registro.h"

void gravar_dados_binarios(RegistroSinal *dados, int numRegistros, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "wb");

    if (fp == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    size_t escritos = fwrite(
        dados,                      
        sizeof(RegistroSinal),      
        numRegistros,               
        fp                          
    );

    if (escritos != numRegistros) {
        printf("AVISO: Somente %zu de %d registros foram escritos com sucesso.\n", escritos, numRegistros);
    } else {
        printf("SUCESSO: %d registros binários escritos em %s\n", numRegistros, nomeArquivo);
    }

    fclose(fp);
}

int ler_dados_binarios(RegistroSinal **dados, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "rb");

    if (fp == NULL) {
        perror("Erro ao abrir o arquivo para leitura");
        return 0;
    }

    fseek(fp, 0, SEEK_END); 
    long tamanhoArquivo = ftell(fp); 
    fseek(fp, 0, SEEK_SET); 

    int numRegistros = tamanhoArquivo / sizeof(RegistroSinal);
    if (numRegistros == 0) {
        fclose(fp);
        return 0;
    }

    *dados = (RegistroSinal *)malloc(tamanhoArquivo);
    if (*dados == NULL) {
        perror("Erro de alocação de memória");
        fclose(fp);
        return 0;
    }

    size_t lidos = fread(*dados, sizeof(RegistroSinal), numRegistros, fp);

    if (lidos != numRegistros) {
        printf("AVISO: Lidos %zu registros, mas esperados %d.\n", lidos, numRegistros);
    }

    fclose(fp);
    return (int)lidos;
}