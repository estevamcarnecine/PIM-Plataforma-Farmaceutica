#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

typedef struct {
    time_t timestamp; 
    int id_dispositivo;
    float valor_sinal;
} RegistroDeDados;

int main() {
    RegistroDeDados novo_registro;
    
    novo_registro.timestamp = time(NULL); 
    novo_registro.id_dispositivo = 101; 
    novo_registro.valor_sinal = 37.5; 

    FILE *arquivo = fopen("dados_clinicos.bin", "ab+");

    if (arquivo == NULL) {
        printf("ERRO: Nao foi possivel abrir o arquivo binario para escrita.\n");
        return 1;
    }

    size_t itens_escritos = fwrite(&novo_registro, sizeof(RegistroDeDados), 1, arquivo);

    fclose(arquivo);

    if (itens_escritos == 1) {
        printf("SUCESSO: Registro de dados salvo.\n");
        printf("Tempo de registro (Timestamp): %ld\n", novo_registro.timestamp);
    } else {
        printf("AVISO: Ocorreu um problema durante a escrita do dado.\n");
    }

    return 0;
}