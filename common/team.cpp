#include "team.hpp"
#include <cstring>

using namespace std;

char parse_team_string(const char* team_string) {
	if (strcasecmp(team_string, "blue") == 0) {
		return 'A';
	} else if (strcasecmp(team_string, "red") == 0) {
		return 'B';
	}
	return 0;
}
