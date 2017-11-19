#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>

#define PIN 3
#define HOUR_PIN 6
#define MINUTE_PIN 7

int pixels = 72;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixels, PIN, NEO_GRBW + NEO_KHZ800);

struct singleWord {
  uint16_t from;
  uint16_t to;
};

singleWord klokken_er = { 0, 8 };
singleWord hel = { 0, 0 };
singleWord fem = { 9, 11 };
singleWord ti = { 12, 13 };
singleWord paa_1 = { 14, 15 };
singleWord kvart = { 16, 20 };
singleWord paa_2 = { 21, 22 };
singleWord over = { 23, 26 };
singleWord halv = { 27, 30 };

singleWord numbers[12];

uint32_t dayColor = strip.Color(255, 255, 255);
uint32_t nightColor = strip.Color(0, 0, 255);

unsigned long lastTimeAdjustment = 0;
int buttonDelay = 500;

void setup() {
  setTime(0,0,0,19,11,2017);
  
  numbers[0] = { 31, 33 };
  numbers[1] = { 34, 35 };
  numbers[2] = { 36, 38 };
  numbers[3] = { 39, 42 };
  numbers[4] = { 43, 45 };
  numbers[5] = { 46, 49 };
  numbers[6] = { 50, 52 };
  numbers[7] = { 53, 56 };
  numbers[8] = { 57, 58 };
  numbers[9] = { 59, 60 };
  numbers[10] = { 61, 66 };
  numbers[11] = { 67, 71 };
  
  //Serial.begin(9600);
  // put your setup code here, to run once:
  strip.setBrightness(255);
  strip.begin();
  strip.show();

  pinMode(HOUR_PIN, INPUT);
  pinMode(MINUTE_PIN, INPUT);
}

void loop() {
  blank();
  long currentMs = millis();

  if ((currentMs - lastTimeAdjustment) > buttonDelay) {
    int hourButtonPress = digitalRead(HOUR_PIN);
    int minuteButtonPress = digitalRead(MINUTE_PIN);
    
    if (hourButtonPress == HIGH) {
      //Serial.print("Hour button pressed ");
      //Serial.println(hourButtonPress);
      adjustTime(60 * 60);
      lastTimeAdjustment = currentMs;
    } else if (minuteButtonPress == HIGH) {
      //Serial.print("Minute button pressed ");
      //Serial.println(minuteButtonPress);
      adjustTime(60 * 5);
      lastTimeAdjustment = currentMs;
    }
  }
  
  int hours = hourFormat12();
  //Serial.print("hour time is ");
  //Serial.println(hours);

  int minutes = minute();
  singleWord hourWord = minutesToHours(hours, minutes);
  singleWord minuteWord = minutesToWords(minutes);
  singleWord positionWord = minutesToPosition(minutes);
  singleWord halfWord = halfOrWhole(minutes);

  /*if (currentMs % 1000 == 0) {
    Serial.print(hours);
    Serial.print(" ");
    Serial.println(minutes);
  }*/
  
  /*
  Serial.print("hour: ");
  Serial.print(hourWord.from);
  Serial.print("-");
  Serial.println(hourWord.to);
  Serial.print("Minute: ");
  Serial.print(minuteWord.from);
  Serial.print("-");
  Serial.println(minuteWord.to);
  Serial.print("Pos: ");
  Serial.print(positionWord.from);
  Serial.print("-");
  Serial.println(positionWord.to);
*/
  uint32_t color = colorFromTime();

  displayWord(klokken_er, color);
  displayWord(hourWord, color);
  displayWord(minuteWord, color);
  displayWord(positionWord, color);
  displayWord(halfWord, color);
    
  strip.show();
}

void displayWord(singleWord w, uint32_t color) {
  for (int i = w.from; i <= w.to; i++) {
    strip.setPixelColor(i, color);
  }
}

singleWord minutesToHours(int hours, int minutes) {
  if (minutes <= 17) {
    return numbers[hours-1];
  }
  if (hours == 12) {
    return numbers[0];
  }
  return numbers[hours];
}

singleWord minutesToWords(int minutes) {
  if (minutes < 3) {
    return hel;
  } else if (minutes < 7) {
    return fem;
  } else if (minutes < 13) {
    return ti;
  } else if (minutes < 17) {
    return kvart;
  } else if (minutes < 23) {
    return ti;
  } else if (minutes < 27) {
    return fem;
  } else if (minutes < 33) {
    return halv;
  } else if (minutes < 37) {
    return fem;
  } else if (minutes < 43) {
    return ti;
  } else if (minutes < 47) {
    return kvart;
  } else if (minutes < 53) {
    return ti;
  } else if (minutes < 57) {
    return fem;
  }
  return hel;
}

singleWord minutesToPosition(int minutes) {
  if (3 < minutes && minutes <= 17) {
    return over;
  } else if (17 < minutes && minutes <= 27) {
    return paa_1;
  } else if (33 < minutes && minutes <= 43) {
    return over;
  } else if (43 < minutes && minutes <= 47) {
    return paa_2;
  } else if (47 < minutes && minutes <= 57) {
    return paa_1;
  }
  return hel;
}

singleWord halfOrWhole(int minutes) {
  if (17 < minutes && minutes <= 37) {
    return halv;
  }
  return hel;
}

uint32_t colorFromTime() {
  int h = hour();
  if (6 <= h && h < 18) {
    return dayColor;
  }
  return nightColor;
}

void blank() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  strip.show();
}

