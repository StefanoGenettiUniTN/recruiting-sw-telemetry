#include <stdio.h>
#include <iostream>
#include <string.h>
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
    
}