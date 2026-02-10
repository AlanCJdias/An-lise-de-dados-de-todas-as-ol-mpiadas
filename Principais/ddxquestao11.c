//descreva a evolução de um determinado país em 10 edições após esse país ganhar a sua primeira medalha.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Bibliotecas/csvUtil.h"
#include "ddxquestao11.h"

#define maxLine 262144
#define maxCampos 64



typedef struct{         //estrutura para guardar as edições.
    int ano;
    char estacao[10];
} Edicao;


typedef struct{         //estrutura para guardar o resumo de cada edição.
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

/* Retorna o índice de uma edição no vetor edicoes[] ou -1 se não existir */
static int indiceEdicao(const Edicao *edicoes, int total, int ano, const char *estacao) {
    for (int i = 0; i < total; i++) {
        if (edicoes[i].ano == ano && strcmp(edicoes[i].estacao, estacao) == 0) return i;
    }
    return -1;
}

/* Zera os contadores de um resumo */
static void zerarResumoEdicao(ResumoEdicao *r) {
    r->ouro = 0;
    r->prata = 0;
    r->bronze = 0;
    r->total = 0;
}


static void graficoTotalMedalhasPorEdicao(const ResumoEdicao *resumos, int n) {
    /* escala simples para não estourar a tela */
    int maxTotal = 0;
    for (int i = 0; i < n; i++) {
        if (resumos[i].total > maxTotal) maxTotal = resumos[i].total;
    }

    int escala = 1;                 /* 1 '#' = 1 medalha */
    if (maxTotal > 60) escala = 5;  /* 1 '#' = 5 medalhas */
    else if (maxTotal > 30) escala = 2; /* 1 '#' = 2 medalhas */

    printf("\n=== Grafico: Total de medalhas por edicao (1 # = %d) ===\n", escala);

    for (int i = 0; i < n; i++) {
        int barras = resumos[i].total / escala;
        if (resumos[i].total > 0 && barras == 0) barras = 1; /* garante visibilidade */

        printf("%d %s | ", resumos[i].edicao.ano, resumos[i].edicao.estacao);

        for (int j = 0; j < barras; j++) putchar('#');

        printf(" (%d)\n", resumos[i].total);
    }
}








int executarDdxQuestao11(void) {
    char pais[8];
    char escolha[16];

        /* Agora o usuário pode digitar o nome do país ou o NOC */
    char entradaPais[64];

    printf("Digite o nome do país (ex: Brazil) ou o NOC (ex: BRA): ");
    if (!fgets(entradaPais, sizeof(entradaPais), stdin)) return 1;
    removerQuebraLinha(entradaPais);

    if (strlen(entradaPais) == 3) {
        strncpy(pais, entradaPais, sizeof(pais) - 1);
        pais[sizeof(pais) - 1] = '\0';
    } else {
        if (!obterNocPorNomePais(entradaPais, pais, sizeof(pais))) {
            printf("Nao encontrei o país '%s' no noc_regions.csv.\n", entradaPais);
            printf("Dica: tente o nome em inglês (ex: Brazil) ou digite o NOC (ex: BRA).\n");
            return 1;
        }
    }

    printf("NOC selecionado: %s\n", pais);

    printf("Escolha a estacao (Winter / Summer / Both): ");
    if (!fgets(escolha, sizeof(escolha), stdin)) return 1;
    removerQuebraLinha(escolha);

    int filtrarWinter, filtrarSummer;
    definirFiltroEstacao(escolha, &filtrarWinter, &filtrarSummer);

    FILE *arquivo = abrirArquivoDados("results/results.csv", "r");
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

    /*Estamos usando o aquivo da pasta results, ela não possui Year/Season, esta como Games.
      Alterei o idxs para um so Games*/

    int idxGames = encontrarIndiceColuna(campos, totalCabecalho, "Games");
    int idxNoc   = encontrarIndiceColuna(campos, totalCabecalho, "NOC");
    int idxMedal = encontrarIndiceColuna(campos, totalCabecalho, "Medal");

    if (idxGames < 0 || idxNoc < 0 || idxMedal < 0) {
        printf("Erro: colunas necessarias nao encontradas (Games, NOC, Medal).\n");
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

        if (n <= idxMedal || n <= idxNoc || n <= idxGames) continue;

        const char *noc = campos[idxNoc];
        const char *games = campos[idxGames];
        const char *medal = campos[idxMedal];

        /* ALTERAÇÃO: a coluna Games é texto (ex: "2016 Summer").
        Então extraímos ano e estação usando a função da biblioteca. */
        int year = 0;
        char season[10]; /* "Summer" ou "Winter" */

        if (!extrairAnoEEstacaoGames(games, &year, season, sizeof(season))) {
            continue; /* pula linhas com Games fora do padrão */
        }

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

 /* Passo B: montar até 10 edições a partir da primeira medalha (por estação) */
    ResumoEdicao resumoWinter[10];
    int totalWinter = 0;

    ResumoEdicao resumoSummer[10];
    int totalSummer = 0;

    /* Monta 10 Winter */
    if (filtrarWinter && achouPrimeiraWinter) {
        int idxInicio = indiceEdicao(edicoes, totalEdicoes, primeiraWinter.ano, "Winter");
        if (idxInicio >= 0) {
            for (int i = idxInicio; i < totalEdicoes && totalWinter < 10; i++) {
                if (strcmp(edicoes[i].estacao, "Winter") == 0) {
                    resumoWinter[totalWinter].edicao = edicoes[i];
                    zerarResumoEdicao(&resumoWinter[totalWinter]);
                    totalWinter++;
                }
            }
        }
    }

    /* Monta 10 Summer */
    if (filtrarSummer && achouPrimeiraSummer) {
        int idxInicio = indiceEdicao(edicoes, totalEdicoes, primeiraSummer.ano, "Summer");
        if (idxInicio >= 0) {
            for (int i = idxInicio; i < totalEdicoes && totalSummer < 10; i++) {
                if (strcmp(edicoes[i].estacao, "Summer") == 0) {
                    resumoSummer[totalSummer].edicao = edicoes[i];
                    zerarResumoEdicao(&resumoSummer[totalSummer]);
                    totalSummer++;
                }
            }
        }
    }

    /* Se não conseguiu montar nenhuma lista, encerra */
    if ((filtrarWinter && totalWinter == 0) && (filtrarSummer && totalSummer == 0)) {
        printf("\nNao foi possivel montar as 10 edicoes (pais sem medalha na estacao escolhida ou dados insuficientes).\n");
        return 0;
    }

    /* Passo C: segunda passada no CSV para contar TOTAL de medalhas por edição */
    arquivo = abrirArquivoDados("results/results.csv", "r");
    if (!arquivo) {
        perror("Erro ao abrir results.csv (segunda passada)");
        return 1;
    }

    /* pular cabeçalho */
    if (!fgets(linha, sizeof(linha), arquivo)) {
        fclose(arquivo);
        return 1;
    }

    while (fgets(linha, sizeof(linha), arquivo)) {
        removerQuebraLinha(linha);
        int n = separarCsv(linha, campos, maxCampos);

        if (n <= idxMedal || n <= idxNoc || n <= idxGames) continue;

        const char *noc = campos[idxNoc];
        const char *games = campos[idxGames];
        const char *medal = campos[idxMedal];

        if (strcmp(noc, pais) != 0) continue;
        if (!medalhaValidaLocal(medal)) continue;

        int year = 0;
        char season[10];
        if (!extrairAnoEEstacaoGames(games, &year, season, sizeof(season))) continue;

        /* Conta no Winter */
        if (totalWinter > 0 && strcmp(season, "Winter") == 0) {
            for (int i = 0; i < totalWinter; i++) {
                if (resumoWinter[i].edicao.ano == year) {
                    resumoWinter[i].total++;
                    break;
                }
            }
        }

        /* Conta no Summer */
        if (totalSummer > 0 && strcmp(season, "Summer") == 0) {
            for (int i = 0; i < totalSummer; i++) {
                if (resumoSummer[i].edicao.ano == year) {
                    resumoSummer[i].total++;
                    break;
                }
            }
        }
    }

    fclose(arquivo);

    /* Passo D: mostrar o gráfico */
    if (totalWinter > 0) {
        printf("\n--- GRAFICO WINTER (total de medalhas por edicao) ---\n");
        graficoTotalMedalhasPorEdicao(resumoWinter, totalWinter);
    }

    if (totalSummer > 0) {
        printf("\n--- GRAFICO SUMMER (total de medalhas por edicao) ---\n");
        graficoTotalMedalhasPorEdicao(resumoSummer, totalSummer);
    }

    return 0;

}