#include <16F887.h>
#device adc=10
#fuses HS, PROTECT
#use delay(clock=20M)
#use i2c(Master, Fast, sda=PIN_C4, scl=PIN_C3)
#include <phat_nhac_bao_thuc.c>
#include <chong_doi_nut_nhan.c>
#include <DS3231.c>
#include <quet_led.c>

// BIEN FUNTION
#define MENU 1
#define UP 2
#define SNOOZE 3

// KHAI BAO DOC ADC
#define LM35 7
#define BATTERY_ADC 6
#define LED pin_c5  // chan cang cua led

// KHAI BAO BIEN
unsigned int8 Mode=0;
signed int16 nhiet_do=30;
signed int16 nhiet_do_tam=0;
unsigned int8 so_lan_do = 50;
unsigned int8 ma_led_7_doan[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
unsigned int8 gio_bao_thuc=0, phut_bao_thuc=0;
signed int8 vi_tri_nhap=0;
int1 co_bao_thuc=0;
int1 co_nhap_lieu=0;
int1 update_time_flag=0; // Co cap nhat thoi gian tu DS3231
signed int8 bien_dem_nhiet = 0;




void main()
{
    set_tris_b(0b11100000);
    port_b_pullups(0b11100000);
    set_tris_c(0b11011011);
    set_tris_e(0b1110);
    
    setup_timer_0(T0_INTERNAL | T0_DIV_4);
    set_timer0(256 - 250);
    setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
    set_timer1(65536 - 62500);
    
    setup_adc(ADC_CLOCK_DIV_32);
    setup_adc_ports(SAN7 | VSS_VDD);
    set_adc_channel(7);
 
    khoi_tao_pwm_buzzer();
    

    delay_ms(100); // Ð?i DS3231 ?n ð?nh
    doc_thoi_gian_ds3231();
    
    enable_interrupts(INT_RB);
    enable_interrupts(INT_TIMER0);
    enable_interrupts(INT_TIMER1);
    enable_interrupts(GLOBAL);
    
    while(TRUE)
    { 
        if(update_time_flag)
        {
            doc_thoi_gian_ds3231();
            update_time_flag = 0;
        }
        
        xu_ly_nhac_bao_thuc();
        
        if(gio == gio_bao_thuc && phut == phut_bao_thuc && !(gio == 0 && phut == 0))
        {
            if(dang_bao_thuc() == 0) 
            {
                bao_thuc(1); 
                co_bao_thuc = 1;
            }
        }
        else 
        {
            if(dang_bao_thuc() == 1) 
            {
                bao_thuc(0); 
                co_bao_thuc = 0;
            }
        }
        
        // DOC NUT NE
        if( phim != 0 )
        {
            if(co_bao_thuc == 1)
            {
                phim = 0;
                continue;
            }
            
            if (phim == MENU)
            {
                if ( Mode == 3 && co_nhap_lieu == 0 )
                {
                    Mode = 4;
                }
                else if (Mode == 3 && co_nhap_lieu == 1)
                {
                    vi_tri_nhap++;
                    if (vi_tri_nhap > 1)
                    {
                        gio_bao_thuc = gio_tam;
                        phut_bao_thuc = phut_tam;
                        co_nhap_lieu = 0;
                        Mode = 0;
                    }
                }
                else if (Mode == 4 && co_nhap_lieu == 0)
                {
                    Mode = 0;
                }
                else if (Mode == 4 && co_nhap_lieu == 1)
                {
                    vi_tri_nhap++;
                    if (vi_tri_nhap > 5)
                    {
                        int16 nam_tam_full = 2000 + nam_tam;
                        int8 max_ngay = max_ngay_trong_thang(thang_tam, nam_tam_full);
                        
                        if (ngay_tam > max_ngay)
                            ngay_tam = max_ngay;
                        
                        thu = tinh_thu_zeller(ngay_tam, thang_tam, nam_tam_full);
                        
                        ghi_thoi_gian_ds3231();
                        co_nhap_lieu = 0;
                        Mode = 0;
                    }
                }
                else
                {
                    Mode = (Mode + 1) % 5;
                    co_nhap_lieu = 0;
                    vi_tri_nhap = 0;
                }
            }
            
            if (phim == UP)
            {
                if (Mode == 3 && co_nhap_lieu == 0)
                {
                    co_nhap_lieu = 1;
                    vi_tri_nhap = 0;
                    gio_tam = gio_bao_thuc;
                    phut_tam = phut_bao_thuc;
                }
                else if (Mode == 3 && co_nhap_lieu == 1)
                {
                    if (vi_tri_nhap == 0)
                    {
                        gio_tam++;
                        if (gio_tam > 23) gio_tam = 0;
                    }
                    else if (vi_tri_nhap == 1)
                    {
                        phut_tam++;
                        if (phut_tam > 59) phut_tam = 0;
                    }
                }
                else if (Mode == 4 && co_nhap_lieu == 0)
                {
                    co_nhap_lieu = 1;
                    vi_tri_nhap = 0;
                    gio_tam = gio;
                    phut_tam = phut;
                    giay_tam = giay;
                    ngay_tam = ngay;
                    thang_tam = thang;
                    nam_tam = nam % 100; // Ch? l?y 2 ch? s? cu?i
                }
                else if (Mode == 4 && co_nhap_lieu == 1)
                {
                    if (vi_tri_nhap == 0)
                    {
                        gio_tam++;
                        if (gio_tam > 23) gio_tam = 0;
                    }
                    else if (vi_tri_nhap == 1)
                    {
                        phut_tam++;
                        if (phut_tam > 59) phut_tam = 0;
                    }
                    else if (vi_tri_nhap == 2)
                    {
                        giay_tam++;
                        if (giay_tam > 59) giay_tam = 0;
                    }
                    else if (vi_tri_nhap == 3)
                    {
                        ngay_tam++;
                        if (ngay_tam > 31) ngay_tam = 1;
                    }
                    else if (vi_tri_nhap == 4)
                    {
                        thang_tam++;
                        if (thang_tam > 12) thang_tam = 1;
                    }
                    else if (vi_tri_nhap == 5)
                    {
                        nam_tam++;
                        if (nam_tam > 99) nam_tam = 0;
                    }
                }
            }
            
            // X? l? NÚT SNOOZE khi không báo th?c (toggle LED)
            if (phim == SNOOZE && co_bao_thuc == 0)
            {
                output_toggle(LED);
            }
            
            phim = 0;
        }
        
        // Mode 0: Display HH:MM:SS (hours-minutes-seconds)
        if (Mode == 0)
        {
            quet_led(ma_led_7_doan[gio/10%10], ma_led_7_doan[gio%10], 
                     ma_led_7_doan[phut/10%10], ma_led_7_doan[phut%10], 
                     ma_led_7_doan[giay/10%10], ma_led_7_doan[giay%10]);
        }
        
        // Mode 1: Display dX:DD:MM (day-date-month)
        if (Mode == 1)
        {
            unsigned int8 d1, d2;
            
            // Hi?n th? th? theo format Zeller chu?n: 0=T7, 1=CN, 2=T2, 3=T3, 4=T4, 5=T5, 6=T6
            if (thu == 1) { // CN (Ch? nh?t)
                d1 = 0b00111001;  // 'C'
                d2 = 0b01010100;  // 'n'
            }
            else if (thu == 0) { // T7 (Th? 7)
                d1 = 0b01011110;  // 'd'
                d2 = ma_led_7_doan[7]; // '7'
            }
            else { // T2-T6 (Th? 2 ð?n Th? 6)
                d1 = 0b01011110;  // 'd'
                d2 = ma_led_7_doan[thu]; // 2,3,4,5,6
            }
            
            quet_led(d1, d2, ma_led_7_doan[ngay/10%10], ma_led_7_doan[ngay%10], 
                     ma_led_7_doan[thang/10%10], ma_led_7_doan[thang%10]);
        }
        
        // Mode 2: Display n-:XX *C (temperature)
        if (Mode == 2)
        {
            bien_dem_nhiet++;
            nhiet_do_tam +=   (float)(((read_adc() * 4.88) / 10.0));
            if (bien_dem_nhiet == so_lan_do)
            {
                  nhiet_do = nhiet_do_tam/so_lan_do;
                  nhiet_do_tam = 0;
                  bien_dem_nhiet = 0;
            }

            quet_led(0b01010100, 0b01000000, ma_led_7_doan[nhiet_do/10%10], 
                     ma_led_7_doan[nhiet_do%10], 0b01100011, 0b00111001);
        }
        
        // Mode 3: Display T-:HH:MM (set alarm time)
        if (Mode == 3)
        {
            if (co_nhap_lieu == 0)
            {
                quet_led(0b01111100, 0b01000000, ma_led_7_doan[gio_bao_thuc/10%10], 
                         ma_led_7_doan[gio_bao_thuc%10], ma_led_7_doan[phut_bao_thuc/10%10], 
                         ma_led_7_doan[phut_bao_thuc%10]);
            }
            else
            {
                quet_led(0b01111100, 0b01000000, ma_led_7_doan[gio_tam/10%10], 
                         ma_led_7_doan[gio_tam%10], ma_led_7_doan[phut_tam/10%10], 
                         ma_led_7_doan[phut_tam%10]);
            }
        }
        
        // Mode 4: Set time (HH:MM:SS) and date (DD:MM:YY)
        if (Mode == 4)
        {
            if (co_nhap_lieu == 0)
            {
                quet_led(ma_led_7_doan[0], ma_led_7_doan[0], ma_led_7_doan[0], 
                         ma_led_7_doan[0], ma_led_7_doan[0], ma_led_7_doan[0]);
            }
            else
            {
                if (vi_tri_nhap <= 2) // Hien thi: HH:MM:SS
                {
                    quet_led(ma_led_7_doan[gio_tam/10%10], ma_led_7_doan[gio_tam%10], 
                             ma_led_7_doan[phut_tam/10%10], ma_led_7_doan[phut_tam%10], 
                             ma_led_7_doan[giay_tam/10%10], ma_led_7_doan[giay_tam%10]);
                }
                else if (vi_tri_nhap <= 5) // Hien thi: DD:MM:YY
                {
                    quet_led(ma_led_7_doan[ngay_tam/10%10], ma_led_7_doan[ngay_tam%10], 
                             ma_led_7_doan[thang_tam/10%10], ma_led_7_doan[thang_tam%10], 
                             ma_led_7_doan[nam_tam/10%10], ma_led_7_doan[nam_tam%10]);
                }
            }
        }
        
        delay_ms(1);
    }
}

#INT_TIMER1
void dinh_thoi_timer1()
{
    set_timer1(65536 - 62500);
    update_time_flag = 1;
}
