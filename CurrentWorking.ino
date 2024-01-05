// Macros
#define RESET 6789
#define START 6587
#define STOP 5432
#define ADD_TO_QUEUE 9898 
#define REMOVE_FROM_QUEUE 21212
#define SEND_SIGNAL 6678

#define EMPTY 9090


// Pins
#define pwmPin 9                   // The pin where the PWM signal will be generated


// Vars
unsigned long periodMs;            // Reciprocal of Freq taken later
int percentOn;
int dutyCycle;
float frequencyHz;
int pulseLength; 
bool PWMFlag = false;
bool resetFlag = false;             // When pulse needs to be reset/changed
bool stopFlag = false;             // When pulse time is over and should stop
bool printOnce = false;            // Use to print once
bool printOutputOnce = true;      // Use to output On/Off timestamps Once
bool stopButton = false;           // 
bool onOffFlag = false; 
bool startedOnce = false;          // Did we take the start time of the device?
unsigned long flashingDuration = 0; // How long stimulation should occur
unsigned long stoppingDuration = 0;
unsigned long TimeStop = 0; // T0 for flashing on/off
unsigned long TimeBegin = 0; // T0 for overall time measured from start of pulse, will be reset by reset button 


// Runs Once
void setup() {
  pinMode(pwmPin, OUTPUT);         // Set the PWM pin as an output
  analogWrite(pwmPin, 0);          // Start with 0% duty cycle (LED off)
  Serial.begin(9600);             // Start serial communication
  Serial.println("Welcome! \nPlease Enter the signal properties into the text box and press \"Send\" to observe it");
  TimeStop = millis();
}


// Main Loop
void loop() {
  
  /////////////////////////////////////////////////////////////////////////
  // Checking for inputs from Python UI
  if (Serial.available()) {
    // GIven we don't want to reset(and under the premise there is new input)
    unsigned long NextResponse = Serial.parseInt(); // Parse first response(maybe vals atm)
    
    // Reset ALL vals
    if (NextResponse == RESET) {
      Serial.println("Please Wait(timestamp WILL be reset)...");
      // reset timestamp flag
      if (startedOnce) {
        startedOnce = false;// will be reset after pressing "Start"
      }
      resetFlag = false;
      onOffFlag = false; // Turns off Cycling
      stopButton = true;
      PWMFlag = false; // Turns off PWM
      analogWrite(pwmPin, 0); // hopefully keep signal flat?
      NextResponse = EMPTY; // reset to dif val that represents "EMPTY" after
      Serial.println("Ready to input new values, please click \"Send\"");
      

    // Input Data
    } else if (NextResponse == SEND_SIGNAL) {
      resetFlag = true;
      onOffFlag = false;
      PWMFlag = false; // Turns off PWM 
      stopButton = true;
       analogWrite(pwmPin, 0); // hopefully keep signal flat?
      NextResponse = EMPTY; // reset to dif val that represents "EMPTY" after
    // Start Stimulation
    } else if (NextResponse == START) {
      Serial.println("Starting...");
      stopButton = false; // vesitigal?
      onOffFlag = true; // allow signal to turn on
      TimeStop = millis(); // reset timer so that it times properly after starting
      NextResponse = EMPTY; // reset to dif val that represents "EMPTY" after
      PWMFlag = true;
      analogWrite(pwmPin, 0); // hopefully keep signal flat?

     
    // Pause Stimulation
    } else if (NextResponse == STOP) {
      Serial.println("Pausing(timestamp WILL NOT be reset)...");
      
      stopButton = true;
      PWMFlag = false; // Turns off PWM
      onOffFlag = false; // Turns off Cycling
      NextResponse = EMPTY; // reset to dif val that represents "EMPTY" after

    // Add 2 Queue
    } else if (NextResponse == ADD_TO_QUEUE) {
      NextResponse = EMPTY; // reset to dif val that represents "EMPTY" after

    // Remove from Queue 
    } else if (NextResponse == REMOVE_FROM_QUEUE) {
      NextResponse = EMPTY; // reset to dif val that represents "EMPTY" after

    // Catch all Error Case 
    } else if (NextResponse != 0) { // right now first val will return an "Invalid Response" but that should change later when i add queueing 
//      Serial.print("Invalid Response: ");
      Serial.println(NextResponse);
//      NextResponse = EMPTY; // reset to dif val that represents "EMPTY" after
    }
    
    
    if (resetFlag) {
      // Parse vals +++++++++++++++++++++++
      dutyCycle = Serial.parseInt();
      frequencyHz = Serial.parseFloat();
      pulseLength = Serial.parseInt();
      
      flashingDuration = Serial.parseInt();
      stoppingDuration = Serial.parseInt();
      // ++++++++++++++++++++++++++++++++++

      // Print inputted vals-----------
      Serial.print("Duty: ");
      Serial.print(dutyCycle);
      Serial.print("%, Freq: ");
      Serial.print(frequencyHz);
      Serial.print("[Hz], Pulse Length: ");
      Serial.print(pulseLength);
      percentOn = (pulseLength * frequencyHz)/10;
      Serial.print("[ms], On/Off: ");
      Serial.print(percentOn);
      Serial.print("%, Flashing Duration: ");
      Serial.print(flashingDuration);
      Serial.print("[s], Stopping Duration: ");
      Serial.print(stoppingDuration);
      Serial.println("[s]\n");
      // -----------------------------

      
      TimeStop = millis();

      // Calculations for 
      bool valErrorFlag = false;
      
      
      if ((dutyCycle < 0) || (dutyCycle > 100)) {
        Serial.println("Duty Cycle out of bounds(0-100)");
        valErrorFlag = true;
      }
      if ((frequencyHz < 0) || (frequencyHz > 500)) {
        Serial.println("Frequency out of bounds(0-500)");
        valErrorFlag = true;
      }
      if ((pulseLength * frequencyHz)/1000 >= 1) {
        Serial.println("Pulse Length must be <= the period(1/frequency):");
        valErrorFlag = true;
      }
      if (valErrorFlag == false) {
        Serial.println("Valid values.\n");
        resetFlag = false; // reset flag is set down so that this doesn't run again until needed
        printOnce = true;
        periodMs = 1000 / frequencyHz;  // Calculate the period in milliseconds
        
      } else {
        //duty cycle (0-100%), frequency (Hz), Pulse Length (>0milliseconds & <1/freq), flashing Duration (milliseconds), and stopping Duration (milliseconds)
        Serial.println("Please press \"Reset\" and enter new values.");
        
        valErrorFlag = false; // put flag down bc error messages have been printed
        resetFlag = false; // put flag down, wait for input to be pressed for reset first, then send data to actually reset
      }
    } else {
      if (printOnce) {
//        PWMFlag = true;
        Serial.println("Click 'Start' to begin stimulation or Click 'RESET' to enter a new stimulation pattern:");
        printOnce = false;
      }
      
    }
  }
  /////////////////////////////////////////////////////////////////////////
  //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  /////////////////////////////////////////////////////////////////////////
  // On/Off Cycle for amounts of time On/Off
  if (onOffFlag == true) {  
    if (!startedOnce) { // timestamp flag down? 
      startedOnce = true; // put it up
      TimeBegin = millis(); // take new timestamp time
    }
    if (stopFlag == false && millis() - TimeStop > flashingDuration * 1000) {
        stopFlag = true;
        PWMFlag = false;
        TimeStop = millis();
        if (printOutputOnce) { // Printing Output Message...
        // Brightness: w%, Frequency: xHz, Duty Cycle: y%, Flashing Duration zs \n
//          Serial.println("Signal Off"); // Could be reversed w/ LED Driver? nahhhhhh

          // Change later to current pulse vals rather than input vals
          Serial.print("OFF, Brightness: "); // Print ON timestamp for Nico
          Serial.print(dutyCycle); // Print ON timestamp for Nico
          Serial.print("%, Frequency: "); // Print ON timestamp for Nico
          Serial.print(frequencyHz); // Print ON timestamp for Nico
          Serial.print("Hz, Pulse Width: "); // Print ON timestamp for Nico
          Serial.print(pulseLength); // Print ON timestamp for Nico
          Serial.print("[ms], Flashing Duration: "); // Print ON timestamp for Nico
          Serial.print(flashingDuration); // Print ON timestamp for Nico
          Serial.print("sec, Stopping Duration: "); // Print ON timestamp for Nico
          Serial.print(stoppingDuration); // Print ON timestamp for Nico
          Serial.print("sec, Time: "); // Print ON timestamp for Nico
          Serial.println((millis()- TimeBegin)/1000); // millis()- TimeBegin

          
          printOutputOnce = false;
        }
    }
  
    //
    if (stopFlag == true && millis() - TimeStop > stoppingDuration * 1000) {
      PWMFlag = true;
      stopFlag = false;
      TimeStop = millis();
      if (!printOutputOnce) { // Printing Output Message...
        // Change later to current pulse vals rather than input vals
          Serial.print("ON, Brightness: "); // Print OFF timestamp for Nico
          Serial.print(dutyCycle); // Print ON timestamp for Nico
          Serial.print("%, Frequency: "); // Print OFF timestamp for Nico
          Serial.print(frequencyHz); // Print OFF timestamp for Nico
          Serial.print("Hz, Pulse Width: "); // Print OFF timestamp for Nico
          Serial.print(pulseLength); // Print ON timestamp for Nico
          Serial.print("[ms], Flashing Duration: "); // Print OFF timestamp for Nico
          Serial.print(flashingDuration); // Print OFF timestamp for Nico
          Serial.print("sec, Stopping Duration: "); // Print OFF timestamp for Nico
          Serial.print(stoppingDuration); // Print OFF timestamp for Nico
          Serial.print("sec, Time: "); // Print OFF timestamp for Nico
          Serial.println((millis()- TimeBegin)/1000); // millis()- TimeBegin

//        Serial.println("Signal On"); // Signal is High here tho after stopping duration over
        printOutputOnce = true;
      }
    }
  } // onOffFlag end 
    // Stops PWM always if we are paused. Should be ok once we press start again.
    if (onOffFlag) {
      
    }
  /////////////////////////////////////////////////////////////////////////
  //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  /////////////////////////////////////////////////////////////////////////
  // Loops signal for Freq
    // Set PWM and Flashing
    if (PWMFlag) {
      if ((millis() % periodMs) < ((periodMs * percentOn) / 100)) {
        analogWrite(pwmPin, 255 * dutyCycle / 100);
      } else {
        analogWrite(pwmPin, 0);
      }
    } else {
      analogWrite(pwmPin, 0);
    }
    
    /////////////////////////////////////////////////////////////////////////
}
