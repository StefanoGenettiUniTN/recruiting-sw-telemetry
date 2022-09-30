#include <stdio.h>
#include <iostream>
using namespace std;

extern "C"{
    #include "fake_receiver.h"
}

#include "Fsa.h"
#define CAN_DUMP_FILE "../candump.log" //CAN data flow

int main(void){

    //Variable operation control
    int result_open_can;
    int result_can_receive;
    //...

    char message[MAX_CAN_MESSAGE_SIZE];
    Fsa finite_state_machine;   //object which implements the final state machine behaviour

    cout << "Welcome to Project 2" << endl;

    //Open data flow source
    result_open_can = open_can(CAN_DUMP_FILE);
    if(result_open_can==-1){
        cerr<<"Error opening simulated CAN interface"<<endl;
        return -1;
    }

    //Read the data flow
    while(result_can_receive=can_receive(message)>0){
        if(result_can_receive==-1){
            cerr<<"Error can_receive"<<endl;
            return -1;
        }
        finite_state_machine.transition(message);
    }

    //Close CAN interface
    close_can();

    return 0;
}