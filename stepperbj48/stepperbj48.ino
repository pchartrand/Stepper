

/*
   BYJ48 Stepper motor code
   Connect :
   IN1 >> D8
   IN2 >> D9
   IN3 >> D10
   IN4 >> D11
   VCC ... 5V Prefer to use external 5V Source
   Gnd
   written By :Mohannad Rawashdeh
  http://www.instructables.com/member/Mohannad+Rawashdeh/
     28/9/2013
  */

#define IN1  8
#define IN2  9
#define IN3  10
#define IN4  11
const int NBSTEPS = 4096;
const long STEPTIME = 900;
int Step = 0;
boolean Clockwise = true;

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
    Serial.begin(9600);
    Serial.println("Starting...");
    pinMode(IN1, OUTPUT); 
    pinMode(IN2, OUTPUT); 
    pinMode(IN3, OUTPUT); 
    pinMode(IN4, OUTPUT); 
}

void loop(){
  unsigned long currentMicros;
  int stepsLeft = NBSTEPS;
  time = 0;
  while(stepsLeft > 0){
    currentMicros = micros();
    if(currentMicros - lastTime >= STEPTIME){
      stepper(); 
      time += micros() - lastTime;
      lastTime = micros();
      stepsLeft--;
    }
  }
  Serial.println(time);
  Serial.println("Wait...!");
  delay(2000);
  Clockwise = !Clockwise;
  stepsLeft = NBSTEPS;
}

void writeStep(int outArray[4]){
  for (int i=0;i < 4; i++){
    digitalWrite(IN1 + i, outArray[i]);
  }
}

void stepper(){
  if ((Step >= 0) && (Step < 8)){
    writeStep(stepsMatrix[Step]);
  }else{
    writeStep(arrayDefault);
  }
  setDirection();
} 

void setDirection(){
  (Clockwise) ? (Step++) : (Step--);

  if(Step>7){
    Step=0;
  }else if(Step<0){
    Step=7; 
  }
}

