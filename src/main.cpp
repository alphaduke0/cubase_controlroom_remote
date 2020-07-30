#include "MIDIUSB.h"
#include <TM1637Display.h>
#include <SimpleRotary.h>

// Button inputs
static const uint8_t button_pin_1 = 6;
static const uint8_t button_pin_2 = 7;
static const uint8_t button_pin_3 = 8;
static const uint8_t button_pin_4 = 9;

// Encoder
static const uint8_t encoder_A = 14;
static const uint8_t encoder_B = 15;
static const uint8_t encoder_C = 13;

static const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};


// Values
static const uint8_t volume_note = 4;
int volume = 0;
int volume_old = 0;
unsigned int volume_steps = 9;
bool dim_state = false;

static const uint8_t display_clk = 5;
static const uint8_t display_dio = 4;
TM1637Display display(display_clk, display_dio);


// Midi channel of device
static const uint8_t midi_channel = 0;

// Button array
static const uint8_t buttons[] = {
  button_pin_1,
  button_pin_2,
  button_pin_3,
  button_pin_4
};

// Rotatry encoder
SimpleRotary encoder(encoder_A, encoder_B, encoder_C);



void setup() {
  Serial.begin(115200);
  // Todo: replace with loop
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);

  pinMode(encoder_A, INPUT);
  pinMode(encoder_B, INPUT);

  // Display init
  display.setBrightness(0x0f);

  // debounce rotary
  encoder.setDebounceDelay(3);
}

void PrintEncoderStates()
{
  String encoder_value_a(digitalRead(encoder_A));
  String encoder_value_b(digitalRead(encoder_B));
  String encoder_value_c(digitalRead(encoder_C));
  String encoder_state = "A:" + encoder_value_a + "B:" + encoder_value_b + "C:" + encoder_value_c;
  Serial.println(encoder_state);
}

int ReadEncoderValue()
{
  static bool old_A = false;
  static bool old_B = false;
  bool new_A = digitalRead(encoder_A);
  bool new_B = digitalRead(encoder_B);

  int direction = 0;

  // Check if encoder values changed

  // Read direction
  if (new_A == true && new_B == false)
  {
    direction = 1;
    // Set new as old values
    old_A = new_A;
    old_B = new_B;
  }
  if (new_A == false && new_B == true)
  {
    direction = -1;
    // Set new as old values
    old_A = new_A;
    old_B = new_B;
  }

  return direction;
}

void PrintButtonStates()
{
  uint8_t number_of_buttons = sizeof(buttons) / sizeof(uint8_t);
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
  uint8_t number_of_buttons = sizeof(buttons) / sizeof(uint8_t);

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

void UpdateVolume()
{
  int direction = encoder.rotate();
  if (direction == 1)
  {
    //CW
    volume -= volume_steps;
  }
  if (direction == 2)
  {
    volume += volume_steps;
  }
  if ( volume != volume_old )
  {
    if (volume >= 127)
    {
      volume = 127;
    }

    if (volume < 0)
    {
      volume = 0;
    }
    volume_old = volume;
    noteOn(midi_channel, volume_note, volume);
    delay(10);
    noteOff(midi_channel, volume_note, volume);

    MidiUSB.flush();
    display.showNumberDec(volume, false);
  }
}

void ReadMidi(void)
{
  midiEventPacket_t midiPacket = MidiUSB.read();
  if (midiPacket.header != 0) {
    Serial.print("Received: ");
    Serial.print((uint16_t)midiPacket.header);
    Serial.print("-");
    Serial.print((uint16_t)midiPacket.byte1);
    Serial.print("-");
    Serial.print((uint16_t)midiPacket.byte2);
    Serial.print("-");
    Serial.println((uint16_t)midiPacket.byte3);
  }

  // Volume update
  if ((uint16_t)midiPacket.header == 8
      && (uint16_t)midiPacket.byte1 == 128
      && (uint16_t)midiPacket.byte2 == 4)
  {
    volume = midiPacket.byte3;
    volume_old = volume;
    display.showNumberDec(volume,false);
  }

  // Monitor A/B
  if ((uint16_t)midiPacket.header == 8
      && (uint16_t)midiPacket.byte1 == 128
      && (uint16_t)midiPacket.byte2 == 2
      && (uint16_t)midiPacket.byte3 == 0)
  {
    // Monitor B
    Serial.println("Monitor B");
    display.showNumberHexEx(0xB, 0, false);
  }

  if ((uint16_t)midiPacket.header == 8
      && (uint16_t)midiPacket.byte1 == 128
      && (uint16_t)midiPacket.byte2 == 3
      && (uint16_t)midiPacket.byte3 == 0)
  {
    // Monitor A
    Serial.println("Monitor A");
    display.showNumberHexEx(0xA, 0, false);
  }

  // Talkback off
  if ((uint16_t)midiPacket.header == 8
      && (uint16_t)midiPacket.byte1 == 128
      && (uint16_t)midiPacket.byte2 == 0
      && (uint16_t)midiPacket.byte3 == 0)
  {
    Serial.println("Talkback off");
  }

  // Talkback on
  if ((uint16_t)midiPacket.header == 8
      && (uint16_t)midiPacket.byte1 == 128
      && (uint16_t)midiPacket.byte2 == 0
      && (uint16_t)midiPacket.byte3 == 127)
  {
    Serial.println("Talkback on");
  }

  // DIM ON
  if ((uint16_t)midiPacket.header == 8
      && (uint16_t)midiPacket.byte1 == 128
      && (uint16_t)midiPacket.byte2 == 1
      && (uint16_t)midiPacket.byte3 == 127)
  {
    Serial.println("DIM on");
    display.showNumberHexEx(0xD, 0, false);
    dim_state = true;
  }
  // DIM OFF
  if ((uint16_t)midiPacket.header == 8
      && (uint16_t)midiPacket.byte1 == 128
      && (uint16_t)midiPacket.byte2 == 1
      && (uint16_t)midiPacket.byte3 == 0)
  {
    Serial.println("Dim off");
    display.showNumberHexEx(0xD, 0, false);
    dim_state = false;
  }
}

void loop() {
  // PrintButtonStates();
  ReadMidi();
  MidiSendButtonState();
  UpdateVolume();
  //PrintEncoderStates();

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