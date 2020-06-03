//
// Created by Nishinoyama on 2020/05/09.
//

#ifndef SWISSSYSTEMTOURNAMENT_SWISSSYSTEMTOURNAMENT_H
#define SWISSSYSTEMTOURNAMENT_SWISSSYSTEMTOURNAMENT_H

#pragma once
#include "Player.h"
#include <vector>
#include <string>

class SwissSystemTournament {

private :

    int PlayerNumber;
    int MatchingNumber;
    int matchedRounds;
    bool hasDummyPlayer;
    bool loadingJsonMode{};
    std::string MatchingName{};
    std::vector<Player> players;
    std::vector<Player*> playersPermutation;
    constexpr const static double EPS = 1e-8;
    constexpr const static int INF = 1e9;

public :

    explicit SwissSystemTournament();
    explicit SwissSystemTournament( int roundNumber, const std::string& matchingName );
    ~SwissSystemTournament();
    void build();

    int imagPlayerNumber() const;

    void Matching();
    void OutputMatching();
    void InputMatchResult();
    void CalculatePlayerState();
    void OutputPlayerState();
    void Print() const;
    void OutputFinalResult();
    void SortPlayers();
    void MakeJSONData();

    void initFromJSON();
};


#endif //SWISSSYSTEMTOURNAMENT_SWISSSYSTEMTOURNAMENT_H
