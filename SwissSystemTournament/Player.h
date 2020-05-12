//
// Created by Nishinoyama on 2020/05/09.
//

#ifndef SWISSSYSTEMTOURNAMENT_PLAYER_H
#define SWISSSYSTEMTOURNAMENT_PLAYER_H
#include <set>
#include <vector>

class Player {

public:

    explicit Player();
    ~Player();

    void pushMatchedResults( Player* player, bool victory );
    void calculateMatchWinPercentage();
    void calculateOpponentMatchWinPercentage();

    bool isDummy;
    int id;
    int points;
    Player* opponent;
    double opponentMatchWinPercentage;
    int roundCount;
    double matchWinPercentage;

    std::set<int> matchedPlayerID;
    std::vector<std::pair<Player*,bool>> matchedResults;

};

#endif //SWISSSYSTEMTOURNAMENT_PLAYER_H
