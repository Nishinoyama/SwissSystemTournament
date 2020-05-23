//
// Created by Nishinoyama on 2020/05/15.
//

#include "MatchResult.h"

MatchResult::MatchResult() = default;
MatchResult::MatchResult(int winCount, int drawCount, int loseCount, bool withdraw, bool withdrawn, Player* opponent)
: winCount(winCount), drawCount(drawCount), loseCount(loseCount), opponent(opponent), withdraw(withdraw), withdrawn(withdrawn){
}

bool MatchResult::isWin() const {
    if( withdrawn ) return true;
    return winCount > loseCount;
}

bool MatchResult::isDraw() const {
    return winCount == loseCount;
}

bool MatchResult::isLose() const {
    if( withdraw ) return false;
    return  winCount < loseCount;
}

bool MatchResult::isAvail() const {
    return !withdraw && !withdrawn;
}
