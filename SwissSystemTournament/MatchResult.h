//
// Created by Nishinoyama on 2020/05/15.
//

#ifndef SWISSSYSTEMTOURNAMENT_MATCHRESULT_H
#define SWISSSYSTEMTOURNAMENT_MATCHRESULT_H

#include "Player.h"

class Player;

class MatchResult {


public:

    // データとして保存すべきフィールド
    int id{};
    int opponentID{};
    int winCount{};
    int loseCount{};
    int drawCount{};
    bool withdraw{}; // withdraw:棄権能動
    bool withdrawn{}; // withdrawn:棄権受動

    int roundNO{};
    Player* opponent = nullptr;

    MatchResult();
    MatchResult(int winCount, int drawCount, int loseCount, bool withdraw, bool withdrawn, Player *opponent);

    bool isWin() const;
    bool isDraw() const;
    bool isLose() const;
    bool isAvail() const;
};


#endif //SWISSSYSTEMTOURNAMENT_MATCHRESULT_H
