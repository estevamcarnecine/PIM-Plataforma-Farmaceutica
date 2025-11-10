#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Vetor de entrada (simulação de sinal biomédico)

// Primeira função: Filtragem digital
void filtro_media_movel(const double *entrada, double *saida, int n, int jan) {
    
    for(int i = 0; i < n; i++) {
        double soma = 0.0;
        int cont = 0;
        for (int j = -jan / 2; j <= jan / 2; j++ ) {
            if (i + j >= 0 && i + j < n ) {
                soma += entrada[i + j];
                cont++;
            }
        }
        saida[i] = soma / cont;
    }
}

// segunda função:      gravação segura em binário

void gravar_binario(const char *filename, const double *dados, int n) {
    FILE *fp = fopen(filename, "wb");
    if (fp) {
        fwrite(dados, sizeof(double), n, fp);
        fclose(fp);
    }
}