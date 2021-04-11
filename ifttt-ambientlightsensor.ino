
// This web page is my savior
// SO CODE IS FROM THIS WEBSITE
//http://wiki.sunfounder.cc/index.php?title=GY-30_Digital_Light_Intensity_Measuring_Module

#include <Wire.h>

// GY-30
// BH1750FVI
// in ADDR 'L' mode 7bit addr

#define ADDR 0b0100011

// addr 'H' mode


    int val = 0; // reset
    bool sunny = false;
    int ifttt = 2; // 0 means no light, 1 means sunlight, 2 means no result yet.
    bool sunlight = false; // variable for saying whether there is sunlight
    bool nowSunlight = false; // variable for comparing whether there is still sunlight or not

void setup() {
    // Starting up the sensor stuff
    Serial.begin(9600);
    Wire.begin();
    Wire.beginTransmission(ADDR);
    Wire.write(0b00000001);
    Wire.endTransmission();
    //Particle.variable("lx", val); 
    Particle.variable("ifttt", ifttt); 
    Particle.variable("sunny", sunny); 

}

void loop() {
    
    val = 0; // reset
    
    // collect sensor data
    Wire.beginTransmission(ADDR);
    Wire.write(0b00000111);
    Wire.endTransmission();
    Wire.beginTransmission(ADDR);
    Wire.write(0b00100000);
    Wire.endTransmission();
    
    delay(120); // typical read delay 120ms
    
    Wire.requestFrom(ADDR, 2); // 2byte every time
    
    // read wire result and send it to val variable
    for (val = 0; Wire.available() >= 1; ) {
        //char c = Wire.read();
        val = Wire.read();//(val << 8) + (c & 0xFF);
    }
    val = val / 1.2;
    
    // output to serial
    Serial.print("lx: ");
    Serial.println(val);
    
    // checks weather sunny or not
    if(val>30){ 
        sunlight=true;
        //sunny = true;
        Serial.print("Sunny " + String(sunlight));
        ifttt=1;
    }
    else{
        sunlight=false;
        //sunny = false; 
        Serial.print("Dark " + String(sunlight));
        ifttt=0;
    }
    
    // checks whether still sunny or not. Weather anything has changed since last result was read. 
    // Then sends new data to IFTTT website
    if((sunlight==true&&nowSunlight==false)||(sunlight==false&&nowSunlight==true)){
        sendTheEmail();
    }
    else{
        Serial.println(" - No changes sent");
    }
    ifttt=2;
    delay(10000); // 10 second delay before next check

}
void sendTheEmail(){
         //Particle.publish("lx:", val, PRIVATE); // value sent to particle website then read by IFTTT website
        Particle.publish("IFTTT: ", String(ifttt), PRIVATE);
        Particle.publish("Sunny: ", String(sunny), PRIVATE); // value sent to particle website then read by IFTTT website
        
        Spark.variable("ifttt", ifttt);
        nowSunlight=sunlight; // changes the last result (the nowSunlight variable)
        Serial.println(" - Sent changes");
}