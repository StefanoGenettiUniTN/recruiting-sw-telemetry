#include "State.h"
#include "fake_receiver.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <ctime>
/**
	This class abstracts the Finite State Machine behaviour.
	
	Methods:
	- Fsa(): set the initial state.
	- getCurrentState(): get the current state of the machine. The possible states are enumerated in State.h.
	- transition(message): given the message recived in input the machine has to perform some operations according to the current state and eventually change state

	Attributes:
	- currentState: current state of the machine
*/

class Fsa
{
public:
	Fsa();
	State getCurrentState();
	void transition(char message[MAX_CAN_MESSAGE_SIZE]);
	ofstream raw_log;   //log file where to store raw data
	void create_session();
	void close_session();

private:
	State currentState;
};