#include "SwissSystemTournament.h"
#include <cstdio>
#include <iostream>

int main() {

    int N,i;
    std::string matchingName;
    std::string tmp;
    std::cout << "MatchingName:";
    std::cin >> matchingName;
    std::cout << "MatchingMembers:";
    std::cin >> N;
    while( true ){
        std::cout << "MatchingTime:";
        std::cin >> i;
        if( i == -1 ) break;
        SwissSystemTournament s(N, i, matchingName);
        std::cout << "No." << i << " MatchInputReady?:";
        std::cin >> tmp;
        s.InputMatchResult();
        s.OutputFinalResult();
        s.MakeJSONData();
    }
	return 0;
}
