/*
    Arduino AD0
         | 
VCC---2K---330---620---1K---3K3--
         |     |     |    |     |
       Right   Up  Down  Left Select
       (SW1) (SW2) (SW3) (SW4) (SW5)
         |     |     |    |     |
Gnd------------------------------
*/

//////////////////////////////////////////////////////////////////////////
//  multiple buttons on 1 analog pin
//////////////////////////////////////////////////////////////////////////
byte read_LCD_buttons() {              // read the buttons
  int adc_key_in = analogRead(0);       // read the value from the sensor

  //value read: 0(0V), 130(0.64V), 306(1.49V), 479(2.33V), 722(3.5V), 1023(4.97V)
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 75)   return btnRIGHT;
  if (adc_key_in < 218)  return btnUP;
  if (adc_key_in < 392)  return btnDOWN;
  if (adc_key_in < 600)  return btnLEFT;
  if (adc_key_in < 800)  return btnSELECT;
  return btnNONE;
}

//////////////////////////////////////////////////////////////////////////
// debounce a button
//////////////////////////////////////////////////////////////////////////
int counter = 0;       // how many times we have seen new value
long previous_time = 0;    // the last time the output pin was sampled
byte debounce_count = 10; // number of millis/samples to consider before declaring a debounced input
byte current_state = 0;   // the debounced input value

byte  key_press(){
  // If we have gone on to the next millisecond
  if(millis() != previous_time)
  {
    byte this_button = read_LCD_buttons();

    if(this_button == current_state && counter > 0) counter--;

    if(this_button != current_state) counter++; 

    // If the Input has shown the same value for long enough let's switch it
    if(counter >= debounce_count) {
      counter = 0;
      current_state = this_button;
      return this_button;
    }
    previous_time = millis();
  }
  return 0;
}

