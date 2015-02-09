/*
 Aaron Robinson
 1/31/15
 
 Parses a dictionary text file and creates a sub-dictionary containing
 words of length N, stored in a vector.
 
 That sub-dictionary is then transformed into an undirected, unweighted graph
 represented as an adjecency list. Each word is a node, and each node's neighbors
 are defined by words that differ by 1 letter (e.g. "DOG" and "HOG" are neighbors).

 The program then calculates the shortest path between two given words of length N.
 */

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <climits>

using namespace std;

// Word struct used to keep track of words and their neighbors
struct Word {
    Word(string str) {
        name = str;
        visited = false;
        predecessor = NULL;
        distance = -1;
    }
    int distance;
    Word *predecessor;
    bool visited;
    string name;
    vector<int> nextHopIndices;
};

/*==================================================================================
    FUNCTION PROTOTYPES
 ===================================================================================*/

// function that parses dictionary file, getting all words of length N
void createWords(vector<Word *>& list, ifstream& input, int N);

// binary search for quickly checking if a word is in the given list of words
int binarySearch(vector<Word *>& list, string wordString, int low, int high);

// finds the immediate neighbors for a given Word (differ by 1 letter)
void generateNeighbors(Word *word, int wordIndex, vector<Word *>& list);

// Breadth First Search: finds the distance between two given Words
int BFS(Word *start, Word *goal, vector<Word *>& list);

// Prints the path between two given words
void printPath(Word *sourceWord, Word *destinationWord);

/*==================================================================================
    MAIN FUNCTION
 ===================================================================================*/

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Usage: " << argv[0] << " <input_file>\n";
        exit(1);
    }
    
    // Open dictionary file (all words)
    ifstream dictFile;
    dictFile.open(argv[1]);
    
    if(!dictFile) {
        cout << "I/O Error: Cannot open file " << argv[1] << endl;
        exit(1);
    }
    
    // Choose length N
    int length;
    cout << "Choose an integer N to get words of length N: ";
    cin >> length;
    
    // Initialize list of Word objects for storing all the Words of length N
    vector<Word *> wordsList;
    cout << "Calling createWords() to create Word objects...\n";
    createWords(wordsList, dictFile, length);
    
    int neighborCount = 0;
    float averageDegree = 0;
    int highestDegree = 0;
    int lowestDegree = INT_MAX;
    Word *wordWithMostNeighbors = new Word("");
    Word *wordWithFewestNeighbors = new Word("");
    vector<Word *> wordsWithoutNeighbors;
    // Print out all the words of length N and their neighbors count
    for(int i = 0; i < wordsList.size(); i++) {
        cout << left << setw(6) << i;
        generateNeighbors(wordsList[i], i, wordsList);
        cout << wordsList[i]->name << ": " << wordsList[i]->nextHopIndices.size();
        
        if (wordsList[i]->nextHopIndices.size() == 0)
            wordsWithoutNeighbors.push_back(wordsList[i]);
        if (wordsList[i]->nextHopIndices.size() > highestDegree) {
            wordWithMostNeighbors = wordsList[i];
            highestDegree = (int)wordsList[i]->nextHopIndices.size();
        }
        if (wordsList[i]->nextHopIndices.size() < lowestDegree) {
            wordWithFewestNeighbors = wordsList[i];
            lowestDegree = (int)wordsList[i]->nextHopIndices.size();
        }
        
        //        for (int nextHopIndex = 0; nextHopIndex < wordsList[i]->nextHopIndices.size(); nextHopIndex++) {
        //            cout << wordsList[wordsList[i]->nextHopIndices[nextHopIndex]]->name << ", ";
        //        }
        
        cout << endl;
        neighborCount += wordsList[i]->nextHopIndices.size();
    }
    
    averageDegree = neighborCount / wordsList.size();
    cout << "\nAverage neighbor count (average degree): " << averageDegree << endl;
    cout << "Word(s) with most neighbors (highest degree): " << wordWithMostNeighbors->name;
    cout << " (" << highestDegree << ")\n";
    // look for other words with same degree
//    for (int i = 0; i < wordsList.size(); i++) {
//        if (wordsList[i]->nextHopIndices.size() == highestDegree && wordsList[i] != wordWithMostNeighbors) {
//            cout << wordWithMostNeighbors->name << ": " << highestDegree << endl;
//        }
//    }

    cout << "Word with fewest neighbors (lowest degree): " << wordWithFewestNeighbors->name;
    cout << " (" << lowestDegree << ")\n";
    
    if (wordsWithoutNeighbors.size() > 0) {
        cout << "Words with no neighbors (" << wordsWithoutNeighbors.size() << "):\n";
        for (int i = 0; i < wordsWithoutNeighbors.size(); i++) {
            cout << wordsWithoutNeighbors[i]->name << ": " << wordsWithoutNeighbors[i]->nextHopIndices.size() << endl;
        }
        cout << "--------------------------------------------\n";
    }
    
    // Use BFS to find the distance between two words
    string a, b;
    cout << "\nFind the distance between two words.\n";
    cout << "First word: ";
    cin >> a;
    for (auto iter = a.begin();
         iter < a.end();
         iter++) {
        *iter = toupper(*iter);
    }
    int x = binarySearch(wordsList, a, 0, (int)wordsList.size() - 1);
    if (x == -1) {
        cout << a << " is not a word.\n";
        exit(1);
    }
    
    cout << "Second word: ";
    cin >> b;
    for (auto iter = b.begin();
         iter < b.end();
         iter++) {
        *iter = toupper(*iter);
    }
    int y = binarySearch(wordsList, b, 0, (int)wordsList.size() - 1);
    if (y == -1){
        cout << b << " is not a word.\n";
        exit(1);
    }
    
    int distance = BFS(wordsList[x], wordsList[y], wordsList);
    if (distance == -1)
        cout << "Path between " << a << " and " << b << " doesn't exist.\n";
    else
        cout << "Shortest distance between " << a << " and " << b << ": " << wordsList[y]->distance << endl;
    
    dictFile.close();
    
    return 0;
}

/*==================================================================================
    HELPER FUNCTIONS
 ===================================================================================*/

void createWords(vector<Word *>& list, ifstream& input, int requiredLength) {
    string currentLineString, previousLineString;
    Word *tempWord;
    while (getline(input, currentLineString)) {
        if (currentLineString.length() == requiredLength) {
            for (auto iter = currentLineString.begin();
                 iter < currentLineString.end();
                 iter++) {
                *iter = toupper(*iter);
            }
            if (currentLineString == previousLineString) {
                continue;
            }
            tempWord = new Word(currentLineString);
            list.push_back(tempWord);
            previousLineString = currentLineString;
        }
    }
}

void generateNeighbors(Word *currentWord, int currentWordIndex, vector<Word *>& wordsList) {
    for (int currentLetterIndex = 0; currentLetterIndex < currentWord->name.length(); currentLetterIndex++) {
        string tempWordString = currentWord->name;
        for (unsigned char letter = 'A'; letter <= 'Z'; letter++) {
            if (tempWordString[currentLetterIndex] == letter)
                continue;
            tempWordString[currentLetterIndex] = letter;
            int foundWordIndex = binarySearch(wordsList, tempWordString, 0, (int)wordsList.size() - 1);
            if (foundWordIndex != -1 && foundWordIndex != currentWordIndex) {
                currentWord->nextHopIndices.push_back(foundWordIndex);
            }
        }
    }
}

int binarySearch(vector<Word *>& list, string wordString, int lowIndex, int highIndex) {
    while (lowIndex <= highIndex) {
        int mid = lowIndex + (highIndex - lowIndex) / 2;
        if (list[mid]->name == wordString) {
            return mid;
        } else if (list[mid]->name > wordString) {
            highIndex = mid - 1;
        } else {
            lowIndex = mid + 1;
        }
    }
    return -1;
}

int BFS(Word *start, Word *goal, vector<Word *>& list) {
    queue<Word *> Q;
    vector<Word *> workingSet;
    
    start->distance = 0;
    start->visited = true;
    workingSet.push_back(start);
    Q.push(start);
    
    while (!Q.empty()) {
        Word *currentWord = Q.front();
        Q.pop();
        if (currentWord->name == goal->name) {
            Word *itr = goal;
            printPath(start, itr);
            cout << endl;
            return goal->distance;
        }
        for (int i = 0; i < currentWord->nextHopIndices.size(); i++) {
            Word *neighbor = list[currentWord->nextHopIndices[i]];
            int neighborIsInWorkingSet = binarySearch(workingSet, neighbor->name, 0, (int)workingSet.size() - 1);
            if (neighborIsInWorkingSet == -1 && !neighbor->visited) {
                neighbor->visited = true;
                neighbor->predecessor = currentWord;
                neighbor->distance = currentWord->distance + 1;
                workingSet.push_back(neighbor);
                Q.push(neighbor);
            }
        }
    }
    return -1;
}

void printPath(Word *source, Word *iterator) {
    if (iterator == source) {
        cout << source->name;
        return;
    }
    printPath(source, iterator->predecessor);
    cout << " > " << iterator->name;
}
