// BI?N �I?U KHI?N BEEP
static unsigned int8 dang_phat_nhac = 0;          // 0: kh�ng ph�t, 1: �ang ph�t
static unsigned int16 beep_counter = 0;           // �?m th?i gian beep
static unsigned int8 beep_state = 0;              // 0: t?t, 1: b?t
static unsigned int16 total_time = 0;             // T?ng th?i gian �? beep (gi�y)

// C?U H?NH BEEP
#define BEEP_FREQUENCY  2000    // T?n s? beep (Hz)
#define BEEP_ON_TIME    150     // Th?i gian b?t beep (ms) 
#define BEEP_OFF_TIME   50     // Th?i gian t?t beep (ms)
#define PAUSE_TIME      800    // Th?i gian ngh? gi?a c�c chu?i beep (ms)
#define BEEPS_PER_SET   5       // S? beep trong 1 chu?i (t�t t�t t�t t�t t�t)
#define TOTAL_ALARM_TIME 60     // T?ng th?i gian b�o th?c (gi�y)

static unsigned int8 beep_count = 0;              // �?m s? beep trong 1 chu?i
static unsigned int8 is_pausing = 0;              // �ang trong th?i gian ngh?

// ===== H�M C�I �?T PWM CHO T?N S? =====
void cai_dat_tan_so_pwm(unsigned int tan_so) {
    if(tan_so == 0) {
        // T?t �m thanh
        set_pwm1_duty(0);
    } else {
        // T�nh PR2 cho t?n s? mong mu?n
        unsigned int16 pr2_val = (20000000UL / (4 * 16 * tan_so)) - 1;
        
        if(pr2_val > 255) pr2_val = 255;
        if(pr2_val < 1) pr2_val = 1;
        
        setup_ccp1(CCP_PWM);
        setup_timer_2(T2_DIV_BY_16, pr2_val, 1);
        set_pwm1_duty(128); // 50% duty cycle - �m thanh to v?a
    }
}

// ===== H�M KH?I T?O PWM CHO BUZZER =====
void khoi_tao_pwm_buzzer() {
    setup_ccp1(CCP_PWM);
    setup_timer_2(T2_DIV_BY_16, 255, 1);
    set_pwm1_duty(0);
}

// ===== H�M �I?U KHI?N B�O TH?C CH�NH =====
void bao_thuc(unsigned int8 trang_thai) {
    if(trang_thai == 1) {
        // B?T b�o th?c
        if(dang_phat_nhac == 0) {
            dang_phat_nhac = 1;
            beep_counter = 0;
            beep_state = 1;          // B?t �?u v?i beep b?t
            beep_count = 0;
            is_pausing = 0;
            total_time = 0;
            
            // Ph�t beep �?u ti�n
            cai_dat_tan_so_pwm(BEEP_FREQUENCY);
        }
    } else {
        // T?T b�o th?c
        dang_phat_nhac = 0;
        beep_state = 0;
        beep_counter = 0;
        beep_count = 0;
        is_pausing = 0;
        total_time = 0;
        set_pwm1_duty(0);
    }
}

// ===== H�M X? L? BEEP (G?I TRONG V?NG L?P CH�NH) =====
void xu_ly_nhac_bao_thuc() {
    if(dang_phat_nhac == 1) {
        beep_counter++;
        
        // Ki?m tra th?i gian t?ng (t? �?ng t?t sau 1 ph�t)
        if(beep_counter >= 1000) { // 1 gi�y = 1000ms
            beep_counter = 0;
            total_time++;
            
            if(total_time >= TOTAL_ALARM_TIME) {
                // T? �?ng t?t sau 60 gi�y
                bao_thuc(0);
                return;
            }
        }
        
        if(is_pausing) {
            // �ang trong th?i gian ngh?
            if(beep_counter >= PAUSE_TIME) {
                // H?t th?i gian ngh?, b?t �?u chu?i beep m?i
                is_pausing = 0;
                beep_count = 0;
                beep_state = 1;
                beep_counter = 0;
                cai_dat_tan_so_pwm(BEEP_FREQUENCY);
            }
        } else {
            // �ang trong chu?i beep
            if(beep_state == 1) {
                // Beep �ang b?t
                if(beep_counter >= BEEP_ON_TIME) {
                    // T?t beep
                    beep_state = 0;
                    beep_counter = 0;
                    set_pwm1_duty(0);
                    beep_count++;
                }
            } else {
                // Beep �ang t?t
                if(beep_counter >= BEEP_OFF_TIME) {
                    beep_counter = 0;
                    
                    if(beep_count >= BEEPS_PER_SET) {
                        // �? �? 5 beep, chuy?n sang ngh?
                        is_pausing = 1;
                        beep_counter = 0;
                    } else {
                        // Ch�a �? 5 beep, ti?p t?c beep
                        beep_state = 1;
                        cai_dat_tan_so_pwm(BEEP_FREQUENCY);
                    }
                }
            }
        }
    }
}

// ===== H�M KI?M TRA TR?NG TH�I B�O TH?C =====
unsigned int8 dang_bao_thuc() {
    return dang_phat_nhac;
}



