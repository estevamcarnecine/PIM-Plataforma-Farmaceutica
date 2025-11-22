#include <stdio.h>
#include <stdlib.h>
#include "registro.h"

void aplicar_filtro_digital(RegistroSinal *dadosEntrada, RegistroSinal *dadosSaida, int numRegistros) {
    
    int i, j;
    
    for (i = 0; i < numRegistros; i++) {
        
        float soma = 0.0;
        int contagem = 0;
        
        for (j = 0; j < TAMANHO_JANELA; j++) {
            
            if (i - j >= 0) {
                soma += dadosEntrada[i - j].ValorSinal;
                contagem++;
            }
        }
        
        if (contagem > 0) {
            dadosSaida[i].timestamp = dadosEntrada[i].timestamp;
            dadosSaida[i].ValorSinal = soma / contagem;
        }
    }
    
    printf("Rotina de filtragem digital (Média Móvel) concluída.\n");
}