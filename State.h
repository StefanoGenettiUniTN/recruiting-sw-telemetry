#include <map>

using namespace std;

enum State {
	Idle,
	Run
};
typedef enum State State;

map<State, State> transitionFunction = {
	{Idle, Run},
	{Run, Idle}
};