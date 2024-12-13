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
// Servo servo4;

  unsigned int count1 = 0;                              //Initializing count1 & count2 
  unsigned int count2 = 0;                                

  int activAngles [4];

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
//  servo4.attach(10);                                     //Signal wire of servo-4 is connected to pin 10

  int angle1 = 0;
  int angle2 = 0;
  int angle3 = 0;                                       // initializing servo angles
//  int angle4 = 0;



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
    int max_pos_angle1 = 90;              // Defining experimented positions of the servos  
    int max_pos_angle2 = 120;             // At the active position
    int max_pos_angle3 = 105;             // Change here only if needed

  // Checking servo positions if those are already in the active positions
    if (angle1 != 90 || angle2 != 120 || angle3 != 105)     
    {

        for (int b2 = angle2; b2 <= 90; b2++)
        {
          servo2.write(b2);
          delay(20);
        }

        for (int b3 = angle3; b3 >= max_pos_angle3 ; b3--)
        {
          servo3.write(b3); 
          delay(20);
        }

        for (int b2 = 90; b2<=max_pos_angle2; b2++)
        {
          servo2.write(b2);
          delay(20);
        }

        for (int b1 = angle1; b1 <= max_pos_angle1 ; b1++)
        {
          servo1.write(b1);
          delay(20);
        }

      activAngles[0] = servo1.read();
      activAngles[1] = servo2.read();
      activAngles[2] = servo3.read();
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
    for (int c3 = a3; c<= 45; c3--)
    {
      servo3.write(c3);
      delay(10);
    }

  }  

void pour ()
  {
    for (int d1=90; d1>=0; d1--)
    {
      servo1.write(d1);
      delay(20);
    }
    for (int d2=120; d2>=90; d2--)
    {
      servo2.write(d2);
      delay (10);
    }
    for (int d3=15; d3<= 105; d3--)
    {
      servo3.write(d3);
    }
    for (int d2= 90; d2<= 120; d2++)
  }

  void returnToActive ()
  {
    for (int q1=0; q1<=90; q1++ )
    {
      servo1.write(q1);
    }
  }
