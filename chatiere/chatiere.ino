
#define IN1  8
const int START_OPENING_PIN = 4;
const int CHANGE_DIRECTION_PIN = 2;
const int CLOSED_PIN = 3;
const int BAUDRATE = 9600;
const int NBSTEPS = 4096;
const int DEBOUNCE = 200;
const long STEPTIME = 900;
const float stepAngle = 0.087890625;

int arrayDefault[4] = {LOW, LOW, LOW, LOW};

int stepsMatrix[8][4] = {
  {LOW,  LOW,  LOW,  HIGH},
  {LOW,  LOW,  HIGH, HIGH},
  {LOW,  LOW,  HIGH, LOW},
  {LOW,  HIGH, HIGH, LOW},
  {LOW,  HIGH, LOW,  LOW},
  {HIGH, HIGH, LOW,  LOW},
  {HIGH, LOW,  LOW,  LOW},
  {HIGH, LOW,  LOW,  HIGH},
};

int Step = 0;
long steps = 0;
boolean goingClockwise = true;
boolean isRunning = false;

unsigned long reverseDirectionInterrupt;
unsigned long lastStopInterrupt;

unsigned long lastTime;

void setup(){
    Serial.begin(BAUDRATE);
    Serial.println("Demarrage...");
    pinMode(START_OPENING_PIN, INPUT);
    pinMode(CHANGE_DIRECTION_PIN, INPUT);
    pinMode(CLOSED_PIN, INPUT);
    attachInterrupt(0, reverseDirection, FALLING);
    attachInterrupt(1, stopIt, FALLING);
    for(int i=0; i<4; i++){
      pinMode(IN1+i, OUTPUT); 
    }
}

float angle(){
  return stepAngle * (steps % 4096);
}

void reverseDirection(){
  unsigned long now = millis();
  if (now > reverseDirectionInterrupt + DEBOUNCE){
    if(goingClockwise){
      Serial.println("<--");
    }else{
      Serial.println("-->");
    }
    goingClockwise = !goingClockwise;
  }
  reverseDirectionInterrupt = now;
}

void stopIt(){
  unsigned long now = millis();
  if (now > lastStopInterrupt + DEBOUNCE){
    if(isRunning){
      Serial.println(angle());
      Serial.println("Arret\n");
      isRunning = false;
      goingClockwise = true;
    }
  }
  lastStopInterrupt = now;
}

void writeStep(int outArray[4]){
  for (int i=0;i < 4; i++){
    digitalWrite(IN1 + i, outArray[i]);
  }
}

void stepper(){
  if (isRunning){
    if ((Step >= 0) && (Step < 8)){
      writeStep(stepsMatrix[Step]);
    }else{
      writeStep(arrayDefault);
    }
  }
  setDirection();
} 

void setDirection(){
  if(goingClockwise) {
    Step--;
    if(isRunning){
      steps++;
    }
  }else{
   Step++;
   if(isRunning){
      steps--;
    }
  }

  if(Step>7){
    Step=0;
  }else if(Step<0){
    Step=7; 
  }
}


void loop(){  
  unsigned long now;
  if ((isRunning == false) && (digitalRead(START_OPENING_PIN) == LOW)){
    isRunning = true;
  }
  do {
      now = micros();
      if((now - lastTime) >= STEPTIME){
        lastTime = now;
        stepper();
      }
  } while(isRunning);
}


