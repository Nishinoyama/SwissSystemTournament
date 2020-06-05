//
// Created by Nishinoyama on 2020/05/09.
//

#include <cstdio>
#include <algorithm>
#include <iostream>
#include "Player.h"

Player::Player() = default;

Player::~Player() = default;

void Player::pushMatchedResults( MatchResult result ) {
    matchedResults.push_back(result);
    if( result.isAvail() ) roundCount++;
    if( result.opponent != nullptr ) matchedPlayerID.insert( result.opponent->id );
}

void Player::calculatePoints() {
    points = 0;
    for( const MatchResult & result : matchedResults ){
        if( !result.isAvail() ){
            if( result.withdrawn && !result.withdraw ) points += 3;
        }else{
            if( result.isWin()  ) points += 3;
            if( result.isDraw() ) points += 1;
        }
    }
}

void Player::calculateMatchWinPercentage() {
    // 3 は 勝ち点
    if( roundCount != 0 ) matchWinPercentage = 1.0*points/roundCount/3;
    else matchWinPercentage = 0.0;
}

void Player::calculateOpponentMatchWinPercentage() {
    opponentMatchWinPercentage = 0.0;
    for ( const MatchResult & result : matchedResults ){
        if( !result.isAvail() ) continue;
        double winPercentage = std::max( 1.0 / 3, result.opponent->matchWinPercentage );
        opponentMatchWinPercentage += winPercentage / roundCount;
    }
}

void Player::calculateGameWinPercentage() {
    gameWinPercentage = 0.0;
    int gamePoint = 0;
    int gameRounds = 0;
    for ( const MatchResult & result : matchedResults ) {
        if( !result.isAvail() ) continue;
        gamePoint += result.winCount*3+result.drawCount;
        gameRounds += result.winCount+result.drawCount+result.loseCount;
    }
    if( gameRounds ) gameWinPercentage = 1.0 / 3 * gamePoint / gameRounds;
}

void Player::calculateOpponentGameWinPercentage() {
    opponentGameWinPercentage = 0.0;
    for ( const MatchResult & result : matchedResults ){
        if( !result.isAvail() ) continue;
        double winPercentage = result.opponent->gameWinPercentage;
        opponentGameWinPercentage += winPercentage / roundCount;
    }
}

void Player::buildMatchResult( std::vector<Player>& players ){
    for( auto& matchResult : matchedResults ){
        matchedPlayerID.insert(matchResult.opponentID);
        matchResult.opponent = &players[matchResult.opponentID];
    }
}


