#include <Servo.h>
#include <ezButton.h>

/*Main motive/functionality of the code:
1. When button1 will be pressed: System will be activated through sysActiv function
2. When button2 will be pressed: It will be executing necessary scooping actions, following to the conditions
3. When button1 will be pressed again: System will be deactivated and parked through park function
4. The scooping will be executed only when system is active, otherwise it will print error message
*/

ezButton button1(2);                                   // Button 1 for activation/parking
ezButton button2(3);                                   // Button 2 for scooping action

Servo servo1;                                          //Creating servo objects inside the class Servo
Servo servo2;
Servo servo3;
Servo servo4;

  unsigned int count1 = 0;                              //Initializing count1 & count2 
  unsigned int count2 = 0;                                


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Power ON");                            //Display status: "Power ON"

  button1.setDebounceTime(50);                           //Set button debounce time 50 ms (custom) for false click protection
  button2.setDebounceTime(50);

  button1.setCountMode(COUNT_FALLING);                   // Button press action will be detected when button is released from pressed
  button2.setCountMode(COUNT_FALLING);

  bool scoopingAction = false;
  bool systempark = false
  bool systemActive = false

  servo1.attach(5);                                      //Signal wire of servo-1 is connected to pin 5
  servo2.attach(6);                                      //Signal wire of servo-2 is connected to pin 6
  servo3.attach(9);                                      //Signal wire of servo-3 is connected to pin 9
  servo4.attach(10);                                     //Signal wire of servo-4 is connected to pin 10

  int angle1 = 0;
  int angle2 = 0;
  int angle3 = 0;                                       // initializing servo angles
  int angle4 = 0;

  delay(1000);
  Serial.println("System initialized");

}

void loop() {
  button1.loop();
  button2.loop();

  if (button1.isPressed())
    {
      count1 += 1;
    }
  if (button2.isPressed() && count1 == 1)
    {
      count2 += 1;
    }

  if (count1 == 1)
    {
      Serial.println("Activating system. Please wait...");
        sysActiv();
      Serial.println("System activated");
      break;
    }

  if (count1 == 2)
    {
      Serial.println("Parking the system. Please wait...");
      parking();
      delay(1000);
        count1 = 0;
      Serial.println("Parked");
      break;
    }
  
  if (count2 == 1 && count1 == 1)
    {
      Serial.println("Scooping");
      scooping();
        scoopingAction = true;
        count2 = 0;
      Serial.println("Scooping done");
      break;
    }

 /* if (count3 == 1 && scoopingAction == true)
    {
      Serial.println("Pouring materials");
      pour();
      Serial.println("Pouring success");
      break;
    }*/
  

}

void sysActiv ()
{
    if (systempark == true)
    {

        for (int b2 = 0; b2<= 90; b2++)
        {
          servo2.write(b2);
          delay(20);
        }

        for (int b3 = 90; b3 >= 45; b3--)
        {
          servo3.write(b3);
          delay(20);
        }

        for (int b2 = 90; b2<=135; b2++)
        {
          servo2.write(b2);
          delay(20);
        }

        for (int b1 = 0; b1<= 90; b1++)
        {
          servo1.write(b1);
          delay(20);
        }
    }

    else
    {
      parking();
    }
}
void parking ()         //Call this function when system is going to park
  {
    for(int a1 = angle1; a1 >= 0; a1--)
    {
      servo1.write(a1);
      delay(50);
    }

    for (int a2 = angle2; a2 >= 90; a2--)
    {
      servo2.write(a2);
      delay(50);
    }
    
    if (angle3 > 90 )
    {
      for (int a3 = angle3; a3 >= 90; a3--)
      {
      servo3.write(a3);
      delay(50);
      }
    }
    else if(angle3 < 90)
    {
      for (int a3 = angle3; a3 <= 90; a3++)
      {
      servo3.write(a3);
      delay(50);
      }
    }

    for (int a2 = 90; a2 >= 0; a2--)
    {
      servo2.write(a2);
      delay(50);
    }
    
    systempark = true;
    
  }

void scooping ()
  {
    for
    {
      servo4.write(c4);
      servo1.write(c1);
      delay(10);
    }

  }  

void pour (int angle1, int angle2, int angle3, int angle4)
  {

  }
