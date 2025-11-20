# 📑 Especificação do Formato de Dados Binários

Este documento define a estrutura e o layout do dado binário gravado pelo módulo de aquisição de dados em C (`src/c_edge/data_acquisition.c`).

A conformidade com esta especificação é obrigatória para a camada de integração **FFI (Foreign Function Interface)** em Python ou C++, garantindo a correta desserialização dos registros.

## Estrutura do Registro (`DataRecord`)

Um registro de dados é composto pelos seguintes campos e tamanhos. A arquitetura atual assume um sistema **Little-Endian**.

| Ordem | Campo | Tipo C | Tamanho (Bytes) | Descrição |
| :--- | :--- | :--- | :--- | :--- |
| 1 | `timestamp_ms` | `long long` | 8 | Tempo em milissegundos desde a Epoch (Unix). Usado para rastreabilidade (FDA). |
| 2 | `sensor_value` | `float` | 4 | Valor da medição (ex: temperatura, pressão) no formato IEEE 754 de precisão simples. |
| 3 | `sensor_id` | `unsigned char` | 1 | Identificador único do sensor de origem. |
| 4 | `checksum` | `unsigned int` | 4 | Hash de verificação de integridade para auditoria. |

## Tamanho Total e Alinhamento

* **Tamanho Total do Registro:** 17 bytes (8 + 4 + 1 + 4).
* **Alinhamento:** A camada de integração deve garantir que não haja bytes de *padding* adicionados por compiladores, lendo o registro em um bloco de 17 bytes.