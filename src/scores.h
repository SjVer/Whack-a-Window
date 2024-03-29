#pragma once

#include <vector>

using std::vector;

namespace Tmpl8 {

#define NAME_LEN 8

struct ScoresEntry {
    char name[NAME_LEN];
    int score;
};

using Scores = vector<ScoresEntry>;

Scores getScores();
Scores getTop3Scores();

void clearScores();
void saveScore(char* name, int score);

}