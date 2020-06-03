#include "SwissSystemTournament.h"
#include <cstdio>
#include <iostream>

int main() {

    int N;
    std::string matchingName;
    std::string tmp;
    std::cout << "MatchingName:";
    std::cin >> matchingName;
    std::cout << "MatchingTimes:";
    std::cin >> N;
    for( int i = 0; i < N; i++ ){
        SwissSystemTournament s(i, matchingName);
        std::cout << "No." << i << " MatchInputReady?:";
        // std::cin >> tmp;
        s.InputMatchResult();
        s.OutputFinalResult();
        s.MakeJSONData();
    }
	return 0;
}
