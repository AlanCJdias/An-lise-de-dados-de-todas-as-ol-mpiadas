//descreva a evolução de um determinado país em 10 edições após esse país ganhar a sua primeira medalha.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Bibliotecas/csvUtil.h"
#include "ddxquestao11.h"

#define maxLine 262144
#define maxCampos 64



typedef struct{
    int ano;
    char estacao[10];
} Edicao;


typedef struct{
    Edicao edicao;
    int ouro;
    int prata;
    int bronze;
    int total;
}ResumoEdicao;

/* Compara edições para ordenação (ano + estação). Winter antes de Summer no mesmo ano. */
static int ordemEstacao(const char *estacao) {
    if (strcmp(estacao, "Winter") == 0) return 0;
    return 1;
}

static int compararEdicao(const void *a, const void *b) {
    const Edicao *x = (const Edicao *)a;
    const Edicao *y = (const Edicao *)b;

    if (x->ano != y->ano) return x->ano - y->ano;
    return ordemEstacao(x->estacao) - ordemEstacao(y->estacao);
}

/* Retorna 1 se a medalha for Gold/Silver/Bronze (e não vazia/NA). */
static int medalhaValidaLocal(const char *medalha) {
    if (!medalha) return 0;
    if (medalha[0] == '\0') return 0;
    if (strcmp(medalha, "NA") == 0) return 0;
    return 1;
}

/* Normaliza escolha do usuário para 3 flags simples. */
static void definirFiltroEstacao(const char *escolha, int *filtrarWinter, int *filtrarSummer) {
    *filtrarWinter = 0;
    *filtrarSummer = 0;

    if (strcmp(escolha, "Winter") == 0 || strcmp(escolha, "winter") == 0) {
        *filtrarWinter = 1;
        return;
    }
    if (strcmp(escolha, "Summer") == 0 || strcmp(escolha, "summer") == 0) {
        *filtrarSummer = 1;
        return;
    }

    /* Qualquer outro valor vira "Both" */
    *filtrarWinter = 1;
    *filtrarSummer = 1;
}

/* Verifica se uma edição já existe no vetor (busca linear). */
static int edicaoExiste(const Edicao *edicoes, int total, int ano, const char *estacao) {
    for (int i = 0; i < total; i++) {
        if (edicoes[i].ano == ano && strcmp(edicoes[i].estacao, estacao) == 0) return 1;
    }
    return 0;
}

int executarDdxQuestao11(void) {
    char pais[8];
    char escolha[16];

    printf("Digite o pais (NOC) ex: BRA, USA: ");
    if (!fgets(pais, sizeof(pais), stdin)) return 1;
    removerQuebraLinha(pais);

    printf("Escolha a estacao (Winter / Summer / Both): ");
    if (!fgets(escolha, sizeof(escolha), stdin)) return 1;
    removerQuebraLinha(escolha);

    int filtrarWinter, filtrarSummer;
    definirFiltroEstacao(escolha, &filtrarWinter, &filtrarSummer);

    FILE *arquivo = fopen("arquivoscsvs/results/results.csv", "r");
    if (!arquivo) {
        perror("Erro ao abrir results.csv");
        return 1;
    }

    char linha[maxLine];
    char *campos[maxCampos];

    /* Lê cabeçalho e descobre índices das colunas */
    if (!fgets(linha, sizeof(linha), arquivo)) {
        fclose(arquivo);
        return 1;
    }
    removerQuebraLinha(linha);
    int totalCabecalho = separarCsv(linha, campos, maxCampos);

    int idxYear   = encontrarIndiceColuna(campos, totalCabecalho, "Year");
    int idxSeason = encontrarIndiceColuna(campos, totalCabecalho, "Season");
    int idxNoc    = encontrarIndiceColuna(campos, totalCabecalho, "NOC");
    int idxMedal  = encontrarIndiceColuna(campos, totalCabecalho, "Medal");

    /* Se seu CSV não tiver Year/Season e tiver Games, ajuste aqui:
       int idxGames = encontrarIndiceColuna(campos, totalCabecalho, "Games");
    */

    if (idxYear < 0 || idxSeason < 0 || idxNoc < 0 || idxMedal < 0) {
        printf("Erro: colunas necessarias nao encontradas (Year, Season, NOC, Medal).\n");
        fclose(arquivo);
        return 1;
    }

    /* Passo A: achar a primeira medalha por estação (se for Both, acha duas) */
    int achouPrimeiraWinter = 0, achouPrimeiraSummer = 0;
    Edicao primeiraWinter = {0, ""};
    Edicao primeiraSummer = {0, ""};

    /* Também vamos coletar todas as edições em que o país aparece (por estação filtrada) */
    Edicao edicoes[512];
    int totalEdicoes = 0;

    while (fgets(linha, sizeof(linha), arquivo)) {
        removerQuebraLinha(linha);
        int n = separarCsv(linha, campos, maxCampos);

        if (n <= idxMedal || n <= idxNoc || n <= idxSeason || n <= idxYear) continue;

        const char *noc = campos[idxNoc];
        const char *season = campos[idxSeason];
        const char *medal = campos[idxMedal];

        int year;
        if (!converterInt(campos[idxYear], &year)) continue;

        if (strcmp(noc, pais) != 0) continue;

        /* Filtra estação conforme escolha do usuário */
        if (strcmp(season, "Winter") == 0 && !filtrarWinter) continue;
        if (strcmp(season, "Summer") == 0 && !filtrarSummer) continue;

        /* Guarda edição de participação (única) */
        if (!edicaoExiste(edicoes, totalEdicoes, year, season)) {
            edicoes[totalEdicoes].ano = year;
            strncpy(edicoes[totalEdicoes].estacao, season, sizeof(edicoes[totalEdicoes].estacao) - 1);
            edicoes[totalEdicoes].estacao[sizeof(edicoes[totalEdicoes].estacao) - 1] = '\0';
            totalEdicoes++;
        }

        /* Descobre primeira medalha por estação */
        if (medalhaValidaLocal(medal)) {
            if (strcmp(season, "Winter") == 0) {
                if (!achouPrimeiraWinter) {
                    achouPrimeiraWinter = 1;
                    primeiraWinter.ano = year;
                    strcpy(primeiraWinter.estacao, "Winter");
                } else {
                    /* Mantém a menor edição (mais antiga) */
                    Edicao atual = { year, "Winter" };
                    if (compararEdicao(&atual, &primeiraWinter) < 0) primeiraWinter = atual;
                }
            } else if (strcmp(season, "Summer") == 0) {
                if (!achouPrimeiraSummer) {
                    achouPrimeiraSummer = 1;
                    primeiraSummer.ano = year;
                    strcpy(primeiraSummer.estacao, "Summer");
                } else {
                    Edicao atual = { year, "Summer" };
                    if (compararEdicao(&atual, &primeiraSummer) < 0) primeiraSummer = atual;
                }
            }
        }
    }

    fclose(arquivo);

    /* Ordena edições de participação */
    qsort(edicoes, (size_t)totalEdicoes, sizeof(Edicao), compararEdicao);

 

    printf("\nBase pronta. Proximo passo: montar as 10 edicoes a partir da primeira medalha e contar.\n");
    printf("Total edicoes encontradas (filtradas): %d\n", totalEdicoes);

    if (filtrarWinter) {
        if (achouPrimeiraWinter) printf("Primeira medalha Winter: %d Winter\n", primeiraWinter.ano);
        else printf("Nenhuma medalha Winter encontrada.\n");
    }
    if (filtrarSummer) {
        if (achouPrimeiraSummer) printf("Primeira medalha Summer: %d Summer\n", primeiraSummer.ano);
        else printf("Nenhuma medalha Summer encontrada.\n");
    }

    return 0;
}