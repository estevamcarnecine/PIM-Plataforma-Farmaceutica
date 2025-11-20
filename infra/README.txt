# Arquitetura de Rede e Infraestrutura (Pilar I)

Este documento descreve a topologia lógica e a segmentação de rede para garantir a resiliência e a segurança dos dados clínicos.

## 1. Topologia de Alto Nível

O ecossistema será dividido em três camadas principais:

| Camada | Função Principal | Tecnologias Chave | Requisitos Regulatórios |
| :--- | :--- | :--- | :--- |
| **Borda (Edge)** | Coleta de dados de baixa latência e processamento local. | C (io_seguro), Dispositivos Embarcados. | Integridade do Dado (Journaling). |
| **Agregação** | Servidores de dados centrais, armazenamento redundante. | Servidores de Dados, RAID. | Disponibilidade e Redundância. |
| **Core (Data Center)** | Controlador SDN, Servidor de Modelos de IA e Sistema de Auditoria. | Python, TensorFlow/PyTorch, Base de Dados SQL. | Rastreabilidade e Explicabilidade (XAI). |

## 2. Segmentação Lógica (VLANs)

Para segurança, a rede será segmentada logicamente usando VLANs:

1.  **VLAN ID 10 - Dados Clínicos:** Tráfego de sinal não processado da Borda para a Agregação.
2.  **VLAN ID 20 - Controle SDN:** Comunicação entre o controlador e os switches (protocolo OpenFlow).
3.  **VLAN ID 30 - Gerenciamento:** Acesso de administradores e sistema de monitoramento.