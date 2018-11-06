#define STATE_IDLE 0
#define STATE_ACTIVE 1

int sensors[2];
int sensor_values[2];
int motor_drivers[2];

int sensor_1_pin = A0;    
int sensor_2_pin = A1;     

int current_state = STATE_IDLE;

// 10 mins scan cycle
int idle_state_scan_cycle = 5000;

// 1 second scan cycle
int active_state_scan_cycle = 2000;

int motor_1_out = 2; // Digital out 2
int motor_2_out = 3; // Digital out 3

// --------------------------------------------------------------
// Setup
// --------------------------------------------------------------
void setup() {

  pinMode(2, OUTPUT);

  // setup coms for debugging
  Serial.begin(9600);
  Serial.println("-----------------------------------");
  Serial.println("Starting the plant watering system");
  Serial.println("-----------------------------------");
   
  sensors[0] = sensor_1_pin;
  sensors[1] = sensor_2_pin;
  
  motor_drivers[0] = motor_1_out;
  motor_drivers[1] = motor_2_out;

  Serial.println("1. Sensor config");
  Serial.println("-----------------------------------");
  for (int i = 0; i < sizeof(sensors) / sizeof(int);  i ++)
  {
    Serial.print("--> Sensor : ");
    Serial.print(i);
    Serial.print(", Input : ");
    Serial.println(sensors[i]);
  }
  
  Serial.println("2. Motor config");
  Serial.println("-----------------------------------");
  for (int i = 0; i < sizeof(motor_drivers) / sizeof(int);  i ++)
  {
    Serial.print("--> Motor : ");
    Serial.print(i);
    Serial.print(", Output : ");
    Serial.println(motor_drivers[i]);
    pinMode(motor_drivers[i], OUTPUT);
  }  

  Serial.println("3. Init values array");
  Serial.println("-----------------------------------");
  for (int i = 0; i < sizeof(motor_drivers) / sizeof(int);  i ++)
  {
    sensor_values[i] = 0;
  }  
}

// --------------------------------------------------------------
// Check sensor status and update values
// --------------------------------------------------------------
void checkStatus()
{
  for (int i = 0; i < sizeof(sensor_values) / sizeof(int); i++)
  {
    sensor_values[i] = analogRead(sensors[i]);
    Serial.print("Sensor : ");
    Serial.print(i);
    Serial.print(", Humidity : ");
    Serial.println(sensor_values[i]);
  }
}

// --------------------------------------------------------------
// Check and issue water
// --------------------------------------------------------------
void issueWaterSupplyIfRequired()
{
  bool one_sensor_low = false;
  
  for (int i = 0; i < sizeof(sensor_values) / sizeof(int); i++)
  {
    if (sensor_values[i] > 950)
    {
      if (digitalRead(motor_drivers[i]) != HIGH)
      {
        Serial.print("Activating motor ");
        Serial.println(i);
        digitalWrite(motor_drivers[i], HIGH);
        digitalWrite(2, HIGH);
      }

      one_sensor_low = true;

    }
    else
    {
      if (digitalRead(motor_drivers[i]) != LOW)
      {
        Serial.print("Deactivating motor ");
        Serial.println(i);
        digitalWrite(motor_drivers[i], LOW);
        digitalWrite(2, LOW);
      }
   }
  }
  
  if (one_sensor_low)
  {
    current_state = STATE_ACTIVE;
    Serial.println("System state ACTIVE");
  }
  else
  {
    current_state = STATE_IDLE;
    Serial.println("System state IDLE");
  }
}

// --------------------------------------------------------------
// Crap loop
// --------------------------------------------------------------
void loop() {

  // check the humidity
  checkStatus();

  // issue water supply if required
  issueWaterSupplyIfRequired();

  if (current_state == STATE_IDLE)
    delay(idle_state_scan_cycle);
  else
    delay(active_state_scan_cycle);
}
