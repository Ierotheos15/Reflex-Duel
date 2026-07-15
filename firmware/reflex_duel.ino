#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD: I2C address 0x27, 16 cols, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
#define BTN1      2   // Player 1 button
#define BTN2      3   // Player 2 button
#define P1_WIN    4   // Player 1 Blue LED (win)
#define P1_LOSE   5   // Player 1 Red LED (lose)
#define P2_WIN    6   // Player 2 Yellow LED (win)
#define P2_LOSE   7   // Player 2 Red LED (lose)
#define BUZZER    8   // Passive buzzer

// Game state
int score1 = 0;
int score2 = 0;
const int WIN_SCORE = 3;

// ---- Sound helpers ----
void beep(int freq, int dur) {
  tone(BUZZER, freq, dur);
  delay(dur + 20);
}

void soundCountdown() { beep(440, 150); }
void soundGo()        { beep(880, 300); }
void soundWin()       { beep(1046, 150); delay(50); beep(1318, 150); delay(50); beep(1568, 300); }
void soundFalse()     { beep(200, 500); }
void soundChamp()     { 
  beep(784, 100); beep(784, 100); beep(784, 100);
  beep(659, 400); beep(698, 100); beep(784, 400);
  beep(698, 100); beep(784, 200);
}

// ---- LED helpers ----
void clearLEDs() {
  digitalWrite(P1_WIN,  LOW);
  digitalWrite(P1_LOSE, LOW);
  digitalWrite(P2_WIN,  LOW);
  digitalWrite(P2_LOSE, LOW);
}

void showRoundResult(int winner) {
  clearLEDs();
  if (winner == 1) {
    digitalWrite(P1_WIN,  HIGH);
    digitalWrite(P2_LOSE, HIGH);
  } else {
    digitalWrite(P2_WIN,  HIGH);
    digitalWrite(P1_LOSE, HIGH);
  }
}

// ---- LCD helpers ----
void lcdClear() { lcd.clear(); }

void lcdPrint(const char* line1, const char* line2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(line1);
  lcd.setCursor(0, 1); lcd.print(line2);
}

void showScores() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("P1: ");  lcd.print(score1);
  lcd.setCursor(8, 0); lcd.print("P2: ");  lcd.print(score2);
  lcd.setCursor(0, 1); lcd.print("First to ");  lcd.print(WIN_SCORE); lcd.print(" wins!");
}

// ---- Wait for any button (returns 1 or 2) ----
int waitForAnyButton() {
  while (true) {
    if (digitalRead(BTN1) == LOW) { delay(50); return 1; }
    if (digitalRead(BTN2) == LOW) { delay(50); return 2; }
  }
}

// ---- Play one round, returns winner (1 or 2) or 0 for false start ----
int playRound() {
  lcdPrint("Ready...", "Don't press yet!");
  delay(1000);

  // Countdown 3-2-1
  for (int i = 3; i >= 1; i--) {
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print(i);
    soundCountdown();

    // Check for false start during countdown
    unsigned long t = millis();
    while (millis() - t < 800) {
      if (digitalRead(BTN1) == LOW) return -1; // P1 false start
      if (digitalRead(BTN2) == LOW) return -2; // P2 false start
    }
  }

  // Random delay 500ms - 3000ms before GO
  unsigned long waitTime = random(500, 3000);
  unsigned long start = millis();
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("........");

  while (millis() - start < waitTime) {
    if (digitalRead(BTN1) == LOW) return -1;
    if (digitalRead(BTN2) == LOW) return -2;
  }

  // GO!
  lcdPrint("     GO!!", "  Press now!!");
  soundGo();

  // Wait for button press
  unsigned long goTime = millis();
  while (millis() - goTime < 5000) {
    bool b1 = digitalRead(BTN1) == LOW;
    bool b2 = digitalRead(BTN2) == LOW;
    if (b1 && b2) {
      // Simultaneous — too close to call, redo
      return 0;
    }
    if (b1) {
      unsigned long rt = millis() - goTime;
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("P1 wins round!");
      lcd.setCursor(0, 1); lcd.print(rt); lcd.print("ms");
      return 1;
    }
    if (b2) {
      unsigned long rt = millis() - goTime;
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("P2 wins round!");
      lcd.setCursor(0, 1); lcd.print(rt); lcd.print("ms");
      return 2;
    }
  }
  // Timeout — no one pressed
  return 0;
}

// ---- Setup ----
void setup() {
  pinMode(BTN1,   INPUT_PULLUP);
  pinMode(BTN2,   INPUT_PULLUP);
  pinMode(P1_WIN,  OUTPUT);
  pinMode(P1_LOSE, OUTPUT);
  pinMode(P2_WIN,  OUTPUT);
  pinMode(P2_LOSE, OUTPUT);
  pinMode(BUZZER,  OUTPUT);

  clearLEDs();
  randomSeed(analogRead(0));

  lcd.init();
  lcd.backlight();
  lcdPrint(" Reflex Duel!", "Press any btn");
  waitForAnyButton();
  delay(300);
}

// ---- Main loop ----
void loop() {
  score1 = 0;
  score2 = 0;
  clearLEDs();
  showScores();
  delay(1500);

  while (score1 < WIN_SCORE && score2 < WIN_SCORE) {
    delay(500);
    int result = playRound();

    if (result == -1) {
      // P1 false start
      soundFalse();
      lcdPrint("P1 FALSE START!", "P2 gets point");
      showRoundResult(2);
      score2++;
      delay(2000);
    } else if (result == -2) {
      // P2 false start
      soundFalse();
      lcdPrint("P2 FALSE START!", "P1 gets point");
      showRoundResult(1);
      score1++;
      delay(2000);
    } else if (result == 1) {
      soundWin();
      showRoundResult(1);
      score1++;
      delay(2000);
    } else if (result == 2) {
      soundWin();
      showRoundResult(2);
      score2++;
      delay(2000);
    } else {
      // Tie / timeout
      lcdPrint("  Too close!", " Redo round");
      delay(1500);
      continue;
    }

    showScores();
    delay(1500);
  }

  // Game over
  clearLEDs();
  if (score1 >= WIN_SCORE) {
    lcdPrint(" Player 1 WINS!", "   Champion!");
    digitalWrite(P1_WIN, HIGH);
  } else {
    lcdPrint(" Player 2 WINS!", "   Champion!");
    digitalWrite(P2_WIN, HIGH);
  }
  soundChamp();
  delay(4000);

  lcdPrint("Play again?", "Press any btn");
  waitForAnyButton();
  delay(300);
}
