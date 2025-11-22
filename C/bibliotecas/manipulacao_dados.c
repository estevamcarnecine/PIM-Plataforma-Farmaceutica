#include <stdio.h>
#include <stdlib.h>

// Estrutura para um ponto de sinal (exemplo de precisão de ponto flutuante)
typedef struct {
    int timestamp;
    float ValorSinal; // O ponto flutuante de precisão exigido no PIM.
} RegistroSinal;

void gravar_dados_binarios(RegistroSinal *dados, int numRegistros, const char *nomeArquivo) {
    // 1. Abrir o arquivo no modo de escrita binária ("wb")
    // 'wb' (write binary) garante que a precisão do float não seja perdida.
    FILE *fp = fopen(nomeArquivo, "wb");

    if (fp == NULL) {
        // Se a abertura falhar, informa o erro e encerra a função.
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    // 2. Escrever o bloco de dados usando fwrite
    // size_t escritos: armazena quantos itens (RegistroSinal) foram escritos.
    size_t escritos = fwrite(
        dados,                      // Ponteiro para o bloco de memória dos dados
        sizeof(RegistroSinal),      // Tamanho de um único item (registro)
        numRegistros,               // Número de itens a serem escritos
        fp                          // Ponteiro para o arquivo aberto
    );

    if (escritos != numRegistros) {
        // A checagem é essencial para a auditoria: verifica se todos os dados foram persistidos.
        printf("AVISO: Somente %zu de %d registros foram escritos com sucesso.\n", escritos, numRegistros);
    } else {
        printf("SUCESSO: %d registros binários escritos em %s\n", numRegistros, nomeArquivo);
    }

    // 3. Fechar o arquivo para liberar o recurso e garantir a escrita física.
    fclose(fp);
}