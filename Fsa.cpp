#include "Fsa.h"

using namespace std;

Fsa::Fsa()
{   
	currentState = Idle;
}

State Fsa::getCurrentState()
{
	return this->currentState;
}

void Fsa::transition(char message[MAX_CAN_MESSAGE_SIZE]){
    int counter;

    string ID_string="";
    uint16_t ID;

    string payload_string="";
    uint64_t payload;

    //Parsing
    counter=0;

    //Read ID
    while(message[counter]!='#'){
        ID_string+=message[counter];
        counter++;
    }
    ID = stoul(ID_string, nullptr, 16);

    //Read payload
    counter++;
    while(message[counter]!=NULL){
        payload_string+=message[counter];
        counter++;
    }
    payload = stoul(payload_string, nullptr, 16);
    //...end parsing

    //Different behavoiur according to current state
    switch(this->currentState){
        case Idle:
            //When receive the start message transition to Run state
            if(ID==0x0A0 && payload==0x6601){
                this->currentState = transitionFunction[currentState];
                this->create_session();
            }
            if(ID==0x0A0 && payload==0xFF01){
                this->currentState = transitionFunction[currentState];
                raw_log.open("../log/log.txt", ios_base::app);
                this->create_session();
            }
            break;

        case Run:
            //When receive the stop message transition to Idle state
            if(ID==0x0A0 && payload==0x66FF){
                this->currentState = transitionFunction[currentState];
                this->close_session();
                break;
            }
            raw_log<<"("<<time(0)<<") "<<ID_string<<"#"<<payload_string<<endl;
            break;
    }
}

void Fsa::create_session(){
    string raw_log_session_name;    //each session has its own log file for raw data

    //Create session identifier in order to uniquely identify the log file for raw data
    //Structure: [unix time]_[rand value]
    raw_log_session_name = "../log/";
    raw_log_session_name += to_string(time(0));
    raw_log_session_name += "_";
    srand(time(0));
    raw_log_session_name += to_string((rand()%(1000-1+1))+1);
    //...end creating session identifier

    raw_log.open(raw_log_session_name, ios_base::app);
}

void Fsa::close_session(){
    raw_log.close();
}