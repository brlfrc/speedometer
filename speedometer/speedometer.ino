/*
https://github.com/Simsso/ShiftRegister74HC595/tree/master library for display

*/

#include <Servo.h>
#include <ShiftRegister74HC595.h>
#define wheel_radius 32     //in cm


// Calculate velocity from the angular Velocity
const int reedSwitchPin = 2;
volatile unsigned int count = 0;

double calculate_speed(int interval){
  unsigned long startTime = millis(); // Record the start time
  unsigned int revolutions = 0; // Initialize revolutions count
  
  while (millis() - startTime < interval) { // While the specified time interval has not elapsed
    // Check if the reed switch has triggered
    if (digitalRead(reedSwitchPin) == LOW) {
      revolutions++; // Increment revolutions count
      while (digitalRead(reedSwitchPin) == LOW) {} // Wait for the reed switch to return to HIGH (optional)
    }
  }

  return 2*3.1416*revolutions / (interval / 1000.0) * wheel_radius*0.036;  //2*pi*r*omega * unit conversion (cm/s -> km/h)
}

//Speedometer using a servo
Servo speedometer;  // Create a servo object
int max_speed= 20;

void set_servo (int speed){
  int angle = int(speed/max_speed)*180;
  // Ensure that the angle is within the valid range of 0 to 180 degrees
  if (angle < 0) {
    angle = 0;
  } else if (angle > 180) {
    angle = 180;
  }

  // Set the servo to the specified angle
  speedometer.write(angle);
}

// create a global shift register object
// parameters: <number of shift registers> (data pin, clock pin, latch pin)
ShiftRegister74HC595<2> sr(A4, A5, 6);

uint8_t  numberB[] = {B11000000, //0
                      B11111001, //1
                      B10100100, //2
                      B10110000, //3
                      B10011001, //4
                      B10010010, //5
                      B10000011, //6
                      B11111000, //7
                      B10000000, //8
                      B10011000 //9
                     };

void show_velocity(int speed){
  //Split speed to digits:
    int speed2=speed % 10 ;
    int speed1=(speed / 10) % 10;
    uint8_t numberToPrint[]= {numberB[speed2],numberB[speed1]};
    sr.setAll(numberToPrint);
}

void setup(){
  pinMode(reedSwitchPin, INPUT_PULLUP); // Enable internal pull-up resistor
  speedometer.attach(3);  // Attach the servo to pin 3
}

void loop() {
  int speed = int(calculate_speed(3000)); //interval = 3s
  show_velocity(speed);
  set_servo (speed);
}