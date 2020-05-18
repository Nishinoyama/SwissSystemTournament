//
// Created by Nishinoyama on 2020/05/15.
//

#ifndef SWISSSYSTEMTOURNAMENT_MATCHRESULT_H
#define SWISSSYSTEMTOURNAMENT_MATCHRESULT_H

#include "Player.h"

class Player;

class MatchResult {


public:

    int id{};
    int winCount{};
    int loseCount{};
    int drawCount{};
    int roundNO{};
    Player* opponent = nullptr;
    bool isBye{}; // Bye:不戦勝

    MatchResult();
    MatchResult(int winCount, int drawCount, int loseCount, Player* opponent );

    bool isWin() const;
    bool isDraw() const;
    bool isLose() const;
};


#endif //SWISSSYSTEMTOURNAMENT_MATCHRESULT_H
