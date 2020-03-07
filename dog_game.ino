#include <LiquidCrystal.h>
#include "characters.h"
#define scrollSpeed 850
#define upperRand 800
#define lowerRand 400

LiquidCrystal lcd(1, 2, 4, 5, 6, 7); // RS, E, D4, D5, D6, D7

unsigned long timeDog, timeAddFence, timeAddbird, timeScroll;
unsigned int fr_range, fr_speed, dogPosition, yesButton, noButton, jumpButton, landButton, addTime, score, aux, delayTime, life, wait;
bool finish, ok, toggle;

///////////////////////// Queue functions:
typedef struct queue_typedef
{
  int range;
  struct queue_typedef *next;
} Queue;

Queue * f , * l;

void deleteQ (Queue ** first)
{
  lcd.setCursor(0, 1);
  lcd.print(" ");
  score += 1;
  Queue* temp = *first;
  *first = temp->next;
  free(temp);
}

void insertQ (Queue** first, Queue** last)
{
  Queue* node = (Queue*)malloc(sizeof(Queue));
  node->range = 15;
  node->next = NULL;
  if (*first == NULL) //first node -> create new
  {
    *first = node;
    *last = node;
    return;
  }
  Queue* temp = *last;
  temp->next = node;
  *last = node;
}
///////////////////////// <end> Queue functions

void displayDog() {
  if (millis() - timeDog >= 250)
  {
    lcd.setCursor(0, dogPosition);
    lcd.write((byte)1);
  }
  if (millis() - timeDog >= 250)
  {
    timeDog += 500;
    lcd.setCursor(0, dogPosition);
    lcd.write((byte)0);
  }
};

void displayFence(Queue** first) {
  Queue* current = *first;
  if (current == NULL) return;
  if (millis() - timeScroll >= scrollSpeed)
  {
    while (current != NULL)
    {
      lcd.setCursor(current->range, 1);
      lcd.print(" ");
      current->range  -= 1;
      lcd.setCursor(current->range, 1);
      lcd.write((byte)2);
      current = current->next;
    }
    timeScroll += scrollSpeed;
    if (f->range == 0 && dogPosition == 1)
    {
      life -= 1;
      if (life == 0)
      {
        finish = true;
        gameOver();
      }
    }
  }
  if ((*first)->range < 0) deleteQ(first);
};

void displayBird() {
  if (millis() - timeAddbird >= fr_speed)
  {
    aux = fr_speed;
    lcd.setCursor(fr_range, 0);
    lcd.print(" ");
    if (fr_range == 0)
    {
      if (dogPosition == 0)
      {
        life -= 1;
        toggle = !toggle;
        if (life == 0)
        {
          finish = true;
          gameOver();
        }
      }
      fr_range = 14;
      score += 1;
      fr_speed = (rand() % (upperRand - lowerRand + 1)) + lowerRand;
      toggle = !toggle;
      lcd.setCursor(14, 0); lcd.print("  ");
    }
    fr_range  -= 1;
    lcd.setCursor(fr_range, 0);
    lcd.write((byte)3);
    timeAddbird += aux;
  }
}

void displayHearts() {
  lcd.setCursor(14, 0);
  switch (life) {
    case 2: lcd.write((byte)5); lcd.write((byte)5); break;
    case 1: lcd.write((byte)4); lcd.write((byte)5); break;
    case 0: lcd.write((byte)4); lcd.write((byte)4); break;
  }
}

void gameOver()
{
  lcd.clear(); lcd.setCursor(0, 1);  lcd.write((byte)0);
  lcd.setCursor(4, 0); lcd.print("Score: "); lcd.print(score);
  lcd.setCursor(4, 1); lcd.print("Play Again?");
  displayHearts();
}

void setup() {
  lcd.clear(); lcd.begin(16, 2);
  // Init LCD chars:
  lcd.createChar(0, dog);   lcd.createChar(1, dog_move);
  lcd.createChar(2, fence); lcd.createChar(3, bird);
  lcd.createChar(4, emptyHeart); lcd.createChar(5, fullHeart);
  lcd.home(); lcd.setCursor(0, 1);  lcd.write((byte)0);
  lcd.setCursor(14, 0); lcd.print(score);

  fr_range = 13; dogPosition = 1; addTime = 0; score = 0; life = 2;
  fr_speed = (rand() % (upperRand - lowerRand + 1)) + lowerRand;
  delayTime = (rand() % (3200 - 600 + 1)) + 600;
  // Init Queue:
  f = NULL; l = NULL;

  // Timers:
  timeDog = millis();
  timeAddFence = millis();
  timeAddbird = millis();
  timeScroll = millis();

  //Booleans:
  finish = false; ok = true; toggle = true;

  //Buttons:
  pinMode(8, INPUT);  //  JUMP
  pinMode(9, INPUT);  //  LAND
  pinMode(10, INPUT); //  YES
  pinMode(11, INPUT); //  NO
}

void loop() {
  if (!finish) {
    //Game on:

    if (toggle) {
      lcd.setCursor(14, 0);
      lcd.print(score);
    } else {
      displayHearts();
    }
    jumpButton = digitalRead(8);
    landButton = digitalRead(9);
    if (landButton == HIGH)
    {
      dogPosition = 1;
      lcd.setCursor(0, 1);
      lcd.write((byte)0);
      lcd.setCursor(0, 0);
      lcd.print(" ");
    }
    if (jumpButton == HIGH)
    {
      dogPosition = 0;
      lcd.setCursor(0, 0);
      lcd.write((byte)0);
      lcd.setCursor(0, 1);
      lcd.print(" ");
    }
    displayDog();
    displayFence(&f);
    displayBird();
    if (millis() - timeAddFence >= delayTime) {
      insertQ(&f, &l);
      timeAddFence += delayTime;
      delayTime = (rand() % (3200 - 1200 + 1)) + 1200;
    }

  } else {
    // Game off:
    if (ok) {
      yesButton = digitalRead(10);
      noButton  = digitalRead(11);
      if (yesButton == HIGH) {
        finish = false;
        setup();
      }
      if (noButton == HIGH) {
        lcd.clear();
        lcd.setCursor(1, 0); lcd.print("G A M E");
        lcd.setCursor(7, 1); lcd.print("O V E R");
        ok = false;
      }
    }
  }
};
