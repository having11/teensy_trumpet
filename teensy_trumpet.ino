#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 1
#define LED_NUM 30
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_NUM, PIN, NEO_GRB + NEO_KHZ800);
// GUItool: begin automatically generated code
AudioInputAnalog         adc1(A9);           //xy=199.01666259765625,187.01666259765625
AudioMixer4              mixer1;         //xy=420.01666259765625,193.01666259765625
AudioAnalyzePeak         peak1;          //xy=626.0166625976562,219.01666259765625
AudioAnalyzeNoteFrequency notefreq1;      //xy=627.0166625976562,160.01666259765625
AudioConnection          patchCord1(adc1, 0, mixer1, 0);
//AudioConnection          patchCord2(mixer1, peak1);
AudioConnection          patchCord3(mixer1, notefreq1);
AudioConnection patchCord4(adc1,peak1);
// GUItool: end automatically generated code
void setup(){
  AudioMemory(30);
  notefreq1.begin(.15);
  Serial.begin(115200);
  pixels.begin();
  pixels.setBrightness(10);
  for(int i=0;i<LED_NUM;i++){
    pixels.setPixelColor(i,pixels.Color(150,150,150));
  }
  pixels.show();
}

//int monoPeak=0.0;

elapsedMillis quietTime;

void loop(){
  float monoPeak;
  if(peak1.available()){
     monoPeak = peak1.read() * 30.0;
   }
  if(notefreq1.available()){
    quietTime = 0;
    float note = notefreq1.read();
    float prob = notefreq1.probability();
    if(note>1500||note<60){
      note = 0;
    }
    if(note==0.0){} //Do nothing
    else if(note >=800.0){
      for(int i=0;i<monoPeak*1.8;i++){
        pixels.setPixelColor(i,pixels.Color(240,0,0));
      }
      for(int i=monoPeak*1.8;i<LED_NUM;i++){
        pixels.setPixelColor(i,pixels.Color(0,0,0));
      }
      delay(500);
    }
    else{
      for(int i=0;i<LED_NUM;i++){
        pixels.setPixelColor(i,Wheel(map(note,60,800,0,255)));
      }
    }
    if(monoPeak>3){
      pixels.setBrightness(10+monoPeak*1.8);
    }
    pixels.show();
    Serial.printf("Note: %3.2f | Probability: %.2f | Peak: %1.3f\n",note,prob,monoPeak);
  }
  else{
    if(quietTime > 750){
      for(int i=0;i<LED_NUM;i++){
        pixels.setPixelColor(i,pixels.Color(0,0,0));
      }
      pixels.show();
    }
  }
}

uint32_t Wheel(byte wheelPos){
  wheelPos = 255 - wheelPos;
  if(wheelPos<125){
    return pixels.Color(255-wheelPos*2,wheelPos*2,0);
  }
  wheelPos -=125;
  return pixels.Color(0,255-wheelPos*2,wheelPos*2);
}
