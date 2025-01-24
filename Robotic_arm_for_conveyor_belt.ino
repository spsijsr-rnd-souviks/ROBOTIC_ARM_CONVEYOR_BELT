#include <Servo.h>
#include <ezButton.h>

/*Main motive/functionality of the code:
1. When button1 will be pressed: System will be activated through sysActiv function
2. When button2 will be pressed: It will be executing necessary scooping actions, following to the conditions
3. When button1 will be pressed again: System will be deactivated and parked through park function
4. When button3 will be pressed, system will come and orient at active position by checking wheather the scooping is done or not
    If done, then it will pour the material and come back to the active position and stop. If not then it will be directly come to active position
4. The scooping will be executed only when system is active, otherwise it will print error message
*/

ezButton start_stop(2);                                   // Button start_stop for activation/parking
ezButton scoop_pour(3);                                   // Button pause_resume for scooping action
ezButton emergency_stop(4);                               // Button emergency_stop for stopping the system in the middle of the operation  

Servo servo1;                                          //Creating servo objects inside the class Servo
Servo servo2;
Servo servo3;
// Servo servo4;

//Variables defining
    int activ_angle1 = 140;              // Defining experimented positions of the servos  
    int activ_angle2 = 120;             // At the active position
    int activ_angle3 = 60;
    
    int interim_angle2 = 45;             // Change here only if needed
    int interim_pour_angle2 = 90;

    int park_angle1 = 0;                  // Defining experimented angle values at park position
    int park_angle2 = 0;                  // Change angle values here if needed
    int park_angle3 = 0;


    int scoop_arm_angle_max = 150;            //Experimented angle values of the scooping arm for scooping operation. 
    int scoop_arm_angle_min = 60;             // Change here if needed  

//    int pourAngle1= 90;                   //Defining experimented pouring angles
//    int pourAngle2 = 120;                 //Change angle values here if required
//    int pourAngle3 = 15;



   unsigned int count1 = 0;                 //Initializing count1 & count2 
   unsigned int count2 = 0;                                

   int curr_Angles[3] = {0, 0, 0};
/*
   int angle1 = 0;
   int angle2 = 0;
   int angle3 = 0;                           // initializing servo angles
//  int angle4 = 0;
*/
  bool scoopingAction = false;
  bool systempark = false;
  bool systemActive = false;
  bool pouringStatus = false;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Power ON");                            //Display status: "Power ON"

  start_stop.setDebounceTime(50);                           //Set button debounce time 50 ms (custom) for false click protection
  scoop_pour.setDebounceTime(50);
  emergency_stop.setDebounceTime(50);

  servo1.attach(5);                                      //Signal wire of servo-1 is connected to pin 5
  servo2.attach(6);                                      //Signal wire of servo-2 is connected to pin 6
  servo3.attach(9);                                      //Signal wire of servo-3 is connected to pin 9
//  servo4.attach(10);                                     //Signal wire of servo-4 is connected to pin 10
  delay(500);

  servo1.write(park_angle1);
  delay(500);
  servo2.write(park_angle2);
  delay(500);
  servo3.write(park_angle3);

  delay (1000);
  Serial.println("System initialized");

}
/*
void loop() {

    int buttonState = buttonStatus ();

    switch (buttonState)
    {
        case 1:
            if(count1 % 2 == 1){
              sysActiv();
            }
            else{
              parking();
            }
            //start_Stop_Handler();
            break;

        case 2:
            scooping_Handler();
            break;
        
        case 3:
            emergency_Handler();
            break;

        default:
            parking();
            break;
    }

}
*/

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
        
        case 3:
            emergency_Handler();
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
    if (curr_Angles[0] != activ_angle1 || curr_Angles[1] != activ_angle2 || curr_Angles[2] != activ_angle3)     
    {

        for (int b2 = curr_Angles[1]; b2 <= interim_angle2; b2++)
        {
          servo2.write(b2);
          delay(50);
        }

        for (int b3 = curr_Angles[2]; b3 <= activ_angle3 ; b3++)
        {
          servo3.write(b3); 
          delay(50);
        }

        for (int b2 = interim_angle2; b2 <= activ_angle2; b2++)
        {
          servo2.write(b2);
          delay(50);
        }

        for (int b1 = curr_Angles[0]; b1 <= activ_angle1 ; b1++)
        {
          servo1.write(b1);
          delay(50);
        }

        Serial.println("System has been activated");

        curr_Angles[0] = servo1.read();
        curr_Angles[1] = servo2.read();
        curr_Angles[2] = servo3.read();
    }

    else
    {
      Serial.println("Ready for scooping action");
    }
}

void parking ()         //Call this function when system is going to park
  {
    // int park_angle1 = 0;       // Defining experimented angle values at park position
    // int park_angle2 = 0;       // Change angle values here if needed
    // int park_angle3 = 180;

 // Checking servo angles wheather those are already in the park angle   
    if (curr_Angles[0] != park_angle1 || curr_Angles[1] != park_angle2 || curr_Angles[2] != park_angle3)
    {
        for(int a1 = curr_Angles[0]; a1 >= park_angle1 ; a1--)
        {
            servo1.write(a1);
            delay(50);
        }

        if (curr_Angles[1] > 90)
          {
              for (int a2 = curr_Angles[1]; a2 >= interim_angle2; a2--)
                  {
                      servo2.write(a2);
                      delay(50);
                  }
          }
        else
          {
              for (int a2 = curr_Angles[1]; a2 <= interim_angle2; a2++)
                  {
                      servo2.write(a2);
                      delay(50);
                  }
          }

        for (int a3 = curr_Angles[2]; a3 >= park_angle3; a3--)
        {
            servo3.write(a3);
                delay(50);
        }

        for (int a2 = interim_angle2; a2 >= park_angle2; a2--)
        {
            servo2.write(a2);
            delay(50);
        }

        Serial.println("System is parked");

        curr_Angles[0] = servo1.read();
        curr_Angles[1] = servo2.read();
        curr_Angles[2] = servo3.read();
    }

    else
    {
        Serial.println("System is ready");
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
         if (curr_Angles[0] == activ_angle1 && curr_Angles[1] == activ_angle2 && curr_Angles[2] == activ_angle3)
             {
                Serial.println("System is ready for material scooping. Starting action...");
                delay(500);

                 for (int c3 = scoop_arm_angle_min; c3 <= scoop_arm_angle_max; c3++)
                 {
                     servo3.write(c3);
                     delay(50);
                  }
                    
                  scoopingAction = true;        // Toggle scoopingAction variable when scooping is done
                  Serial.println("Scooping Done");

                    curr_Angles[0] = servo1.read();
                    curr_Angles[1] = servo2.read();
                    curr_Angles[2] = servo3.read();
              }
     }
            
    else
    {
      Serial.println("System is not active. Activate the system first");
    }
  }  

  

void pour ()
  {
  //  int park_angle1 = 0;     //Pour angle at the beginning postion
  //  int pourAngle1= 90;
  //  int pourAngle2 = 120;

    if(scoopingAction == true)
    {
        Serial.println("Scooping done. Pouring now...");          
      //pour mechanism begin if condition is matched here
        if (curr_Angles[0] == activ_angle1 && curr_Angles[1] == activ_angle2 && curr_Angles[2] == scoop_arm_angle_min)
        {
          for (int i = activ_angle1; i>= park_angle1 ; i--)
          {
            servo1.write(i);
            delay(50);
          }

          delay(500);

          for(int j = activ_angle2; j >= interim_pour_angle2; j--)
          {
            servo2.write(j);
            delay(30);
          }

          delay(500);

          for( int k = scoop_arm_angle_max ; k >= scoop_arm_angle_min ; k--)
          {
            servo3.write(k);
            delay(40);
          }

          for(int l = interim_pour_angle2; l <= activ_angle2; l++)
          {
            servo2.write(l);
            delay(50);
          }
          
          pouringStatus = true;
          Serial.println("Pouring done");
        }

        curr_Angles[0] = servo1.read();
        curr_Angles[1] = servo2.read();
        curr_Angles[2] = servo3.read();
    }
  }

  void returnToActive ()
  {
    if (curr_Angles[1] != activ_angle2 || curr_Angles[2] != activ_angle3)
    {
      servo2.write(activ_angle2);
      delay(500);

      servo3.write(activ_angle3);
      delay(500);
    }
    
    for (int i = park_angle1; i <= activ_angle1; i++)
    {
      servo1.write(i);
      delay(50);
    }

  }

// Return button status as per the button is pressed
int buttonStatus ()
  {
    int status = 0;

    start_stop.loop();
    scoop_pour.loop();
    emergency_stop.loop();
    
    if (start_stop.isPressed())
    {
        status = 1;
        systemActive =!systemActive;       // Toggle system active/deactive state

    }

    if (scoop_pour.isPressed())
    {
        status = 2;
    }

    if (emergency_stop.isPressed())
    {
        status = 3;
    }

    return status;
  }

// Activate-Deactivate handler
// Directly called into switch case in void loop
void start_Stop_Handler()              
  { 

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
        
      scooping();
      delay(1000);
      pour();
      delay(1000);
      returnToActive();

    }
}

void emergency_Handler()
{
  if (scoopingAction)
  {
    pour();
    delay(1000);
    returnToActive();
  }
  else
  {
    returnToActive();
  }
}

