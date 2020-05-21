//
// Created by Nishinoyama on 2020/05/09.
//

#include "SwissSystemTournament.h"
#include "picojson.h"
#include <cstdio>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <bitset>

SwissSystemTournament::SwissSystemTournament() {
    int playerNumber;
    scanf("%d", &playerNumber );

    PlayerNumber = playerNumber;
    // 奇数人だったらダミーのプレイヤーを作成する
    hasDummyPlayer = playerNumber%2;
    players.assign( playerNumber, Player() );
    playersPermutation.assign( imagPlayerNumber(), nullptr );
    MatchingNumber = 0; while ( imagPlayerNumber() >= 1 << MatchingNumber) MatchingNumber++;

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
        // CalculatePlayerState();
        // OutputPlayerState();
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

    /** マッチングをDPで処理
     * dp : dp本体
     * dp[ni][S] := [0,ni) までマッチング済み、[ni,ni+K)までのマッチング済み（bit管理）での最小の偏り
     * K は 少なくとも(ラウンド数+2)で全員のマッチングは成立する。（数学的証明は厳密には行っていない）
     * ni+k+1番目とマッチング成立時の変遷 dp[ni][S] + cost -> dp[ni+1][S>>1|1<<k]
     * ただし、S&(1<<k) == 0 である必要がある。 （マッチングのダブり）
     * もし、S&1（niがマッチング済み）なら、dp[ni][S] -> dp[ni+1][S>>1]
     *
     * rb : dpの変遷元
     * rb[ni][S] := { 変遷元のS', 変遷でマッチングしたか }
     */
    std::vector<std::vector<int>> dp(imagPlayerNumber()+1, std::vector<int>(1<<(matchedRounds+2), 1e9 ) );
    std::vector<std::vector<std::pair<int,int>>> rb(imagPlayerNumber()+1, std::vector<std::pair<int,int>>(1<<(matchedRounds+2), {-1,-1} ) );
    dp[0][0] = 0;
    for( int ni = 0; ni < imagPlayerNumber(); ni++ ){
        for( int bi = 0; bi < 1 << (matchedRounds+2); bi++ ){
            int nni = ni+1;
            if( dp[ni][bi] == INF ) continue;
            if( bi&1 ){
                if( dp[ni][bi] < dp[nni][bi>>1] ){
                    dp[nni][bi>>1] = dp[ni][bi];
                    rb[nni][bi>>1] = {bi,0};
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
    std::string matchingOutputFileName;
    matchingOutputFileName += "matching_" + std::to_string(matchedRounds) + ".json";
    std::fstream fs( matchingOutputFileName, std::ios::out );
    std::bitset<4000> searched;
    fs << "{\n\t\"matchingList\":[\n";
    for (int i = 0; i < PlayerNumber; i++ ) {
        // if( playersPermutation[i]->isDummy || playersPermutation[i]->opponent->isDummy ) continue;
        if( !searched[playersPermutation[i]->id] ) {
            if( i ) fs << ",\n";
            fs << "\t\t{\n"
               << "\t\t\t\"playerID\" : " << playersPermutation[i]->id << ",\n"
               << "\t\t\t\"opponentID\" : " << playersPermutation[i]->opponent->id << ",\n"
               << "\t\t\t\"win\" : 0,\n"
               << "\t\t\t\"lose\" : 0,\n"
               << "\t\t\t\"draw\" : 0,\n"
               << "\t\t\t\"withdraw\" : false,\n"
               << "\t\t\t\"withdrawn\" : false\n"
               << "\t\t}";
            // fs << playersPermutation[i]->id << "," << playersPermutation[i]->opponent->id << std::endl;
            searched[playersPermutation[i]->id] = true;
            searched[playersPermutation[i]->opponent->id] = true;
        }
        // printf("%d v.s. %d  ", playersPermutation[i]->id, playersPermutation[i]->opponent->id );
    }
    fs << "\n\t]\n}\n";
}

void SwissSystemTournament::InputMatchResult() {

    std::vector<bool> calculatedPlayer( imagPlayerNumber(),false);

    for (int i = 0; i < PlayerNumber; i ++ ) {
        Player* left = playersPermutation[i];
        if( !calculatedPlayer[left->id] ){
            Player* right = left->opponent;
            // printf("%d v.s. %d ? ", leftID, rightID );

            // scanf( "%d", &isWin );
            int leftWinCount;
            int leftDrawCount;
            int leftLoseCount;

            // scanf( "%d %d %d", &leftWinCount, &leftDrawCount, &leftLoseCount );

            /**
             * 実験用の入力値
             */
            leftWinCount = left->id%4;
            leftLoseCount = right->id%3;
            leftDrawCount = 5-leftWinCount-leftLoseCount;
            /**
             *
             */

            MatchResult leftResult(leftWinCount,leftDrawCount,leftLoseCount,right);
            MatchResult rightResult(leftLoseCount,leftDrawCount,leftWinCount,left);

            left->pushMatchedResults(leftResult);
            right->pushMatchedResults(rightResult);
            calculatedPlayer[left->id] = true;
            calculatedPlayer[right->id] = true;

        }
    }
}

void SwissSystemTournament::CalculatePlayerState() {

    // [参考Web] https://kirisamemagic.diarynote.jp/201401060210226433/
    for ( Player &player : players ){
        player.calculatePoints();
    }

    for ( Player &player : players ){
        player.calculateMatchWinPercentage();
    }
    for ( Player &player : players ){
        player.calculateOpponentMatchWinPercentage();
    }

    for ( Player &player : players ){
        player.calculateGameWinPercentage();
    }
    for ( Player &player : players ){
        player.calculateOpponentGameWinPercentage();
    }

}

void SwissSystemTournament::OutputPlayerState() {}

void SwissSystemTournament::SortPlayers() {

    CalculatePlayerState();
    std::sort(playersPermutation.begin(), playersPermutation.end(),
              []( const Player* lp, const Player* rp) -> bool {
                  if( rp->isDummy ) return true;
                  if( lp->isDummy ) return false;
                  if (lp->points == rp->points) {
                      if (std::abs(lp->opponentMatchWinPercentage - rp->opponentMatchWinPercentage) < EPS) {
                          if (std::abs(lp->gameWinPercentage - rp->gameWinPercentage) < EPS) {
                              if (std::abs(lp->opponentGameWinPercentage - rp->opponentGameWinPercentage) < EPS) {
                                  return lp->rating > rp->rating;
                              } else { return lp->opponentGameWinPercentage > rp->opponentGameWinPercentage; }
                          } else { return lp->gameWinPercentage > rp->gameWinPercentage; }
                      } else { return lp->opponentMatchWinPercentage > rp->opponentMatchWinPercentage; }
                  } else { return lp->points > rp->points; }
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
        printf("%3dth ID:%3d Pts:%2d R:%d OMWP:%0.04lf GWP:%0.04lf OGWP:%0.04lf",
               ++grade-tie, player->id, player->points, player->roundCount, player->opponentMatchWinPercentage,
               player->gameWinPercentage, player->opponentGameWinPercentage  );
        int rounds = 0;
        for( const MatchResult result : player->matchedResults ){
            if( result.isBye ) continue;
            printf( " No%02d:%3d%s", ++rounds, result.opponent->id, result.isWin()?"o":result.isDraw()?"-":"x" );
        }
        printf("\n");
    }
}

int SwissSystemTournament::imagPlayerNumber() const {
    return PlayerNumber + hasDummyPlayer;
}
