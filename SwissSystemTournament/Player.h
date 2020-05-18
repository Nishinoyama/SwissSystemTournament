//
// Created by Nishinoyama on 2020/05/09.
//

#ifndef SWISSSYSTEMTOURNAMENT_PLAYER_H
#define SWISSSYSTEMTOURNAMENT_PLAYER_H

#include "MatchResult.h"

#include <set>
#include <vector>

class MatchResult;

class Player {

public:

    explicit Player();
    ~Player();

    void pushMatchedResults(MatchResult result);
    void calculatePoints();
    void calculateMatchWinPercentage();
    void calculateOpponentMatchWinPercentage();
    void calculateGameWinPercentage();
    void calculateOpponentGameWinPercentage();

    bool isDummy{};
    int id{};
    int points{};
    Player* opponent{};
    int roundCount{};
    double matchWinPercentage{};
    double opponentMatchWinPercentage{};
    double gameWinPercentage{};
    double opponentGameWinPercentage{};
    int rating{};

    std::set<int> matchedPlayerID{};
    std::vector<MatchResult> matchedResults{};

};

#endif //SWISSSYSTEMTOURNAMENT_PLAYER_H
