# DIGITAL CLOCK /|\  
*reation by Tony Huynh Tran*

---

## Hardware Requirements  
This digital clock is built around the **PIC16F887 microcontroller** and integrates various peripherals for a feature-rich experience. Below are the hardware components and their pin assignments:  

### Microcontroller  
- **PIC16F887**: Core processor running at 20 MHz with a high-speed (HS) crystal oscillator.  

### Display  
- **6 x 7-Segment LEDs**: Arranged in an 88:88:88 format with two fixed colons (always on).  
  - **PORTD**: Outputs segment codes (a-g, dp).  
  - **PORTC**: Scans the digits using pins **RC0, RC1, RC2, RC5, RC6, RC7**.  
  - *Note*: Pins **RC3 (SCL)** and **RC4 (SDA)** are reserved for I2C, though unused in this project.  

### Peripherals  
- **Temperature Sensor**: **LM35** connected to **AN7** (ADC channel 7) for temperature readings.  
- **Buzzer**: Connected to **E2** for alarm sound output.  
- **Lighting LED**: Connected to **C5**, toggled for illumination in Mode 0.  
- **Battery Monitoring**: Voltage monitored via **AN6** (ADC channel 6) for low battery detection.  

### Buttons  
- **MODE**: **RB0** – Cycles through modes (0 → 1 → 2 → 3 → 0)
  - `Cn`: Sunday  
  - `T2`: Monday  
  - `T3`: Tuesday  
  - `T4`: Wednesday  
  - `T5`: Thursday  
  - `T6`: Friday  
  - `T7`: Saturday  

### Mode 2: Temperature  
- **Display**: `N-:XX°C` (e.g., N-:27C for 27°C).  
- Temperature is measured using the LM35 sensor via ADC on **AN7**.  

### Mode 3: Set Alarm  
- **Display**: `T-:HH:MM` (e.g., T-:07:30 for 7:30 AM).  
- Edit with **UP** and confirm with **OK**.  
- Buzzer on **E2** triggers when time matches; press **SNOOZE/LED** to snooze for 10 minutes.  

### Mode 4: Set Time and Date  
- **Step 1**: Set time (`HH:MM:SS`).  
- **Step 2**: Set date (`DD:MM:YY`).  
- Edit with **UP** and confirm with **OK**.  
- Day of the week is auto-calculated after date setting.  

---

## Button Functions  
- **MODE**: Switches between modes in a loop (0 → 1 → 2 → 3 → 0)
- **OK**: Saves the current value and moves to the next editable field.  
- **UP**: Increments the selected value; wraps around to minimum if maximum is exceeded.  
- **SNOOZE/LED**:  
  - When alarm rings: Stops buzzer and snoozes for 10 minutes.  
  - In Mode 0: Toggles the LED on **C5**.  

---

## Display and Editing  
- **Normal Display**: Shows values clearly without blinking.  
- **Editing**: Selected digit blinks (on/off via `blink_state`), but the current value remains visible.  
- **7-Segment Control**:  
  - **PORTD**: Outputs segment codes (e.g., `0x3F` for 0, `0x06` for 1).  
  - **PORTC**: Scans digits sequentially (RC0–RC7), 1ms per digit.  

**Example Scanning Code**:  
```c
void quet_led(unsigned int8 d1, unsigned int8 d2, unsigned int8 d3, unsigned int8 d4, unsigned int8 d5, unsigned int8 d6) {
   output_d(~d1); output_high(PIN_B0); delay_us(280); output_low(PIN_B0);
   output_d(~d2); output_high(PIN_B1); delay_us(280); output_low(PIN_B1);
   output_d(~d3); output_high(PIN_B2); delay_us(280); output_low(PIN_B2);
   output_d(~d4); output_high(PIN_B3); delay_us(280); output_low(PIN_B3);
   output_d(~d5); output_high(PIN_B4); delay_us(280); output_low(PIN_B4);
   output_d(~d6); output_high(PIN_E0); delay_us(280); output_low(PIN_E0);
}
```

---

## Setting Time and Date  
1. Enter Mode 4 with **MODE**.  
2. Press **UP** to start editing.  
3. Adjust `HH:MM:SS`, then `DD:MM:YY` using **UP** and **OK**.  
4. After setting the year, the system validates the date and calculates the day of the week using Zeller’s Congruence.  

---

## Setting Alarm  
1. Enter Mode 3 with **MODE**.  
2. Press **UP** to edit.  
3. Adjust `HH:MM` using **UP** and **OK**.  
4. Alarm triggers buzzer when time matches; snooze with **SNOOZE/LED**.  

---

## Low Battery Mode  
- Monitors battery voltage on **AN6**.  
- Below 30%: Enters sleep mode, disables LEDs, and preserves Timer1 for timekeeping.  

---

## Installation and Usage  
1. **Assemble Hardware**: Connect components as per pin assignments.  
2. **Compile and Flash**: Use PIC C Compiler to compile the code and program the PIC16F887.  
3. **Power On**: Operate using the buttons as described.  

---

## Troubleshooting  
- **No Display**: Check PORTD and PORTC connections.  
- **Button Issues**: Verify debounce delay and pin assignments.  
- **Alarm Not Working**: Confirm buzzer on **E2** and alarm settings.  
- **Temperature Errors**: Ensure LM35 is on **AN7** and ADC is configured.  

---

## Creator’s Mark  
Crafted with precision and care by **Tony Huynh Tran** on August 6, 2025.  
A subtle signature woven into time: **/|\** – a symbol of balance and creativity.  

*“Time ticks forward, but its rhythm is ours to shape.”*
