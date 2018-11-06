#define SENSOR_THRESHOLD 950

#define IDLE_SCAN_CYCLE 5000
#define ACTIVE_SCAN_CYCLE 2000
#define ATTACHED_SUB_SYSTEM_COUNT 2


int sensors[2];
bool sensor_states[2];
int motor_drivers[2];

int sensor_1_pin = A0;  // Analog in 0   
int sensor_2_pin = A1;  // Analog in 1   

int motor_1_out = 2; // Digital out 2
int motor_2_out = 3; // Digital out 3

int idle_state();
int active_state();

int (*state_fcn)() = idle_state;

// --------------------------------------------------------------
// Setup
// --------------------------------------------------------------
void setup() {
  // setup coms for debugging
  Serial.begin(9600);
  Serial.println("-----------------------------------");
  Serial.println("Starting the plant watering system");
  Serial.println("-----------------------------------");
   
  sensors[0] = sensor_1_pin;
  sensors[1] = sensor_2_pin;
  
  motor_drivers[0] = motor_1_out;
  motor_drivers[1] = motor_2_out;

  Serial.print("1. Scan cycle : ");
  Serial.print(IDLE_SCAN_CYCLE / 1000);
  Serial.println(" seconds");
  
  Serial.println("-----------------------------------");
  Serial.println("2. Sensor config");
  Serial.println("-----------------------------------");
  for (int i = 0; i < ATTACHED_SUB_SYSTEM_COUNT;  i ++)
  {
    Serial.print("--> Sensor : ");
    Serial.print(i);
    Serial.print(", Input : ");
    Serial.println(sensors[i]);
  }

  Serial.println("-----------------------------------");
  Serial.println("3. Motor config");
  Serial.println("-----------------------------------");
  for (int i = 0; i < ATTACHED_SUB_SYSTEM_COUNT;  i ++)
  {
    Serial.print("--> Motor : ");
    Serial.print(i);
    Serial.print(", Output : ");
    Serial.println(motor_drivers[i]);
    pinMode(motor_drivers[i], OUTPUT);
  }  

  for (int i = 0; i < ATTACHED_SUB_SYSTEM_COUNT;  i ++)
  {
    sensor_states[i] = false;
  }  

  Serial.println("-----------------------------------");
  Serial.println("Start watching");
  Serial.println("-----------------------------------");
}

// --------------------------------------------------------------
// Check sensor status and update values
// --------------------------------------------------------------
bool EvaluateSensorStates()
{
  bool one_sensor_active = false;
  for (int i = 0; i < ATTACHED_SUB_SYSTEM_COUNT; i++)
  {
    int sensorValue = analogRead(sensors[i]);
    Serial.print("Sensor : ");
    Serial.print(i);
    Serial.print(", Humidity : ");
    Serial.println(sensorValue);

    if (sensorValue > SENSOR_THRESHOLD)
    {
      sensor_states[i] = true;
      one_sensor_active = true;
    }
    else
    {
      sensor_states[i] = false;
    }
  }

  Serial.println("-----------------------------------");
  
  if (one_sensor_active)
  {
    return true;
  }
  else
  {
    return false;
  }
}

// --------------------------------------------------------------
// Change Motor State
// --------------------------------------------------------------
void SetMotorState(int motorIndex, bool state)
{
  if (digitalRead(motor_drivers[motorIndex]) != state)
  {
     digitalWrite(motor_drivers[motorIndex], state);
      
      Serial.print("Motor ");
      Serial.print(motorIndex);
      Serial.println(state ? " Activated." : " Deactivated.");
  }
}

// --------------------------------------------------------------
// Idle state
// --------------------------------------------------------------
int idle_state()
{
  if (EvaluateSensorStates())
  {
    Serial.println("System state ACTIVE");
    state_fcn = active_state;
    return ACTIVE_SCAN_CYCLE;
  }
  else
  {
    return IDLE_SCAN_CYCLE;
  }
}

// --------------------------------------------------------------
// Active state
// --------------------------------------------------------------
int active_state()
{
  for (int i = 0; i < ATTACHED_SUB_SYSTEM_COUNT;  i ++)
  {
    SetMotorState(i,sensor_states[i]);   
  }  
  if (EvaluateSensorStates())
  {
    return ACTIVE_SCAN_CYCLE;
  }
  else
  {
    Serial.println("System state IDLE");

    for (int i = 0; i < ATTACHED_SUB_SYSTEM_COUNT;  i ++)
    {
      SetMotorState(i,false);
    } 
  
    state_fcn = idle_state;
    return IDLE_SCAN_CYCLE;
  }
}

// --------------------------------------------------------------
// THE ALMIGHTY LOOP :D :D
// --------------------------------------------------------------
void loop() {
  int next_delay = (*state_fcn)();

  delay(next_delay);
}
