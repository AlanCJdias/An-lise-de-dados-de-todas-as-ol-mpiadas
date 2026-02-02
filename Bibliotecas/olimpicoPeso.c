/*
   Funções auxiliares de CSV para cálculo do peso médio olímpico
*/


#include "olimpicoPeso.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Tamanho máximo de uma linha no CSV
#define tamanhoLinha 262144

// Remove \n e \r do final da linha 
static void removerQuebraLinha(char *linha) {
    size_t n = strlen(linha);
    while (n > 0 && (linha[n - 1] == '\n' || linha[n - 1] == '\r')) {
        linha[n - 1] = '\0';
        n--;
    }
}


  // Separa uma linha CSV em campos respeitando aspas

static int separarCsv(char *linha, char **campos, int maxCampos) {
    int dentroAspas = 0;
    int total = 0;
    char *inicio = linha;

    for (char *p = linha; *p; p++) {
        if (*p == '"') {
            dentroAspas = !dentroAspas;
        } else if (*p == ',' && !dentroAspas) {
            *p = '\0';
            if (total < maxCampos)
                campos[total++] = inicio;
            inicio = p + 1;
        }
    }

    if (total < maxCampos)
        campos[total++] = inicio;

    // remove aspas externas 
    for (int i = 0; i < total; i++) {
        char *campo = campos[i];
        size_t len = strlen(campo);
        if (len >= 2 && campo[0] == '"' && campo[len - 1] == '"') {
            campo[len - 1] = '\0';
            campos[i] = campo + 1;
        }
    }

    return total;
}

// Procura o índice de uma coluna pelo nome 
static int encontrarColuna(char **cabecalho, int total, const char *nome) {
    for (int i = 0; i < total; i++) {
        if (strcmp(cabecalho[i], nome) == 0)
            return i;
    }
    return -1;
}

 //Conversões seguras 
static int converterInteiro(const char *texto, int *saida) {
    char *fim;
    long v = strtol(texto, &fim, 10);
    if (fim == texto) return 0;
    *saida = (int)v;
    return 1;
}

static int converterLong(const char *texto, long *saida) {
    char *fim;
    long v = strtol(texto, &fim, 10);
    if (fim == texto) return 0;
    *saida = v;
    return 1;
}

static int converterDouble(const char *texto, double *saida) {
    char *fim;
    double v = strtod(texto, &fim);
    if (fim == texto) return 0;
    *saida = v;
    return 1;
}

    // Comparador para ordenação e busca 
static int compararLong(const void *a, const void *b) {
    long x = *(const long *)a;
    long y = *(const long *)b;
    return (x > y) - (x < y);
}


   // Função principal
  

int calcularPesoMedioOlimpico(
    const char *caminhoResultados,
    const char *caminhoBios,
    int ano,
    const char *estacao,
    EstatisticasPeso *saida
) {
    if (!caminhoResultados || !caminhoBios || !estacao || !saida)
        return -1;

    memset(saida, 0, sizeof(*saida));

    // Parte responsável por ler o arquivo results.csv

    FILE *arquivoResultados = fopen(caminhoResultados, "r");
    if (!arquivoResultados)
        return -1;

    char *linha = malloc(tamanhoLinha);
    if (!linha) {
        fclose(arquivoResultados);
        return -1;
    }

    char *campos[256];

    // Cabeçalho 
    fgets(linha, tamanhoLinha, arquivoResultados);
    removerQuebraLinha(linha);
    int totalCampos = separarCsv(linha, campos, 256);

    int colunaAno     = encontrarColuna(campos, totalCampos, "Year");
    int colunaEstacao = encontrarColuna(campos, totalCampos, "Season");
    int colunaId      = encontrarColuna(campos, totalCampos, "athlete_id");

    if (colunaAno < 0 || colunaEstacao < 0 || colunaId < 0) {
        free(linha);
        fclose(arquivoResultados);
        return -1;
    }

    long *idsAtletas = NULL;
    long totalIds = 0;
    long capacidade = 0;

    while (fgets(linha, tamanhoLinha, arquivoResultados)) {
        removerQuebraLinha(linha);
        int n = separarCsv(linha, campos, 256);

        int anoLinha;
        long idAtleta;

        if (n <= colunaId) continue;
        if (!converterInteiro(campos[colunaAno], &anoLinha)) continue;
        if (anoLinha != ano) continue;
        if (strcmp(campos[colunaEstacao], estacao) != 0) continue;
        if (!converterLong(campos[colunaId], &idAtleta)) continue;

        if (totalIds >= capacidade) {
            capacidade = capacidade == 0 ? 1024 : capacidade * 2;
            idsAtletas = realloc(idsAtletas, capacidade * sizeof(long));
            if (!idsAtletas) {
                free(linha);
                fclose(arquivoResultados);
                return -1;
            }
        }

        idsAtletas[totalIds++] = idAtleta;
    }

    fclose(arquivoResultados);

    // Vai remover IDs duplicados
    qsort(idsAtletas, totalIds, sizeof(long), compararLong);

    long idsUnicos = 0;
    for (long i = 0; i < totalIds; i++) {
        if (i == 0 || idsAtletas[i] != idsAtletas[i - 1])
            idsAtletas[idsUnicos++] = idsAtletas[i];
    }

    saida->atletasParticipantes = idsUnicos;

   // Parte responsável por ler o arquivo bios.csv

    FILE *arquivoBios = fopen(caminhoBios, "r");
    if (!arquivoBios) {
        free(idsAtletas);
        free(linha);
        return -1;
    }

    fgets(linha, tamanhoLinha, arquivoBios);
    removerQuebraLinha(linha);
    totalCampos = separarCsv(linha, campos, 256);

    int colunaIdBios = encontrarColuna(campos, totalCampos, "athlete_id");
    int colunaPeso   = encontrarColuna(campos, totalCampos, "Weight");

    if (colunaIdBios < 0 || colunaPeso < 0) {
        free(idsAtletas);
        free(linha);
        fclose(arquivoBios);
        return -1;
    }

    double somaPesos = 0.0;
    long atletasComPeso = 0;
    long atletasSemPeso = 0;

    while (fgets(linha, tamanhoLinha, arquivoBios)) {
        removerQuebraLinha(linha);
        int n = separarCsv(linha, campos, 256);

        long idAtleta;
        double peso;

        if (n <= colunaPeso) continue;
        if (!converterLong(campos[colunaIdBios], &idAtleta)) continue;

        if (!bsearch(&idAtleta, idsAtletas, idsUnicos,
                      sizeof(long), compararLong))
            continue;

        if (converterDouble(campos[colunaPeso], &peso)) {
            somaPesos += peso;
            atletasComPeso++;
        } else {
            atletasSemPeso++;
        }
    }

    fclose(arquivoBios);

    saida->atletasComPeso = atletasComPeso;
    saida->atletasSemPeso = atletasSemPeso;
    saida->pesoMedio =
        atletasComPeso > 0 ? somaPesos / atletasComPeso : 0.0;

    free(idsAtletas);
    free(linha);

    return 0;
}