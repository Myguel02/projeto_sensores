# Projeto de Processamento e Consulta de Dados de Sensores

Este projeto em C consiste em um conjunto de três programas interligados, projetados para organizar, consultar e gerar dados de sensores.

## Estrutura do Projeto

O projeto é organizado com os seguintes arquivos:

* `common.h`: Contém definições de estruturas de dados (`leitura`, `data_type`) e protótipos de funções utilitárias que são compartilhadas entre os programas.
* `utils.c`: Implementa funções utilitárias, como conversão de datas/horas para/de timestamps Unix epoch.
* `organizador_dados.c` (Programa 1): Responsável por organizar dados brutos de sensores em arquivos separados e ordenados.
* `consultor_dados.c` (Programa 2): Permite consultar a leitura mais próxima de um sensor em um dado instante.
* `gerador_testes.c` (Programa 3): Ferramenta para gerar um arquivo de dados de teste com leituras aleatórias.
* `output/`: Diretório onde os executáveis compilados serão salvos.

## Programas

### Programa 1 – Organização dos Dados (`organizador_dados.c`)

Este programa processa um arquivo de entrada contendo leituras de diversos sensores misturadas.

**Formato do Arquivo de Entrada:**
Cada linha do arquivo deve seguir o formato:
`<TIMESTAMP>,<ID_SENSOR>,<VALOR>`

* `<TIMESTAMP>`: Data e hora da medida no formato Unix epoch.
* `<ID_SENSOR>`: String sem espaços que identifica o sensor (ex: `temperatura`, `umidade`).
* `<VALOR>`: Valor da medida do sensor.

**Tipos de Dados Suportados:**
O programa suporta inferência e tratamento dos seguintes tipos de dados para os valores dos sensores:
* Números inteiros (Ex: `10`, `-5`)
* Booleano (Ex: `true`, `false`)
* Número racional (ponto flutuante) (Ex: `25.7`, `0.5`)
* String (qualquer texto sem espaços, até um limite de 16 caracteres, embora o `VALOR` no arquivo de entrada possa conter mais, o tipo inferido será `String` se não for um dos outros)

**Funcionalidades:**
* Lê o arquivo de entrada especificado como argumento.
* Identifica os diferentes sensores presentes no arquivo.
* Separa os dados, criando um arquivo `.txt` distinto para cada sensor (ex: `temperatura.txt`, `umidade.txt`).
* Dentro de cada arquivo de sensor, as leituras são **ordenadas por timestamp em ordem decrescente** (do mais recente para o mais antigo).

---

### Programa 2 – Consulta por Instante (`consultor_dados.c`)

Este programa pesquisa a medida de um sensor específico mais próxima de uma data e hora informada. Ele assume que os arquivos de dados dos sensores (gerados pelo Programa 1) estão ordenados por timestamp, permitindo o uso de busca binária.

**Funcionalidades:**
* Recebe o nome de um sensor e uma data/hora exata como argumentos da linha de comando.
* Localiza, utilizando **busca binária**, a leitura mais próxima do instante fornecido no arquivo de dados correspondente ao sensor.
* Exibe as informações completas da leitura encontrada (Timestamp, ID do Sensor, Valor e Tipo Inferido).

---

### Programa 3 – Geração de Arquivo de Teste (`gerador_testes.c`)

Este programa é uma ferramenta auxiliar para criar arquivos de dados de teste com leituras aleatórias, úteis para testar os Programas 1 e 2.

**Funcionalidades:**
* Recebe como argumentos da linha de comando:
    * Data e hora de início do intervalo.
    * Data e hora de fim do intervalo.
    * Nomes dos sensores e seus respectivos tipos de dados (pares de `nome_sensor tipo_dado`).
* Para cada sensor especificado, gera 2000 leituras aleatórias.
* Os valores das leituras são gerados aleatoriamente de acordo com o tipo de dado especificado para o sensor.
* Os timestamps são sorteados dentro do intervalo de data/hora fornecido e gerados em ordem decrescente.
* O arquivo de saída (`dados_gerados.txt`) segue o formato `<TIMESTAMP> <ID_SENSOR> <VALOR>`.

---

## Compilação dos Programas

Para compilar os programas, navegue até a pasta raiz do projeto (`C:\Users\mygue\OneDrive\Documentos\atividade n2`) usando seu terminal (Git Bash, CMD, PowerShell).

Certifique-se de que a pasta `output` exista dentro do diretório do seu projeto. Se não existir, crie-a:
`mkdir output`

### Compilar Programa 1 (`organizador_dados.c`)

```bash
gcc organizador_dados.c utils.c -o output/organizador_dados.exe -Wall -Wextra -g3 -std=c11 -I .
