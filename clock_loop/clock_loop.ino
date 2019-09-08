// Potentially update every second instead (microperformance)
// Thoughts are that this is far from optimised
#include <FastLED.h>
#include <time.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const char* ssid     = "27 Corner Hall_Upstairs";
const char* password = "Password@1";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String formattedDate;
#define WIDTH 32
#define HEIGHT 8
#define NUM_LEDS WIDTH*HEIGHT
#define DATA_PIN 4
#define FPS 100
CRGB leds[NUM_LEDS];
CRGB matrix[HEIGHT][WIDTH];
bool number_array[13][15] = {
{1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1},  // 0
{0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1},  // 1
{1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1},  // 2
{1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1},  // 3
{1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1},  // 4
{1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1},  // 5
{1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1},  // 6
{1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},  // 7
{1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},  // 8
{1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1},  // 9
{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},  // : (10)
{1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1},  // % (11)
{0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0},  // + (12)
};

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB(0,0,0);
  }
  FastLED.show();
  delay(2000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
}

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600x
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);
}

void place_3x5_num(int num, int row, int col) {
  for (int p=0; p<5; p++) {
    for (int q=0; q<3; q++) {
      if (number_array[num][3*p+q]) {
        matrix[row+p][col+q] = CRGB(100, 0, 100);
      }
      else{
        matrix[row+p][col+q] = CRGB(0, 0, 0);
      }
    }
  }
}

void show_pixels() {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (i%2) {
        leds[WIDTH*i+WIDTH-1-j] = matrix[i][j];
      }
      else {
        leds[WIDTH*i+ j] = matrix[i][j];
      }
    }
  }
  FastLED.show();
  FastLED.delay(1000/FPS);
  Serial.println(matrix[1][2].red);
}

void clock_loop1() {

  place_3x5_num(formattedDate.substring(0,1).toInt(), 1, 2);
  place_3x5_num(formattedDate.substring(1,2).toInt(), 1, 6);
  place_3x5_num(formattedDate.substring(3,4).toInt(), 1, 12);
  place_3x5_num(formattedDate.substring(4,5).toInt(), 1, 16);
  place_3x5_num(formattedDate.substring(6,7).toInt(), 1, 22);
  place_3x5_num(formattedDate.substring(7,8).toInt(), 1, 26);
  Serial.println(formattedDate.substring(0,2));

  matrix[2][10] = CRGB(0, 0, 30);
  matrix[4][10] = CRGB(0, 0, 30);
  matrix[2][20] = CRGB(0, 0, 30);
  matrix[4][20] = CRGB(0, 0, 30);
  
  show_pixels();
}

void loop() {

//  while(!timeClient.update()) {
//    timeClient.forceUpdate();
//  }
  formattedDate = timeClient.getFormattedTime();
  clock_loop1();
  delay(50);
  Serial.println("loop");
  
}
