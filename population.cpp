#include "genetic_algorithm.cpp"
#include <algorithm>

// Config
#define INDIVIDUALS 6
float MIN_MUT = 0.05, MAX_MUT = 0.3;
float MUTATION_STEP = (MAX_MUT - MIN_MUT)*2;
#define ROUNDS 100
#define CROSSOVER_ROUNDS 5

class POPULATION {
    private:
    // Stores the population and each individual's win rate
    vector<pair<BOT, int>> pop;
    // The best individual is always stored and unchanged
    pair<BOT, int> BEST;
    // How many rounds the BEST bot has stayed the same
    int stagnation;
    // Current mutation rate
    float MUTATION_RATE;

    public:
    /**
     * @brief creates a population of bots (alternating symbols) and with 0 wins.
     */
    POPULATION() : pop(INDIVIDUALS), BEST(), stagnation(0), MUTATION_RATE(MIN_MUT) {
        for(int i = 0; i < INDIVIDUALS; i += 2) {
            BOT aux('X');
            pop[i] = {aux, 0};
        }
        //for(int i = 1; i < INDIVIDUALS; i += 2) {
          //  BOT aux('O');
            //pop[i] = {aux, 0};
        //}
        
        BEST = {pop[0]};
    }

    void update_mutation_rate() {
        float factor = min(1.0, stagnation / 10.0); 

        MUTATION_RATE = MIN_MUT + (MAX_MUT - MIN_MUT) * factor;
    }

    vector<long long> mutate(vector<long long> genome)
    {
        vector<long long> mutated;
        // Instead of using the old rand() from C, I am using the random library from C++. rd is the random device that we use to generate the pseudo-random numbers, and rng is the generator, using Mersenne-Twister algorithm.
        random_device rd;
        mt19937 rng(rd());

        // The chance of mutating is defined by a random applied into an uniform distribution, while the noise is defined by a random choose in a normal distribution
        uniform_real_distribution<double> chanceDist(0.0, 1.0);
        normal_distribution<double> noiseDist(0.0, MUTATION_STEP);

        // This part of the code is the same, I just changed the calculations of the noise and the ceiling for the random number to use standard C++ functions.
        for (auto &g : genome)
        {
            if (chanceDist(rng) <= MUTATION_RATE)
            {
                int noise = noiseDist(rng);
                int m = g + static_cast<int>(std::round(noise));
                m = std::max(1, m);
                mutated.push_back(m);
            }
            else
                mutated.push_back(g);
        }
        return mutated;
    }

    void crossover(void) {
        // Sorting based on win rate
        sort(pop.begin(), pop.end(), 
            [](const pair<BOT, int>& a, const pair<BOT, int>& b) {return a.second > b.second;});

        // the best crosses over with every other individual and creates a new population
        vector<pair<BOT, int>> new_pop;
        // Updates BEST and the stagnation rate
        if(BEST.second < pop[0].second) {
            BEST = pop[0];
            stagnation = 0;
        }
        else
            stagnation++;

        new_pop.push_back(BEST);
            
        for(int i = 1; i < INDIVIDUALS; i++) {
            BOT child;
            //if(i % 2) child.symbol = 'X'; else child.symbol = 'O';
            // The child has all the BEST's genomes
            child.symbol = 'X';
            child.genomes = BEST.first.genomes;

            for(auto& [board_state, genome] : pop[i].first.genomes) {
                if(child.genomes.count(board_state)) { // Both parents have this genome
                    // Average of both parent's genomes
                    for(int j = 0; j < 9; j++) {
                        child.genomes[board_state][j] += genome[j];
                        child.genomes[board_state][j] /= 2;
                    }
                }
                else // Only the current individual has this genome
                    child.genomes[board_state] = genome;
                // Applies mutation
                update_mutation_rate();
                child.genomes[board_state] = mutate(child.genomes[board_state]);
            }
            // Win rate is the average between the parent's last win rate
            new_pop.push_back({child, (BEST.second + pop[i].second) / 2});
        }
        pop = new_pop;
    }

    void train_population(bool print = false, bool save_load = false) {
        if(save_load) {
            for(int i = 0; i < INDIVIDUALS; i += 2) {
                BOT p1('X'), p2('O');
                string file_name = "X";
                file_name.push_back(i + '0');
                file_name.append(".txt");
                p1.load_genomes(file_name);
                file_name[0] = 'O';
                p1.load_genomes(file_name);
                pop[i] = {p1, 0};
                pop[i+1] = {p2, 0};
            }
        }

        // Setup Random Number Generator
        auto rng = default_random_engine(time(NULL));

        pair<int, pair<int, int>> winrate_table[INDIVIDUALS];
        for (int i = 0; i < INDIVIDUALS; i++){
            winrate_table[i] = {0, {0, 0}};
        }
        for(int j = 0; j < ROUNDS; j++) {
            // This ensures random matchmaking every generation
            shuffle(pop.begin(), pop.end(), rng);

            // Simulates rounds and generates new populations
            for(int i = 0; i < INDIVIDUALS; i += 2) {
                pop[i].first.symbol = 'X';
                pop[i+1].first.symbol = 'O';
                TicTacToeBOT game(pop[i].first, pop[i+1].first);
                int result = game.botVSbot(print);
                if (result == WIN) {
                    pop[i].second += 1;
                    pop[i+1].second -= 1;
                    winrate_table[i].first++;
                    winrate_table[i+1].second.second++;
                } else if (result == LOSS) {
                    pop[i].second -= 1;
                    pop[i + 1].second += 1;
                    winrate_table[i + 1].first++;
                    winrate_table[i].second.second++;
                } else if (result == DRAW) {
                    winrate_table[i].second.first++;
                    winrate_table[i+1].second.first++;
                }
            }
            for (int i = 0; i < INDIVIDUALS; ++i)
            {
                cout << "WIN/DRAW RATE BOT " << i << ": WINS: " << winrate_table[i].first << " DRAWS: " << winrate_table[i].second.first << " LOSSES: " << winrate_table[i].second.second << endl;
            }
            if(j % CROSSOVER_ROUNDS == 0) // Creates a new generation every defined number of rounds
                crossover();
        }
        if(save_load) {
            for(int i = 0; i < INDIVIDUALS; i += 2) {
                string file_name = "";
                file_name += pop[i].first.symbol;
                file_name += i + '0' ;
                file_name.append(".txt");
                pop[i].first.save_genomes(file_name);
                file_name[0] = pop[i+1].first.symbol;
                pop[i+1].first.save_genomes(file_name);
            }
        }        
    }

    void train_population_minimax(bool print = false, bool save_load = false) {
    
    // Supondo que pop é um vetor de BOTs de Aprendizado (pop.size() == INDIVIDUALS)

    if (save_load) {
        // Carregamento: Assumindo que todos os BOTs são 'X'
        for (int i = 0; i < INDIVIDUALS; ++i) {
            string file_name = "X";
            file_name.push_back(i + '0');
            file_name.append(".txt");
            pop[i].first.load_genomes(file_name); // pop[i].first é o BOT
        }
    }

    // 1. Instancia o Minimax Player fixo, que será o oponente 'O'
    Optimal_algorithm fixed_minimax_o('O');
    
    // Setup Random Number Generator (Não mais necessário para shuffle, mas mantido se for usado no crossover)
    //auto rng = default_random_engine(time(NULL));

    pair<int,pair<int, int>> winrate_table[INDIVIDUALS];
    for(int i = 0; i < INDIVIDUALS; i++){
        winrate_table[i] = {0, {0, 0}};
    }

    for (int j = 0; j < ROUNDS; j++) {
        // ***********************************************
        // REMOVER: Não há necessidade de shuffle se o oponente é fixo.
        // ***********************************************
        // shuffle(pop.begin(), pop.end(), rng); 

        // Simulates rounds
        for (int i = 0; i < INDIVIDUALS; ++i) { // Iterar sobre todos os BOTs 'X'
            
            // 2. Cria o controlador TicTacToeMiniMax
            // É necessário um construtor que aceite um BOT X e um Minimax O
            // Seu construtor atual é: TicTacToeMiniMax() : X('X'), minimax_o('O').
            // Se você quer que o BOT pop[i].first seja testado, você precisa de um construtor
            // ou uma maneira de substituir o BOT interno da classe.
            
            // *** ASSUMINDO NOVO CONSTRUTOR: TicTacToeMiniMax(BOT& x_bot, Optimal_algorithm& o_bot) ***

            // Exemplo de como usar sua estrutura atual (se o BOT for movido/copiado):
            // Isso pode ser complexo, a melhor abordagem é criar um novo controlador
            // que aceite o BOT a ser treinado e o Minimax fixo.
            
            TicTacToeMiniMax game;
            game.X = pop[i].first; // Atribui o BOT de Aprendizado atual ao controlador
            // game.minimax_o já está inicializado como 'O'

            // 3. Roda o jogo: BOT 'X' vs. Minimax 'O'
            // O resultado é do ponto de vista do BOT 'X'.
            int result = game.botVSminimax(print); 

            // 4. Atualiza a pontuação e substitui o BOT (agora com genomas atualizados)
            
            // O update_genomes já ocorreu dentro de game.botVSminimax
            // Aqui, apenas atualizamos o BOT na população e sua pontuação de performance.
            pop[i].first = game.X; 

            if (result == WIN) {
                pop[i].second += 1;
                winrate_table[i].first++;
            } else if (result == LOSS) {
                pop[i].second -= 1;
                winrate_table[i].second.second++;
            } else if (result == DRAW) {
                winrate_table[i].second.first++;
            }
            // Se for DRAW (0), a pontuação (pop[i].second) não muda.
        }

        // 5. Impressão e Crossover (ajustado para INDIVIDUALS)
        for (int i = 0; i < INDIVIDUALS; ++i)
        {
            cout << "WIN/DRAW RATE BOT " << i << ": WINS: " << winrate_table[i].first << " DRAWS: " << winrate_table[i].second.first << " LOSSES: " << winrate_table[i].second.second << endl;
        }

        if (j % CROSSOVER_ROUNDS == 0) // Cria uma nova geração a cada X rodadas
            crossover();
    }
    
    if (save_load) {
        // Salvamento: Assumindo que todos os BOTs são 'X'
        for (int i = 0; i < INDIVIDUALS; ++i) {
            string file_name = "X";
            file_name.push_back(i + '0');
            file_name.append(".txt");
            pop[i].first.save_genomes(file_name);
        }
    }
}
};

int main(void) {
    POPULATION p;
    //p.train_population(true, true);
    p.train_population_minimax(true,false);
    return 0;
}