#include "scores.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#define SCORES_FILE "scores.bin"

namespace Tmpl8 {

Scores getScores() {
    FILE* f = fopen(SCORES_FILE, "rb");
    if (!f) {
        printf("No scores file!\n");
        fclose(fopen(SCORES_FILE, "w"));
        f = fopen(SCORES_FILE, "rb");
        return Scores();
	}
    
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    if (size % sizeof(ScoresEntry)) {
        printf("Invalid scores file! (%zu %% %zu = %zu)\n",
            size, sizeof(ScoresEntry),
            size % sizeof(ScoresEntry)
        );
        fclose(fopen(SCORES_FILE, "w"));
        return Scores();
    }

    size_t count = size / sizeof(ScoresEntry);
    ScoresEntry* data = new ScoresEntry[count];
    fread(data, sizeof(ScoresEntry), count, f);
    fclose(f);

    Scores scores{};
    for (int i = 0; i < count; i++) scores.push_back(data[i]);
    return scores;
}

Scores getTop3Scores() {
    Scores scores = getScores();

    auto compFn = [](ScoresEntry a, ScoresEntry b) {
        return a.score > b.score;
    };
    std::sort(scores.begin(), scores.end(), compFn);

    while (scores.size() > 3) scores.pop_back();
    return scores;
}

void clearScores() {
    remove(SCORES_FILE);
}

void saveScore(char* name, int score) {
    if (strlen(name) > NAME_LEN) {
        printf("Name too long! (%s)\n", name);
        return;
    }

    FILE* f = fopen(SCORES_FILE, "ab");
    if (!f) {
        printf("Could not open scores file!");
        exit(1);
	}

    ScoresEntry* entry = new ScoresEntry();
    strncpy(entry->name, name, strlen(name) * sizeof(char));
    entry->score = score;
    
    fwrite(entry, sizeof(ScoresEntry), 1, f);
    fclose(f);
}

}