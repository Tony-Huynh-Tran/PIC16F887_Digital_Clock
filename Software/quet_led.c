
// HAM QUET LED
void quet_led(unsigned int8 d1, unsigned int8 d2, unsigned int8 d3, unsigned int8 d4, unsigned int8 d5, unsigned int8 d6)
{
    output_d(~d1);
    output_high(pin_b0);
    delay_us(280);
    output_low(pin_b0);
    
    output_d(~d2);        // B?t DP cho digit 2
    output_high(pin_b1);
    delay_us(280);
    output_low(pin_b1);
    
    output_d(~0x80);      // B?t DP cho digit 2
    output_high(pin_b1);
    delay_us(280);
    output_low(pin_b1);
    
    output_d(~d3);
    output_high(pin_b2);
    delay_us(280);
    output_low(pin_b2);
    
    output_d(~d4);        // B?t DP cho digit 4
    output_high(pin_b3);
    delay_us(280);
    output_low(pin_b3);
    
    output_d(~0x80);      // B?t DP cho digit 4
    output_high(pin_b3);
    delay_us(280);
    output_low(pin_b3);
    
    output_d(~d5);
    output_high(pin_b4);
    delay_us(280);
    output_low(pin_b4);
    
    output_d(~d6);
    output_high(pin_e0);
    delay_us(280);
    output_low(pin_e0);
}
