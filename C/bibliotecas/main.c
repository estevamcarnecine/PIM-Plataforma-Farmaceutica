#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "registro.h" // Apenas esta linha traz a struct, macros e protótipos

int main() {
    RegistroSinal dadosBrutos[NUM_AMOSTRAS];
    const char *nomeArquivo = "sinais_brutos.bin";

    // 1. Geração de Dados Brutos
    printf("--- 1. Gerando Dados Brutos ---\n");
    srand(time(NULL));
    for (int i = 0; i < NUM_AMOSTRAS; i++) {
        dadosBrutos[i].timestamp = i;
        dadosBrutos[i].ValorSinal = (float)(10.0 + (rand() % 100) / 10.0); 
    }

    // 2. Gravação Segura
    printf("--- 2. Gravando Dados Binários ---\n");
    gravar_dados_binarios(dadosBrutos, NUM_AMOSTRAS, nomeArquivo);

    // 3. Leitura dos Dados Gravados
    RegistroSinal *dadosLidos = NULL;
    int lidos = ler_dados_binarios(&dadosLidos, nomeArquivo);

    if (lidos > 0) {
        printf("--- 3. Sucesso na Leitura: %d registros ---\n", lidos);
        
        // 4. Filtragem Digital
        RegistroSinal *dadosFiltrados = (RegistroSinal *)malloc(lidos * sizeof(RegistroSinal));
        if (dadosFiltrados == NULL) {
            perror("Erro de alocação para dados filtrados");
            free(dadosLidos);
            return 1;
        }

        printf("--- 4. Aplicando Filtro Digital (Janela %d) ---\n", TAMANHO_JANELA);
        aplicar_filtro_digital(dadosLidos, dadosFiltrados, lidos);
        
        // 5. Demonstração
        printf("\nComparação: Sinal Original vs. Sinal Filtrado (últimos 5)\n");
        printf("--------------------------------------------------\n");
        for (int i = lidos - 5; i < lidos; i++) {
             if (i >= 0) {
                 printf("Tempo %d: Original=%.2f | Filtrado=%.2f\n", 
                        dadosLidos[i].timestamp, 
                        dadosLidos[i].ValorSinal, 
                        dadosFiltrados[i].ValorSinal);
             }
        }
        
        free(dadosLidos);
        free(dadosFiltrados);
    }

    return 0;
}