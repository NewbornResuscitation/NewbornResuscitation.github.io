#include <SFE_BMP180.h> //Github: https://github.com/LowPowerLab/SFE_BMP180
#include <Statistic.h>  //Github: https://github.com/RobTillaart/Arduino/tree/master/libraries/Statistic; Arduino Forum: https://playground.arduino.cc/Main/Statistics/

//Testing was done with Serial monitor, code is commented out for reference

SFE_BMP180 bmp180;

//define led pins
int bled = 13; // the pin the "low-pressure" red LED is connected to
int rled = 12; // the pin the "high-pressure" red LED is connected to
int gled = 11; // the pin the green LED is connected to

//
int gledState = LOW;
int frequency = 50; //breaths per minute
unsigned long previousMillis = 0;
const long interval = 60/(frequency*2*0.001);

//length of list
int list_length = 60;

int i=0;

// Frequency: 40 breaths/min* (1min/60sec)=1.5breaths/s

//define lists
double List_write[60]; //~2.0 seconds worth of data; format "List_write[list_length]" gives the necessary length
Statistic List_analysis;


void setup() {
  Serial.begin(9600);

  pinMode(rled, OUTPUT); // Declare the LED as an output
  pinMode(bled, OUTPUT); // Declare the LED as an output
  pinMode(gled, OUTPUT); // Declare the LED as an output

  List_analysis.clear();

  bool success = bmp180.begin();
  
  if (success) {
    Serial.println("BMP180 init success");
  }
}


void loop() {

  char status;
  double T, P;
  bool success = false;

  status = bmp180.startTemperature();

  if (status != 0) {
    delay(5);
    status = bmp180.getTemperature(T);
    
    if (status != 0) {
      status = bmp180.startPressure(3);

      if (status != 0) {
        delay(status);
        status = bmp180.getPressure(P, T);

        if (status != 0) {
          P = P*100; //convert from hPa to Pa
          
        }
      }
    }
  }

List_write[i] = P;

//increment i by 1
i=i+1;

//reset lists when i == list_length
if ( i == list_length ) {
  List_analysis.clear();
  //Serial.print("Time elapsed: ");
  //Serial.print(millis()/1000);
  //Serial.println(" s");
  
  for (int n = 0; n < list_length; n++) {
    List_analysis.add(List_write[n]);
  }

  double mini = List_analysis.minimum(); // calculates minimum pressure in Pa
  double maxi = List_analysis.maximum(); // calculates maximum pressure in Pa
  
  double Applied_P = maxi-mini;
  Applied_P = Applied_P / 98.067; //convert to cm H2O
  //Serial.print(Applied_P);
  //Serial.print(Applied_P);
  //Serial.println(" cm H2O");

  if (Applied_P > 40.00) {
    digitalWrite(rled, LOW);
    digitalWrite(bled, LOW);
    digitalWrite(rled, HIGH); // Turn the rled on
    //Serial.println("Applied pressure too high");
  }
  else if (Applied_P < 20.00) {
    digitalWrite(rled, LOW);
    digitalWrite(bled, LOW);
    digitalWrite(bled, HIGH); // Turn the bled on
    //Serial.println("Applied pressure too low");
  }
    else {
    digitalWrite(rled, LOW);
    digitalWrite(bled, LOW);
  }

  //Serial.println("");

  //reset i to zero
  i=0;
}

// frequency indicator
 unsigned long currentMillis = millis();
 if (currentMillis - previousMillis >= interval){
  previousMillis = currentMillis;
  if (gledState == LOW){
    gledState = HIGH; 
    //Serial.println("");
    //Serial.print("FreqLED state ON, time(ms): ");
    //Serial.println(millis());  
  } else {
    gledState = LOW;
  }
  digitalWrite(gled,gledState);
 }


}
