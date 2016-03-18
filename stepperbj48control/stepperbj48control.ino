
#define IN1  8

const int NBSTEPS = 4096;
const float UNDEGRE = 11.38;

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
  unsigned long currentMicros;
  while(stepsLeft > 0){
    if(micros() - lastTime >= dureeStep){
      stepper(); 
      lastTime = micros();
      steps++;
      stepsLeft--;
    }
  }
  return steps;
}

long tourneAngle(int angle, boolean horaire){
  int stepsLeft = floor(UNDEGRE * angle);
  long dureeStep = STEPTIME * (360 / angle);
  long steps = 0;

  Clockwise = horaire;
  unsigned long currentMicros;
  while(stepsLeft > 0){
    if(micros() - lastTime >= STEPTIME){
      stepper(); 
      lastTime = micros();
      steps++;
      stepsLeft--;
    }
  }
  return steps;
}

long tourneDurant(long dureeMillis, boolean horaire){
  Clockwise = horaire;
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
  return steps;
}

long tourneSteps(long steps, boolean horaire){
  Clockwise = horaire;
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


void loop(){
  Serial.println("rotation en secondes, vitesse constante");
  long start = 0;
  for (int i=1; i< 6; i++){
    start = micros();
    boolean horaire = i % 2;
    long steps = tourneDurant(i*1000, horaire);
    report(i, steps, " s.", micros() - start);
    delay(1000);
  }
  delay(1000);
  Serial.println("\nrotation en degres, vitesse constante");
  for (int i=1; i< 7; i++){
    start = micros();
    boolean horaire = i % 2;
    long steps = tourneAngle(i*60, horaire);
    report(i*60, steps, " deg.", micros() - start);
    delay(1000);
  }
  delay(1000);
  Serial.println("\nrotation en degres, duree fixe");
  for (int i=1; i< 7; i++){
    start = micros();
    boolean horaire = i % 2;
    long steps = tourneAngleCompense(i*60, horaire);
    report(i*60, steps, " deg.", micros() - start);
    delay(1000);
  }
  delay(1000);
  Serial.println("\nrotation en pas, vitesse constante");
  for (int i=1; i< 6; i++){
    start = micros();
    boolean horaire = i % 2;
    long steps = tourneSteps(i*100, horaire);
    report(i*100, steps, " pas", micros() - start);
    delay(1000);
  }
  delay(3000);

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

