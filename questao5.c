//Para uma determinada edição dos jogos olímpicos, calcule o peso médio dos atletas medalhistas.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 262144

int contemElemento(int *lista, int tamanho, int valor) {
    for (int i = 0; i < tamanho; i++) {
        if (lista[i] == valor)
            return 1;
    }
    return 0;
}

void transformarAspasParaZero(char *texto) {
    int dentro = 0;
    for (int i = 0; texto[i]; i++) {
        if (texto[i] == '"')
            dentro = !dentro, texto[i] = '0';
        else if (dentro)
            texto[i] = '0';
    }
}

int main(){
    //primeireo passo será guardar o ID de todos os atletas que ganharam algum jogo de determinada edição.

    double peso_total=0,qtdd_atletas=0;// esse será o que a questão pede.

    FILE *arquivo = fopen("arquivoscsvs/results/results.csv","r");//pega os arquivos com todos os jogos.
    int edicao_escolhida=9999;
    char linha[MAX_LINE];

    while (1==1){
        printf("Digite qual edição das olímpiadas você deseja calcular o peso médio:");
        scanf("%d", &edicao_escolhida);
        if(edicao_escolhida%2 != 0){
            printf("Nesse ano não ouve competição.");
            return 0;// realiza o fim do programa.
        }else{
            break;
        }
    }

    //agora precisamos fazer uma lista dinâmica para armazenar todos os ID's dos atletas que ganharma uma olípiada em determinado ano.
    int *lista = NULL;
    int tamanho = 0;

    fgets(linha, MAX_LINE, arquivo);//apenas para remover o cabeçalho.
    int ganhadores=0;

    while(fgets(linha, MAX_LINE, arquivo)){


        transformarAspasParaZero(linha);

        //formato do arquivo: Games,Event,Team,Pos,Medal,As,athlete_id.
        char *games, *Event, *Team, *Pos, *Medal, *As, *athlete_id;

        //eu tenho que remover a parte que está entre aspas.
        

        games = strtok(linha, ",");
        Event = strtok(NULL, ",");
        Team = strtok(NULL, ",");
        Pos = strtok(NULL, ",");
        Medal = strtok(NULL, ",");
        As = strtok(NULL, ",");
        athlete_id = strtok(NULL, ","); 

        if(games==NULL || strlen(games)==0){
            continue;
        }

        if(edicao_escolhida!=(atoi(games))){
            continue;// Se o ano da competição for direfernte, ele não deve contabilizar.
        }

        if (Medal==NULL || strlen(Medal) == 0) {
            continue; // não ganhou medalha
        }

        //tá dando erro de pegar os campos errados, o csv tem muitas "".

        if (athlete_id!=NULL && strlen(athlete_id) > 0 ) {//verifica se há String do id do atleta
            printf("ANO: %d, Medal: '%s', ID: '%s'\n", atoi(games), Medal, athlete_id);
            if(contemElemento(lista,tamanho,atoi(athlete_id)) == 1){//verifica se esse atleta ja está na lista.
                continue;
            }
            // NO LOOP DO results.csv
            int id = atoi(athlete_id); // converte de string para int
            int *temp = realloc(lista, (tamanho + 1) * sizeof(int));//refaz a lista adicionando esse id.
            if (temp == NULL) {
                free(lista);
                printf("Erro de memória.\n");
                return 1;//precisa ter o return, pq se não dará erro.
            }
            lista = temp;
            lista[tamanho] = id;
            tamanho++;
            ganhadores++;
        }
    }
    printf("Total de atletas medalhistas únicos: %d\n", ganhadores);

    FILE *bio = fopen("arquivoscsvs/clean-data/bios.csv","r");

    fgets(linha, MAX_LINE, bio);//apenas para remover o cabeçalho.    

    while(fgets(linha, MAX_LINE, bio)){

        //essa parte eu acho que está certo.
        //o arquivo original CSV é:
        //athlete_id,name,born_date,born_city,born_region,born_country,NOC,height_cm,weight_kg,died_date

        // Pula linhas vazias
        if(strlen(linha) < 2){
            continue;
        } 

        char *id, *name, *born, *born_city, *born_region, *born_country, *NOC, *height, *weight;
        double peso;

        transformarAspasParaZero(linha);

        //aqui abaixo está o reconhecimendo dos dados:

        id = strtok(linha, ",");//parte que também nos interessa.
        name = strtok(NULL, ",");
        born = strtok(NULL, ",");
        born_city = strtok(NULL, ",");
        born_region = strtok(NULL, ",");
        born_country = strtok(NULL, ",");
        NOC = strtok(NULL, ",");
        height = strtok(NULL, ",");
        weight = strtok(NULL, ",");//parte que nos interessa.

        if(weight == NULL || strlen(weight) == 0 || id==NULL || strlen(id)==0){
            continue;
        }
        if(strlen(weight) < 2 || strlen(weight)> 6 ){
            continue;
        }

        // NO LOOP DO bios.csv  
        //printf("ID: '%s', Weight: '%s'\n", id, weight); teste para vê se pegava corretamnete o ID e o peso.
        if(contemElemento(lista,tamanho,atoi(id)) == 1){//funçãpo para percorrer a lista e verificar se o id está presente na lista dos ganhadores.
            peso = atof(weight);
            peso_total += peso;
            qtdd_atletas++;//adiciona um atleta na quantidade de atletas total daquele ano.
        }
    }
    if(qtdd_atletas!=0){
        printf("O peso médio na olímpiada do ano de %d foi de: %.2lf",edicao_escolhida,peso_total/qtdd_atletas);
    }
    fclose(bio);//não é mais necessário ter esse arquivo aberto.
    fclose(arquivo);//não é mais necessário ter esse arquivo aberto.
    free(lista);
    return 0;
}
 