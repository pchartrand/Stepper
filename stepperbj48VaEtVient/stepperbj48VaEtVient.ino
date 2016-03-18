
#define IN1  8
const int directionPin = 2;
const int startStopPin = 3;
const int BAUDRATE = 9600;
const int NBSTEPS = 4096;
const int DEBOUNCE = 200;
const long STEPTIME = 900;
const float stepAngle = 0.087890625;

int arrayDefault[4] = {LOW, LOW, LOW, LOW};

int stepsMatrix[8][4] = {
  {LOW, LOW, LOW, HIGH},
  {LOW, LOW, HIGH, HIGH},
  {LOW, LOW, HIGH, LOW},
  {LOW, HIGH, HIGH, LOW},
  {LOW, HIGH, LOW, LOW},
  {HIGH, HIGH, LOW, LOW},
  {HIGH, LOW, LOW, LOW},
  {HIGH, LOW, LOW, HIGH},
};

int Step = 0;
long steps = 0;
boolean Clockwise = true;
boolean Running = false;

unsigned long lastChangeDirectionInterrupt;
unsigned long lastStartStopInterrupt;

unsigned long lastTime;

void setup(){
    Serial.begin(BAUDRATE);
    Serial.println("Demarrage...");
    pinMode(directionPin, INPUT);
    pinMode(startStopPin, INPUT);
    attachInterrupt(0, changeDirection, FALLING);
    attachInterrupt(1, startStop, FALLING);
    for(int i=0; i<4; i++){
      pinMode(IN1+i, OUTPUT); 
    }
}

float angle(){
  return stepAngle * (steps % 4096);
}

void changeDirection(){
  unsigned long now = millis();
  if (now > lastChangeDirectionInterrupt + DEBOUNCE){
    if(Clockwise){
      Serial.println("<--");
    }else{
      Serial.println("-->");
    }
    Clockwise = !Clockwise;
  }
  lastChangeDirectionInterrupt = now;
}

void startStop(){
  unsigned long now = millis();
  if (now > lastStartStopInterrupt + DEBOUNCE){
    if(Running){
      Serial.println(angle());
      Serial.println("Arret\n");
    }else{
      Serial.println("Depart");
    }
    Running = !Running;
  }
  lastStartStopInterrupt = now;
}

void loop(){  
  unsigned long now;
  do {
      now = micros();
      if((now - lastTime) >= STEPTIME){
        lastTime = now;
        stepper();
        if (Running && !(steps % 256)){
          Serial.println(angle());
        }
      }
  } while(Running);
}

void writeStep(int outArray[4]){
  for (int i=0;i < 4; i++){
    digitalWrite(IN1 + i, outArray[i]);
  }
}

void stepper(){
  if (Running){
    if ((Step >= 0) && (Step < 8)){
      writeStep(stepsMatrix[Step]);
    }else{
      writeStep(arrayDefault);
    }
  }
  setDirection();
} 

void setDirection(){
  if(Clockwise) {
    Step--;
    if(Running){
      steps++;
    }
  }else{
   Step++;
   if(Running){
      steps--;
    }
  }

  if(Step>7){
    Step=0;
  }else if(Step<0){
    Step=7; 
  }
}

