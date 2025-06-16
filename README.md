# Organização de Dados de Sensores

Este projeto em C tem como objetivo processar um arquivo de dados brutos de sensores, extrair as leituras, organizar esses dados por sensor e tipo, e gerar arquivos de saída limpos e ordenados por timestamp para cada sensor.

## Funcionalidades

* **Leitura de Dados Brutos:** Processa um arquivo de texto contendo leituras de sensores no formato `timestamp ID_SENSOR VALOR`.
* **Inferência de Tipo:** Tenta inferir o tipo de dado (inteiro, booleano, flutuante ou string) para cada sensor com base no formato do primeiro valor lido.
* **Organização por Sensor:** Agrupa todas as leituras pertencentes ao mesmo `ID_SENSOR`.
* **Ordenação por Tempo:** Ordena as leituras de cada sensor cronologicamente (pelo `timestamp`).
* **Geração de Arquivos de Saída:** Cria um arquivo `.txt` separado para cada `ID_SENSOR` detectado, contendo apenas as leituras desse sensor, já ordenadas.

## Como Compilar

Para compilar o projeto, certifique-se de ter um compilador C (como GCC, Clang ou MinGW) instalado. Navegue até o diretório raiz do projeto no seu terminal e execute o seguinte comando:

```bash
gcc organizador_dados.c utils.c -o output/organizador_dados.exe -Wall -Wextra -g3 -std=c11
