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
                this->create_session();
            }
            break;

        case Run:
            //When receive the stop message transition to Idle state
            if(ID==0x0A0 && payload==0x66FF){
                this->store_statistics();                               //create csv file with interesting session statistics
                this->currentState = transitionFunction[currentState];
                this->close_session();                                  //close current session
                break;
            }

            //Write record in log file
            raw_log<<"("<<time(0)<<") "<<ID_string<<"#"<<payload_string<<endl;

            //Update statistics increasing the number of messages with the current ID and the time when the message has been recived
            update_statistics(ID_string);

            break;
    }
}

void Fsa::create_session(){
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

void Fsa::update_statistics(string message_id){
    message_identifiers.push_back(message_id);
    number_of_messages[message_id]++;

    uint64_t millisecond_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    mean_time[message_id].push_back(millisecond_time);
}

void Fsa::store_statistics(){
    string statistics_file_name = raw_log_session_name+"_"+"statistics.csv";    //each session has its own statistics csv file

    statistics.open(statistics_file_name);

    //Write CSV header
    statistics<<"ID,number_of_messages,mean_time"<<endl;

    for(int i=0; i<message_identifiers.size(); ++i){
        string message_ID = message_identifiers[i];

        int occurences = number_of_messages[message_ID];

        long double mean_time_between_ID = -1.0;
        vector<uint64_t> timestamps = mean_time[message_ID];
        unsigned long int time_differences_sum=0;

        //If number of occurences of the given ID is greather than 1 --> compute the statistics of the elapsed time beween messages of the same ID
        if(occurences>1){
            for(int t=0; t<timestamps.size()-1; t++){
                unsigned long int diff = timestamps[t+1]-timestamps[t];
                time_differences_sum+=diff;
            }
            mean_time_between_ID = time_differences_sum/(1.0*(occurences-1));
        }

        //Write CSV record [message_ID,number_of_messages,mean_time]
        statistics<<message_ID<<","<<occurences<<","<<mean_time_between_ID<<endl;
    }


    statistics.close();
}