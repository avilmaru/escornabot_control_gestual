
// ---------------------------------------------------------------------------
// Escornabot: Control por gestos - v1.0 - 18/10/2019
//
// AUTOR:
// Creado por Angel Villanueva - @avilmaru
//
// LINKS:
// Blog: http://www.mecatronicalab.es
//
//
// HISTORICO:
// 18/10/2019 v1.0 - Release inicial.
//
// ---------------------------------------------------------------------------
  

#include <ArduinoBLE.h>
#include <Arduino_APDS9960.h>

const int R_LED_PIN = 22;
const int G_LED_PIN = 23;
const int B_LED_PIN = 24;

const char* deviceServiceUuid = "ffe0";
const char* deviceServiceCharacteristicUuid = "ffe1";

int gesture = -1;
int lastGesture = -1;
String command = "";
unsigned long startTime = millis(); 
unsigned long timeInterval = 0; 
unsigned long LIMIT = 500;

void setup() {

  /* Todas las trazas se encuentran comentadas */
  
  //Serial.begin(9600);
  //while (!Serial);

  if (!APDS.begin()) {
    //Serial.println("Error initializing APDS9960 sensor!");
     while (1);
  }

   APDS.setGestureSensitivity(75); // [1..100]
  
  // begin ble initialization
  if (!BLE.begin()) {
    //Serial.println("starting BLE failed!");
    while (1);
  }

  //Serial.println("BLE Central - gesture control");

  pinMode(R_LED_PIN, OUTPUT);
  pinMode(G_LED_PIN, OUTPUT);
  pinMode(B_LED_PIN, OUTPUT);

  setColor("BLACK");
  

}

void loop() {
  
     connectToPeripheral();
}


void connectToPeripheral(){

  BLEDevice peripheral;

  do
  {
    // start scanning for peripherals
    BLE.scanForUuid(deviceServiceUuid);
    peripheral = BLE.available();
    
  } while (!peripheral);

  
  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    //Serial.print("Found  ");
    //Serial.print(peripheral.address());
    //Serial.print(" '");
    //Serial.print(peripheral.localName());
    //Serial.print("' ");
    //Serial.print(peripheral.advertisedServiceUuid());
    //Serial.println();
  
    // stop scanning
    BLE.stopScan();
  
    controlPeripheral(peripheral);
   
  }
  
}

void controlPeripheral(BLEDevice peripheral) {

  
  // connect to the peripheral
  //Serial.println("Connecting ...");

  if (peripheral.connect()) {
    //Serial.println("Connected");
  } else {
    //Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  //Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    //Serial.println("Attributes discovered");
  } else {
    //Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic gestureCharacteristic = peripheral.characteristic(deviceServiceCharacteristicUuid);
    
  if (!gestureCharacteristic) {
    //Serial.println("Peripheral does not have gesture characteristic!");
    peripheral.disconnect();
    return;
  } else if (!gestureCharacteristic.canWrite()) {
    //Serial.println("Peripheral does not have a writable gesture characteristic!");
    peripheral.disconnect();
    return;
  }

  
  while (peripheral.connected()) {

    command = gestureDetectection();
    if (command != "")
    {
      int n = command.length(); 
      // declaring character array 
      char char_array[n + 1]; 
      // copying the contents of the string to char array 
      strcpy(char_array, command.c_str()); 
      gestureCharacteristic.writeValue((char*)char_array);
      delay(300);
      setColor("BLACK");
    }
  
  }

  //Serial.println("Peripheral disconnected!");

}
  
String gestureDetectection(){

  String _command;  

  if (APDS.gestureAvailable()) { // a gesture was detected
    
    gesture = APDS.readGesture();
    startTime = millis();
    
  }

  timeInterval = millis() - startTime;
    
  if (timeInterval > LIMIT)
  {

      switch (lastGesture) {
          case GESTURE_UP:
            //Serial.println("Detected UP gesture");
            setColor("GREEN");
            _command = "n\n";
            gesture = -1;
            break;
          case GESTURE_DOWN:
            //Serial.println("Detected DOWN gesture");
            setColor("GREEN");
            _command = "s\n";
            gesture = -1;
            break;
          case GESTURE_LEFT:
            //Serial.println("Detected LEFT gesture");
            _command = "w\n";
            setColor("GREEN");
            gesture = -1;
            break;
          case GESTURE_RIGHT:
            //Serial.println("Detected RIGHT gesture");
            _command = "e\n";
            setColor("GREEN");
            gesture = -1;
            break;
          default:
            ////Serial.println("NO gesture detected!");
            _command = "";
            gesture = -1;
            break;
        }
   
    
  }else{
  
     if ((gesture == GESTURE_DOWN) && (lastGesture == GESTURE_UP)) {
      
        //Serial.println("Detected GO gesture");
        setColor("BLUE");
        _command = "g\n";
        gesture = -1;
        
      }else if ((gesture == GESTURE_RIGHT) && (lastGesture == GESTURE_LEFT)) {
        
        //Serial.println("Detected RESET gesture");
        setColor("RED");
        _command = "r\n";
        gesture = -1;
        
      }else if ((gesture == GESTURE_UP) && (lastGesture == GESTURE_DOWN)) {
      
        //Serial.println("Detected GO gesture");
        setColor("BLUE");
        _command = "g\n";
        gesture = -1;
        
      }else if ((gesture == GESTURE_LEFT) && (lastGesture == GESTURE_RIGHT)) {
        
        //Serial.println("Detected RESET gesture");
        setColor("RED");
        _command = "r\n";
        gesture = -1;
      }
      
      
  }

  lastGesture = gesture;
  return _command;
  
    
}


void setColor(String color)
{

  if (color == "RED")
  {
    digitalWrite(R_LED_PIN, LOW);  // High values -> lower brightness
    digitalWrite(G_LED_PIN, HIGH);
    digitalWrite(B_LED_PIN, HIGH);
    
  }else if (color == "GREEN")
  {
    digitalWrite(R_LED_PIN, HIGH);  // High values -> lower brightness
    digitalWrite(G_LED_PIN, LOW);
    digitalWrite(B_LED_PIN, HIGH);
    
  }else if (color == "BLUE")
  {
    digitalWrite(R_LED_PIN, HIGH);  // High values -> lower brightness
    digitalWrite(G_LED_PIN, HIGH);
    digitalWrite(B_LED_PIN, LOW);
    
  }else if (color == "BLACK")
  {
    digitalWrite(R_LED_PIN, HIGH);  // High values -> lower brightness
    digitalWrite(G_LED_PIN, HIGH);
    digitalWrite(B_LED_PIN, HIGH);
  }        

}
