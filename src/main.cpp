#include "MIDIUSB.h"

// Button inputs
static const uint8_t button_pin_1 = 6;
static const uint8_t button_pin_2 = 7;
static const uint8_t button_pin_3 = 8;
static const uint8_t button_pin_4 = 9;

// Midi channel of device
static const uint8_t midi_channel = 0;

// Button array
static const uint8_t buttons[] = {
  button_pin_1,
  button_pin_2,
  button_pin_3,
  button_pin_4
};

void setup() {
  Serial.begin(115200);
  // Todo: replace with loop
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);   
}

void PrintButtonStates()
{
  uint8_t number_of_buttons = sizeof(buttons)/sizeof(uint8_t);
  String output;
  for (uint8_t button_index = 0; button_index < number_of_buttons; ++button_index )
  {
    String value(digitalRead(buttons[button_index]));
    String button_number(button_index);
    output += ", Button[" + button_number + "]: " + value;
  }
  Serial.println(output);  
}

void MidiSendButtonState()
{
  bool flush_needed = false;
  static bool button_states_old[] = {
    false,
    false,
    false,
    false
  };

  static bool button_states_new[] = {
    false,
    false,
    false,
    false
  };

  // Set number of buttons
  uint8_t number_of_buttons = sizeof(buttons)/sizeof(uint8_t);

  // Read button states
  for (uint8_t button_index = 0; button_index < number_of_buttons; ++button_index )
  {
    button_states_new[button_index] = digitalRead(buttons[button_index]);
  }

  // Write button state to midi
  for (uint8_t button_index = 0; button_index < number_of_buttons; ++button_index )
  {
    if (button_states_old[button_index] != button_states_new[button_index])
    {
      // Active low
      if (button_states_new[button_index] == false)
      {
        // Send note on
        Serial.print("Button[");
        Serial.print(button_index);
        Serial.print("]: Note on");
        Serial.println();
        noteOn(midi_channel, button_index, 127);
        flush_needed = true;
      }
      // Active low
      if (button_states_new[button_index] == true)
      {
        // Send note off
        Serial.print("Button[");
        Serial.print(button_index);
        Serial.print("]: Note off");
        Serial.println();
        noteOff(midi_channel, button_index, 127);
        flush_needed = true;
      }
    }

    // Update old state
    button_states_old[button_index] = button_states_new[button_index];
    if (flush_needed)
    {
      MidiUSB.flush();
    }
  }
}

void loop() {
  //PrintButtonStates();
  MidiSendButtonState();
  // put your main code here, to run repeatedly:
  delay(5);
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}