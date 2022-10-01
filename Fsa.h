#include "State.h"
#include "fake_receiver.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <ctime>
#include <chrono>
#include <vector>
#include <map>

/**
	This class abstracts the Finite State Machine behaviour.
	
	Methods:
	- Fsa(): 								set the initial state.
	- getCurrentState(): 					get the current state of the machine. The possible states are enumerated in State.h.
	- transition(message): 					given the message recived in input the machine has to perform some operations according to the current state and eventually change state
	- create_session():						create a session when we move to the run state 
	- close_session(): 						close the session when we go to idle from run state
	- update_statistics(string message_id):	each time we read a message from the data flow, we update the interesting statistics of the current session
	- store_statistics()					store the statistics in a CSV file

	Attributes:
	- currentState: 		current state of the machine
	- raw_log_session_name:	each session has its own log file for raw data
	- message_identifiers:	message IDs
	- number_of_messages:	message ID occurences
	- mean_time:			key: message ID ; value: vector of timestamps

	- ofstream raw_log:		log file where to store raw data
	- ofstream statistics:	file with the stored statistics
*/

class Fsa
{
public:
	Fsa();

	ofstream raw_log;   								//log file where to store raw data
	ofstream statistics;								//file with the stored statistics

	State getCurrentState();
	
	void transition(char message[MAX_CAN_MESSAGE_SIZE]);

	void create_session();
	void close_session();

	void update_statistics(string message_id);
	void store_statistics();							//store the statistics in a CSV file

private:
	State currentState;

	string raw_log_session_name;    					//each session has its own log file for raw data

	vector<string> message_identifiers;					//message IDs
	map<string, int> number_of_messages;				//message ID occurences
	map<string, vector<uint64_t>> mean_time;			//key: message ID ; value: vector of timestamps
};