#include <Servo.h>
#include <ezButton.h>

/*Main motive/functionality of the code:
1. When button1 will be pressed: System will be activated through sysActiv function
2. When button2 will be pressed: It will be executing necessary scooping actions, following to the conditions
3. When button1 will be pressed again: System will be deactivated and parked through park function
4. The scooping will be executed only when system is active, otherwise it will print error message
*/

ezButton start_stop(2);                                   // Button start_stop for activation/parking
ezButton pause_resume(3);                                   // Button pause_resume for scooping action

Servo servo1;                                          //Creating servo objects inside the class Servo
Servo servo2;
Servo servo3;
// Servo servo4;

  unsigned int count1 = 0;                              //Initializing count1 & count2 
  unsigned int count2 = 0;                                

  int activAngles [4] = {0, 0, 0, 0};

  int angle1 = 0;
  int angle2 = 0;
  int angle3 = 0;                                       // initializing servo angles
//  int angle4 = 0;

  bool scoopingAction = false;
  bool systempark = false
  bool systemActive = false

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Power ON");                            //Display status: "Power ON"

  button1.setDebounceTime(50);                           //Set button debounce time 50 ms (custom) for false click protection
  button2.setDebounceTime(50);

  button1.setCountMode(COUNT_FALLING);                   // Button press action will be detected when button is released from pressed
  button2.setCountMode(COUNT_FALLING);

  servo1.attach(5);                                      //Signal wire of servo-1 is connected to pin 5
  servo2.attach(6);                                      //Signal wire of servo-2 is connected to pin 6
  servo3.attach(9);                                      //Signal wire of servo-3 is connected to pin 9
//  servo4.attach(10);                                     //Signal wire of servo-4 is connected to pin 10

  delay(1000);
  Serial.println("System initialized");

}

void loop() {

    int buttonState = buttonStatus ();

    switch (buttonState)
    {
        case 1:
            start_Stop_Handler();
            break;

        case 2:
            pause_Play_Handler();
            break;
        
        default:
            parking();
            break;
    }


}

int sysActiv()
{
    int max_pos_angle1 = 90;              // Defining experimented positions of the servos  
    int max_pos_angle2 = 120;             // At the active position
    int max_pos_angle3 = 105;             // Change here only if needed

  // Checking servo positions if those are already in the active positions
    if (angle1 != max_pos_angle1 || angle2 != max_pos_angle2 || angle3 != max_pos_angle3)     
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

        angle1 = servo1.read();
        angle2 = servo2.read();
        angle3 = servo3.read();
    }

    else
    {
      Serial.println("System is already in active position");
    }

    return angle1;
    return angle2;
    return angle3;
}

int parking ()         //Call this function when system is going to park
  {
    int park_angle1 = 0;       // Defining experimented angle values at park position
    int park_angle2 = 0;       // Change angle values here if needed
    int park_angle3 = 180;

 // Checking servo angles wheather those are already in the park angle   
    if (angle1 != park_angle1 || angle2 != park_angle2 || angle3 != park_angle3)
    {
        for(int a1 = angle1; a1 >= park_angle1 ; a1--)
        {
            servo1.write(a1);
            delay(20);
        }

        if (angle2 > 90)
        {
            for (int a2 = angle2; a2 >= 90; a2--)
                {
                    servo2.write(a2);
                    delay(20);
                }
        }
        else
        {
            for (int a2 = angle2; a2 <= 90; a2++)
                {
                    servo2.write(a2);
                    delay(20);
                }
        }
            
        for (int a3 = angle3; a3 <= park_angle3; a3++)
        {
            servo3.write(a3);
                delay(20);
        }

        for (int a2 = 90; a2 >= park_angle2; a2--)
        {
            servo2.write(a2);
            delay(20);
        }

        Serial.println("System is parked");

        angle1 = servo1.read();
        angle2 = servo2.read();
        angle3 = servo3.read();
    }

    else
    {
        Serial.println("System is already in park position");
    }

    return angle1;
    return angle2;
    return angle3;
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

// Return button status as per the button is pressed
int buttonStatus ()
  {
    int status = 0;

    start_stop.loop();
    pause_play.loop();
    
    if (start_stop.isPressed())
    {
        status = 1;
    }

    if (pause_play.isPressed())
    {
        status = 2;
    }

    return status;
  }

// Activate-Deactivate handler
// Directly called into switch case in void loop
void start_Stop_Handler()              
{
    systemActive =!systemActive;       // Toggle system active/deactive state 

    if (systemActive == true)
    {
        sysActiv();
    }   
    else
    {
        parking();
    }
}

// Scooping-pouring handler
// Directly called in switch case in void loop
// Change here according to the functionality 
void scooping_Handler()
{

    if (systemActive == true)
    {
        scoopingAction =!scoopingAction;

        if (scoopingAction == true)
        {
            scooping();
            pour();
        }
        else
        {
            sysActiv();
        }
    }
}