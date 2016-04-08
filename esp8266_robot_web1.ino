/* Robot ESP8266 Webserver
   Based on ESP8266Webserver, DHTexample, and BlinkWithoutDelay (thank you)

   Version 1.0  5/3/2014  Version 1.0   Mike Barela for Adafruit Industries

   Adapted by Aaron Brodney to drive his new robot
   March 16 2016

   Last Updated: March 27 2016

*/


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

const char* ssid     = "Brodney-Fast";
const char* password = "menagerie!";


const long defaultCommandDuration = 250;
const long defaultTurnDuration = 100;
const long defaultSpeed = 230;

long commandDuration = defaultCommandDuration;
long turnDuration = defaultTurnDuration;
long currentSpeed = defaultSpeed;

ESP8266WebServer server(80);

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// And connect 2 DC motors to port M3 & M4 !
Adafruit_DCMotor *L_MOTOR = AFMS.getMotor(4);
Adafruit_DCMotor *R_MOTOR = AFMS.getMotor(3);


String webString = "";   // String to display

void handle_root() {
  server.send(200, "text/html", "<b>Hello from the robot esp8266.</b>");
  delay(100);
}


void shortTurns()
{
  Serial.println("SHORT TURNS");
  turnDuration = defaultTurnDuration / 2;
}

void regularTurns() 
{
  Serial.println("REGULAR TURNS");
  turnDuration = defaultTurnDuration;
}


void setMotorSpeed(long xSpeed)
{
  Serial.println("bilaterial setSpeed");
  
  L_MOTOR->setSpeed(xSpeed);
  R_MOTOR->setSpeed(xSpeed);
  
}

void highSpeed() // not quite full speed, but still higher than normal
{
  Serial.println("Motors to HIGH SPEED");
  //L_MOTOR->setSpeed(245);
  //R_MOTOR->setSpeed(245);
  setMotorSpeed(245);
  
}

void fullSpeed()
{
  // this may strip the gears in the the motor!
  Serial.println("Motors to FULL speed");
 // L_MOTOR->setSpeed(255);
 // R_MOTOR->setSpeed(255);
 setMotorSpeed(255);
}

void normalSpeed()
{
  Serial.println("Motors to DEFAULT SPEED");
  //L_MOTOR->setSpeed( defaultSpeed );
  //R_MOTOR->setSpeed( defaultSpeed );
  setMotorSpeed( defaultSpeed );
  
}

void mediumSpeed()
{
  Serial.println("Motors to HALF speed");
  //L_MOTOR->setSpeed(190);
  //R_MOTOR->setSpeed(190);
  setMotorSpeed(190);
}

void slowSpeed()
{
  Serial.println("Motors to SLOW speed");
  //L_MOTOR->setSpeed(150);
  //R_MOTOR->setSpeed(150);
  setMotorSpeed(150);
}


void runForward(long runTime = commandDuration)
{
  Serial.print("RUN FORWARD:");
  Serial.println(runTime);

  L_MOTOR->run( FORWARD );
  R_MOTOR->run( FORWARD );
  delay(runTime);
  L_MOTOR->run( RELEASE );
  R_MOTOR->run( RELEASE );
}


void runBackward(long runTime = commandDuration)
{
  Serial.print("RUN BACKWARD:");
  Serial.println(runTime);

  L_MOTOR->run( BACKWARD );
  R_MOTOR->run( BACKWARD );
  delay(runTime);
  L_MOTOR->run( RELEASE );
  R_MOTOR->run( RELEASE );
}

void turnRight(long runTime = turnDuration)
{

  Serial.print("TURN RIGHT: ");
  Serial.println(runTime);

  L_MOTOR->run( FORWARD );
  R_MOTOR->run( BACKWARD );
  delay(runTime);
  L_MOTOR->run( RELEASE );
  R_MOTOR->run( RELEASE );

}

void turnLeft(long runTime = turnDuration)
{
  Serial.print("TURN LEFT: ");
  Serial.println(runTime);
  L_MOTOR->run( BACKWARD );
  R_MOTOR->run( FORWARD );
  delay(runTime);
  L_MOTOR->run( RELEASE );
  R_MOTOR->run( RELEASE );
}

void testMotors()
{
  runForward(500);
  delay(1500);
  runBackward(500);
  delay(1500);
  turnRight();
  delay(1500);
  turnLeft();
}

void stopMotors()
{
  L_MOTOR->run( RELEASE );
  R_MOTOR->run( RELEASE );
}

void printStatus()
{
  Serial.println("ALIVE!");
  // print robot status information

}

void resetToDefaults()
{
  turnDuration = 100;
  commandDuration = 250;
  L_MOTOR->setSpeed( defaultSpeed );
  R_MOTOR->setSpeed( defaultSpeed );
  R_MOTOR->run(RELEASE);
  L_MOTOR->run(RELEASE);

}

void rightMicroTurns()
{
  Serial.println("10 Right micro turns");
  for ( int i = 1 ; i < 25 ; i++ ) {
    turnRight(25);
    delay(250);
  }
}

void setup(void)
{

  Serial.begin(115200); 
  Wire.begin();

  // changed from the default (1.6khz, default)
  AFMS.begin(8000);  // create with the default frequency 1.6KHz

  // turn on motors
  L_MOTOR->setSpeed(defaultSpeed);
  L_MOTOR->run(RELEASE);

  R_MOTOR->setSpeed(defaultSpeed);
  R_MOTOR->run(RELEASE);


  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\Trying to connect");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Robot Control Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Received IP address from DHCP server: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_root);


  server.on("/rightMicroTurns", []() {
      webString = "MICRO TURNS TO THE RIGHT\r\n";
      server.send(200, "text/plain", webString);
      rightMicroTurns();
  });
  
  server.on("/shortTurns", []() {
    webString = "Short run turns\r\n";
    server.send(200, "text/plain", webString);
    shortTurns();  
  });

  server.on("/regularTurns", []() {
    webString = "Regular turns\r\n";
    server.send(200, "text/plain", webString);
    regularTurns();
  });
  
  server.on("/forward", []() {

    webString = "Commanding Motors Forward\r\n";
    server.send(200, "text/plain", webString);
    runForward();
  });

  server.on("/backward", []() {

    webString = "Commanding Motors Backward\r\n";
    server.send(200, "text/plain", webString);
    runBackward();
  });

  server.on("/right", []() {
    webString = "Commanding Right\r\n";
    server.send(200, "text/plain", webString);
    turnRight();
  });

  server.on("/left", []() {
    webString = "Commanding Left\r\n";
    server.send(200, "text/plain", webString);
    turnLeft();
  });

  server.on("/speedHigh", []() {
      webString = "COMMANDING HIGH SPEED\r\n";
      server.send(200, "text/plain", webString);
      highSpeed();
  });
  
  server.on("/speedFast", []() {
    webString = "Commanding Speed Fast\r\n";
    server.send(200, "text/plain", webString);
    fullSpeed();
  });


  server.on("/speedMedium", []() {
    webString = "Commanding Speed Medium\r\n";
    server.send(200, "text/plain", webString);
    mediumSpeed();
  });


  server.on("/speedSlow", []() {
    webString = "Commanding Speed Slow\r\n";
    server.send(200, "text/plain", webString);
    slowSpeed();
  });


  server.on("/speedNormal", []() {
    webString = "COMMANDING DEFAULT SPEED\r\n";
    server.send(200, "text/plain", webString);
    normalSpeed();
  });

  server.on("/testMotors", []() {
    webString = "Running Motor Test Procedure\r\n";
    server.send(200, "text/plain", webString);
    testMotors();
  });

  server.on("/stop", []() {
    webString = "ALL STOP\r\n";
    server.send(200, "text/plain", webString);
    stopMotors();
  });

  server.on("/defaults", []() {
    webString = "RESET TO DEFAULTS\r\n";
    server.send(200, "text/plain", webString);
    resetToDefaults();
  });

  server.on("/veryLong", []() {
      webString = "VERY LONG COMMANDS HALF-SEC\r\n";
      server.send(200, "text/plain", webString);
      commandDuration = 750;
  });
  

  server.on("/longDuration", []() {
    webString = "LONG COMMANDS\r\n";
    server.send(200, "text/plain", webString);
    commandDuration = 500;
  });

  server.on("/normalDuration", []() {
    webString = "NORMAL COMMANDS\r\n";
    server.send(200, "text/plain", webString);
    commandDuration = 250;
  });

  server.on("/shortDuration", []() {
    webString = "SHORT COMMANDS\r\n";
    server.send(200, "text/plain", webString);
    commandDuration = 125;
  });


  server.on("/restart", []() {
      webString = "RESARTING 8266 MCU in 1 sec\r\n";
      server.send(200, "text/plain", webString);
      delay(1000);
      ESP.restart();
  });

  server.begin();
  Serial.println("Robot Command HTTP server started");

  // default to full speed
  normalSpeed();

}

void loop(void)
{
  server.handleClient();
}



