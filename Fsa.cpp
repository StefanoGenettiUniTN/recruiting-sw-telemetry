#include <stdio.h>
#include <iostream>
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
    cout<<"Message: "<<message<<endl;
}