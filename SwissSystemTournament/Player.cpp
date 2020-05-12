//
// Created by Nishinoyama on 2020/05/09.
//

#include "Player.h"

Player::Player() {
    points = 0;
    opponent = nullptr;
    id = -1;
    roundCount = 0;
    matchWinPercentage = 0.00;
    opponentMatchWinPercentage = 0.00;
    isDummy = false;
}

Player::~Player() = default;

void Player::pushMatchedResults(Player *player, bool victory) {
    matchedResults.emplace_back(player,victory);
    if( !player->isDummy ) roundCount++;
    matchedPlayerID.insert(player->id);
}

void Player::calculateMatchWinPercentage() {
    // 3 は 勝ち点
    if( roundCount != 0 ) matchWinPercentage = 1.0*points/roundCount/3;
    else matchWinPercentage = 0.0;
}

void Player::calculateOpponentMatchWinPercentage() {
    opponentMatchWinPercentage = 0.0;
    for ( const auto & result : matchedResults ){
        double winPercentage = std::max( 1.0 / 3, result.first->matchWinPercentage );
        opponentMatchWinPercentage += winPercentage / roundCount;
    }
}

