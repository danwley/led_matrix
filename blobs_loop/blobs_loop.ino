#include <FastLED.h>
#define WIDTH 32
#define HEIGHT 8
#define NUM_LEDS WIDTH*HEIGHT
#define DATA_PIN 4
#define FPS 100
CRGB leds[NUM_LEDS];
CRGB matrix[HEIGHT][WIDTH];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  for (int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(0, 0, 0);
  }
  FastLED.show();
  delay(2000);
}

float get_magnitude(float x, float y) {
  return pow(pow(x, 2)+pow(y, 2), 0.5);
}

CRGB* blend_col(int c[21]) {
  const int steps = 166;
  static CRGB blended[6*steps];

  for (int change=0; change<6; change++) {
    int rc = c[change*3 + 0] - c[(change+1)*3 + 0];
    int gc = c[change*3 + 1] - c[(change+1)*3 + 1];
    int bc = c[change*3 + 2] - c[(change+1)*3 + 2];

    int ri = rc/steps;
    int gi = gc/steps;
    int bi = bc/steps;

    for (int inc=0; inc<steps; inc++) {
      int r = c[change*3 + 0] - inc * ri;
      int g = c[change*3 + 1] - inc * gi;
      int b = c[change*3 + 2] - inc * bi;
      blended[change*steps+inc] = CRGB(r/10, g/4, b/4);
    }
  }
  return blended;
}

int c[21] = {255, 0, 0, 255, 255, 0, 0, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 255, 255, 0, 0};
CRGB *hsv = blend_col(c);

void blobs_loop1() {
  int num_balls = 3;
  float b_pos[num_balls][2];
  float b_vel[num_balls][2];
  int speed_balls = 1;

  for (int i=0; i<num_balls; i++) {
    b_pos[i][0] = (float)random(0, WIDTH);
    b_pos[i][1] = (float)random(0, HEIGHT);
    b_vel[i][0] = (float)random(0, 100)/100;
    b_vel[i][1] = ((float)random(0, 100))/100;
  }

  for (auto& vel : b_vel) {
    float mag = get_magnitude(vel[0], vel[1]);
    float mult = speed_balls/mag;
    vel[0] *= mult;
    vel[1] *= mult;
  }

  while (true) {
    Serial.println("START");
    
    for (int ball=0; ball<num_balls; ball++) {
      b_pos[ball][0] = b_pos[ball][0] + b_vel[ball][0];
      b_pos[ball][1] = b_pos[ball][1] + b_vel[ball][1];
  
      if (0 >= b_pos[ball][0] || b_pos[ball][0] > WIDTH) {
        b_vel[ball][0] *= -1;
        
        float mag = get_magnitude(b_vel[ball][0], b_vel[ball][1]);
        float mult = speed_balls/mag;
        b_vel[ball][0] *= mult;
        b_vel[ball][1] *= mult;
        
        b_pos[ball][0] += b_vel[ball][0];
      }
      
      if (0 >= b_pos[ball][1] || b_pos[ball][1] > HEIGHT) {
        b_vel[ball][1] *= -1;
        float mag = get_magnitude(b_vel[ball][0], b_vel[ball][1]);
        float mult = speed_balls/mag;
  
        b_vel[ball][0] *= mult;
        b_vel[ball][1] *= mult;
  
        b_pos[ball][1] += b_vel[ball][1];
      }
    }
  
    for (int j=0; j<HEIGHT; j++) {
      for (int i=0; i<WIDTH; i++) {
        int min_dist = WIDTH;
        for (int ball=0; ball<num_balls; ball++) {
          float dist = get_magnitude(b_pos[ball][0]-i, b_pos[ball][1]-j);
          if (min_dist > dist) {
            min_dist = dist;
          }
        }
        int col_index = min_dist*20;
        col_index = min(995 , col_index);
        matrix[j][i] = hsv[col_index];
//        Serial.println(matrix[j][i].red);
      }
    }
    Serial.println(b_pos[0][0]);
    Serial.println("END");
  
    show_pixels();
  }
}

void show_pixels() {
//  for (int i = 0; i < NUM_LEDS; i++) {
//    leds[i].setRGB(0,0,0);
//    delay(1);
//    Serial.println("STOP");
//    FastLED.show();
//  }
  Serial.println("SHOW");
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
  Serial.println(matrix[2][2].red);
}

void loop() {
  Serial.println("loop");
  blobs_loop1();
}
