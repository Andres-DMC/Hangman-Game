#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <set>

using namespace std;

// Cross-platform function to clear the console
void clearScreen() {
#ifdef _WIN32
    system("cls"); // Windows
#else
    system("clear"); // Linux/macOS
#endif
}

// Display the man (no stick), erasing bottom-up
void displayHangman(int wrongGuesses) {
    // Full visual from bottom (legs) to top (stick)
    vector<string> fullHangman = {
        "  _______     ",  // 0 - Top of stick
        " |/     |    ",  // 1 - Rope/structure
        " |      O     ",  // 2 - Head
        " |     /|\\   ",  // 3 - Arms and torso
        " |      |     ",  // 4 - Torso line (can be merged with above)
        " |     / \\   ",  // 5 - Legs
        "_|___          ", // 6 - Ground
        ""                // 7 - Extra spacing
    };

    // Order to remove from bottom-up (legs → torso → arms → head → stick)
    vector<int> removalOrder = {5, 4, 3, 2, 1, 0}; // Indexes in fullHangman

    // Remove parts equal to wrongGuesses
    for (int i = 0; i < wrongGuesses && i < removalOrder.size(); ++i) {
        fullHangman[removalOrder[i]] = ""; // Remove part
    }

    // Print updated hangman
    cout << "\n";
    for (const string& line : fullHangman) {
        cout << line << endl;
    }
    cout << "\n";
}

// Display current guessed state of the word
void displayWord(const string& word, const vector<char>& guessedLetters) {
    for (char c : word) {
        if (find(guessedLetters.begin(), guessedLetters.end(), c) != guessedLetters.end()) {
            cout << c << " ";
        } else {
            cout << "_ ";
        }
    }
    cout << endl;
}

// Check if the entire word has been guessed
bool isWordGuessed(const string& word, const vector<char>& guessedLetters) {
    for (char c : word) {
        if (find(guessedLetters.begin(), guessedLetters.end(), c) == guessedLetters.end()) {
            return false;
        }
    }
    return true;
}

// Let the user select a category and return a random word from it
string chooseCategoryAndWord() {
    vector<string> animals = {"tiger", "elephant", "dolphin", "giraffe", "penguin"};
    vector<string> countries = {"brazil", "canada", "japan", "germany", "mexico"};
    vector<string> names = {"emily", "david", "linda", "kevin", "sofia"};
    vector<string> actions = {"jump", "run", "swim", "dance", "sleep"};
    vector<string> custom;

    int choice;
    cout << "Choose a category:\n";
    cout << "1. Animals\n";
    cout << "2. Countries\n";
    cout << "3. Names\n";
    cout << "4. Actions\n";
    cout << "5. Custom (You enter the words)\n";
    cout << "Enter your choice (1-5): ";
    cin >> choice;

    while (choice < 1 || choice > 5) {
        cout << "Invalid choice. Please enter a number between 1 and 5: ";
        cin >> choice;
    }

    if (choice == 5) {
        cout << "Enter words (type 'done' to finish):\n";
        string word;
        while (true) {
            cin >> word;
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            if (word == "done") break;
            custom.push_back(word);
        }

        if (custom.empty()) {
            cout << "No custom words entered. Using 'hangman' as default.\n";
            return "hangman";
        }

        return custom[rand() % custom.size()];
    }

    vector<string> selectedCategory;
    switch (choice) {
        case 1: selectedCategory = animals; break;
        case 2: selectedCategory = countries; break;
        case 3: selectedCategory = names; break;
        case 4: selectedCategory = actions; break;
    }

    return selectedCategory[rand() % selectedCategory.size()];
}

// Hint system: return a random unguessed letter from the word
char giveHint(const string& word, const set<char>& guessedLetters) {
    vector<char> options;
    for (char c : word) {
        if (guessedLetters.find(c) == guessedLetters.end()) {
            options.push_back(c);
        }
    }

    if (!options.empty()) {
        return options[rand() % options.size()];
    }

    return '\0'; // No hints possible
}

int main() {
    srand(time(0));

    string word = chooseCategoryAndWord();
    vector<char> guessedLettersVec;
    set<char> guessedLettersSet; // For quick lookup
    int maxChances = 6;
    int wrongGuesses = 0;
    int points = 0;
    int hintsLeft = 3;

    cout << "\n=== Welcome to Hangman! ===" << endl;

    while (wrongGuesses < maxChances && !isWordGuessed(word, guessedLettersVec)) {
        clearScreen(); // Clear the console at the start of each round

        displayHangman(wrongGuesses);
        cout << "Word: ";
        displayWord(word, guessedLettersVec);

        cout << "Wrong guesses left: " << (maxChances - wrongGuesses) << endl;
        cout << "Points: " << points << " | Hints left: " << hintsLeft << endl;
        cout << "Guessed letters: ";
        for (char c : guessedLettersVec) {
            cout << c << " ";
        }
        cout << "\nEnter a letter or type 'hint': ";
        string input;
        cin >> input;
        transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (input == "hint") {
            if (hintsLeft > 0 && points >= 5) {
                char hint = giveHint(word, guessedLettersSet);
                if (hint != '\0') {
                    cout << "💡 Hint: Try the letter '" << hint << "'\n";
                    guessedLettersVec.push_back(hint);
                    guessedLettersSet.insert(hint);
                    points -= 5;
                    hintsLeft--;
                    continue;
                } else {
                    cout << "No more hints available!\n";
                }
            } else if (points < 5) {
                cout << "Not enough points to use a hint!\n";
            } else {
                cout << "No hints left!\n";
            }
            continue;
        }

        if (input.length() != 1 || !isalpha(input[0])) {
            cout << "Please enter a valid single letter.\n";
            continue;
        }

        char guess = input[0];

        if (guessedLettersSet.count(guess)) {
            cout << "You already guessed that letter!\n";
            continue;
        }

        guessedLettersVec.push_back(guess);
        guessedLettersSet.insert(guess);

        if (word.find(guess) == string::npos) {
            cout << "Wrong guess!" << endl;
            wrongGuesses++;
            points -= 7;
        } else {
            cout << "Correct guess!" << endl;
            int count = count_if(word.begin(), word.end(), [guess](char c){ return c == guess; });
            points += count * 5;
        }
    }

    if (isWordGuessed(word, guessedLettersVec)) {
        cout << "\n🎉 Congratulations! You guessed the word: " << word << endl;
    } else {
        displayHangman(maxChances);
        cout << "\n💀 Game Over! The correct word was: " << word << endl;
    }

    cout << "Final Score: " << points << " points\n";
    return 0;
}
