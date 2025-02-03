#include <Servo.h>
#include <ezButton.h>

// Define buttons
ezButton start_button(2);
ezButton scoop_pour(3);
ezButton stop_button(4);

// Define servo motors
Servo servo1; 
Servo servo2; 
Servo servo3;

// Define states
enum State {
              IDLE,
              ACTIVE,
              SCOOPING,
              POURING,
              PARKING
            };

State currentState = IDLE;
unsigned long scoopingStartTime = 0;
const long scoopingDuration = 5000; //  Scooping runs for 5 seconds

// Servo angles
int activ_angle1 = 140; 
int activ_angle2 = 120; 
int activ_angle3 = 60;

int park_angle1 = 0; 
int park_angle2 = 0; 
int park_angle3 = 0;

int interim_angle2 = 45; 
int interim_pour_angle2 = 90;     // This is set for smoother arm positioning

int scoop_arm_angle_max = 150;    // Scooping arm angles
int scoop_arm_angle_min = 60;

long material_weight = 0;         
long weight_threshold = 15;       // Setting threshold weight upto which the scooping action will continue

// System status flags for different operations
bool systemActive = false; 
bool scoopingAction = false; 
bool pouringStatus = false;

// Store current servo angles
int curr_Angles[3] = {0, 0, 0};

void setup() 
{
    Serial.begin(9600);
    Serial.println("Power ON");

    start_button.setDebounceTime(50);
    scoop_pour.setDebounceTime(50);
    stop_button.setDebounceTime(50);

    servo1.attach(5);
    servo1.write(park_angle1);
    servo2.attach(6);
    servo2.write(park_angle2);
    servo3.attach(9);
    servo3.write(park_angle3);
    
    
    
  //  delay(500);



    delay(1000);
    Serial.println("System initialized");
}

void loop() 
{
    start_button.loop();
    scoop_pour.loop();
    stop_button.loop();

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
            break;

        case ACTIVE:
            Serial.println("System is ACTIVATED...");
            break;

        case SCOOPING:
            // Checks condition for start action
            if (material_weight < weight_threshold) 
            {
                scooping();
                Serial.print("Scooping complete! | ");
                material_weight += 5;
                Serial.print("Item weight =");
                Serial.println(material_weight);
                currentState = POURING;
            }             
            
            else
            {
              Serial.println("Threshold reached...");   // Prevents action if action button is pressed after it's complete
              currentState = ACTIVE; 
            }

            break;

        case POURING:
            Serial.println("Pouring material...");
            pour();
//            material_weight += 5;
            delay(500);
            returnToActive();

            // Checks condition for further actions
            if (material_weight <= weight_threshold)
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
            Serial.println("Parking system...");
            parking();
            systemActive = false;           // Reset active state flag
            material_weight = 0;            // Reset material weight for further scooping action
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
        return;
    }

    if (currentState == ACTIVE) 
    {
        Serial.println("Scooping started...");
        currentState = SCOOPING;
    }
}

void parking_Handler() 
{
    if (currentState == SCOOPING || currentState == POURING) 
    {
        Serial.println("Finishing current task before parking...");
        return;
    }
    currentState = PARKING;
}

// ======================== Functional Actions ========================

void sysActiv() 
{
    Serial.println("Activating system...");
    moveServo(servo2, curr_Angles[1], interim_angle2, 50);
    Serial.println("Servo2 responded");
    moveServo(servo3, curr_Angles[2], activ_angle3, 50);
    Serial.println("Servo3 responded");
    moveServo(servo2, interim_angle2, activ_angle2, 50);
    Serial.println("Servo2 responded again from intermediate");
    moveServo(servo1, curr_Angles[0], activ_angle1, 50);
    Serial.println("Servo1 responded");

    curr_Angles[0] = activ_angle1;
    curr_Angles[1] = activ_angle2;
    curr_Angles[2] = activ_angle3;
}

void parking() 
{
    Serial.println("Parking system...");
    moveServo(servo1, curr_Angles[0], park_angle1, 50);
    Serial.println("Servo1 responded");
    moveServo(servo2, curr_Angles[1], interim_angle2, 30);
    Serial.println("Servo2 responded");
    moveServo(servo3, curr_Angles[2], park_angle3, 50);
    Serial.println("Servo3 responded");
    moveServo(servo2, interim_angle2, park_angle2, 40);
    Serial.println("Servo2 responded from intermediate");

    curr_Angles[0] = park_angle1;
    curr_Angles[1] = park_angle2;
    curr_Angles[2] = park_angle3;
}

void scooping()
{
  moveServo(servo3, scoop_arm_angle_min, scoop_arm_angle_max, 50);
  Serial.println("Servo3 responded for scooping");
  scoopingAction = true;
}

void pour() 
{
    if (!scoopingAction) 
    {
        Serial.println("No material to pour.");
        return;
    }

    Serial.println("Pouring process...");
    moveServo(servo1, activ_angle1, park_angle1, 50);
    Serial.println("Servo1 responded for pour");
    moveServo(servo2, activ_angle2, interim_pour_angle2, 30);
    moveServo(servo3, scoop_arm_angle_max, scoop_arm_angle_min, 40);
    Serial.println("Servo3 responded for pour");
    moveServo(servo2, interim_pour_angle2, activ_angle2, 50);
    Serial.println("Servo2 responded for pour");

    pouringStatus = true;
    Serial.println("Pouring complete.");

    curr_Angles[1] = activ_angle2;
    curr_Angles[2] = scoop_arm_angle_min;
}

void returnToActive() 
{
    moveServo(servo2, curr_Angles[1], activ_angle2, 50);
    moveServo(servo3, curr_Angles[2], activ_angle3, 50);
    moveServo(servo1, park_angle1, activ_angle1, 50);
    Serial.println("System returned to ACTIVE...");
}

// ======================== Function to move servo ========================

void moveServo(Servo &servo, int from, int to, int delayTime) 
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
