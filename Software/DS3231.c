// DS3231 ADDRESS
#define DS3231_ADDR 0xD0


// KHAI BAO BIEN 
unsigned int8 thu=0, ngay=27, thang=7, gio=0, phut=0, giay=0;
unsigned int16 nam=2025;
// CAC BIEN TAM CHO MODE 3,4
unsigned int8 gio_tam=0, phut_tam=0, giay_tam=0;
unsigned int8 ngay_tam=0, thang_tam=0, nam_tam=0;

// HAM CHUYEN DOI BCD SANG DEC
unsigned int8 bcd_to_dec(unsigned int8 bcd_val)
{
    return ((bcd_val >> 4) * 10) + (bcd_val & 0x0F);
}

// HAM CHUYEN DOI DEC SANG BCD
unsigned int8 dec_to_bcd(unsigned int8 dec_val)
{
    return ((dec_val / 10) << 4) + (dec_val % 10);
}

// HAM CHUYEN DOI THU TU DS3231 SANG ZELLER FORMAT
// DS3231: 1=CN, 2=T2, 3=T3, 4=T4, 5=T5, 6=T6, 7=T7
// Zeller chu?n: 0=T7, 1=CN, 2=T2, 3=T3, 4=T4, 5=T5, 6=T6
unsigned int8 ds3231_to_zeller(unsigned int8 ds3231_day)
{
    if(ds3231_day == 1) return 1; // CN
    if(ds3231_day == 7) return 0; // T7
    return ds3231_day; // T2-T6 gi? nguyên
}

// HAM CHUYEN DOI THU TU ZELLER SANG DS3231 FORMAT
unsigned int8 zeller_to_ds3231(unsigned int8 zeller_day)
{
    if(zeller_day == 0) return 7; // T7
    if(zeller_day == 1) return 1; // CN
    return zeller_day; // T2-T6 gi? nguyên
}

// DUNG CONG THUC ZELLER CHUAN DE TINH THU
// K?t qu?: 0=T7, 1=CN, 2=T2, 3=T3, 4=T4, 5=T5, 6=T6
unsigned int8 tinh_thu_zeller(unsigned int8 q, unsigned int8 m, unsigned int16 y)
{
    signed int16 K, J, h;
    signed int16 m_temp = m;
    signed int16 y_temp = y;
    
    // Ði?u ch?nh cho tháng 1 và 2 (coi nhý tháng 13, 14 c?a nãm trý?c)
    if (m_temp < 3) {
        m_temp += 12;
        y_temp -= 1;
    }
    
    K = y_temp % 100;
    J = y_temp / 100;
    
    // Công th?c Zeller chu?n
    h = (q + ((13 * (m_temp + 1)) / 5) + K + (K / 4) + (J / 4) - (2 * J)) % 7;
    
    // Ð?m b?o k?t qu? dýõng
    if (h < 0) h += 7;
    
    // Công th?c Zeller chu?n tr? v?: 0=T7, 1=CN, 2=T2, 3=T3, 4=T4, 5=T5, 6=T6
    return (unsigned int8)h;
}

// HAM DOC THOI GIAN TU DS3231
void doc_thoi_gian_ds3231()
{
    unsigned int8 data[7];
    
    // Ghi ð?a ch? register 0x00 ð? ð?c t? ð?u
    i2c_start();
    i2c_write(DS3231_ADDR);       // Write address
    i2c_write(0x00);              // Register address (seconds)
    i2c_stop();
    
    // Ð?c d? li?u
    i2c_start();
    i2c_write(DS3231_ADDR | 1);   // Read address
    
    // Ð?c 7 byte d? li?u
    for(int i = 0; i < 7; i++)
    {
        if(i == 6) {
            data[i] = i2c_read(0);    // NACK cho byte cu?i
        } else {
            data[i] = i2c_read(1);    // ACK cho các byte khác
        }
    }
    i2c_stop();
    
    // Chuy?n ð?i và gán giá tr?
    giay = bcd_to_dec(data[0] & 0x7F);  // Seconds (bit 7 = 0)
    phut = bcd_to_dec(data[1] & 0x7F);  // Minutes
    gio = bcd_to_dec(data[2] & 0x3F);   // Hours (24h format)
    ngay = bcd_to_dec(data[4] & 0x3F);  // Date
    thang = bcd_to_dec(data[5] & 0x1F); // Month
    nam = bcd_to_dec(data[6]) + 2000;   // Year (20xx)
    
    // Tính th? b?ng công th?c Zeller chu?n thay v? ð?c t? DS3231
    thu = tinh_thu_zeller(ngay, thang, nam);
}

// HAM GHI THOI GIAN VAO DS3231
void ghi_thoi_gian_ds3231()
{
    // Chuy?n ð?i th? sang format DS3231
    unsigned int8 thu_ds3231 = zeller_to_ds3231(thu);
    
    i2c_start();
    i2c_write(DS3231_ADDR);           // Write address
    i2c_write(0x00);                  // Register address (seconds)
    i2c_write(dec_to_bcd(giay_tam));  // Seconds
    i2c_write(dec_to_bcd(phut_tam));  // Minutes
    i2c_write(dec_to_bcd(gio_tam));   // Hours (24h format)
    i2c_write(thu_ds3231);            // Day of week
    i2c_write(dec_to_bcd(ngay_tam));  // Date
    i2c_write(dec_to_bcd(thang_tam)); // Month
    i2c_write(dec_to_bcd(nam_tam));   // Year (ch? 2 ch? s? cu?i)
    i2c_stop();
}



// TINH NGAY TOI DA TRONG THANG
int8 max_ngay_trong_thang(int8 thang, int16 nam)
{
    if (thang == 2) {
        if ((nam % 4 == 0 && nam % 100 != 0) || (nam % 400 == 0))
            return 29;
        return 28;
    }
    if (thang == 4 || thang == 6 || thang == 9 || thang == 11)
        return 30;
    return 31;
}
