#include <QueueArray.h>//data structure to hanlde data packets
#include <Streaming.h>//Makes output formatting easy
#include <MsTimer2.h>//handles interrupts

//vars====================================================
//id will be used to determine node's turn to send
#define ID 2
//num of nodes in network
#define num_nodes 3

//This array will hold all packets
//For each packet there will be three elements: [id, time, value]
QueueArray <unsigned short> queue;

boolean output = HIGH; //Track if LED should be on or off
unsigned short tm; //Timer
//=========================================================

//Helper functions ========================================
void sample();
void transmit();
void stop_sampling();
//=========================================================

//run program ===============================================
void setup() {
  Serial.begin(9600);//baud
  pinMode(13, OUTPUT); //sync led
  pinMode(A0, INPUT); //sensor
  tm = 0;//init time
  // set the printer of the queue.
  queue.setPrinter(Serial);
}

void loop() {
  //init char with a dummy value
  char cmd = '0';
  //Get byte off of serial buffer
  cmd = Serial.read();


  //If R then begin sampling
  //If S then re-sync
  //If T then terminate sampling
  if (cmd == 'R') {
    //start sampling
    MsTimer2::set(1000, sample); //Sample 1hz
    MsTimer2::start();
  }else if (cmd == 'S') {
    //sync lights and samples
    MsTimer2::stop();
    output = HIGH;
    MsTimer2::set(1000, sample); //Sample 1hz
    MsTimer2::start();
  }else if (cmd == 'T') {
    //Stop sampling
    MsTimer2::stop();
    stop_sampling();
  }
}
//===================================================================

//Helper Functions===================================================
//samples and toggles led in unison.
void sample() {
  //Toggle light D13 LED--------------------
  digitalWrite(13, output);
  output = !output;
  //----------------------------------------
  //create data packet
  queue.push(ID);
  queue.push(tm);
  queue.push(analogRead(A0));

  //We add 1 to the number of nodes so that we don't get a 0 value on the mod
  //operation for the last node in the set of nodes
  if (tm % num_nodes + 1 == ID) { //node's turn to transmit
    transmit();
  }

  tm++;//increment the time counter
}

void stop_sampling() {

  //Allow the nodes to transmit their info in order
  //Node 1 will send immediately, Node 2 after 1 second, Node 3 after 2 seconds, etc
  //One second was picked because it should be safe for this assignment and several
  //more nodes, but I'd imagine after about 10 nodes 1 second my not be enough time
  //for a node to transmit all of it's data
  delay((ID - 1) * 1000);
  transmit();

  digitalWrite(13, LOW); //ensure that light is of when iterrupt is stopped
  tm = 0; //Reset the timer
}

//Transmit all packets in the buffer and empty it as you go
void transmit(){
    unsigned short count = 0; //Used to track if an entire packet has Tx

  while (!queue.isEmpty()) {
    //This will remove the front element in the buffer and pass it to the Serial port
    Serial << queue.dequeue();

    //Inc counter
    count ++;
    //If count is 3 then an entire packet has been sent
    //Reset count, send a new line, and continue
    //Else add a comma
    if (count == 3) {
      count = 0;
      Serial << endl;
    } else {
      Serial << ",";
    }
  }
}

//=============================================================================

