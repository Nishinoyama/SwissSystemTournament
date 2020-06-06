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
#include <iomanip>
#include <direct.h>

SwissSystemTournament::SwissSystemTournament( int playerNumber, int roundNumber, const std::string& matchingName) {
    PlayerNumber = playerNumber;
    matchedRounds = roundNumber;
    MatchingName = matchingName;
    if( matchedRounds == 0 ){
        build();
        MakeJSONData();
    }
    initFromJSON();
    Matching();
    OutputMatching();
}

SwissSystemTournament::SwissSystemTournament() = default;

SwissSystemTournament::~SwissSystemTournament() {
    std::cout << "Finished Successfully" << std::endl;
}

void SwissSystemTournament::build(){

    // 奇数人だったらダミーのプレイヤーを作成する
    hasDummyPlayer = PlayerNumber%2;
    players.assign( imagPlayerNumber(), Player() );
    playersPermutation.assign( imagPlayerNumber(), nullptr );
    for (int i = 0; i < imagPlayerNumber(); i++){
        players[i].id = i;
        playersPermutation[i] = &players[i];
    }
    if( hasDummyPlayer ){
        players[PlayerNumber].withdrew = true;
    }

    _mkdir(MatchingName.c_str());

    Print();
}

void SwissSystemTournament::MakeJSONData() {
    picojson::object id;
    picojson::array playersJSON;
    id.insert(std::make_pair("playerNumber", picojson::value(double(PlayerNumber)) ));
    id.insert(std::make_pair("roundNumber", picojson::value(double(matchedRounds)) ));
    for( const Player& p : players ){
        picojson::object playerObject;
        playerObject.insert(std::make_pair("id",picojson::value(double(p.id))));
        playerObject.insert(std::make_pair("name",picojson::value(p.name)));
        playerObject.insert(std::make_pair("rating",picojson::value(double(p.rating))));
        playerObject.insert(std::make_pair("withdrew",picojson::value(p.withdrew)));
        picojson::array results;
        for( const auto& r : p.matchedResults ){
            picojson::object result;
            result.insert(std::make_pair("opponentID",picojson::value(double(r.opponentID))));
            result.insert(std::make_pair("winCount",picojson::value(double(r.winCount))));
            result.insert(std::make_pair("loseCount",picojson::value(double(r.loseCount))));
            result.insert(std::make_pair("drawCount",picojson::value(double(r.drawCount))));
            result.insert(std::make_pair("withdraw",picojson::value(r.withdraw)));
            result.insert(std::make_pair("withdrawn",picojson::value(r.withdrawn)));
            results.emplace_back(result);
        }
        playerObject.insert(std::make_pair("matchedResults",picojson::value(results)));
        playersJSON.emplace_back(playerObject);
    }
    id.insert(std::make_pair("players", picojson::value(playersJSON)));
    std::ofstream ofs( MatchingName+"/data_"+std::to_string(matchedRounds)+".json");
    ofs << picojson::value(id).serialize(true) << std::endl;
    ofs.close();
    std::cout << "save JSON data successfully" << std::endl;
}

void SwissSystemTournament::initFromJSON() {
    std::ifstream ifs( MatchingName + "/data_" + std::to_string(matchedRounds) + ".json", std::ios::in );
    const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    picojson::value dataJSON;
    const std::string err = picojson::parse(dataJSON, json);
    if( !err.empty() ) {
        std::cerr << err;
    }

    // std::cout << dataJSON << std::endl;
    picojson::object& obj = dataJSON.get<picojson::object>();
    PlayerNumber = obj["playerNumber"].get<double>();
    picojson::array& playerArray = obj["players"].get<picojson::array>();
    players.clear();
    for( const picojson::value& playerValue : playerArray ){
        picojson::object playerObject = playerValue.get<picojson::object>();
        Player player;
        player.id       = playerObject["id"]      .get<double>();
        player.name     = playerObject["name"]    .get<std::string>();
        player.rating   = playerObject["rating"]  .get<double>();
        player.withdrew = playerObject["withdrew"].get<bool>();
        picojson::array& matchedResultsArray = playerObject["matchedResults"].get<picojson::array>();
        for( const picojson::value& matchedResultValue : matchedResultsArray ){
            picojson::object matchedResultObject = matchedResultValue.get<picojson::object>();
            MatchResult matchedResult;
            matchedResult.opponentID = matchedResultObject["opponentID"].get<double>();
            matchedResult.winCount   = matchedResultObject["winCount"  ].get<double>();
            matchedResult.loseCount  = matchedResultObject["loseCount" ].get<double>();
            matchedResult.drawCount  = matchedResultObject["drawCount" ].get<double>();
            matchedResult.withdraw   = matchedResultObject["withdraw"  ].get<bool>();
            matchedResult.withdrawn  = matchedResultObject["withdrawn" ].get<bool>();
            player.pushMatchedResults(matchedResult);
        }
        players.push_back(player);
    }

    PlayerNumber = players.size();
    hasDummyPlayer = PlayerNumber%2;
    playersPermutation.clear();
    for( int i = 0; i < imagPlayerNumber(); i++ ) {
        players[i].buildMatchResult(players);
        playersPermutation.push_back(&players[i]);
    }

    std::cout << "load JSON data successfully" << std::endl;
    // OutputFinalResult();
}

void SwissSystemTournament::Print() const {
    printf("PlayerNumber:%d\nMatchingNumber:%d\n", PlayerNumber, MatchingNumber );
}

void SwissSystemTournament::Matching() {

    for (Player& player : players) {
        player.opponent = nullptr;
    }

    SortPlayers();

    std::cout << "matching.";
    /** マッチングをDPで処理
     * dp : dp本体
     * dp[ni][S] := [0,ni) までマッチング済み、[ni,ni+K)までのマッチング済み（bit管理）での最小の偏り
     * K は 少なくとも(ラウンド数+2)で全員のマッチングは成立する。（数学的証明は厳密には行っていない）
     * K = 6 から実際に調査、マッチング不成立ならKを増やしていく。
     * ni+k+1番目とマッチング成立時の変遷 dp[ni][S] + cost -> dp[ni+1][S>>1|1<<k]
     * ただし、S&(1<<k) == 0 である必要がある。 （マッチングのダブり）
     * もし、S&1（niがマッチング済み）なら、dp[ni][S] -> dp[ni+1][S>>1]
     *
     * rb : dpの変遷元
     * rb[ni][S] := { 変遷元のS', 変遷でマッチングしたか }
     */
    for( int ext = std::min( 6, matchedRounds+2 ); ext < INF; ext++ ){
        std::vector<std::vector<int>> dp(imagPlayerNumber() + 1, std::vector<int>(1 << ext, 1e9));
        std::vector<std::vector<std::pair<int, int>>> rb(imagPlayerNumber() + 1,
                                                         std::vector<std::pair<int, int>>(1 << ext, {-1, -1}));
        dp[0][0] = 0;
        for (int ni = 0; ni < imagPlayerNumber(); ni++) {
            for (int bi = 0; bi < 1 << ext; bi++) {
                int nni = ni + 1;
                if (dp[ni][bi] == INF) continue;
                if (bi & 1) {
                    if (dp[ni][bi] < dp[nni][bi >> 1]) {
                        dp[nni][bi >> 1] = dp[ni][bi];
                        rb[nni][bi >> 1] = {bi, 0};
                    }
                    continue;
                }
                for (int pi = 0; pi < ext; pi++) {
                    int ppi = ni + pi + 1;
                    if (ppi >= imagPlayerNumber()) break;
                    if ((bi >> 1) & (1 << pi)) continue;
                    int bbi = (bi >> 1) | (1 << pi);
                    int cost = playersPermutation[ni]->points - playersPermutation[ppi]->points;
                    if (playersPermutation[ni]->matchedPlayerID.find(playersPermutation[ppi]->id) ==
                        playersPermutation[ni]->matchedPlayerID.end()) {
                        if (dp[ni][bi] + cost < dp[nni][bbi]) {
                            dp[nni][bbi] = dp[ni][bi];
                            rb[nni][bbi] = {bi, 1};
                        }
                    }
                }
            }
            if (ni % 50 == 0) std::cout << ".";
        }

        int rbn = imagPlayerNumber(), rbb = 0;
        const static auto invertLeftShift = []( int k ){
            int count = 0;
            int bit = 1;
            while( !( k & bit ) ) count++, bit <<= 1;
            return count;
        };
        bool matchSuccess = true;
        while( rbn != 0 || rbb != 0 ){
            int tmp = rb[rbn][rbb].first;
            if( tmp<0 ){
                matchSuccess = false;
                break;
            }
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
        if( matchSuccess ) break;
    }
    std::cout << " complete!" << std::endl;
}

void SwissSystemTournament::OutputMatching() {
    std::vector<bool> searched(imagPlayerNumber(),false);
    picojson::object matchingJson;
    picojson::array matchingList;
    for( const Player* p : playersPermutation ) {
        if( !searched[p->id] ) {
            picojson::object matching;
            matching.insert(std::make_pair("playerID",picojson::value(double(p->id))));
            matching.insert(std::make_pair("opponentID",picojson::value(double(p->opponent->id))));
            // matching.insert(std::make_pair("winCount", picojson::value(0.0)));
            // matching.insert(std::make_pair("loseCount",picojson::value(0.0)));
            // matching.insert(std::make_pair("drawCount",picojson::value(0.0)));
            matching.insert(std::make_pair("winCount", picojson::value(double(p->id%7))));
            matching.insert(std::make_pair("loseCount",picojson::value(double(p->opponent->id%6))));
            matching.insert(std::make_pair("drawCount",picojson::value(double(11-p->id%7-p->opponent->id%6))));
            matching.insert(std::make_pair("withdraw", picojson::value(false)));
            matching.insert(std::make_pair("withdrawn",picojson::value(false)));
            searched[p->id] = true;
            searched[p->opponent->id] = true;
            matchingList.emplace_back(matching);
        }
    }
    matchingJson.insert(std::make_pair("matchingList",picojson::value(matchingList)));
    std::string matchingFileName;
    matchingFileName = MatchingName + "/matching_" + std::to_string(matchedRounds);
    std::fstream fs ( matchingFileName + ".json", std::ios::out );
    fs << picojson::value(matchingJson).serialize(true) << std::endl;
    fs.close();
}

void SwissSystemTournament::InputMatchResult() {
    std::cout << "match result proceed... ";

    std::ifstream ifs( MatchingName + "/matching_" + std::to_string(matchedRounds) + ".json", std::ios::in );
    const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    picojson::value matchingJSON;
    const std::string err = picojson::parse(matchingJSON, json);
    if( !err.empty() ) {
        std::cerr << err;
    }

    picojson::object matchingListObject = matchingJSON.get<picojson::object>();
    picojson::array matchingList = matchingListObject["matchingList"].get<picojson::array>();
    for ( const picojson::value& matchingValue : matchingList ){
        picojson::object matching = matchingValue.get<picojson::object>();
        int leftID        = matching["playerID"  ].get<double>();
        int rightID       = matching["opponentID"].get<double>();
        int leftWinCount  = matching["winCount"  ].get<double>();
        int leftDrawCount = matching["drawCount" ].get<double>();
        int leftLoseCount = matching["loseCount" ].get<double>();
        bool leftDrew      = matching["withdraw"  ].get<bool>();
        bool leftDrawn     = matching["withdrawn" ].get<bool>();
        Player *left = &players[leftID];
        Player *right = &players[rightID];

        // /**
        //  * 相手がドロップしてた（withdrew）なら勝手に不戦勝にする
        //  */
        if( !leftDrew ) leftDrew = left->withdrew;
        if( !leftDrawn ) leftDrawn = right->withdrew;
        // /**
        //  *
        //  */

        MatchResult leftResult(leftWinCount, leftDrawCount, leftLoseCount, leftDrew, leftDrawn, right);
        MatchResult rightResult(leftLoseCount, leftDrawCount, leftWinCount, leftDrawn, leftDrew, left);
        left->pushMatchedResults(leftResult);
        right->pushMatchedResults(rightResult);
    }

    matchedRounds++;
    std::cout << " complete!" << std::endl;
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

    playersPermutation.clear();
    for( int i = 0; i < imagPlayerNumber(); i++ ){
        playersPermutation.push_back(&players[i]);
    }

    CalculatePlayerState();
    std::sort(playersPermutation.begin(), playersPermutation.end(),
              []( const Player* lp, const Player* rp) -> bool {
                  if( rp->withdrew == lp->withdrew ) {
                      if (lp->points == rp->points) {
                          if (std::abs(lp->opponentMatchWinPercentage - rp->opponentMatchWinPercentage) < EPS) {
                              if (std::abs(lp->gameWinPercentage - rp->gameWinPercentage) < EPS) {
                                  if (std::abs(lp->opponentGameWinPercentage - rp->opponentGameWinPercentage) < EPS) {
                                      return lp->rating > rp->rating;
                                  } else { return lp->opponentGameWinPercentage > rp->opponentGameWinPercentage; }
                              } else { return lp->gameWinPercentage > rp->gameWinPercentage; }
                          } else { return lp->opponentMatchWinPercentage > rp->opponentMatchWinPercentage; }
                      } else { return lp->points > rp->points; }
                  } else { return rp->withdrew; }
              });
};

void SwissSystemTournament::OutputFinalResult() {
    std::cout << "player standing HTML make...";
    SortPlayers();
    int grade = 0;
    int tie = 0;
    std::string resultHTMLFileName;
    resultHTMLFileName = MatchingName + "/result_" + std::to_string(matchedRounds) + ".html";
    std::fstream fs( resultHTMLFileName, std::ios::out );
    fs << "<html>"
       << "<head>"
       << "<meta charset='UTF-8'>"
       << "<style>"
       << "table {"
       << "border-collapse: collapse;"
       << "}"
       << "td,th{"
       << "border: solid 1px;"
       << "}"
       << ".win-match{"
       << "background: #CDFCDD"
       << "}"
       << ".lose-match{"
       << "background: #FDDCCD"
       << "}"
       << ".draw-match{"
       << "background: #FDFCCD"
       << "}"
       << ".no-match{"
       << "background: #cDcCCD"
       << "}"
       << "</style>"
       << "</head>"
       << "<body>"
       << "<table>"
       << "<tr>"
       << "<th>順位</th>"
       << "<th>ID</th>"
       << "<th>R数</th>"
       << "<th>勝点</th>"
       << "<th>OMWP</th>"
       << "<th>GWP</th>"
       << "<th>OGWP</th>";
    for (int i = 0; i < matchedRounds; ++i) {
        fs << "<th>R" << i+1 << "</th>";
    }
    fs << "</tr>";
    for ( const Player* playerP : playersPermutation ){
        Player player = *playerP;
        if( player.withdrew ) continue;
        fs << "<tr>";
        // if( grade ){
        //     if( std::abs( player.opponentMatchWinPercentage - (player-1).opponentMatchWinPercentage ) < EPS
        //         &&  player.points == (player-1).points ) tie++;
        //     else tie = 0;
        // }
        fs << "<td>" << ++grade-tie << "</td>"
           << "<td>" << player.id << "</td>"
           << "<td>" << player.roundCount << "</td>" << std::fixed << std::setprecision(4)
           << "<td>" << player.points << "</td>"
           << "<td>" << player.opponentMatchWinPercentage << "</td>"
           << "<td>" << player.gameWinPercentage << "</td>"
           << "<td>" << player.opponentGameWinPercentage << "</td>";
        int rounds = 0;
        for( const MatchResult result : player.matchedResults ){
            fs << "<td ";
            if( result.isAvail() ) {
                fs << "class='" << (result.isWin() ? "win" : result.isDraw() ? "draw" : "lose")
                   << "-match'>" << result.opponent->id << " " << (result.isWin() ? "o" : result.isDraw() ? "-" : "x");
            }else{
                fs << "class='no-match'>";
            }
            fs << "</td>";
        }
        fs << "</tr>";
    }
    fs << "</table>"
       << "</body>"
       << "</html>";
    fs.close();
    std::cout << " complete!" << std::endl;
    std::cout << players.size() << std::endl;
}

int SwissSystemTournament::imagPlayerNumber() const {
    return PlayerNumber + hasDummyPlayer;
}

void SwissSystemTournament::dropOutByPoint( int supPoint ) {
    for( auto& player : players ){
        if ( player.points <= supPoint ) player.withdrew = true;
    }
}
