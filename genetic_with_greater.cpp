#include <iostream>
#include <array>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>

using namespace std;

// --- CONSTANTES ---
const char EMPTY_CELL = ' '; // Define uma célula vazia para o tabuleiro
#define WIN 1
#define LOSS -1
#define DRAW 0

// --- ESTRUTURAS E CLASSES FUNDAMENTAIS ---

/**
 * @class BOARD
 * @brief Gerencia o estado do grid 3x3 do Jogo da Velha.
 */
class BOARD {
private:
    short int used_cells; // Conta o número de movimentos para verificar o empate

public:
    vector<char> grid; // ' ', 'X' ou 'O'

    /**
     * @brief Construtor: Cria um tabuleiro vazio.
     */
    BOARD() : used_cells(0), grid(9) {
        reset_board();
    }

    /**
     * @brief Reseta o tabuleiro para as configurações padrão.
     */
    void reset_board(void) {
        for (auto& symbol : grid)
            symbol = EMPTY_CELL;
        used_cells = 0;
    }

    /**
     * @brief Obtém o símbolo em uma célula específica.
     * @param x O índice da linha (0-2).
     * @param y O índice da coluna (0-2).
     * @return O símbolo ('X', 'O' ou ' ').
     */
    char get_cell(short int x, short int y) const {
        return grid[x*3 + y];
    }
    
    /**
     * @brief Checa se um movimento é válido.
     * @param x O índice da linha (0-2).
     * @param y O índice da coluna (0-2).
     * @return true se o movimento estiver dentro dos limites e a célula estiver vazia.
     */
    bool valid_move(short int x, short int y) {
        // Movimento fora dos limites
        if(x < 0 || x > 2 || y < 0 || y > 2)
            return false;

        // Célula já usada
        if(grid[x*3 + y] != EMPTY_CELL)
            return false;

        return true;
    }

    /**
     * @brief Imprime o estado atual do tabuleiro no console.
     */
    void draw_board(void) {
        cout << "-------------" << endl;
        for (int i = 0; i < 3; i++) {
            cout << "| ";
            for (int j = 0; j < 3; j++) {
                cout << grid[i*3 + j] << " | ";
            }
            cout << endl << "-------------" << endl;
        }
    }

    /**
     * @brief Checa se o tabuleiro está cheio (empate).
     * @return true se todas as 9 células estiverem usadas.
     */
    bool full(void) const {
        return used_cells == 9;
    }

    /**
     * @brief Coloca o símbolo de um jogador no tabuleiro.
     * @param player O símbolo do jogador ('X' ou 'O').
     * @param x O índice da linha (0-2).
     * @param y O índice da coluna (0-2).
     * @return true se o movimento foi bem-sucedido.
     */
    bool make_move(char player, short int x, short int y) {
        // Em um jogo real, você checaria a validade. Aqui, assumimos que o algoritmo
        // só tenta movimentos válidos, mas fazemos a checagem interna por segurança.
        if(!valid_move(x, y))
            return false;      
        
        grid[x*3 + y] = player;
        used_cells++;
        return true;
    }

    /**
     * @brief Checa se a última jogada resultou em uma vitória.
     * @param x A linha da última jogada.
     * @param y A coluna da última jogada.
     * @return true se a última jogada criou um 3-em-linha.
     */
    bool check_win(short int x, short int y) {
        char symbol = grid[x*3 +y];
        if(symbol == EMPTY_CELL)
            return false;

        // Vitória Horizontal
        if(grid[x*3] == symbol && grid[x*3 + 1] == symbol && grid[x*3 + 2] == symbol)
            return true;

        // Vitória Vertical
        if(grid[y] == symbol && grid[3 + y] == symbol && grid[6 + y] == symbol)
            return true;

        // Vitória Diagonal
        if(grid[0] != EMPTY_CELL && grid[0] == grid[4] && grid[0] == grid[8]) // Principal
            return true;
        if(grid[2] != EMPTY_CELL && grid[2] == grid[4] && grid[4] == grid[6]) // Secundária
            return true;

        return false;
    }
};

// --- FUNÇÕES CORE DO MINIMAX ---
// Estas funções são implementadas de forma genérica para serem usadas pelo MinimaxPlayer.

// Define a estrutura para um movimento
struct Move {
    int row, col;
};

/**
 * @brief Checa se há movimentos restantes no tabuleiro.
 */
bool isMoveLeft(const BOARD& b){
    for(int i = 0; i<3; i++)
        for(int j= 0; j<3;j++)
            if(b.get_cell(i,j)==EMPTY_CELL)
                return true;
    return false;
}

/**
 * @brief Avalia o estado atual do tabuleiro.
 * @param player O símbolo do Maximizador (AI).
 * @param opponent O símbolo do Minimizador (Oponente).
 * @return +10 se o player vencer, -10 se o opponent vencer, 0 caso contrário.
 */
int evaluate(const BOARD &b, char player, char opponent){
    // Checagem de Linhas
    for(int row = 0; row<3; row++){
        if(b.get_cell(row, 0) != EMPTY_CELL && 
           b.get_cell(row, 0) == b.get_cell(row, 1) &&
           b.get_cell(row, 1) == b.get_cell(row, 2))
        {
            if(b.get_cell(row, 0) == player) return +10;
            else if(b.get_cell(row,0) == opponent) return -10;
        }
    }
    // Checagem de Colunas
    for(int col = 0; col<3; col++){
        if(b.get_cell(0, col) != EMPTY_CELL && 
           b.get_cell(0, col) == b.get_cell(1, col) &&
           b.get_cell(1, col) == b.get_cell(2, col))
        {
            if(b.get_cell(0, col) == player) return +10;
            else if(b.get_cell(0, col) == opponent) return -10;
        }
    }

    // Checagem de Diagonais
    // Diagonal Principal
    if(b.get_cell(0,0) != EMPTY_CELL &&
       b.get_cell(0,0) == b.get_cell(1,1) && 
       b.get_cell(1,1) == b.get_cell(2,2))
    {
        if(b.get_cell(0, 0) == player) return +10;
        else if(b.get_cell(0,0) == opponent) return -10;
    }

    // Diagonal Secundária
    if(b.get_cell(0,2) != EMPTY_CELL &&
       b.get_cell(0,2) == b.get_cell(1,1) && 
       b.get_cell(1,1) == b.get_cell(2,0))
    {
        if(b.get_cell(0,2) == player) return +10;
        else if(b.get_cell(0,2) == opponent) return -10;
    }
    
    return 0; // Nenhum vencedor
}

/**
 * @brief A função recursiva Minimax.
 */
int minimax(BOARD board, int depth, bool isMax, char player, char opponent){
    int score = evaluate(board, player, opponent);
    
    // Condições terminais
    if(score == 10) return score - depth; // Maximizador vence
    if(score == -10) return score + depth; // Minimizador vence
    if(!isMoveLeft(board)) return 0; // Empate

    // Vez do Maximizador
    if(isMax){
        int best = -1000;
        for(int i = 0; i<3; i++){
            for(int j = 0; j<3; j++){
                if(board.get_cell(i, j) == EMPTY_CELL){
                    BOARD nb = board;
                    nb.make_move(player, i, j);
                    best = max(best, minimax(nb, depth+1, false, player, opponent));
                }
            }
        }
        return best;
    }
    // Vez do Minimizador
    else{
        int best = 1000;
        for(int i = 0; i<3; i++){
            for(int j = 0; j<3; j++){
                if(board.get_cell(i, j) == EMPTY_CELL){
                    BOARD nb = board;
                    nb.make_move(opponent, i, j);
                    best = min(best, minimax(nb, depth+1, true, player, opponent));
                }
            }
        }
        return best;
    }
}

/**
 * @brief Encontra o movimento ótimo usando Minimax.
 */
Move findBestMove(BOARD board, char player, char opponent){
    int bestVal = -1000;
    Move bestMove = {-1, -1};

    for(int i = 0; i<3; i++){
        for(int j = 0; j<3; j++){
            if(board.get_cell(i, j) == EMPTY_CELL){
                BOARD nb = board;
                nb.make_move(player, i, j);

                // Chama minimax: é a vez do Minimizador (false)
                int moveVal = minimax(nb, 0, false, player, opponent);

                if(moveVal > bestVal){
                    bestVal = moveVal;
                    bestMove = {i, j};
                }
            }
        }
    }
    return bestMove;
}

// --- CLASSE MINIMAX PLAYER (Adversário Ótimo) ---

/**
 * @class MinimaxPlayer
 * @brief Jogador automático que usa o algoritmo Minimax para seleção de movimento ideal.
 */
class MinimaxPlayer {
public:
    char symbol;
    char opponent_symbol;

    MinimaxPlayer(char s) : symbol(s) {
        opponent_symbol = (s == 'X' ? 'O' : 'X');
    }

    /**
     * @brief Escolhe o próximo movimento usando o Minimax.
     */
    pair<short, short> choose_move(const BOARD& board) {
        Move best_move = findBestMove(board, symbol, opponent_symbol);
        return {(short)best_move.row, (short)best_move.col};
    }
    
    // Métodos de BOT (manter para compatibilidade/clareza, mas não usados)
    void register_move(const vector<char>& grid, const short& x, const short& y) { /* Minimax não rastreia */ }
    void update_genomes(const short& result) { /* Minimax não tem genomas */ }
    void clear_history(void) { /* Minimax não tem histórico */ }
};

// --- CLASSE BOT (Jogador Evolutivo/Aprendiz) ---

/**
 * @class BOT
 * @brief Gerencia um jogador automático que evolui/aprende através de genomas (algoritmo genético simples).
 */
class BOT {
    private:
    vector<vector<char>> last_game;
    vector<pair<short, short>> moves;

    vector<char> rotate_90(vector<char> raw_grid) {
        vector<char> new_grid(9);
        for(int i =0; i<3; i++){
            for(int j =0; j<3; j++){
                new_grid[i * 3 + j] = raw_grid[(2-j) * 3 + i];
            }
        }
        return new_grid;
    }

    pair<vector<char>, pair<short, short>> get_canonical(const vector<char>& raw_grid, const pair<short, short>& raw_move) {
        vector<char> canon_grid = raw_grid;
        pair<short, short> canon_move = raw_move;
        
        // As 4 rotações (0, 90, 180, 270)
        vector<char> current_grid = raw_grid;
        pair<short, short> current_move = raw_move;

        for(int k = 0; k < 4; k++) {
            if (current_grid < canon_grid) {
                canon_grid = current_grid;
                canon_move = current_move;
            }

            // Próxima rotação
            current_grid = rotate_90(current_grid);
            current_move = {2 - current_move.second, current_move.first};
        }
        return {canon_grid, canon_move};
    }

    public:
    map<vector<char>, vector<int>> genomes;
    char symbol;

    BOT(char symbol = 'X') : symbol(symbol) {}

    void clear_history(void) {
        last_game.clear();
        moves.clear();
    }

    void register_move(const vector<char>& grid, const short& x, const short& y) {
        auto canon = get_canonical(grid, {x, y});
        last_game.push_back(canon.first);
        moves.push_back(canon.second);
    }

    void update_genomes(const short& result) {
        int reward = 0;

        if(result == WIN) {
            reward = 10;
        } else if(result == LOSS) {
            reward = -10;
        } else if(result == DRAW) {
            reward = 5;
        }

        for(size_t i = 0; i < last_game.size(); ++i) {
            const vector<char>& board = last_game[i];
            const pair<short, short>& move = moves[i];
            
            // Re-checar canonical, embora já devia estar
            auto canon = get_canonical(board, move);
            vector<char>& canon_board = canon.first;
            pair<short, short> canon_move = canon.second;

            if(genomes.count(canon_board) == 0) {
                // Inicializa com pontuações aleatórias (1 a 10)
                genomes[canon_board].resize(9);
                for(int& score : genomes[canon_board])
                    score = rand() % 10 + 1;
            }
            
            short move_index = canon_move.first * 3 + canon_move.second;
            genomes[canon_board][move_index] += reward;

            if (genomes[canon_board][move_index] < 1) {
                genomes[canon_board][move_index] = 1; // Garante que a pontuação é positiva
            }
        }
    }

    /**
     * @brief Escolhe o movimento do BOT Evolutivo com base nos genomas (Roleta).
     */
    pair<short, short> choose_move(BOARD board) {
        // Encontra a forma canônica do tabuleiro atual
        vector<char> current_grid = board.grid;
        // Move fictício para obter o grid canônico.
        auto canon_result = get_canonical(current_grid, {0, 0}); 
        const vector<char>& canon_grid = canon_result.first;


        // 1. Randomiza se for um estado novo (ainda não aprendido)
        if(genomes.count(canon_grid) == 0) {
            vector<pair<short, short>> valid_moves;
            for(short x = 0; x < 3; ++x) 
                for(short y = 0; y < 3; ++y) 
                    if(board.valid_move(x, y)) 
                        valid_moves.push_back({x, y});

            int random_index = rand() % valid_moves.size();
            return valid_moves[random_index];
        }

        // 2. Seleção por Roleta (ponderada pelo genoma)
        vector<pair<pair<short, short>, int>> valid_moves;
        int sum_of_scores = 0;
        const vector<int>& scores = genomes[canon_grid];

        for(short x = 0; x < 3; ++x) {
            for(short y = 0; y < 3; ++y) {
                if(board.valid_move(x, y)) {                    
                    // Acessamos o genoma pela chave CANÔNICA
                    valid_moves.push_back({{x, y}, scores[x*3 + y]});
                    sum_of_scores += scores[x*3 + y];
                }
            }
        }
        
        // Se a soma for zero (o que não deve acontecer se todos os genomas tiverem floor 1)
        if (sum_of_scores == 0) {
             return valid_moves.back().first;
        }

        int random_pick = rand() % sum_of_scores;
        int current_sum = 0;
        for(auto& move : valid_moves) {
            current_sum += move.second;
            if(random_pick < current_sum) {
                return move.first;
            }
        }

        return valid_moves.back().first;
    }

    void print_genome(const BOARD &board, pair<short, short>& move) {
        // Encontra a forma canônica do tabuleiro atual
        vector<char> current_grid = board.grid;
        auto canon_result = get_canonical(current_grid, {0, 0}); 
        const vector<char>& canon_grid = canon_result.first;

        if(genomes.count(canon_grid) == 0){
            cout << "Este estado não tem registros\n";
            return;
        }
        // Imprime os scores canônicos no formato do grid real
        for(int i = 0; i < 9; ++i) {
            cout << genomes[canon_grid][i] << " ";
            if ((i + 1) % 3 == 0) cout << "| ";
        }
        cout << endl;
    }

    bool save_genomes(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Erro: Não foi possível abrir o arquivo para escrita: " << filename << endl;
            return false;
        }

        for (const auto& entry : genomes) {
            const vector<char>& board_key = entry.first;
            const vector<int>& scores = entry.second;

            for (int i = 0; i < 9; ++i) {
                file << (board_key[i] == EMPTY_CELL ? '_' : board_key[i]);
            }
            
            file << " :";

            for (int score : scores) {
                file << " " << score;
            }
            file << "\n";
        }

        file.close();
        return true;
    }

    bool load_genomes(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Info: Arquivo de genomas não encontrado: " << filename << ". Iniciando com genomas vazios." << endl;
            return false;
        }

        genomes.clear();
        string line;
        int line_count = 0;

        while (getline(file, line)) {
            line_count++;
            stringstream ss(line);
            string key_str;
            string separator;
            
            ss >> key_str >> separator;

            if (key_str.length() != 9 || separator != ":") {
                cerr << "Aviso: Pulando linha malformada " << line_count << ": " << line << endl;
                continue;
            }

            vector<char> board_key(9);
            for (int i = 0; i < 9; ++i) {
                board_key[i] = (key_str[i] == '_' ? EMPTY_CELL : key_str[i]);
            }

            vector<int> scores(9);
            bool read_success = true;
            for (int i = 0; i < 9; ++i) {
                if (!(ss >> scores[i])) {
                    read_success = false;
                    break;
                }
            }

            if (!read_success) {
                cerr << "Aviso: Pulando scores malformados na linha " << line_count << endl;
                continue;
            }

            genomes[board_key] = scores;
        }

        file.close();
        return true;
    }
};

// --- CLASSE DO JOGO (Estrutura de Treinamento) ---

/**
 * @class TicTacToeBOT
 * @brief Gerencia o loop do jogo entre um MinimaxPlayer e um BOT evolutivo.
 */
class TicTacToeBOT {
private:
    bool curr_player; // 0 = Player 'X' (Minimax), 1 = Player 'O' (BOT)
    BOARD board;

    MinimaxPlayer* minimax_player;
    BOT* evolutionary_bot;

    void switch_player(void) {
        curr_player = !curr_player;
    }

public:
    TicTacToeBOT(MinimaxPlayer& X, BOT& O) : curr_player(0), board() {
        if (X.symbol != 'X' || O.symbol != 'O') {
            cerr << "Erro: TicTacToeBOT requer MinimaxPlayer como 'X' e BOT como 'O'." << endl;
        }
        minimax_player = &X;
        evolutionary_bot = &O;
    }

    /**
     * @brief Executa um jogo entre o Minimax ('X') e o Bot Evolutivo ('O').
     */
    short botVSbot(const bool& print = true) {
        evolutionary_bot->clear_history(); 
        board.reset_board();

        pair<short, short> move = {-1, -1};
        short result = DRAW;
        
        while(true) {
            if(print)
                board.draw_board();
            
            char current_symbol;
            if (curr_player == 0) { // Minimax Player ('X')
                current_symbol = minimax_player->symbol;
                if(print) cout << "Player " << current_symbol << " (Minimax) move: ";
                move = minimax_player->choose_move(board);
            } else { // Evolutionary BOT ('O')
                current_symbol = evolutionary_bot->symbol;
                if(print) {
                    cout << "Scores (Bot 'O') : ";
                    evolutionary_bot->print_genome(board, move);
                    cout << "Player " << current_symbol << " (BOT) move: ";
                }
                move = evolutionary_bot->choose_move(board);
                
                // O BOT Evolutivo deve registrar o estado do tabuleiro *antes* de fazer o movimento
                // para que o genoma mapeie (estado -> melhor movimento).
                evolutionary_bot->register_move(board.grid, move.first, move.second);
            }

            if(print)
                cout << move.first << " " << move.second << endl;
            
            board.make_move(current_symbol, move.first, move.second);

            // Checagem de vitória/empate
            if(board.check_win(move.first, move.second)) {
                if(print) {
                    board.draw_board();
                    cout << "Player " << current_symbol << " won!\n";
                }
                
                if (curr_player == 0) { // Minimax ganhou
                    evolutionary_bot->update_genomes(LOSS);
                    result = WIN;
                } else { // Bot Evolutivo ganhou
                    evolutionary_bot->update_genomes(WIN);
                    result = LOSS;
                }
                break;
            }

            if(board.full()) {
                if(print) {
                    board.draw_board();
                    cout << "It's a draw!\n";
                }
                evolutionary_bot->update_genomes(DRAW);
                break;
            }

            switch_player();
        }
        return result;
    }
};

