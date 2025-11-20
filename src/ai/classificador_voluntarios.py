import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, confusion_matrix
import joblib

# 1. Simulação de Dados (Substituir por dados reais mais tarde)
# Características (features): Idade, IMC, Histórico de Reação (0 ou 1)
# Rótulo (target): Estrato (0 = Baixo Risco, 1 = Alto Risco)
np.random.seed(42) # Garante que os dados sejam os mesmos sempre (Reprodutibilidade)

dados = {
    'idade': np.random.randint(20, 70, 100),
    'imc': np.random.uniform(18.5, 35.0, 100),
    'historico_reacao': np.random.randint(0, 2, 100),
}
df = pd.DataFrame(dados)

# Criando o rótulo de risco baseado em regras simples (simulação)
df['estrato_risco'] = np.where(
    (df['idade'] > 55) | (df['imc'] > 30) | (df['historico_reacao'] == 1), 
    1,  # Alto Risco
    0   # Baixo Risco
)

print("--- Resumo dos Dados ---")
print(df.head())
print("\nContagem de Estratos:\n", df['estrato_risco'].value_counts())

# 2. Preparação para o Treinamento
X = df[['idade', 'imc', 'historico_reacao']]
y = df['estrato_risco']

# Divide os dados em treino (70%) e teste (30%)
X_treino, X_teste, y_treino, y_teste = train_test_split(X, y, test_size=0.3, random_state=42)

# 3. Treinamento do Modelo
# Usamos o Random Forest, um modelo supervisionado exigido no Pilar III
modelo_classificador = RandomForestClassifier(n_estimators=100, random_state=42)
modelo_classificador.fit(X_treino, y_treino)
print("\nModelo treinado com sucesso!")

# 4. Avaliação (Requisito de Documentação Técnica)
previsoes = modelo_classificador.predict(X_teste)

print("\n--- Relatório de Classificação (Requisito Pilar III) ---")
print(classification_report(y_teste, previsoes))
print("\nMatriz de Confusão:\n", confusion_matrix(y_teste, previsoes))

# 5. Entrega de Modelo Funcional (Persistência)
# Salva o modelo treinado em um arquivo para ser usado no ambiente de produção
caminho_modelo = 'modelos/classificador_voluntarios.joblib'

# Cria a pasta 'modelos' se não existir
import os
os.makedirs('modelos', exist_ok=True) 

joblib.dump(modelo_classificador, caminho_modelo)
print(f"\nModelo funcional salvo em: {caminho_modelo}")