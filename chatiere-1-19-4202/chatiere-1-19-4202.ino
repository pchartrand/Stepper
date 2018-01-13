
#define IN1  8
/* using 1-19-4202 motor
*/
const int CHANGE_DIRECTION_PIN = 2;
const int CLOSED_PIN = 3;
const int EXIT_PIN = 4;
const int ENTERING_PIN = 5;
const int INSIDE_PIN = 6;
const int OUTSIDE_PIN = 7;
const int BAUDRATE = 9600;
const int NBSTEPS = 100;
const int DEBOUNCE = 300;
const long STEPTIME = 5000;
const long MAXTIME = 7000000;
const long WAIT_WHEN_CHANGING_DIRECTION = 3000;
const float stepAngle = 3.6;

int arrayDefault[4] = {LOW, LOW, LOW, LOW};

int stepsMatrix[4][4] = {
  {LOW,  LOW,  HIGH,  HIGH},
  {HIGH, LOW,  LOW,   HIGH},
  {HIGH, HIGH, LOW,   LOW },
  {LOW,  HIGH, HIGH,  LOW },
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
unsigned long startTime = 0;
unsigned long startWait = 0;


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
  return stepAngle * (steps % NBSTEPS);
}

void reverseDirection(){
  unsigned long now = millis();
  if (now > reverseDirectionInterrupt + DEBOUNCE){
    Serial.println("changement de direction"); 
    if(goingClockwise){
      Serial.println("anti-horaire");      
    }else{
      Serial.println("horaire");
    }
    goingClockwise = !goingClockwise;
    //delay(WAIT_WHEN_CHANGING_DIRECTION);
    isRunning = false;
    Serial.println("debut de la pause");
    startWait = now;
  }
  reverseDirectionInterrupt = now;
}

void stopIt(){
  unsigned long now = millis();
  if (now > lastStopInterrupt + DEBOUNCE){
    if(isRunning){
      //Serial.println(angle());
      Serial.println("Arret");
      isRunning = false;
      goingClockwise = true;
    }
  }
  lastStopInterrupt = now;
}

void writeStep(int outArray[4]){
  //Serial.println(Step);
  for (int i=0;i < 4; i++){
    digitalWrite(IN1 + i, outArray[i]);
    //Serial.print(outArray[i]);
  }
  //Serial.println();
}

void stepper(){
  if (isRunning){
    if ((Step >= 0) && (Step < 4)){
      writeStep(stepsMatrix[Step]);
    }else{
      writeStep(arrayDefault);
    }
  }
  setDirection();
} 

void setDirection(){
  if(goingClockwise) {
    Step++;
    if(isRunning){
      steps++;
    }
  }else{
   Step--;
   if(isRunning){
      steps--;
    }
  }

  if(Step>3){
    Step=0;
  }else if(Step<0){
    Step=3; 
  }
}

boolean openingRequested(){
  entering = (digitalRead(ENTERING_PIN) == LOW);
  exiting = (digitalRead(EXIT_PIN) == LOW);
  if (entering){
    Serial.println("demande d'entree");
    startTime = millis();
  }
  if (exiting){
    Serial.println("demande de sortie");
    startTime = millis();
  }
  return  (entering || exiting);
}

void loop(){  
  unsigned long now;
  
  if ((isRunning == false) && (startWait == 0) && openingRequested()){
    startTime = micros();
    isRunning = true;
  }
  
  digitalWrite(OUTSIDE_PIN, exiting);
  digitalWrite(INSIDE_PIN, entering);
  do {
      now = micros();
      if ((startTime > 0) && (startWait == 0) && (now - startTime > MAXTIME) && isRunning) {
        Serial.println("la porte prend trop de temps a s'ouvrir");
        //isRunning = false;
        startTime = 0;
        if(goingClockwise){
          reverseDirection();
        }
      }
      if((now - lastTime) >= STEPTIME){
        lastTime = now;
        stepper();
      }
  } while(isRunning);

  if ((isRunning == false) && (startWait > 0)){
      //Serial.println("en pause");
      unsigned long pauseTime = millis() - startWait;
      if (pauseTime > WAIT_WHEN_CHANGING_DIRECTION ){
        startWait = 0;
        startTime = 0;
        Serial.println("Fin de la pause");
        isRunning = true;
      }
  }else{
    if (entering){
      Serial.println("entree completee\n");
      startTime = 0;
      startWait = 0;
    }
    if (exiting){
      Serial.println("sortie completee\n");
      startTime = 0;
      startWait = 0;
    }
  }
}


