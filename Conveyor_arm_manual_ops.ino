#include <Servo.h>
#include <ezButton.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define buttons
ezButton activate_button(3);
ezButton operation_button(4);
ezButton stop_button(2);
ezButton park_button(5);


// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Define servo motors
Servo servo1; 
Servo servo2; 
Servo servo3;

// Define states for state machine switches
enum State {
              IDLE,
              ACTIVE,
              SCOOPING,
              POURING,
              PARKING
            };

// Initialize state as IDLE
State currentState = IDLE;


// Experimented and tunned Servo angles
int activ_angle1 = 140; 
int activ_angle2 = 117; 
int activ_angle3 = 60;

int park_angle1 = 0; 
int park_angle2 = 0; 
int park_angle3 = 160;

int interim_angle2 = 45; 
int interim_angle2_up = 135;
int interim_pour_angle2 = 90;     // This is set for smoother arm positioning
int pour_angle3 = 30;

int scoop_arm_angle_max = 150;    // Scooping arm angles
int scoop_arm_angle_min = 60;

// iteration variable is defined to track no of scooping iterations
long iteration = 0;         

// System status flags for different operations
bool systemActive = false; 
bool scoopingAction = false; 
bool pouringStatus = false;
static bool servomoving = true;

// Store current servo angles
int curr_Angles[3] = {0, 0, 160};

void setup() 
{
    lcd.init();         // initialize the lcd
    lcd.backlight();    // Turn on the LCD screen backlight
    lcd.setCursor(1, 0);
    lcd.print("Power ON");
    delay(2000);
    lcd.clear();

    activate_button.setDebounceTime(50);
    operation_button.setDebounceTime(50);
    park_button.setDebounceTime(50);

    servo1.attach(9);
    servo2.attach(10);
    servo3.attach(11);

    servo1.write(park_angle1);
    servo2.write(park_angle2);
    servo3.write(park_angle3);
  
    delay(1000);
    
    lcd.setCursor(2, 0);
    lcd.print("System ");
    lcd.setCursor(2, 1);
    lcd.print("intialized");
    delay(2000);
    lcd.clear(); 
}

void loop() 
{
    activate_button.loop();
    operation_button.loop();
    park_button.loop();


  // Read button state 
        if (activate_button.isPressed())
        {
          start_Handler();
        }

        else if (operation_button.isPressed()) 
        {
          scooping_Handler();
        }

        else if (park_button.isPressed()) 
        {
          parking_Handler();
        }

    // Handle state transitions
    switch (currentState) 
    {
        case IDLE:

            lcd.setCursor(0, 0);
            lcd.print("System is Ready  ");

       // Detach servos for stopping servo PWM as it was noisy at IDLE position     
            servo1.detach();
            servo2.detach();
            servo3.detach();
            
            break;

        case ACTIVE:

            lcd.setCursor(0, 0);
            lcd.print("SYSTEM ACTIVATED");
            servomoving = true;
            iteration = 0;
            
            break;

        case SCOOPING:

            // Checks condition for start action
            if(servomoving == true)
            {
                  scooping();

              //  LCD display    
                  lcd.setCursor(0, 0);
                  lcd.print("Scooping");
                  lcd.setCursor(0, 1);
                  lcd.print("complete");
                
                  iteration += 1;
                  
              // LCD display    
                  lcd.setCursor(12, 0);
                  lcd.print("ITR");
                  lcd.setCursor(12, 1);
                  lcd.print(iteration);
                  delay(20);
                  //lcd.clear();
                  currentState = POURING;                
              }
           
           // Prevents action if action button is pressed after it's complete
                else
                {  
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Interrupt");
                  lcd.setCursor(0, 1);
                  lcd.print("Returning...");
                  delay(2000);
                  lcd.clear();
                  currentState = ACTIVE; 
                }
              
            break;

        case POURING:

            lcd.setCursor(0, 0);
            lcd.print("Pouring ");
            lcd.setCursor(0, 1);
            lcd.print("material ");

            pour();

        // Returns the system to home position    
            delay(500);
            returnToActive();
            
            // Checks condition for further actions
            if (servomoving == true)
            {
                currentState = SCOOPING;  
            }
            
            else 
            {
                scoopingAction = false;             
                pouringStatus = false;        // Resets the action flags after it's complete
                currentState = ACTIVE; 

            }
            break;

        case PARKING:
                if ( curr_Angles[0] == park_angle1 || 
                     curr_Angles[1] == park_angle2 ||
                     curr_Angles[2] == park_angle3  
                    )
                        {
                            lcd.clear();
                            lcd.setCursor(0,0);
                            lcd.print("Already Parked");
                            delay(1000);
                            Serial.println("System already parked");
                        }
                    else
                    {
                        Serial.println("Parking system...");
                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcd.print("Parking...");
                        
                        parking();

                        systemActive = false;     // Reset active state flag
                        iteration = 0;            // Reset iteration for further scooping action

                    }

                    currentState = IDLE;            // Reset state case
                    break;
            
    }

}

// ======================== State Handlers ========================

void start_Handler() 
{
    if (!systemActive) 
    {
      
      // Ensure servos are attached everytime while the system is starting
        servo1.attach(9);
        servo2.attach(10);
        servo3.attach(11);

        sysActiv();

        if (!servomoving)
        {
          systemActive = false;
          currentState = IDLE;
          return;
        }
        else
        {
          systemActive = true;
          currentState = ACTIVE;
        }

           
    }
}

void scooping_Handler() 
{
    if (!systemActive) 
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ERROR...");
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print("Activate First..");
        delay(1000);
        lcd.clear();
        return;
    }

    if (currentState == ACTIVE) 
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Scooping");
        lcd.setCursor(0, 1);
        lcd.print("started");
        
        currentState = SCOOPING;
    }


}

void parking_Handler() 
{
    if (currentState == SCOOPING || currentState == POURING) 
    {
        lcd.setCursor(2, 0);
        lcd.print("Finishing current task");
        lcd.setCursor(2, 1);
        lcd.print("before parking");
        delay(2000);
        lcd.clear();
        return;
    }
    currentState = PARKING;
}

// ======================== Functional Actions ========================

void sysActiv() 
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Activating");
    lcd.setCursor(0, 1);
    lcd.print("system...");
    
    moveServo(servo2, park_angle2, interim_angle2, 50, 1);
    moveServo(servo3, park_angle3, activ_angle3, 40, 2);
    moveServo(servo2, interim_angle2, interim_angle2_up , 40, 1);
    moveServo(servo1, park_angle1, activ_angle1, 50, 0);
    moveServo(servo2, interim_angle2_up, activ_angle2 , 40, 1);

    curr_Angles[0] = activ_angle1;
    curr_Angles[1] = activ_angle2;
    curr_Angles[2] = activ_angle3;
    lcd.clear();
}

void parking() 
{

    moveServo(servo2, activ_angle2, interim_angle2_up, 30, 1);    
    moveServo(servo1, activ_angle1, park_angle1, 50, 0);
    moveServo(servo2, interim_angle2_up, interim_angle2, 40, 1);
    moveServo(servo3, activ_angle3, park_angle3, 50, 2);
    moveServo(servo2, interim_angle2, park_angle2, 40, 1);
    
}

void scooping()
{
  lcd.setCursor(0, 0);
  lcd.print("Scooping");
  lcd.setCursor(0, 1);
  lcd.print("Started...");
  moveServo(servo3, scoop_arm_angle_min, scoop_arm_angle_max, 50, 2);
 
  scoopingAction = true;
  
}

void pour() 
{
    if (!scoopingAction) 
    {
        Serial.println("No material to pour.");
        lcd.setCursor(2, 0);
        lcd.print("No material");
        lcd.setCursor(2, 1);
        lcd.print("to pour");
        delay(2000);
        lcd.clear();
        return;
    }

    Serial.println("Pouring process...");
    
    moveServo(servo2, activ_angle2, interim_angle2_up, 30, 1);
    moveServo(servo1, activ_angle1, park_angle1, 50, 0);
    moveServo(servo2, interim_angle2_up, interim_pour_angle2, 30, 1);
    moveServo(servo3, scoop_arm_angle_max, pour_angle3, 40, 2);
    moveServo(servo2, interim_pour_angle2, interim_angle2_up, 50, 1);
    moveServo(servo3, pour_angle3, scoop_arm_angle_min, 30, 2);
   
    pouringStatus = true;
    
}

void returnToActive() 
{
    moveServo(servo3, scoop_arm_angle_min, activ_angle3, 50, 2);
    moveServo(servo1, park_angle1, activ_angle1, 50, 0);
    moveServo(servo2, interim_angle2_up, activ_angle2, 40, 1);
    lcd.setCursor(0, 0);
    lcd.print("System returened");
    lcd.setCursor(0, 1);
    lcd.print("to Active");
    delay(2000);
    lcd.clear();
}



// ============================================ This function is only for manual interruption in operation ==================================== //

void systemReturn()
{
  // Returning only if system is not ACTIVE
  // If button is pressed during the activation process
  // Always returns to IDLE state
  if (!systemActive)
  {
      stopServo(servo1, curr_Angles[0], park_angle1,40);
      stopServo(servo2, curr_Angles[1],interim_angle2,40);
      stopServo(servo3, curr_Angles[2], park_angle3,40);
      stopServo(servo2, curr_Angles[1], park_angle2,40);

      curr_Angles[0] = park_angle1;
      curr_Angles[1] = park_angle2;
      curr_Angles[2] = park_angle3;

  }

  // Returning from active/scooping/pouring position
  // Always returned to ACTIVE state
  else
  {
      stopServo(servo3, curr_Angles[2], activ_angle3, 40);
      stopServo(servo2, curr_Angles[1], interim_angle2_up, 40);
      stopServo(servo1, curr_Angles[0], activ_angle1, 40);
      stopServo(servo2, interim_angle2_up, activ_angle2, 40);

      curr_Angles[0] = activ_angle1;
      curr_Angles[1] = activ_angle2;
      curr_Angles[2] = activ_angle3;

      currentState = ACTIVE;
  }
  
}

// ======================== Function to move servo ========================

 void moveServo(Servo &servo, int from, int to, int delayTime, int angle_index) 
{
    int step = (from < to) ? 1 : -1;
    if (servomoving)
    {
      for (int pos = from; pos != to + step; pos += step) 
      {
          unsigned long previousMillis = millis();

          while (millis() - previousMillis < delayTime) 
          {
              // Check emergency stop button
              stop_button.loop();  // Ensure ezButton updates
              if (stop_button.isPressed()) 
              {
                // Update LCD display //
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Interrupt");
                  lcd.setCursor(0, 1);
                  lcd.print("Returning...");

                  delay(500);
                  systemReturn();  // Call function to return system to safe state
                  servomoving = false;
                  return;
                  //break;
              }
          }

          servo.write(pos);
          curr_Angles [angle_index] = pos;
      }
    }
    else 
    {
      Serial.println("Stopped");
    }
}




// ====================== Servo movement function only for terminating operations ======================= //

void stopServo(Servo &servo, int from, int to, int delayTime) 
{

    if (from < to) 
    {
        for (int pos = from; pos <= to; pos++) 
        {
            servo.write(pos);
            delay(delayTime);
        }
    } 
    else 
    {
        for (int pos = from; pos >= to; pos--) 
        {
            servo.write(pos);
            delay(delayTime);
        }
    }
}
