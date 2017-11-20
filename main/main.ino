#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>

// Pins
#define PIN 3
#define HOUR_PIN 6
#define MINUTE_PIN 7

// Init the pixel strip
int pixels = 72;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixels, PIN, NEO_GRBW + NEO_KHZ800);

/*
 * A data structure to represent words.
 * It has a from and to index in the strip.
 */
struct singleWord {
  uint16_t from;
  uint16_t to;
};

/*
 * Defines the words by indexes on the strip.
 */
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

/*
 * Letter colors.
 */
uint32_t dayColor = strip.Color(255, 255, 255); //White
uint32_t nightColor = strip.Color(0, 0, 255); //Blue

/*
 * Delays and pauses for buttons
 */
unsigned long lastTimeAdjustment = 0;
int buttonDelay = 500;

/*
 * Delays and pauses for general execution
 */
unsigned long lastRun = 0;
unsigned long pauseDuration = 10000;

void setup() {
  // Set the time to midnight
  setTime(0,0,0,19,11,2017);

  // Init the number words
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

  // Start the strip
  strip.setBrightness(255);
  strip.begin();
  strip.show();

  // Init buttons
  pinMode(HOUR_PIN, INPUT);
  pinMode(MINUTE_PIN, INPUT);
}

void loop() {
  long currentMs = millis();

  // Adjust time if buttons are pressed
  if ((currentMs - lastTimeAdjustment) > buttonDelay) {
    int hourButtonPress = digitalRead(HOUR_PIN);
    int minuteButtonPress = digitalRead(MINUTE_PIN);
    
    if (hourButtonPress == HIGH) {
      //Serial.print("Hour button pressed ");
      //Serial.println(hourButtonPress);
      adjustTime(60 * 60);
      lastTimeAdjustment = currentMs;
      lastRun = currentMs; // Trigger a redraw
    } else if (minuteButtonPress == HIGH) {
      //Serial.print("Minute button pressed ");
      //Serial.println(minuteButtonPress);
      adjustTime(60 * 5);
      lastTimeAdjustment = currentMs;
      lastRun = currentMs; // Trigger a redraw
    }
  }

  // Don't redraw letters every single iteration,
  // return if it's less than 'pauseDuration' (10 seconds)
  // since last run
  if ((currentMs - lastRun) < pauseDuration) {
    return;
  }
  lastRun = currentMs;

  // Turn off all letters
  blank();

  int hours = hourFormat12();
  //Serial.print("hour time is ");
  //Serial.println(hours);

  int minutes = minute();
  singleWord hourWord = minutesToHours(hours, minutes);
  singleWord minuteWord = minutesToWords(minutes);
  singleWord positionWord = minutesToPosition(minutes);
  singleWord halfWord = halfOrWhole(minutes);

  //debugWords(hourWord, minuteWord, positionWord);
  
  uint32_t color = colorFromTime();

  displayWord(klokken_er, color);
  displayWord(hourWord, color);
  displayWord(minuteWord, color);
  displayWord(positionWord, color);
  displayWord(halfWord, color);

  // Show it all
  strip.show();
}

/*
 * Turn on pixels based on a word
 */
void displayWord(singleWord w, uint32_t color) {
  for (int i = w.from; i <= w.to; i++) {
    strip.setPixelColor(i, color);
  }
}

/*
 * Get hour word based on current hours and minutes.
 */
singleWord minutesToHours(int hours, int minutes) {
  if (minutes <= 17) {
    return numbers[hours-1];
  }
  if (hours == 12) {
    return numbers[0];
  }
  return numbers[hours];
}

/*
 * Get minute word based on current minutes
 */
singleWord minutesToWords(int minutes) {
  Serial.print("minutes ");
  if (minutes < 3) {
    Serial.println(minutes);
    return hel;
  } else if (minutes < 7) {
    Serial.println(minutes);
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
  Serial.print(" (fall through) ");
  Serial.println(minutes);
  return hel;
}

/*
 * Get "over"/"på" words based on minutes
 */
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

/*
 * Get "halv" or "hel" (nothing) from minutes
 */
singleWord halfOrWhole(int minutes) {
  if (17 < minutes && minutes <= 37) {
    return halv;
  }
  return hel;
}

/*
 * Get the color based on time of day.
 * 
 * This will return white from 06:00 to 18:00
 * and blue the rest of the time.
 */
uint32_t colorFromTime() {
  int h = hour();
  if (6 <= h && h < 18) {
    return dayColor;
  }
  return nightColor;
}

/*
 * Turn off all pixels.
 */
void blank() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  strip.show();
}

/*
 * Prints debugging info. Remember to enable Serial first.
 */
void debugWords(singleWord hourWord, singleWord minuteWord, singleWord positionWord) {
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
}

