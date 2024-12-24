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

//Variables defining
    int max_pos_angle1 = 90;              // Defining experimented positions of the servos  
    int max_pos_angle2 = 120;             // At the active position
    int max_pos_angle3 = 105;             // Change here only if needed

    int park_angle1 = 0;       // Defining experimented angle values at park position
    int park_angle2 = 0;       // Change angle values here if needed
    int park_angle3 = 180;

    int scooping_angle1 = 90;           //Defining experimented scooping angle at beginning
    int scooping_angle2 = 120;            //Change angle values here if required
    int scooping_angle3 = 105;
    int scoop_arm_angle_min = 15;



  unsigned int count1 = 0;                              //Initializing count1 & count2 
  unsigned int count2 = 0;                                

  int lastAngles[3] = {0, 0, 0};

  int angle1 = 0;
  int angle2 = 0;
  int angle3 = 0;                                       // initializing servo angles
//  int angle4 = 0;

  int scoop_arm_angle_max = 105;      //Experimented angle values. Change here if needed
  int scoop_arm_angle_min = 15;

  bool scoopingAction = false;
  bool systempark = false
  bool systemActive = false

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Power ON");                            //Display status: "Power ON"

  start_stop.setDebounceTime(50);                           //Set button debounce time 50 ms (custom) for false click protection
  pause_resume.setDebounceTime(50);

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
            scooping_Handler();
            break;
        
        default:
            parking();
            break;
    }


}

void sysActiv()
{
    // int max_pos_angle1 = 90;              // Defining experimented positions of the servos  
    // int max_pos_angle2 = 120;             // At the active position
    // int max_pos_angle3 = 105;             // Change here only if needed

  // Checking servo positions if those are already in the active positions
    if (lastAngles[0] != max_pos_angle1 || lastAngles[1] != max_pos_angle2 || lastAngles[2] != max_pos_angle3)     
    {

        for (int b2 = lastAngles[1]; b2 <= 90; b2++)
        {
          servo2.write(b2);
          delay(20);
        }

        for (int b3 = lastAngles[2]; b3 >= max_pos_angle3 ; b3--)
        {
          servo3.write(b3); 
          delay(20);
        }

        for (int b2 = 90; b2<=max_pos_angle2; b2++)
        {
          servo2.write(b2);
          delay(20);
        }

        for (int b1 = lastAngles[0]; b1 <= max_pos_angle1 ; b1++)
        {
          servo1.write(b1);
          delay(20);
        }

        Serial.println("System has been activated");

        lastAngles[0] = servo1.read();
        lastAngles[1] = servo2.read();
        lastAngles[2] = servo3.read();
    }

    else
    {
      Serial.println("System is already in active position");
    }
}

void parking ()         //Call this function when system is going to park
  {
    // int park_angle1 = 0;       // Defining experimented angle values at park position
    // int park_angle2 = 0;       // Change angle values here if needed
    // int park_angle3 = 180;

 // Checking servo angles wheather those are already in the park angle   
    if (lastAngles[0] != park_angle1 || lastAngles[1] != park_angle2 || lastAngles[2] != park_angle3)
    {
        for(int a1 = lastAngles[0]; a1 >= park_angle1 ; a1--)
        {
            servo1.write(a1);
            delay(20);
        }

        if (lastAngles[1] > 90)
        {
            for (int a2 = lastAngles[1]; a2 >= 90; a2--)
                {
                    servo2.write(a2);
                    delay(20);
                }
        }
        else
        {
            for (int a2 = lastAngles[1]; a2 <= 90; a2++)
                {
                    servo2.write(a2);
                    delay(20);
                }
        }
            
        for (int a3 = lastAngles[2]; a3 <= park_angle3; a3++)
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

        lastAngles[0] = servo1.read();
        lastAngles[1] = servo2.read();
        lastAngles[2] = servo3.read();
    }

    else
    {
        Serial.println("System is already in park position");
    }
  }


void scooping ()
  {
 // Scooping function is defined only for the scooping action.
 // The 3rd arm will only move for scooping of material.
 // For now there is no servo attached between 3rd arm and scoop.
 // If 4th servo is required, add it here along with thread.
    // int scooping_angle1 = 90;           //these angles can change according to all the servo's angle at the beginning of the scoop
    // int scooping_angle2 = 120;
    // int scooping_angle3 = 105;
    // int scoop_arm_angle_min = 15;

    if (systemActive == true)           // Check if the system is active
    {
         if (lastAngles[0] == scooping_angle1 && lastAngles[1] = scooping_angle2 && lastAngles[2] == scooping_angle3)
             {
                 for (int c3 = scooping_angle3; c3 >= scoop_arm_angle_min; c3--)
                 {
                     servo3.write(c3);
                     delay(10);
                  }

                    scoopingAction =! scoopingAction;    // Toggle scoopingAction variable when scooping is done
                    lastAngles[0] = servo1.read();
                    lastAngles[1] = servo2.read();
                    lastAngles[2] = servo3.read();
    }
     }
            
    else
    {
      Serial.println("System is not active. Activate the system first");
    }
  }  

  

void pour ()
  {
  //     int park_angle1 = 0;     //Pour angle at the beginning postion
  //  int pourAngle1= 90;
  //  int pourAngle2 = 120;
  //  int pourAngle3 = 15;
  
    int max_pos_angle1 = 90;       
    int max_pos_angle2 = 120;        
    int max_pos_angle3 = 105;            
      //pour mechanism begin if condition is matched here
    if (lastAngles[0] == pourAngle1 && lastAngles[1] = pourAngle2 && lastAngles[2] == pourAngle3)
    {
      for (int i = pourAngle1; i>= 90; i--)
      {
        servo1.write(i);
        delay(10);
      }

      for(int j = pourAngle2; j<=150; j++)
      {
        servo2.write(i);
        delay(10);
      }

      for( int k = pourAngle3 ; k<= 120; k++)
      {
        servo3.write(k);
        delay(10);
      }

      // Returning to the scoop position

       for (int i = 0; i<= 90; i++) //servo 1 or arm 1 will move to 90 degree
      {
        servo1.write(i);
        delay(10);
      }
        for (int j = 150; j>=120; j--)  // arm 2 will move from 150 degree to 120 degree
        {
          servo2.write(j);
          delay(10);
        }
        for (int k = 120; k>=105; k--)  //arm 3 will move to its scooping postion
        {
          servo3.write(k);
          delay(10);
        }
    }

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
    pause_resume.loop();
    
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