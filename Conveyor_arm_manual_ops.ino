#include <Servo.h>
#include <ezButton.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define buttons
ezButton start_button(3);
ezButton scoop_pour(4);
ezButton stop_button(5);
ezButton emergency_stop(2);

//const int interruptPin = 2;               // Pin 2 is for taking interrupt input
//volatile bool emergency_stop = false;     // Flag set for interrupt action

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

//unsigned long scoopingStartTime = 0;
//const long scoopingDuration = 5000; //  Scooping runs for 5 seconds

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

long material_weight = 0;         
long weight_threshold = 15;       // Setting threshold weight upto which the scooping action will continue

// System status flags for different operations
bool systemActive = false; 
bool scoopingAction = false; 
bool pouringStatus = false;
static bool servomoving = true;

// Store current servo angles
int curr_Angles[3] = {0, 0, 160};

void setup() 
{
    Serial.begin(9600);
    lcd.init();         // initialize the lcd
    lcd.backlight();    // Turn on the LCD screen backlight
    lcd.setCursor(1, 0);
    lcd.print("Power ON");
    delay(2000);
    lcd.clear();

    //pinMode(emergency_stop, INPUT_PULLUP);

    //pinMode(interruptPin, INPUT_PULLUP);
    //attachInterrupt(digitalPinToInterrupt(interruptPin), stopServo, FALLING);

    start_button.setDebounceTime(50);
    scoop_pour.setDebounceTime(50);
    stop_button.setDebounceTime(50);
    //emergency_stop.setDebounceTime(10);

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
    start_button.loop();
    scoop_pour.loop();
    stop_button.loop();
    //emergency_stop.loop();


    // Read button state 
    if (start_button.isPressed())
    {
      start_Handler();
    }

    else if (scoop_pour.isPressed()) 
    {
      scooping_Handler();
    }

    else if (stop_button.isPressed()) 
    {
      parking_Handler();
    }

    // Handle state transitions
    switch (currentState) 
    {
        case IDLE:

            Serial.println("System is READY...");
            lcd.setCursor(0, 0);
            lcd.print("System is Ready  ");
            break;

        case ACTIVE:

            Serial.println("System is ACTIVATED... Updated final angles are..");
            Serial.println(curr_Angles[0]);
            Serial.println(curr_Angles[1]);
            Serial.println(curr_Angles[2]);
            lcd.setCursor(0, 0);
            lcd.print("SYSTEM ACTIVATED");
            servomoving = true;
            //emergency_status = false;
            break;

        case SCOOPING:

            // Checks condition for start action
            if(servomoving == true)
            {
              //if (material_weight < weight_threshold) 
              //{
                /*if(emergency_stop.isPressed())
                    {
                      Serial.println("Interrupt, returning");
                      returnToActive();
                      break;
                    }
                  */ 
                  scooping();
                  Serial.print("Scooping complete! | ");

              //  LCD display    
                  lcd.setCursor(0, 0);
                  lcd.print("Scooping");
                  lcd.setCursor(0, 1);
                  lcd.print("complete");
                
                  material_weight += 5;

                  Serial.print("Item weight =");
                  Serial.println(material_weight);
                  Serial.println(curr_Angles[0]);
                  Serial.println(curr_Angles[1]);
                  Serial.println(curr_Angles[2]);
                  
              // LCD display    
                  lcd.setCursor(12, 0);
                  lcd.print("W:");
                  lcd.setCursor(14, 0);
                  lcd.print(material_weight);
                  lcd.setCursor(14, 1);
                  lcd.print("K");
                  delay(20);
                  //lcd.clear();
                  currentState = POURING;                
              }
           
                else
                {
                  Serial.println("Threshold reached...");   // Prevents action if action button is pressed after it's complete
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Threshold ");
                  lcd.setCursor(0, 1);
                  lcd.print("reached");
                  delay(2000);
                  lcd.clear();
                  currentState = ACTIVE; 
                }
              
            break;

        case POURING:

            Serial.println("Pouring material...");
            lcd.setCursor(0, 0);
            lcd.print("Pouring ");
            lcd.setCursor(0, 1);
            lcd.print("material ");
            //delay(2000);
            //lcd.clear();
            pour();
            Serial.println(curr_Angles[0]);
            Serial.println(curr_Angles[1]);
            Serial.println(curr_Angles[2]);
//            material_weight += 5;
            delay(500);
            returnToActive();
            Serial.println("Updated servo angles");
            Serial.println(curr_Angles[0]);
            Serial.println(curr_Angles[1]);
            Serial.println(curr_Angles[2]);

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
                        //lcd.setCursor(2, 1);
                        //lcd.print("System");
                        parking();
                        systemActive = false;           // Reset active state flag
                        material_weight = 0;            // Reset material weight for further scooping action
                        Serial.println("Updated servo angles");
                        Serial.println(curr_Angles[0]);
                        Serial.println(curr_Angles[1]);
                        Serial.println(curr_Angles[2]);
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
        sysActiv();
        systemActive = true;
        currentState = ACTIVE;
    }
}

void scooping_Handler() 
{
    if (!systemActive) 
    {
        Serial.println("ERROR: System is not active. Activate first!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ERROR...");
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print("Activate First..");
        delay(1000);
        lcd.clear();
        //lcd.print("SYSTEM ACTIVATED")
        return;
    }

    if (currentState == ACTIVE) 
    {
        Serial.println("Scooping started...");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Scooping");
        lcd.setCursor(0, 1);
        lcd.print("started");
        //delay(2000);
        //lcd.clear();
        currentState = SCOOPING;
    }


}

void parking_Handler() 
{
    if (currentState == SCOOPING || currentState == POURING) 
    {
        Serial.println("Finishing current task before parking...");
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
    Serial.println("Activating system...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Activating");
    lcd.setCursor(0, 1);
    lcd.print("system...");
    //delay(2000);
    
    moveServo(servo2, park_angle2, interim_angle2, 50, 1);
    Serial.println("Servo2 responded");
    moveServo(servo3, park_angle3, activ_angle3, 40, 2);
    Serial.println("Servo3 responded");
    moveServo(servo2, interim_angle2, interim_angle2_up , 40, 1);
    Serial.println("Servo2 responded again from intermediate");
    moveServo(servo1, park_angle1, activ_angle1, 50, 0);
    Serial.println("Servo1 responded");
    moveServo(servo2, interim_angle2_up, activ_angle2 , 40, 1);

    curr_Angles[0] = activ_angle1;
    curr_Angles[1] = activ_angle2;
    curr_Angles[2] = activ_angle3;
    lcd.clear();
}

void parking() 
{
    Serial.println("Parking system...");

    moveServo(servo2, activ_angle2, interim_angle2_up, 30, 1);    
    moveServo(servo1, activ_angle1, park_angle1, 50, 0);
    Serial.println("Servo1 responded");
    moveServo(servo2, interim_angle2_up, interim_angle2, 40, 1);
    Serial.println("Servo2 responded");
    moveServo(servo3, activ_angle3, park_angle3, 50, 2);
    Serial.println("Servo3 responded");
    moveServo(servo2, interim_angle2, park_angle2, 40, 1);
    Serial.println("Servo2 responded from intermediate");
    
    /*curr_Angles[0] = park_angle1;
    curr_Angles[1] = park_angle2;
    curr_Angles[2] = park_angle3; */
    //lcd.clear();
}

void scooping()
{
  lcd.setCursor(0, 0);
  lcd.print("Scooping");
  lcd.setCursor(0, 1);
  lcd.print("Started...");
  moveServo(servo3, scoop_arm_angle_min, scoop_arm_angle_max, 50, 2);
  Serial.println("Servo3 responded for scooping");
  //lcd.setCursor(2, 0);
  //lcd.print("Scooping");
  //lcd.setCursor(2, 1);
  //lcd.print("Activating");
  //delay(2000);
  scoopingAction = true;
  //lcd.clear();
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
    //lcd.setCursor(2, 0);
    //lcd.print("Pouring");
    //lcd.setCursor(2, 1);
    //lcd.print("process");
    //delay(2000);
    moveServo(servo2, activ_angle2, interim_angle2_up, 30, 1);
    moveServo(servo1, activ_angle1, park_angle1, 50, 0);
    Serial.println("Servo1 responded for pour");
    moveServo(servo2, interim_angle2_up, interim_pour_angle2, 30, 1);
    moveServo(servo3, scoop_arm_angle_max, pour_angle3, 40, 2);
    Serial.println("Servo3 responded for pour");
    moveServo(servo2, interim_pour_angle2, interim_angle2_up, 50, 1);
    Serial.println("Servo2 responded for pour");
    moveServo(servo3, pour_angle3, scoop_arm_angle_min, 30, 2);
   //  lcd.clear();
    pouringStatus = true;
    Serial.println("Pouring complete.");
    //lcd.setCursor(2, 0);
    //lcd.print("Pouring ");
    //lcd.setCursor(2, 1);
    //lcd.print("Complete");
    //delay(2000);
    //lcd.clear();
    /*curr_Angles[1] = activ_angle2;
    curr_Angles[2] = scoop_arm_angle_min;*/
}

void returnToActive() 
{
    moveServo(servo3, scoop_arm_angle_min, activ_angle3, 50, 2);
    moveServo(servo1, park_angle1, activ_angle1, 50, 0);
    moveServo(servo2, interim_angle2_up, activ_angle2, 40, 1);
    Serial.println("System returned to ACTIVE...");
    lcd.setCursor(0, 0);
    lcd.print("System returened");
    lcd.setCursor(0, 1);
    lcd.print("to Active");
    delay(2000);
    lcd.clear();
}

void systemReturn()
{
  // Returning only if system is not ACTIVE
  // If button is pressed during the activation process
  // Always returns to IDLE state

  if (!systemActive)
  {
      Serial.println("Inside return park");
      stopServo(servo1, curr_Angles[0], park_angle1,40);
      Serial.println("Returned servo3");
      stopServo(servo2, curr_Angles[1],interim_angle2,40);
      Serial.println("Returned servo2");
      stopServo(servo3, curr_Angles[2], park_angle3,40);
      Serial.println("Returned servo1");
      stopServo(servo2, curr_Angles[1], park_angle2,40);

      curr_Angles[0] = park_angle1;
      curr_Angles[1] = park_angle2;
      curr_Angles[2] = park_angle3;

      currentState = IDLE;
  }

  // Returning from active/scooping/pouring position
  // Always returned to ACTIVE state
  else
  {
      Serial.println("Inside return active");
      stopServo(servo3, curr_Angles[2], activ_angle3, 40);
      Serial.println("Returned servo3");
      stopServo(servo2, curr_Angles[1], interim_angle2_up, 40);
      Serial.println("Returned servo2");
      stopServo(servo1, curr_Angles[0], activ_angle1, 40);
      Serial.println("Returned servo1");
      stopServo(servo2, interim_angle2_up, activ_angle2, 40);

      curr_Angles[0] = activ_angle1;
      curr_Angles[1] = activ_angle2;
      curr_Angles[2] = activ_angle3;

      currentState = ACTIVE;
  }
  

  
  //servomoving = 0;
}

// ======================== Function to move servo ========================
/*
void moveServo(Servo &servo, int from, int to, int delayTime) 
{
    emergency_stop.loop();

    if (from < to) 
    {
        for (int pos = from; pos <= to; pos++) 
        {
            while(emergency_stop.isPressed())
            {
              Serial.println("Interrupt, returning");
              delay(1000);
              returnToActive();
              Serial.println("system activated");
              break;
            }
            

            servo.write(pos);
            Serial.println(pos);
 //           Serial.println(emergency_stop);
            delay(delayTime);
        }
    } 
    else 
    {
        for (int pos = from; pos >= to; pos--) 
        { 
            while(emergency_stop.isPressed())
            {
              Serial.println("Interrupt, returning");
              delay(1000);
              returnToActive();
              Serial.println("system activated");
              break;
            }
            
            servo.write(pos);
            Serial.println(pos);
 //           Serial.println(emergency_stop);
            delay(delayTime);
        }
    }
}
*/
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
              emergency_stop.loop();  // Ensure ezButton updates
              if (emergency_stop.isPressed()) 
              {
                  Serial.println("Interrupt, returning");
                  delay(500);
                  systemReturn();  // Call function to return system to safe state
                  servomoving = false;
                  return;
                  //break;
              }
          }

          servo.write(pos);
          curr_Angles [angle_index] = pos;
          //Serial.print("Servo Position: ");
          //Serial.println(pos);
      }
    }
    else 
    {
      Serial.println("Stopped");
    }
}

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
