#include <Arduino.h>
#include <vector>
#include <iostream>
#include <unordered_map>


std::unordered_map<std::string, char> morseToLetter = {
    {".-", 'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'}, {".", 'E'},
    {"..-.", 'F'}, {"--.", 'G'}, {"....", 'H'}, {"..", 'I'}, {".---", 'J'},
    {"-.-", 'K'}, {".-..", 'L'}, {"--", 'M'}, {"-.", 'N'}, {"---", 'O'},
    {".--.", 'P'}, {"--.-", 'Q'}, {".-.", 'R'}, {"...", 'S'}, {"-", 'T'},
    {"..-", 'U'}, {"...-", 'V'}, {".--", 'W'}, {"-..-", 'X'}, {"-.--", 'Y'},
    {"--..", 'Z'},
    // Add more mappings as needed
};

//use button presses to interpret morse code

#define BUTTON_PIN 21 // GIOP21 pin connected to button
#define LED 2 // LED on board

const int MIN = 200;
const int DOT = 1000; // 1000 milliseconds
const int DASH  = 2000; // 1000 milliseconds
const int NEXT_LETTER_TIME  = 4000; // 1000 milliseconds
const int ADD_SPACE_TIME  = 6000; // 1000 milliseconds

std::vector<char> characterList;

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
//char word[];

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // config GIOP21 as input pin and enable the internal pull-up resistor
  pinMode(LED, OUTPUT);

}

void clearList(std::vector<char>& charList) {
    charList.clear();
}

std::string morseToLetterConverter(const std::vector<char>& morseCode) {
    std::string sequence;
    std::string result;

    for (char dotDash : morseCode) {
        if (dotDash == ' ' && !sequence.empty()) {
            auto it = morseToLetter.find(sequence);
            if (it != morseToLetter.end()) {
                result += it->second; // Append the corresponding letter to the result
            } else {
                result += '?'; // If not found, use a placeholder like '?'
            }
            sequence.clear();
        } else if (dotDash != ' ') {
            sequence += dotDash;
        }
    }

    // Check for the last sequence if it's not followed by a space
    if (!sequence.empty()) {
        auto it = morseToLetter.find(sequence);
        if (it != morseToLetter.end()) {
            result += it->second; // Append the corresponding letter to the result
        } else {
            result += '?'; // If not found, use a placeholder like '?'
        }
    }

    return result;
}

void printChar(){

    std::string decoded = morseToLetterConverter(characterList);

    Serial.println(decoded.c_str());

    clearList(characterList);
}

void addToCharList(std::vector<char>& charList, char newChar) {
    charList.push_back(newChar);
}
char DeterminePress(){
    long pressDuration = releasedTime - pressedTime;

    if(pressDuration > 25 && pressDuration < MIN){
      printChar();
      return ' ';
    } else if(MIN < pressDuration &&  pressDuration < DOT ){
      addToCharList(characterList, '.');
      return '.';
    } else if(DOT < pressDuration && pressDuration < DASH ) {
      addToCharList(characterList, '-');
      return '-';
    } 

    return 'x';
}


void flashLed(){
    
  if(pressedTime < DOT){
       digitalWrite(2, HIGH); // turn the LED on
       delay(50);             // wait for 500 milliseconds
       digitalWrite(2, LOW);  // turn the LED off
       delay(50);  
    } else if(pressedTime < DASH){
      //slow flashes
      digitalWrite(2, HIGH); // turn the LED on
      delay(200);             // wait for 500 milliseconds
      digitalWrite(2, LOW);  // turn the LED off
      delay(200);  
    }
}

void loop() {
  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);

  if(lastState == HIGH && currentState == LOW){
      // button is pressed
    pressedTime = millis();
   //flashLed();
  }  
  else if(lastState == LOW && currentState == HIGH) { // button is released
    releasedTime = millis();

    char response = DeterminePress();

    if(response != 'x'){
        Serial.println(response);
    }

  } 


  // save the the last state
  lastState = currentState;
}










