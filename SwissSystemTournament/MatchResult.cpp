//
// Created by Nishinoyama on 2020/05/15.
//

#include "MatchResult.h"

MatchResult::MatchResult() = default;
MatchResult::MatchResult(int winCount, int drawCount, int loseCount, Player* opponent)
: winCount(winCount), drawCount(drawCount), loseCount(loseCount), opponent(opponent){
}

bool MatchResult::isWin() const {
    return winCount > loseCount;
}

bool MatchResult::isDraw() const {
    return winCount == loseCount;
}

bool MatchResult::isLose() const {
    return  winCount < loseCount;
}
