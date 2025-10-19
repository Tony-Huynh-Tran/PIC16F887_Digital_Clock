// BI?N ÐI?U KHI?N BEEP
static unsigned int8 dang_phat_nhac = 0;          // 0: không phát, 1: ðang phát
static unsigned int16 beep_counter = 0;           // Ð?m th?i gian beep
static unsigned int8 beep_state = 0;              // 0: t?t, 1: b?t
static unsigned int16 total_time = 0;             // T?ng th?i gian ð? beep (giây)

// C?U H?NH BEEP
#define BEEP_FREQUENCY  2000    // T?n s? beep (Hz)
#define BEEP_ON_TIME    150     // Th?i gian b?t beep (ms) 
#define BEEP_OFF_TIME   50     // Th?i gian t?t beep (ms)
#define PAUSE_TIME      800    // Th?i gian ngh? gi?a các chu?i beep (ms)
#define BEEPS_PER_SET   5       // S? beep trong 1 chu?i (tít tít tít tít tít)
#define TOTAL_ALARM_TIME 60     // T?ng th?i gian báo th?c (giây)

static unsigned int8 beep_count = 0;              // Ð?m s? beep trong 1 chu?i
static unsigned int8 is_pausing = 0;              // Ðang trong th?i gian ngh?

// ===== HÀM CÀI Ð?T PWM CHO T?N S? =====
void cai_dat_tan_so_pwm(unsigned int tan_so) {
    if(tan_so == 0) {
        // T?t âm thanh
        set_pwm1_duty(0);
    } else {
        // Tính PR2 cho t?n s? mong mu?n
        unsigned int16 pr2_val = (20000000UL / (4 * 16 * tan_so)) - 1;
        
        if(pr2_val > 255) pr2_val = 255;
        if(pr2_val < 1) pr2_val = 1;
        
        setup_ccp1(CCP_PWM);
        setup_timer_2(T2_DIV_BY_16, pr2_val, 1);
        set_pwm1_duty(128); // 50% duty cycle - âm thanh to v?a
    }
}

// ===== HÀM KH?I T?O PWM CHO BUZZER =====
void khoi_tao_pwm_buzzer() {
    setup_ccp1(CCP_PWM);
    setup_timer_2(T2_DIV_BY_16, 255, 1);
    set_pwm1_duty(0);
}

// ===== HÀM ÐI?U KHI?N BÁO TH?C CHÍNH =====
void bao_thuc(unsigned int8 trang_thai) {
    if(trang_thai == 1) {
        // B?T báo th?c
        if(dang_phat_nhac == 0) {
            dang_phat_nhac = 1;
            beep_counter = 0;
            beep_state = 1;          // B?t ð?u v?i beep b?t
            beep_count = 0;
            is_pausing = 0;
            total_time = 0;
            
            // Phát beep ð?u tiên
            cai_dat_tan_so_pwm(BEEP_FREQUENCY);
        }
    } else {
        // T?T báo th?c
        dang_phat_nhac = 0;
        beep_state = 0;
        beep_counter = 0;
        beep_count = 0;
        is_pausing = 0;
        total_time = 0;
        set_pwm1_duty(0);
    }
}

// ===== HÀM X? L? BEEP (G?I TRONG V?NG L?P CHÍNH) =====
void xu_ly_nhac_bao_thuc() {
    if(dang_phat_nhac == 1) {
        beep_counter++;
        
        // Ki?m tra th?i gian t?ng (t? ð?ng t?t sau 1 phút)
        if(beep_counter >= 1000) { // 1 giây = 1000ms
            beep_counter = 0;
            total_time++;
            
            if(total_time >= TOTAL_ALARM_TIME) {
                // T? ð?ng t?t sau 60 giây
                bao_thuc(0);
                return;
            }
        }
        
        if(is_pausing) {
            // Ðang trong th?i gian ngh?
            if(beep_counter >= PAUSE_TIME) {
                // H?t th?i gian ngh?, b?t ð?u chu?i beep m?i
                is_pausing = 0;
                beep_count = 0;
                beep_state = 1;
                beep_counter = 0;
                cai_dat_tan_so_pwm(BEEP_FREQUENCY);
            }
        } else {
            // Ðang trong chu?i beep
            if(beep_state == 1) {
                // Beep ðang b?t
                if(beep_counter >= BEEP_ON_TIME) {
                    // T?t beep
                    beep_state = 0;
                    beep_counter = 0;
                    set_pwm1_duty(0);
                    beep_count++;
                }
            } else {
                // Beep ðang t?t
                if(beep_counter >= BEEP_OFF_TIME) {
                    beep_counter = 0;
                    
                    if(beep_count >= BEEPS_PER_SET) {
                        // Ð? ð? 5 beep, chuy?n sang ngh?
                        is_pausing = 1;
                        beep_counter = 0;
                    } else {
                        // Chýa ð? 5 beep, ti?p t?c beep
                        beep_state = 1;
                        cai_dat_tan_so_pwm(BEEP_FREQUENCY);
                    }
                }
            }
        }
    }
}

// ===== HÀM KI?M TRA TR?NG THÁI BÁO TH?C =====
unsigned int8 dang_bao_thuc() {
    return dang_phat_nhac;
}



