//
// Created by Nishinoyama on 2020/05/15.
//

#include "MatchResult.h"

MatchResult::MatchResult() = default;
MatchResult::MatchResult(int winCount, int drawCount, int loseCount, bool withdraw, bool withdrawn, Player* opponent)
: winCount(winCount), drawCount(drawCount), loseCount(loseCount), opponent(opponent), withdraw(withdraw), withdrawn(withdrawn){
    opponentID = opponent->id;
}

bool MatchResult::isWin() const {
    return !isDraw() && ( withdrawn || winCount > loseCount);
}

bool MatchResult::isDraw() const {
    return ( withdraw && withdrawn ) || winCount == loseCount;
}

bool MatchResult::isLose() const {
    return  !isDraw() && ( withdraw || winCount < loseCount);
}

bool MatchResult::isAvail() const {
    return !withdraw && !withdrawn;
}
