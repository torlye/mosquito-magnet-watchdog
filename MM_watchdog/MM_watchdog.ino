#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include <StateMachine.h>
#include <Filters.h>
#define Current_sensor A0  // The sensor analog input pin
#define MIN(a,b) (((a)<(b))?(a):(b))

const int STATE_DELAY = 30;                    // delay between each iteration of the main loop
const float testFrequency = 50;                // test signal frequency (Hz)
const float windowLength = 40.0/testFrequency; // how long to average the signal, for statistics
const float intercept = 0.0;                   // to be adjusted based on calibration testing
const float slope = 0.0285;                    // to be adjusted based on calibration testing
const unsigned long printPeriod = 1000;        // Interval to measure RMS current and update LCD
unsigned long previousMillis = 0;              // last time RMS current was computed and LCD updated
unsigned long lastTransitionTime = 0;          // Time of last state transition
float Amps_TRMS;                               // Computed RMS current
float ACS_Value;                               // Raw measured current
const float voltage_threshold = 0.3;           // Voltage threshold to transition state
const int relayOut = 8;                        // Relay output pin
const char stateMsg_On[] = "On ";              // LCD messages for each state
const char stateMsg_OnToOff[] = "On_";
const char stateMsg_Off[] = "Off";
const char stateMsg_OffToOn[] = "On^";
char stateMsg[] = "   ";

LiquidCrystal_PCF8574 lcd(0x27);
RunningStatistics inputStats; // create statistics to look at the raw test signal
StateMachine machine = StateMachine();
State* S0 = machine.addState(&state0_on);
State* S1 = machine.addState(&state1_onToOff);
State* S2 = machine.addState(&state2_off);
State* S3 = machine.addState(&state3_offToOn);

void setup() {
  int error;
  Serial.begin(9600);
  S0->addTransition(&transitionS0S1,S1);
  S1->addTransition(&transition_wait10,S2);
  S1->addTransition(&transitionS1S0,S0);
  S2->addTransition(&transition_wait10,S3);
  S3->addTransition(&transition_wait10,S0);

  pinMode(Current_sensor, INPUT);
  pinMode(relayOut, OUTPUT);
  digitalWrite(relayOut, HIGH);

  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD found.");
    lcd.begin(16, 1, Wire); // initialize the lcd
    lcd.setBacklight(128);
  } else {
    Serial.println(": LCD not found.");
  }
  
  inputStats.setWindowSecs( windowLength );
}

bool alternateFlag = false;

void loop() {
  ACS_Value = analogRead(Current_sensor);  // read the analog in value:
  inputStats.input(ACS_Value);  // log to Stats function
        
    if((unsigned long)(millis() - previousMillis) >= printPeriod) { //every second we do the calculation
      previousMillis = millis();   // update time
      
      Amps_TRMS = max(intercept + slope * inputStats.sigma(), 0);
  
      lcd.home();
      lcd.clear();

      if (machine.currentState == 0 && alternateFlag) {
        printElapsedString();
      }
      else {
        lcd.print(Amps_TRMS,1);
        lcd.print("A ");
        lcd.print(stateMsg);
      }
      alternateFlag = !alternateFlag;
    }
  
  machine.run();
  delay(STATE_DELAY);
}

void state0_on(){
  if(machine.executeOnce){
    digitalWrite(relayOut, HIGH);
    strcpy(stateMsg, stateMsg_On);
    lastTransitionTime = millis();
  }
}

void state1_onToOff(){
  if(machine.executeOnce){
    digitalWrite(relayOut, HIGH);
    strcpy(stateMsg, stateMsg_OnToOff);
    lastTransitionTime = millis();
  }
}

void state2_off(){
  if(machine.executeOnce){
    digitalWrite(relayOut, LOW);
    strcpy(stateMsg, stateMsg_Off);
    lastTransitionTime = millis();
  }
}

void state3_offToOn(){
  if(machine.executeOnce){
    digitalWrite(relayOut, HIGH);
    strcpy(stateMsg, stateMsg_OffToOn);
    lastTransitionTime = millis();
  }
}

bool transitionS0S1(){
  if (Amps_TRMS < voltage_threshold) {
    Serial.println("transition to S1");
    return true;
  }
  return false;
}

bool transitionS1S0(){
  if (Amps_TRMS > voltage_threshold) {
    Serial.println("transition to S0");
    return true;
  }
  return false;
}

bool transition_wait10(){
  if((unsigned long)(millis() - lastTransitionTime) >= 10000) {
    Serial.println("transition to next state");
    return true;
  }
  return false;
}

void printElapsedString() {
  unsigned long elapsedSeconds = (millis() - lastTransitionTime)/1000;
  unsigned int seconds = elapsedSeconds % 60;
  unsigned int minutes = (elapsedSeconds/60) % 60;
  unsigned int hours = (elapsedSeconds/3600) % 24;
  unsigned int days = MIN(elapsedSeconds/86400, 99);
  
  char buf[3];
  if (days > 0) {
    sprintf(buf, "%02u", days);
    lcd.print(buf);
    lcd.print("d");
  }
  sprintf(buf, "%02u", hours);
  lcd.print(buf);
  lcd.print(":");
  sprintf(buf, "%02u", minutes);
  lcd.print(buf);
  if (days == 0) {
    lcd.print(":");
    sprintf(buf, "%02u", seconds);
    lcd.print(buf);
  }
}
