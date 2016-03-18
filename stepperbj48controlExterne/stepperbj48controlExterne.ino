
#define IN1  8
const int directionPin = 2;
const int startStopPin = 3;
const int BAUDRATE = 9600;
const int NBSTEPS = 4096;
const float UNDEGRE = 11.38;
const int DEBOUNCE = 300;
const long STEPTIME = 900;

int Step = 0;
boolean Clockwise = true;
long Running = false;
long lastInterrupt;

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

unsigned long lastTime;
unsigned long time;  

void setup(){
    Serial.begin(BAUDRATE);
    Serial.println("Starting...");
    pinMode(directionPin, INPUT);
    attachInterrupt(0, changeDirection, FALLING);
    attachInterrupt(1, startStop, FALLING);
    for(int i=0; i<4; i++){
      pinMode(IN1+i, OUTPUT); 
    }
}

long tourneAngleCompense(int angle, boolean horaire){
  /* 
   duree de rotation fixe, quelque soit l'angle
  */
  int stepsLeft = floor(UNDEGRE * angle);
  long dureeStep = STEPTIME * (360 / angle);
  long steps = 0;

  Clockwise = horaire;
  Running  = true;
  unsigned long currentMicros;
  while(stepsLeft > 0){
    if(micros() - lastTime >= dureeStep){
      stepper(); 
      lastTime = micros();
      steps++;
      stepsLeft--;
    }
  }
  Running = false;
  return steps;
}

long tourneAngle(int angle, boolean horaire){
  int stepsLeft = floor(UNDEGRE * angle);
  long dureeStep = STEPTIME * (360 / angle);
  long steps = 0;

  Clockwise = horaire;
  Running  = true;
  
  unsigned long currentMicros;
  while(stepsLeft > 0){
    if(micros() - lastTime >= STEPTIME){
      stepper(); 
      lastTime = micros();
      steps++;
      stepsLeft--;
    }
  }
  Running = false;
  return steps;
}

long tourneDurant(long dureeMillis, boolean horaire){
  Clockwise = horaire;
  Running  = true;
  
  long startTime = micros();
  long steps = 0;
  long now;
  do {
      now = micros();
      if((now - lastTime) >= STEPTIME){
        lastTime = now;
        stepper();
        steps++;
      }
  } while((now - startTime) < (1000 * dureeMillis));
  Running = false;
  return steps;
}

long tourneSteps(long steps, boolean horaire){
  Clockwise = horaire;
  Running  = true;
  long startTime = micros();
  long now;
  do {
      now = micros();
      if((now - lastTime) >= STEPTIME){
        lastTime = now;
        stepper();
        steps--;
  
      }
  } while(steps > 0);
  Running = false;
  return steps;
}

long tourneToujours(boolean horaire){
  Clockwise = horaire;
  
  long steps = 0;
  long startTime = micros();
  long now;
  do {
      now = micros();
      if((now - lastTime) >= STEPTIME){
        lastTime = now;
        stepper();
        steps++;
  
      }
  } while(Running);
  return steps;
}

void report(int i, long value, String units, long microsSecondes){
    Serial.print(i);
    Serial.print(units);
    Serial.print(":");
    Serial.print(value);
    Serial.print(" ");
    Serial.println(microsSecondes);
}

void changeDirection(){
  long now = millis();
  if (now > lastInterrupt + DEBOUNCE){
    Serial.print(now);
    if(Clockwise){
      Serial.println(" anti-horaire\n");
    }else{
      Serial.println(" horaire\n");
    }
    Clockwise = !Clockwise;
  }
  lastInterrupt = now;
}

void startStop(){
  long now = millis();
  if (now > lastInterrupt + DEBOUNCE){
    Serial.print(now);
    if(Running){
      Serial.println(" arret\n");
    }else{
      Serial.println(" depart\n");
    }
    Running = !Running;
  }
  lastInterrupt = now;
}

void loop(){
  tourneToujours(Clockwise);
  delay(3000);
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
  (Clockwise) ? (Step--) : (Step++);

  if(Step>7){
    Step=0;
  }else if(Step<0){
    Step=7; 
  }
}

