#include <iostream>
#include <string>
using namespace std;

// ---------- Stack (array based) ----------
class Stack {
    int arr[100];
    int top;
public:
    Stack() { top = -1; }
    bool isEmpty() { return top == -1; }
    bool isFull()  { return top == 99; }
    int  size()    { return top + 1; }
    void push(int x) { if (!isFull()) arr[++top] = x; }
    int  pop()  { return isEmpty() ? -1 : arr[top--]; }
    int  peek() { return isEmpty() ? -1 : arr[top]; }
};

// ---------- Player name ----------
string playerName = "Student";

// Replaces every {name} in a string with the player's name
string withName(string s) {
    size_t pos = 0;
    while ((pos = s.find("{name}", pos)) != string::npos) {
        s.replace(pos, 6, playerName);
        pos += playerName.size();
    }
    return s;
}

// ---------- Items ----------
const int NO_ITEM = -1;
const int WAND = 0;
const int SPELLBOOK = 1;
string itemNames[2] = { "your wand", "the Alohomora spellbook" };
bool inventory[2] = { false, false };

// ---------- Houses (0 = no house) ----------
const int GRYFFINDOR = 1;
const int HUFFLEPUFF = 2;
const int RAVENCLAW  = 3;
const int SLYTHERIN  = 4;
string houseNames[5] = { "", "Gryffindor", "Hufflepuff", "Ravenclaw", "Slytherin" };
string houseLines[5] = {
    "",
    "Brave and bold, you charge ahead without fear. Better be GRYFFINDOR!",
    "Loyal and kind, you never leave a friend behind. Better be HUFFLEPUFF!",
    "Sharp and curious, your mind never rests. Better be RAVENCLAW!",
    "Ambitious and cunning, you will go far. Better be SLYTHERIN!"
};
int scores[5] = { 0, 0, 0, 0, 0 };

// ---------- Game data ----------
struct Option {
    string text;
    int nextPage;
    int needsItem;  // item needed, or NO_ITEM
    int house;      // house this answer gives points to (0 = none)
    int weight;     // how many points it gives
};

struct Page {
    string text;
    int numOptions;
    Option options[4];
    int gives;       // item you pick up here, or NO_ITEM
    bool isWin;      // only matters for endings (numOptions == 0)
    bool isSorting;  // true on the page where the Hat announces your house
};

Page pages[15] = {
    // 0 - Entrance Hall
    { "Welcome to Hogwarts, {name}! You arrive as a new fifth-year student. The Sorting Hat ceremony "
      "starts at midnight, but strange whispers echo through the Entrance Hall.",
      3, { {"Visit the Room of Requirement", 1, NO_ITEM},
           {"Sneak into the Library", 2, NO_ITEM},
           {"Head to the Great Hall", 3, NO_ITEM} }, NO_ITEM, false },

    // 1 - Room of Requirement
    { "The Room of Requirement shapes itself around you. A wand rests on a pedestal, "
      "and a hidden door glows faintly in the wall.",
      2, { {"Take the wand", 11, NO_ITEM},
           {"Open the hidden door", 4, NO_ITEM} }, NO_ITEM, false },

    // 2 - Library
    { "The Library is silent. On a dusty shelf you find a spellbook with the Unlocking Charm, Alohomora. "
      "At the back, an iron gate guards the Restricted Section.",
      2, { {"Unlock the Restricted Section gate", 5, SPELLBOOK},
           {"Return to the Entrance Hall", 0, NO_ITEM} }, SPELLBOOK, false },

    // 3 - Great Hall
    { "The Great Hall is full of floating candles. The Sorting Hat sits on its stool, waiting for you. "
      "\"{name}!\" calls the professor. All eyes turn to you.",
      2, { {"Put on the Sorting Hat", 12, NO_ITEM},
           {"Run away from the ceremony", 7, NO_ITEM} }, NO_ITEM, false },

    // 4 - Hidden corridor
    { "Behind the door is a dark corridor. A huge mountain troll blocks your way, sniffing the air!",
      2, { {"Cast a spell at the troll", 8, WAND},
           {"Punch the troll", 9, NO_ITEM} }, NO_ITEM, false },

    // 5 - Restricted Section
    { "The gate swings open. Among the forbidden books, one glows with ancient magic and whispers "
      "your name: \"{name}...\"",
      1, { {"Open the glowing book", 10, NO_ITEM} }, NO_ITEM, false },

    // 6 - Sorting result (ending, win)
    { "The Hat is silent for a moment. Then it speaks, loud enough for the whole hall to hear...",
      0, {}, NO_ITEM, true, true },

    // 7 - ending (lose)
    { "Filch catches you wandering the corridors after curfew. Detention for the rest of the year, {name}.",
      0, {}, NO_ITEM, false },

    // 8 - ending (win)
    { "Your spell sends the troll flying! The professors arrive and award your house 50 points. "
      "Well done, {name}, you are a hero!",
      0, {}, NO_ITEM, true },

    // 9 - ending (lose)
    { "The troll was not impressed, {name}. You wake up in the Hospital Wing with a very sore arm.",
      0, {}, NO_ITEM, false },

    // 10 - ending (win)
    { "The book reveals you carry ancient magic. The Headmaster smiles: the castle has been waiting "
      "for you, {name}.",
      0, {}, NO_ITEM, true },

    // 11 - wand pickup
    { "You pick up the wand. It warms in your hand and sparks of light dance from the tip.",
      1, { {"Go back to the Room of Requirement", 1, NO_ITEM} }, WAND, false },

    // 12 - Sorting Hat question 1 (worth 3 points)
    { "The Hat settles over your eyes. \"Hmm, a difficult one, {name}. Tell me, what do you value most?\"",
      4, { {"Bravery", 13, NO_ITEM, GRYFFINDOR, 3},
           {"Loyalty", 13, NO_ITEM, HUFFLEPUFF, 3},
           {"Wisdom", 13, NO_ITEM, RAVENCLAW, 3},
           {"Ambition", 13, NO_ITEM, SLYTHERIN, 3} }, NO_ITEM, false },

    // 13 - Sorting Hat question 2 (worth 2 points)
    { "\"Interesting... Now, you find a bag of gold lying in an empty corridor. What do you do?\"",
      4, { {"Use it to fund a daring adventure", 14, NO_ITEM, GRYFFINDOR, 2},
           {"Search for its owner, however long it takes", 14, NO_ITEM, HUFFLEPUFF, 2},
           {"Study it for clues about who lost it", 14, NO_ITEM, RAVENCLAW, 2},
           {"Keep it. Gold can open many doors", 14, NO_ITEM, SLYTHERIN, 2} }, NO_ITEM, false },

    // 14 - Sorting Hat question 3 (worth 2 points)
    { "\"Last question, {name}. A friend is in danger. What do you do?\"",
      4, { {"Rush in without thinking", 6, NO_ITEM, GRYFFINDOR, 2},
           {"Stand by them, no matter what", 6, NO_ITEM, HUFFLEPUFF, 2},
           {"Work out a clever plan", 6, NO_ITEM, RAVENCLAW, 2},
           {"Do whatever it takes, even bend the rules", 6, NO_ITEM, SLYTHERIN, 2} }, NO_ITEM, false }
};

int main() {
    Stack history;   // pages visited
    Stack choices;   // points given by each move (house * 10 + weight), so going back can undo them
    int current = 0;

    cout << "===== HOGWARTS: THE NOTEBOOK ADVENTURE =====\n";
    cout << "\nWhat is your name, young wizard? ";
    string entered;
    getline(cin, entered);
    if (!entered.empty()) playerName = entered;

    history.push(current);
    choices.push(0);

    while (true) {
        Page &p = pages[current];
        cout << "\n--- Page " << current << " ---\n" << withName(p.text) << "\n";

        // Pick up item
        if (p.gives != NO_ITEM && !inventory[p.gives]) {
            inventory[p.gives] = true;
            cout << "(You obtained " << itemNames[p.gives] << "!)\n";
        }

        // Sorting result: the house with the highest score wins
        // (question 1 is worth 3 points and the others 2, so a tie is impossible)
        int sortedHouse = 0;
        if (p.isSorting) {
            sortedHouse = 1;
            for (int h = 2; h <= 4; h++)
                if (scores[h] > scores[sortedHouse]) sortedHouse = h;
            cout << "\n\"" << houseLines[sortedHouse] << "\"\n";
            cout << "\n*** " << playerName << ", you have been sorted into "
                 << houseNames[sortedHouse] << "! ***\n";
        }

        // Ending
        if (p.numOptions == 0) {
            if (p.isWin) {
                if (p.isSorting)
                    cout << "\nWelcome to " << houseNames[sortedHouse] << ", " << playerName
                         << "! Your magical journey begins.\n";
                else
                    cout << "\nCongratulations, " << playerName << "! Your story at Hogwarts has only just begun.\n";
            } else {
                cout << "\n*** GAME OVER ***\nBetter luck next time, " << playerName << ".\n";
            }
            break;
        }

        // Show options
        for (int i = 0; i < p.numOptions; i++) {
            cout << i + 1 << ". " << withName(p.options[i].text);
            int req = p.options[i].needsItem;
            if (req != NO_ITEM && !inventory[req])
                cout << " (locked - you need " << itemNames[req] << ")";
            cout << "\n";
        }
        if (history.size() > 1) cout << "B. Flip back to the previous page\n";

        // Input
        char choice;
        cout << "> ";
        cin >> choice;

        if ((choice == 'b' || choice == 'B') && history.size() > 1) {
            history.pop();                 // remove current page
            int v = choices.pop();         // undo the points that move gave
            if (v / 10 > 0) scores[v / 10] -= v % 10;
            current = history.peek();      // previous page is now on top
        }
        else if (choice >= '1' && choice < '1' + p.numOptions) {
            Option &o = p.options[choice - '1'];
            if (o.needsItem != NO_ITEM && !inventory[o.needsItem]) {
                cout << "You can't do that yet. You need " << itemNames[o.needsItem] << ".\n";
            } else {
                current = o.nextPage;
                history.push(current);
                choices.push(o.house * 10 + o.weight);
                if (o.house > 0) scores[o.house] += o.weight;
            }
        }
        else {
            cout << "Invalid choice, try again.\n";
        }
    }

    // Keep the window open when run by double-clicking the .exe
    cout << "\nPress Enter to exit...";
    cin.ignore(10000, '\n');
    cin.get();
    return 0;
}