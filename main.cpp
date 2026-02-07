#include "header_files/Population.h"
using namespace std;

int main(void) {
    POPULATION p;

    int opc;
    char symbol = ' ';
    
    cout << "------------ MENU -------------\n";
    cout << "Choose 1 to train the population against an optimal algorithm\n";
    cout << "Choose 2 to play against the bot\n";
    cout << "Choose 3 to have the bots compete against each other\n";
    cin >> opc;

    switch (opc)
    {
    case 1:
        p.train_botvsminimax(true,true);
        break;
    
    case 2:
        cout << "Choose your symbol ('X' or 'O'): ";
        do {
            cin >> symbol;
        } while(symbol != 'X' && symbol != 'O');

        p.train_botvsplayer(true, true, symbol == 'X');
        break;

    case 3:
        p.train_botvsbot(true, true);
    
    default:
        break;
    }
    
    return 0;
}