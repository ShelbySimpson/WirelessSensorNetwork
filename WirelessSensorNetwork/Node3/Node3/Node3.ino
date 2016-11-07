#include <QueueArray.h>//data structure to hanlde data packets
#include <Streaming.h>//Makes output formatting easy
#include <MsTimer2.h>//handles interrupts

//vars====================================================
//id will be used to determine node's turn to send
#define ID 3
//num of nodes in network
#define num_nodes 3
unsigned short tm;//track time in seconds
QueueArray <unsigned short> queue;//buffer for data packets
boolean output = HIGH;
//=========================================================

//Helper functions ========================================
void sample();
void emptyBuffer();
void stop_sampling();
//=========================================================

//run program ===============================================
void setup() {
  Serial.begin(9600);//baud
  pinMode(13, OUTPUT); //sync led
  pinMode(5, INPUT); //sensor
  tm = 0;//init time
  // set the printer of the queue.
  queue.setPrinter (Serial);
}

void loop() {
  char cmd = '0';
  cmd = Serial.read();
  if (cmd == 'R') {
    //start sampling
    MsTimer2::stop();
    MsTimer2::set(1000, sample); //Sample 1hz
    MsTimer2::start();
  }

  if (cmd == 'S') {
    //sync lights and samples
    MsTimer2::stop();
    output = HIGH;
    MsTimer2::set(1000, sample); //Sample 1hz
    MsTimer2::start();
    sample();
  }

  if (cmd == 'T') {
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
  queue.push(digitalRead(5));
  if (tm % num_nodes + 1 == ID) { //node's turn to transmit
    //Serial << "this is time" << tm << endl;
    emptyBuffer();
  }
  tm++;//increment the time counter
}

void stop_sampling() {

  delay((ID - 1) * 1000);

  emptyBuffer();
  digitalWrite(13, LOW); //ensure that light is of when iterrupt is stopped
  tm = 0;
}

//empty buffer as packets are sent
void emptyBuffer(){
    unsigned short count = 0;

     while (!queue.isEmpty()) {
      Serial << queue.dequeue();//transmit data
      //format packet output: ID,Time,Data
      count ++;
      if (count == 3) {
        count = 0;
        Serial << endl;
      } else {
        Serial << ",";
      }
    }
}

//=============================================================================
