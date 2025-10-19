int8 phim = 0;
int1 debounce_in_progress = 0;
int8 debounce_timer = 0;

// Th�m bi?n cho gi? n�t UP
int1 up_pressed = 0;
int16 up_repeat_timer = 0;  // �?i th�nh int16 d? d? ch?a 1200
int1 up_auto_repeat = 0;    // C? b�o d� v�o ch? d? auto-repeat

// ===== BI?N �I?U KHI?N N�T SNOOZE =====
int1 snooze_pressed = 0;                    
int16 snooze_hold_timer = 0;                
int1 snooze_action_taken = 0;               
int1 snooze_short_press_detected = 0;       

// Bi?n extern d? main.c c� th? truy c?p
extern int1 co_bao_thuc;
extern unsigned int8 gio_bao_thuc;
extern unsigned int8 phut_bao_thuc;

#int_timer0
void ngat_timer0() {
   set_timer0(256 - 250);   // 1ms
   
   // N?u dang debounce
   if(debounce_in_progress) {
      debounce_timer++;
      if(debounce_timer >= 20) { // ~20ms debounce
         debounce_in_progress = 0;
         debounce_timer = 0;
         
         if(!input(PIN_B5)) phim = 1;        // MENU
         else if(!input(PIN_B6)) {           // UP
            phim = 2;
            up_pressed = 1;        // ��nh d?u dang gi? UP
            up_repeat_timer = 0;   // reset b? d?m gi?
            up_auto_repeat = 0;    // chua v�o ch? d? auto
         }
         else if(!input(PIN_B7)) {           // SNOOZE
            if(co_bao_thuc == 1) {
               // Khi dang b�o th?c, ch? x? l� SNOOZE
               snooze_pressed = 1;
               snooze_hold_timer = 0;
               snooze_action_taken = 0;
               snooze_short_press_detected = 0;
               phim = 0;  // Kh�ng set phim = 3 khi dang b�o th?c
            } else {
               // Khi kh�ng b�o th?c, SNOOZE ho?t d?ng b�nh thu?ng (toggle LED)
               phim = 3;
            }
         }
         else phim = 0;
      }
   }
   
   // X? l� gi? n�t UP
   if(up_pressed) {
      if(input(PIN_B6)) {
         // N?u d� th? n�t th� reset t?t c?
         up_pressed = 0;
         up_repeat_timer = 0;
         up_auto_repeat = 0;
      } else {
         // V?n dang gi? n�t
         up_repeat_timer++;
         
         if(!up_auto_repeat) {
            // Chua v�o ch? d? auto, d?i 1200ms
            if(up_repeat_timer >= 1200) {
               up_auto_repeat = 1;     // B?t ch? d? auto-repeat
               up_repeat_timer = 0;    // Reset timer cho chu k? auto
               phim = 2;               // T?o s? ki?n nh?n d?u ti�n
            }
         } else {
            // �� trong ch? d? auto-repeat, m?i 150ms t?o 1 s? ki?n
            if(up_repeat_timer >= 200) {
               phim = 2;               // T?o s? ki?n nh?n
               up_repeat_timer = 0;    // Reset cho chu k? ti?p theo
            }
         }
      }
   }
   
   // ===== X? L� N�T SNOOZE KHI �ANG B�O TH?C =====
   if(snooze_pressed && co_bao_thuc == 1) {
      if(!input(PIN_B7)) {
         // V?n dang nh?n gi? SNOOZE
         snooze_hold_timer++;
         
         // Nh?n gi? > 1000ms (1 gi�y) = T?T HO�N TO�N
         if(snooze_hold_timer >= 1000 && !snooze_action_taken) {
            bao_thuc(0); // T?T b�o th?c
            co_bao_thuc = 0;
            // X�A GI? B�O TH?C (t?t lu�n)
            gio_bao_thuc = 0;
            phut_bao_thuc = 0;
            snooze_action_taken = 1;  // ��nh d?u d� th?c hi?n h�nh d?ng
         }
      } else {
         // �� th? n�t SNOOZE
         if(!snooze_action_taken) {
            // N?u nh?n ng?n (< 1 gi�y) = SNOOZE 10 PH�T
            if(snooze_hold_timer < 1000) {
               bao_thuc(0); // T?T b�o th?c t?m th?i
               
               // Snooze 10 ph�t
               phut_bao_thuc += 10;
               if(phut_bao_thuc > 59) {
                  phut_bao_thuc -= 60;
                  gio_bao_thuc++;
                  if(gio_bao_thuc > 23) {
                     gio_bao_thuc = 0;
                  }
               }
               co_bao_thuc = 0;
            }
         }
         // Reset t?t c? c? SNOOZE
         snooze_pressed = 0;
         snooze_hold_timer = 0;
         snooze_action_taken = 0;
         snooze_short_press_detected = 0;
      }
   }
}

#int_rb
void ngat_portb() {
   static int8 old_state;
   int8 current_state = input_b();
   int8 changed = current_state ^ old_state;
   old_state = current_state;
   
   if((bit_test(changed,5) && !bit_test(current_state,5)) ||
      (bit_test(changed,6) && !bit_test(current_state,6)) ||
      (bit_test(changed,7) && !bit_test(current_state,7))) {
      
      debounce_in_progress = 1;
      debounce_timer = 0;
   }
}
