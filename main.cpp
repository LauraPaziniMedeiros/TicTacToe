#include <iostream>
#include <string>
#include <time.h>
#include "population.h" 
#include "Play.h"       

using namespace std;

void modo_treinamento() {
    POPULATION p;
    int escolha;
    cout << "\n--- MODO DE TREINAMENTO ---\n";
    cout << "1. Treinar Bot vs Bot\n";
    cout << "2. Treinar Bot vs Minimax (jogador ideal)\n";
    cout << "Escolha: ";
    cin >> escolha;

    bool carregar;
    cout << "Carregar genomas existentes? (1-Sim, 0-Nao): ";
    cin >> carregar;

    cout << "Iniciando treinamento...\n";
    if (escolha == 1) {
        p.train_population(true, carregar); 
    } else {
        p.train_population_minimax(true, carregar);
    }
    cout << "Treinamento concluido.\n";
}

void modo_jogo() {
    string nome_arquivo;
    cout << "\n--- MODO DE JOGO (HUMANO VS BOT) ---\n";
    cout << "Digite o nome do arquivo de genoma para carregar (ex: X0.txt): ";
    cin >> nome_arquivo;

    BOT meuBot('O'); 
    
    if(!meuBot.load_genomes(nome_arquivo)) {
        cout << "ERRO: Nao foi possivel carregar " << nome_arquivo << endl;
        cout << "O bot jogara com movimentos aleatorios.\n";
    } else {
        cout << "Cérebro carregado com sucesso!\n";
    }

    TicTacToeUserVsBot game(meuBot);
    char continuar = 's';
    
    while(continuar == 's' || continuar == 'S') {
        int quem_comeca;
        cout << "Quem comeca? (1-Voce, 0-Bot): ";
        cin >> quem_comeca;
        
        game.play(quem_comeca == 1);

        cout << "Deseja salvar o aprendizado desta partida? (s/n): ";
        char salvar;
        cin >> salvar;
        if(salvar == 's' || salvar == 'S') {
            meuBot.save_genomes(nome_arquivo);
            cout << "Aprendizado salvo em " << nome_arquivo << endl;
        }

        cout << "Jogar novamente? (s/n): ";
        cin >> continuar;
    }
}

int main(void) {
    srand(time(NULL)); 

    while(true) {
        int opcao;
        cout << "\n==============================\n";
        cout << "   JOGO DA VELHA EVOLUTIVO      \n";
        cout << "==============================\n";
        cout << "1. Treinar Populacao (Evoluir)\n";
        cout << "2. Jogar contra o Bot\n";
        cout << "3. Sair\n";
        cout << "Escolha uma opcao: ";
        
        if(scanf("%d", &opcao) == 1){
            switch(opcao) {
                case 1:
                    modo_treinamento();
                    break;
                case 2:
                    modo_jogo();
                    break;
                case 3:
                    cout << "Saindo...\n";
                    return 0;
                default:
                    cout << "Opção invalida! Digite um número de 1 a 3.\n";
            }
        }else{
            printf("Entrada inválida. Digite apenas números no menu.\n");
            while(getchar() != '\n');
        }
    }
    return 0;
}