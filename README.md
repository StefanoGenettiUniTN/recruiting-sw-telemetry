# recruiting-sw-telemetry
## Telemetry Project_2
The purpose of this repository is to implement a possible solution to the second project about telemetry proposed in the [E-Agle recruiting software folder](https://github.com/eagletrt/recruiting-sw.git).

**project_2:** Build the most basic and crucial part of a telemetry software: logging every information received.

The task is to implement a basic Finite State Machine with Idle and Run states, both in Idle and Run data from "CAN" are received and have to be parsed. Two specific messages trigger a state transition. Only in Run state the telemetry logs the data. Then with the parsed messages some basic statistics are computed.

### Finite State Machine
The implemented finite state machine is illustrated in the following schema.
![FSM](https://user-images.githubusercontent.com/57845191/193401858-47b50ea7-75ee-48c4-8486-ee21afd82d3a.png)

The file `State.h` contains:
- `enum State` which lists all the possible states. In this case Run and Idle.
- `map<State, State> transitionFunction` is the transition function. `transitionFunction[current state]` indicates the destination state where the control has to move from the current state. Maybe a easier solution could have been adopted since there are only two states. However, this solution has been considered scalable even if new states are added in a future integration. If the finite state machine had a lot of states it would have been more convenient to adopt a more complex finite state machine design pattern. For example, a solution would have been to implement a class for each possible state with the proper methods to mimic the desired behaviour.

The computational behaviour of the finite state machine is described by the header file `Fsa.h` and its corresponding implementation `Fsa.cpp`. The core method of the class `Fsa` is `void transition(char message[MAX_CAN_MESSAGE_SIZE])`. This method is called whenever a new message is recived from the data stream.

```
#main.cpp
//Read the data flow
while(result_can_receive=can_receive(message)>0){
    if(result_can_receive==-1){
        cerr<<"Error can_receive"<<endl;
        return -1;
    }
    finite_state_machine.transition(message);
}
```

According to the `currentState`, the method performs the proper operations as described by the problem requirements.

```
#Fsa.cpp
//Different behavoiur according to current state
switch(this->currentState){
    case Idle:
        [IDLE STATE BEHAVIOUR]
        break;

    case Run:
        [RUN STATE BEHAVIOUR]
        break;
}
```

As written above, this solution would become impractical for final state machines with a lot of states. However, if the number of possible states is reasonably contained, this structure is easily scalable.

### Logged file
In the Run state it is required to store the raw messages in a file. The log files are stored in the folder `log`. Given that each session must have its own file, the proposed solution creates an unique identifier for each session. The session ID is formed by the unix timestamp and a number generated randomly. Each log file is named with the session unique identifier.
- [unix timestamp]_[random number]

This operation is performed by the method `void create_session()` in `Fsa.cpp`.

```
#Fsa.cpp
//Create session identifier in order to uniquely identify the log file for raw data
//Structure: [unix time]_[rand value]
raw_log_session_name = "../log/";
raw_log_session_name += to_string(time(0));
raw_log_session_name += "_";
srand(time(0));
raw_log_session_name += to_string((rand()%(1000-1+1))+1);
//...end creating session identifier
```

### Parsing
Each received message is parsed in order to get the message ID and the message payload. The parsing is performed by the method `void transition(char message[MAX_CAN_MESSAGE_SIZE])` in `Fsa.cpp`. The computation can be summurized as follows:
1. Read the elements of the array `char message[]` until the algorithm finds the character '#'. Each character is appended to an initially empty string named `string ID_string`. 
2. At this point the string `ID_string` containes the message identifier. This value is converted to be stored in the variable `uint16_t ID` using the instruction `ID = stoul(ID_string, nullptr, 16)`.
3. After that, the elements of the array `char message[]` are visited until the `NULL` character. Each character is appended to an initially empty string named `string payload_string`.
4. Finally, the string `payload_string`, which containes the message payload, is converted to be stored in the variable `uint64_t payload` using the instruction `payload = stoul(payload_string, nullptr, 16)`.

### Statistics
The class `Fsa` has three attributes to store the statistics recorded during the computation:
- `vector<string> message_identifiers`: list of message identifiers which have been recieved.
- `map<string, int> number_of_messages`: for each message ID we store the number of messages which have been received with that ID.
- `map<string, vector<uint64_t>> mean_time`: each message ID has its own vector of timestamps. Every time we receive a message in the Run state we append the timestamp at the end of the list of the corresponing ID. This time, the timestamp is expressed in millisecond otherwise the elapsed time beween messages of the same ID is not significant. In order to retrive the unix time in millisecond I have used the instruction `uint64_t millisecond_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()` from library `chrono.h`.

While the machine is in the Run state the statistics are updated through the method `void update_statistics(string message_id)` which updates the statistics about the messages with ID equals to `string message_id`.

The interesting statistics are stored in a `.csv` file in the log folder. The code which is executed to store the statistics is implemented in the method `void store_statistics()` of class `Fsa`. The statistics are stored when the machine is the Run state and a stop message is received. Each `.csv` file is named with the session name. So, if for example the session ID is 123, then the corresponding file of the statistics is saved as 123_statistics.csv. In this way it is easy to understand which are the statistics which are associated with each session.
