#include "SwissSystemTournament.h"
#include <cstdio>
#include <iostream>

int main() {

    int N,i=0;
    std::string matchingName;
    std::string tmp;
    std::cout << "MatchingName:";
    std::cin >> matchingName;
    std::cout << "MatchingMembers:";
    std::cin >> N;
    while( true ){
        if( i == -1 ) break;
        SwissSystemTournament s(N, i, matchingName);
        std::cout << "No." << i << " MatchInputReady?:";
        std::cin >> tmp;
        s.InputMatchResult();
        // s.dropOutByPoint(i*2-2);
        s.MakeJSONData();
        s.OutputFinalResult();
        i++;
    }
	return 0;
}
