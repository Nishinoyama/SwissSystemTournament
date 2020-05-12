//
// Created by Nishinoyama on 2020/05/09.
//

#include "SwissSystemTournament.h"
#include <cstdio>
#include <algorithm>
#include <cassert>

SwissSystemTournament::SwissSystemTournament()
// : PlayerNumber(PlayerNumber), players(PlayerNumber,Player()), playersPermutation(PlayerNumber)
{
    int playerNumber;
    scanf("%d", &playerNumber );

    PlayerNumber = playerNumber;
    // 奇数人だったらダミーのプレイヤーを作成する
    hasDummyPlayer = playerNumber%2;
    players.assign( playerNumber, Player() );
    playersPermutation.assign( imagPlayerNumber(), nullptr );
    MatchingNumber = 0;
    while ( imagPlayerNumber() >= 1 << MatchingNumber) MatchingNumber++;
    Print();
    scanf("%d", &MatchingNumber );

    matchedRounds = 0;

    for (int i = 0; i < PlayerNumber; i++){
        players[i].id = i;
        playersPermutation[i] = &players[i];
    }

    if( hasDummyPlayer ){
        Player dummy;
        dummy.id = PlayerNumber;
        dummy.isDummy = true;
        playersPermutation[PlayerNumber] = &dummy;
    }

    Print();

    for (int i = 0; i < MatchingNumber; i++){
        Matching();
        OutputMatching();
        InputMatchResult();
        CalculatePlayerState();
        OutputPlayerState();
    }

    OutputFinalResult();

}
SwissSystemTournament::~SwissSystemTournament() = default;

void SwissSystemTournament::Print() const {
    printf("PlayerNumber:%d\nMatchingNumber:%d\n", PlayerNumber, MatchingNumber );
}

void SwissSystemTournament::Matching() {

    for (Player& player : players) {
        player.opponent = nullptr;
    }

    SortPlayers();

    //TODO. 重複を考える

    std::vector<std::vector<int>> dp(imagPlayerNumber()+1, std::vector<int>(1<<(matchedRounds+2), 1e9 ) );
    std::vector<std::vector<std::pair<int,int>>> rb(imagPlayerNumber()+1, std::vector<std::pair<int,int>>(1<<(matchedRounds+2), {-1,-1} ) );
    dp[0][0] = 0;
    for( int ni = 0; ni < imagPlayerNumber(); ni++ ){
        for( int bi = 0; bi < 1 << (matchedRounds+2); bi++ ){
            // dp[ni][bi] からの変遷
            int nni = ni+1;
            if( dp[ni][bi] == INF ) continue;
            if( bi&1 ){
                if( dp[ni][bi] < dp[nni][bi>>1] ){
                    dp[nni][bi>>1] = dp[ni][bi];
                    rb[nni][bi>>1] = {bi,0};
                    // printf("A%d,%d -> %d,%d\n", ni, bi, nni, bi>>1 );
                }
                continue;
            }
            for( int pi = 0; pi < matchedRounds+2; pi++ ){
                int ppi = ni+pi+1;
                if( ppi >= imagPlayerNumber() ) break;
                if( (bi>>1)&(1<<pi) ) continue;
                int bbi = (bi>>1)|(1<<pi);
                int cost = playersPermutation[ni]->points - playersPermutation[ppi]->points;
                if( playersPermutation[ni]->matchedPlayerID.find(playersPermutation[ppi]->id) == playersPermutation[ni]->matchedPlayerID.end() ){
                    if( dp[ni][bi] + cost < dp[nni][bbi] ){
                        dp[nni][bbi] = dp[ni][bi];
                        rb[nni][bbi] = {bi,1};
                        // printf("B%d,%d -> %d,%d\n", ni, bi, nni, bbi );
                    }
                }
            }
        }
    }

    int rbn = imagPlayerNumber(), rbb = 0;
    const static auto invertLeftShift = []( int k ){
        int count = 0;
        int bit = 1;
        while( !( k & bit ) ) count++, bit <<= 1;
        return count;
    };
    while( rbn != 0 || rbb != 0 ){
        int tmp = rb[rbn][rbb].first;
        // printf("%d,%d -> %d,%d (%d)\n", rbn, rbb, rbn-1, tmp, rb[rbn][rbb].second );
        assert( tmp>=0 );
        if( rb[rbn][rbb].second ){
            int transition = (rbb<<1) - tmp;
            int li = rbn-1;
            int ri = li+invertLeftShift(transition);
            playersPermutation[li]->opponent = playersPermutation[ri];
            playersPermutation[ri]->opponent = playersPermutation[li];
        }
        rbn--;
        rbb = tmp;
    }

    matchedRounds++;

}

void SwissSystemTournament::OutputMatching() {
    for (int i = 0; i < PlayerNumber; i++ ) {
        // if( playersPermutation[i]->isDummy || playersPermutation[i]->opponent->isDummy ) continue;
        printf("%d v.s. %d  ", playersPermutation[i]->id, playersPermutation[i]->opponent->id );
    }
    printf("\n");
};

void SwissSystemTournament::InputMatchResult() {

    std::vector<bool> calculatedPlayer( imagPlayerNumber(),false);

    for (int i = 0; i < PlayerNumber; i ++ ) {
        Player* left = playersPermutation[i];
        if( !calculatedPlayer[left->id] ){
            Player* right = left->opponent;
            int isWin;
            // printf("%d v.s. %d ? ", leftID, rightID );

            // scanf( "%d", &isWin );
            isWin = left->id%3<right->id%12;
            if( right->isDummy ) isWin = 1;

            if( isWin ){
                left->points += 3;
            }else{
                right->points += 3;
            }

            calculatedPlayer[left->id] = true;
            calculatedPlayer[right->id] = true;
            left->pushMatchedResults(right,isWin);
            right->pushMatchedResults(left,!isWin);

        }
    }
};
void SwissSystemTournament::CalculatePlayerState() {

    // [参考Web] https://kirisamemagic.diarynote.jp/201401060210226433/
    // マッチ・ウィン・パーセンテージを計算
    for ( Player &player : players ){
        player.calculateMatchWinPercentage();
    }

    // オポネント・マッチ・ウィン・パーセンテージを計算
    for ( Player &player : players ){
        player.calculateOpponentMatchWinPercentage();
    }


};
void SwissSystemTournament::OutputPlayerState() {}

void SwissSystemTournament::SortPlayers() {

    CalculatePlayerState();
    std::sort(playersPermutation.begin(), playersPermutation.end(),
              []( const Player* lp, const Player* rp) -> bool {
                    if( rp->isDummy ) return true;
                    if( lp->isDummy ) return false;
                    if ( lp->points > rp->points ) {
                        return true;
                    } else if ( lp->points < rp -> points ) {
                        return false;
                    } else {
                        if ( std::abs(lp->opponentMatchWinPercentage - rp->opponentMatchWinPercentage) < EPS ) return false;
                        return lp->opponentMatchWinPercentage > rp->opponentMatchWinPercentage;
                    }
              });

};

void SwissSystemTournament::OutputFinalResult() {
    SortPlayers();
    int grade = 0;
    int tie = 0;
    for ( const Player* player : playersPermutation ){
        if( player->isDummy ) continue;
        if( grade ){
            if( std::abs( player->opponentMatchWinPercentage - (player-1)->opponentMatchWinPercentage ) < EPS
            &&  player->points == (player-1)->points ) tie++;
            else tie = 0;
        }
        printf("Grade:%3dth ID:%3d Points:%2d Rounds:%d Opo:%0.04lf",
                ++grade-tie, player->id, player->points, player->roundCount, player->opponentMatchWinPercentage );
        int rounds = 0;
        for( const auto & result : player->matchedResults ){
            if( result.first->isDummy ) continue;
            printf( " No%02d:%3d%s ", ++rounds, result.first->id, result.second?"o":"x" );
        }
        printf("\n");
    }
}

int SwissSystemTournament::imagPlayerNumber() const {
    return PlayerNumber + hasDummyPlayer;
}

