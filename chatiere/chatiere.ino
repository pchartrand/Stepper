
#define IN1  8
const int CHANGE_DIRECTION_PIN = 2;
const int CLOSED_PIN = 3;
const int EXIT_PIN = 4;
const int ENTERING_PIN = 5;
const int INSIDE_PIN = 6;
const int OUTSIDE_PIN = 7;
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
boolean entering = false;
boolean exiting = false;

unsigned long reverseDirectionInterrupt;
unsigned long lastStopInterrupt;

unsigned long lastTime;

void setup(){
    Serial.begin(BAUDRATE);
    Serial.println("Demarrage...");
    pinMode(ENTERING_PIN, INPUT);
    pinMode(EXIT_PIN, INPUT);
    pinMode(CHANGE_DIRECTION_PIN, INPUT);
    pinMode(CLOSED_PIN, INPUT);
    pinMode(INSIDE_PIN, OUTPUT);
    pinMode(OUTSIDE_PIN, OUTPUT);
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

boolean openingRequested(){
  entering = (digitalRead(ENTERING_PIN) == LOW);
  exiting = (digitalRead(EXIT_PIN) == LOW);
  if (entering){
    Serial.println("demande d'entree");
  }
  if (exiting){
    Serial.println("demande de sortie");
  }
  return  (entering || exiting);
}

void loop(){  
  unsigned long now;
  if ((isRunning == false) && openingRequested()){
    isRunning = true;
  }
  digitalWrite(OUTSIDE_PIN, exiting);
  digitalWrite(INSIDE_PIN, entering);
  do {
      now = micros();
      if((now - lastTime) >= STEPTIME){
        lastTime = now;
        stepper();
      }
  } while(isRunning);
  
  if (entering){
    Serial.println("entree completee");
  }
  if (exiting){
    Serial.println("sortie completee");
  }
}


