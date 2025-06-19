# Projeto de Processamento de Dados de Sensores

Este projeto em C consiste em três programas interdependentes para a geração, organização e consulta de dados de sensores.

## Conteúdo do Repositório

* `common.h`: Arquivo de cabeçalho que define estruturas de dados comuns (como `leitura`), tipos enumerados (`data_type` para inferência de tipo) e constantes (`MAX_SENSOR_ID_LENGTH`, `MAX_VALUE_STRING_LENGTH`, `MAX_LINE_LENGTH`).
* `utils.c`: Contém funções utilitárias que são compartilhadas pelos outros programas, incluindo `converter_para_timestamp` (para converter data/hora para timestamp Unix) e `timestamp_para_string` (para converter timestamp para uma string formatada de data/hora).
* `gerar_arquivo_teste.c`: **Programa 1** - Responsável por criar um arquivo de texto (`dados_brutos.txt`) com dados de sensores simulados. Ele gera leituras aleatórias com timestamp, ID do sensor e valores de diferentes tipos (inteiro, float, booleano, string).
* `organizador_dados.c`: **Programa 2** - Lê o arquivo `dados_brutos.txt` gerado pelo Programa 1. Ele processa cada linha, infere o tipo de dado do valor lido e organiza as leituras, salvando-as em arquivos separados para cada sensor (ex: `temp.txt`, `umidade.txt`). As leituras dentro de cada arquivo de sensor são ordenadas por timestamp em ordem decrescente.
* `consulta_instante2.c`: **Programa 3** - Permite consultar os dados de um sensor específico em um determinado instante de tempo. Ele lê o arquivo do sensor correspondente (ex: `temp.txt`), e utiliza uma busca binária otimizada para dados decrescentes para encontrar a leitura com o timestamp mais próximo ao fornecido.
* `output/`: Diretório destinado a armazenar os arquivos executáveis compilados de cada programa.

## Como Compilar os Programas

Para compilar os programas, você precisará ter o compilador GCC (GNU Compiler Collection) configurado no seu ambiente. Os comandos abaixo são formatados para serem executados em um terminal PowerShell no Windows, assumindo que você está no diretório raiz do projeto.

**É crucial incluir o `utils.c` na compilação de cada programa que utiliza suas funções.**


