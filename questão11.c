//todos os imports necessários.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//definindo o tamanho máximo, em bytes de cada linha.
#define MAX_LINE 1024

int main(){

    //primeiramente, o mais essencial será abrir o arquivo.
    FILE *arquivo = fopen("results.csv","r");//esse "r" é apenas para indicar que apenas será lido.
    if (arquivo == NULL) {//caso não ache o arquivo, ele deve resultar nessa parte.
        printf("Erro ao abrir o arquivo.\n");
        return 1;// o return faz o código parar.
    }

    int primeiro_ano = 9999;//serve apenas para ser alterado no futuro.
    char primeira_edicao[100] = "";//serve apenas para ser alterado no futuro.

    char linha[MAX_LINE];
    char pais_escolhido[4];
    printf("Digite o país (NOC com apenas 3 letras, ex: BRA, FRA): ");
    scanf("%3s", pais_escolhido); // esse n3 é para definir o tamanho máximo.

    //isso aqui é para ignorar o cabeçalio.
    fgets(linha, MAX_LINE, arquivo);

    while (fgets(linha, MAX_LINE, arquivo)) {
        char *games, *event, *team, *pos, *medal, *as, *athlete_id, *noc;
        
        games = strtok(linha, ","); //esses strtok servem para basicamentre separarem a string original pegada por "linha" e dividi-lá em partes.
        event = strtok(NULL, ","); //exemplo : 
        team = strtok(NULL, ",");
        pos = strtok(NULL, ",");
        medal = strtok(NULL, ",");
        as = strtok(NULL, ",");
        athlete_id = strtok(NULL, ",");
        noc = strtok(NULL, ",");

        if(!games || !medal || !noc){// se um desses for null, não há relevância, logo não deve nem ser contqbilizado, pq o que interessa são os campeões.
            continue;
        }

        if (strcmp(noc, pais_escolhido) == 0 && strlen(medal) > 0) {//verifica se o paíz escolhido é igual ao país da linha e verifica se há medalha.
            int ano = atoi(games);//pega apenas o início da string e converte até onde for número.

            if (ano < primeiro_ano) {// se esse ano fot maior que o primeiro ano...
                primeiro_ano = ano;//primeiro ano se tornará esse ano.
                strcpy(primeira_edicao, games);//aqui primeira edição receberá o "games" como valor.
            }
        }
    }
    fclose(arquivo);//fecha o arquivo após averiguar todos os dados

    if (primeiro_ano == 9999) {//se o aprimeiro ano continuar o mesmo, então o país não ganhou medalhas.
        printf("O país %s não ganhou medalhas.\n", pais_escolhido);
    } else {
        printf("Primeira medalha do país %s foi em: %s\n",pais_escolhido, primeira_edicao);
    }

    return 0;
}