//Para uma determinada edição dos jogos olímpicos, calcule o peso médio dos atletas medalhistas.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "questao5.h"
#include "../Bibliotecas/csvUtil.h"
#define MAX_LINE 262144

int contemElemento(int *lista, int tamanho, int valor) {
    for (int i = 0; i < tamanho; i++) {
        if (lista[i] == valor)
            return 1;//percorre toda a lista, se tiver na lista, retorna um, se não, retorna 0.
    }
    return 0;
}

int proximoCampo(char **ptr, char **campo) {
    char *p = *ptr;
    int dentroAspas = 0; // Flag para controlar se estamos dentro de aspas(o CSV tem muitas aspas).

    *campo = p; 

    while (*p) { // Enquanto não chegar ao final da string
        if (*p == '"') { // se p for aspas .
            dentroAspas = !dentroAspas; //alterar o valor da variável para informar que está entre aspas.
        } else if (*p == ',' && !dentroAspas) {// se p for igual a , e estiver fora das aspas.
            *p = '\0';
            *ptr = p + 1; // Atualiza ptr para apontar para o próximo campo
            return 1; // aqui ele já achou 0 campo inteiro.
        }
        p++; // Avança para o próximo caractere
    }

    *ptr = p;  // Atualiza ptr para o final da string, ou seja, a parte que foi pega no campo foi "removida".
    return 1;
}

int executarQuestao5(){
    //primeireo passo será guardar o ID de todos os atletas que ganharam algum jogo de determinada edição.

    double peso_total=0,qtdd_atletas=0;// esse será o que a questão pede.

    FILE *arquivo = abrirArquivoDados("results/results.csv", "r");//pega os arquivos com todos os jogos.
    int edicao_escolhida=9999;
    char linha[MAX_LINE];

    while (1==1){
        printf("Digite qual ano das olímpiadas você deseja calcular o peso médio:");
        scanf("%d", &edicao_escolhida);
        if(edicao_escolhida%2 != 0){//se for ímpar, não houve competição.
            printf("Nesse ano não ouve competição.");
            return 0;// realiza o fim do programa.
        }else{
            break;
        }
    }

    //agora precisamos fazer uma lista dinâmica para armazenar todos os ID's dos atletas que ganharma uma olípiada em determinado ano.
    int *lista = NULL; // aqui estarão os ids dos ganhadores de medalhas em determinado ano.
    int tamanho = 0; // aqui é para saber o tamanho da lista. Começa em 0. 

    fgets(linha, MAX_LINE, arquivo);//apenas para remover o cabeçalho.

    while(fgets(linha, MAX_LINE, arquivo)){//vai pegar linha por linha.

        //formato do arquivo: Games,Event,Team,Pos,Medal,As,athlete_id.
        char *games, *Event, *Team, *Pos, *Medal, *As, *athlete_id;

        //eu tenho que remover a parte que está entre aspas.
        //é aqui que a porca torce o rabo.
        //aparentemente, agora está funcionando corretamente.

        char *ptr = linha; //ponteiro que aponta para o a linha.
        char *campo;// ponteiro para armazenar o início do campo.

        //toda essa parte aqui é para separar a linha pelas informações corretas.
        //funcionamento dessas funções explicada na função acima.

        proximoCampo(&ptr, &games);
        proximoCampo(&ptr, &Event);
        proximoCampo(&ptr, &Team);
        proximoCampo(&ptr, &Pos);
        proximoCampo(&ptr, &Medal);
        proximoCampo(&ptr, &As);
        proximoCampo(&ptr, &athlete_id);


        // se a edição não for informada, pule para a próxima linha.
        if(games==NULL || strlen(games)==0){
            continue;
        }

        //se a edição que aconteceu o jogo for diferente, ele deve pular para a próxima linha, esse ano não interessa.
        if(edicao_escolhida!=(atoi(games))){
            continue;// Se o ano da competição for direfernte, ele não deve contabilizar.
        }

        //se ele não ganhou medalha, pule para a próxima linha.
        if (Medal==NULL || strlen(Medal) == 0) {
            continue; // não ganhou medalha
        }

        //se o id estiver algo e o id for maior que zero (garantir quer não é uma string vazia).
        if (athlete_id!=NULL && strlen(athlete_id) > 0 ) {//verifica se há String do id do atleta
            //printf("ANO: %d, Medal: '%s', ID: '%s'\n", atoi(games), Medal, athlete_id);
            if(contemElemento(lista,tamanho,atoi(athlete_id)) == 1){//verifica se esse atleta ja está na lista.
                continue;//se já estiver incluso, ele vai para a próxima 
            }
            // NO LOOP DO results.csv
            int id = atoi(athlete_id); // converte de string para int
            int *temp = realloc(lista, (tamanho + 1) * sizeof(int));//refaz a lista adicionando esse id.
            if (temp == NULL) {
                free(lista);
                printf("Erro de memória.\n");
                return 1;//precisa ter o return, pq se não dará erro.
            }
            lista = temp;//transformará a lista, na lista temporiaria feita.
            lista[tamanho] = id;// na nova posição, será adicionado o id do medalhista.
            tamanho++;//irá adicionar um ao tamanho.
        }
    }

    FILE *bio = fopen("arquivoscsvs/clean-data/bios.csv","r");

    fgets(linha, MAX_LINE, bio);//apenas para remover o cabeçalho.

    while(fgets(linha, MAX_LINE, bio)){//vai pegar linha por linha.

        //essa parte eu acho que está certo.
        //o arquivo original CSV é:
        //athlete_id,name,born_date,born_city,born_region,born_country,NOC,height_cm,weight_kg,died_date

        char *id, *name, *born, *born_city, *born_region, *born_country, *NOC, *height, *weight;//adicionando todos os campos que são necessários para aquestão.
        double peso;

        //aqui abaixo está o reconhecimendo dos dados:
        //usando o mesmo esqueleto da parte anterior:

        char *ptr = linha;
        char *campo;

        //toda essa parte aqui é para separar a linha pelas informações corretas.
        //funcionamento dessas funções explicada na função acima.
        
        proximoCampo(&ptr, &id);
        proximoCampo(&ptr, &name);
        proximoCampo(&ptr, &born);
        proximoCampo(&ptr, &born_city);
        proximoCampo(&ptr, &born_region);
        proximoCampo(&ptr, &born_country);
        proximoCampo(&ptr, &NOC);
        proximoCampo(&ptr, &height);
        proximoCampo(&ptr, &weight);

        // se o peso ou id não existir, ele deve ir para a próxima linha.

        if(weight == NULL || strlen(weight) == 0 || id==NULL || strlen(id)==0){
            continue;
        }

        // NO LOOP DO bios.csv  
        if(contemElemento(lista,tamanho,atoi(id)) == 1){//funçãpo para percorrer a lista e verificar se o id está presente na lista dos ganhadores.
            peso = atof(weight);//pega a string com o valor do peso e transforma em float.
            peso_total += peso;//adiciona esse peso ao peso total.
            qtdd_atletas++;//adiciona um atleta na quantidade de atletas total daquele ano.
        }
    }
    if(qtdd_atletas!=0){//se tiver algum campeão naquela olímpiada:
        printf("O peso médio na olímpiada do ano de %d foi de: %.2lf",edicao_escolhida,peso_total/qtdd_atletas);
    }
    fclose(bio);//não é mais necessário ter esse arquivo aberto.
    fclose(arquivo);//não é mais necessário ter esse arquivo aberto.
    free(lista);
    return 0;
}
 