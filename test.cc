#include <bits/stdc++.h>
#include "player_exception.h"

using namespace std;

int main() {
    try {
        throw OutOfBoundsException();
    }
    catch(const PlayerException& e) {
        cout << e.what() << endl;
    }
    
    return 0;
}
