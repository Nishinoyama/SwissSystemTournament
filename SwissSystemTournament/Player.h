//
// Created by Nishinoyama on 2020/05/09.
//

#ifndef SWISSSYSTEMTOURNAMENT_PLAYER_H
#define SWISSSYSTEMTOURNAMENT_PLAYER_H

#include "MatchResult.h"

#include <set>
#include <vector>
#include <string>

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


    // データとして保存すべきフィールド
    int id{};
    std::string name{};
    int rating{};
    bool withdrew{};
    std::vector<MatchResult> matchedResults{};

    int roundCount{};
    int points{};
    double matchWinPercentage{};
    double opponentMatchWinPercentage{};
    double gameWinPercentage{};
    double opponentGameWinPercentage{};
    std::set<int> matchedPlayerID{};

    Player* opponent{};

    void buildMatchResult( std::vector<Player>& players );
};

#endif //SWISSSYSTEMTOURNAMENT_PLAYER_H
