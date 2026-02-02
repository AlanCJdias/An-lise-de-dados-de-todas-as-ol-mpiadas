#pragma once

/*
   Estrutura que armazena as estatísticas do cálculo
*/
typedef struct {
    double pesoMedio;               // média dos pesos válidos
    long atletasComPeso;            // atletas usados no cálculo
    long atletasSemPeso;            // atletas sem peso informado
    long atletasParticipantes;      // atletas únicos da edição
} EstatisticasPeso;

/*
   Calcula o peso médio dos atletas de uma edição olímpica.

   Parâmetros:
     - caminhoResultados: CSV de participações (results.csv)
     - caminhoBios: CSV de dados físicos (bios.csv)
     - ano: ano da edição (ex: 2016)
     - estacao: "Summer" ou "Winter"
     - saida: estrutura preenchida com os resultados

   Retorno:
     0  -> sucesso
    -1  -> erro de leitura ou colunas não encontradas
*/
int calcularPesoMedioOlimpico(
    const char *caminhoResultados,
    const char *caminhoBios,
    int ano,
    const char *estacao,
    EstatisticasPeso *saida
);