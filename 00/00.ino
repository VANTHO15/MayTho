// led test 4 5 6 7
// VolumNhipTho 4  // 30-80 lần / phút  tương đương góc quay
// VolumTheTichkk 5   // 200-800ml
// VolumHitTho 6     //1:1 Đến 1:4
// VolumNguongApXuat 7
#include <TimerThree.h>
#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
#define dirPin 9
#define stepPin 10
#define motorInterfaceType 1
#define BtnConfig 2
#define BtnHienThi 3
#define BtnXacNhan 18
#define BtnOffCanhBao 19
#define Loa 8
void HienThiLCD();
void FuncBtnConfig();
void FuncBtnHienThi();
void FuncBtnXacNhan();
void FuncCanhBao();
void FuncLoa();
void FuncBtnOffCanhBao();
void FuncDongCo(uint8_t Chieu, uint8_t DoRongXung, uint8_t VanToc);
void ResetViTri();
void DungIm();
void TinhToan(uint8_t NhipTho, float TheTich, float HitTho);
String FuncCamBienBienTro();
volatile uint8_t Count10Ms = 0, CountTime = 0, TrangThaiCanhBao = 0, Lan1CanhBao = 0, Lan1XacNhan = 0, PhatHien = 1, x = 0, CanhBao = 0, a = 0, b = 0, c = 0, d = 0;
volatile String TrangThaiHienThiLCD = "ThongSo", LanDau = "LanDau"; // Trạng thái hiển thị của LCd có 2 chế độ là Thông số và config
volatile uint8_t NhipTho = 30, TheTich = 200, HitTho = 1.1, ApXuatMax = 100;
volatile uint32_t ThuanChieu = 0, NguocChieu = 0, DemThuanChieu = 0, DemNguocChieu = 0, ThoiGianHit = 100, ThoiGianTho = 100;
volatile uint32_t TimeDelayThuan = 100, DemDelayThuan = 0, TimeDelayNguoc = 100, DemDelayNguoc = 0, DelayThuan = 0, DelayNguoc = 0;
void setup()
{
  pinMode ( stepPin, OUTPUT ) ;
  pinMode ( dirPin, OUTPUT ) ;
  pinMode ( 4, OUTPUT ) ;
  pinMode ( 5, OUTPUT ) ;
  pinMode ( 6, OUTPUT ) ;
  pinMode ( 7, OUTPUT ) ;
  pinMode ( Loa, OUTPUT ) ;
  digitalWrite(Loa, CanhBao);
  for (int i = 4; i <= 7; i++)
  {
    digitalWrite(i, 0);
  }
  pinMode(BtnConfig, INPUT_PULLUP);
  pinMode(BtnHienThi, INPUT_PULLUP);
  pinMode(BtnXacNhan, INPUT_PULLUP);
  pinMode(BtnOffCanhBao, INPUT_PULLUP);
  pinMode ( dirPin, OUTPUT ) ;
  attachInterrupt(digitalPinToInterrupt(BtnConfig), FuncBtnConfig, FALLING);
  attachInterrupt(digitalPinToInterrupt(BtnHienThi), FuncBtnHienThi, FALLING);
  attachInterrupt(digitalPinToInterrupt(BtnXacNhan), FuncBtnXacNhan, FALLING);
  attachInterrupt(digitalPinToInterrupt(BtnOffCanhBao), FuncBtnOffCanhBao, FALLING);
  Serial.begin(9600);
  // Time
  //  cli();
  //  TCCR1A = 0;
  //  TCCR1B = 0;
  //  TIMSK1 = 0;
  //  TCCR1B |= 1 << CS12;
  //  TCNT1 = 3036;
  //  TIMSK1 = (1 << TOIE1);
  //  sei();
  // End Timer 1s
  // tạo time
  Timer3.initialize(10000);
  Timer3.attachInterrupt(Timer001s);
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(Timer1s);
  // End time
  lcd.init();
  lcd.backlight();
}
void loop()
{
  //TinhToan(NhipTho, TheTich, HitTho);
  if (LanDau == "LanDau")
  {
    Serial.println("Func lần Đầu");
    ResetViTri();
    LanDau = "LanHai";
    Serial.println("Func lần Đầu thứ 2");
  }
  if (PhatHien == 1)//Cảm biến phát hiện motor về vị trí ban đầu
  {
    ThuanChieu = 1;
    NguocChieu = 0;
    PhatHien = 0;
    Serial.println("Func Phát Hiện");
  }
  if (ThuanChieu == 1)
  {
    Serial.println("Func Thuân Chieu");
    FuncDongCo( 1, 240, 3);
  }
  noInterrupts();
  uint32_t DemThuanChieuOK = DemThuanChieu;
  interrupts();
  if (DemThuanChieuOK >= ThoiGianHit) // end quay thuận chiều
  {
    Serial.println(" Vào Thuân chiều");
    ThuanChieu = 0;
    DemThuanChieu = 0;
    DungIm();
    DelayThuan = 1;
  }
  noInterrupts();
  uint32_t DemDelayThuanOK = DemDelayThuan;
  interrupts();
  if (DemDelayThuanOK >= TimeDelayThuan) // End Delay Thuận
  {
    Serial.println(" Vào Delay thuận");
    DelayThuan = 0;
    DemDelayThuan = 0;
    FuncDongCo( 0, 240, 3);
    NguocChieu = 1;
  }
  noInterrupts();
  uint32_t DemNguocChieuOK = DemNguocChieu;
  interrupts();
  if ((DemNguocChieuOK >= TimeDelayThuan) || (PhatHien == 1)) // end quay ngược chiều
  {
    Serial.println("vào Quay ngược chièu");
    NguocChieu = 0;
    DemNguocChieu = 0;
    DungIm();
    DelayNguoc = 1;
  }
  noInterrupts();
  uint32_t DemDelayNguocOK = DemDelayNguoc;
  Serial.println(DemDelayNguocOK);
  interrupts();
  Serial.println(TimeDelayNguoc);
  Serial.println(DemDelayNguocOK >= TimeDelayNguoc);
  if (DemDelayNguocOK >= TimeDelayNguoc) // End Delay Nghich
  {
    Serial.println("vào delay nghich");
    DelayNguoc = 0;
    DemDelayNguoc = 0;
//    FuncDongCo( 1, 240, 2);
    ThuanChieu = 1;
  }
  HienThiLCD();
  FuncCanhBao();
  FuncLoa();
  digitalWrite(4, a);
  digitalWrite(5, b);
  digitalWrite(6, c);
  digitalWrite(7, d);
  //  Serial.println("Nhip Tho test = :" + String(NhipTho));
  //  Serial.println("The Tich test = :" + String(TheTich));
  //  Serial.println("Hit Tho test = :" + String(HitTho));
  //  Serial.println("Ap Xuat Max test = :" + String(ApXuatMax));
}
void HienThiLCD()
{
  String Str = FuncCamBienBienTro();
  String CB1 = Str.substring(Str.indexOf("a") + 1, Str.indexOf("b"));
  String CB2 = Str.substring(Str.indexOf("b") + 1, Str.indexOf("c"));
  String CB3 = Str.substring(Str.indexOf("c") + 1, Str.indexOf("d"));
  String CB4 = Str.substring(Str.indexOf("d") + 1, Str.indexOf("e"));
  String BT1 = Str.substring(Str.indexOf("e") + 1, Str.indexOf("f"));
  String BT2 = Str.substring(Str.indexOf("f") + 1, Str.indexOf("g"));
  String BT3 = Str.substring(Str.indexOf("g") + 1, Str.indexOf("h"));
  String BT4 = Str.substring(Str.indexOf("h") + 1, Str.indexOf("i"));
  if (TrangThaiHienThiLCD == "ThongSo")
  {
    lcd.setCursor(1, 0);
    lcd.print("CamBien1: " + CB1);
    lcd.setCursor(1, 1);
    lcd.print("CamBien1: " + CB2 );
    lcd.setCursor(1, 2);
    lcd.print("CamBien1: " + CB3 );
    lcd.setCursor(1, 3);
    lcd.print("CamBien1: " + CB4);
    //lcd.clear();
  }
  else if (TrangThaiHienThiLCD == "Config")
  {
    lcd.setCursor(1, 0);
    lcd.print("BienTro1: " + BT1);
    lcd.setCursor(1, 1);
    lcd.print("BienTro2: " + BT2 );
    lcd.setCursor(1, 2);
    lcd.print("BienTro3: " + BT3 );
    lcd.setCursor(1, 3);
    lcd.print("BienTro4: " + BT4);
    //lcd.clear();
  }
}
void FuncBtnConfig()
{
  a = !a;
  TrangThaiHienThiLCD = "Config";
}
void FuncBtnHienThi()
{
  b = !b;
  TrangThaiHienThiLCD = "ThongSo";
}
void FuncBtnXacNhan()
{
  if (Lan1XacNhan != 0)
  {
    c = !c;
    if (TrangThaiHienThiLCD == "Config")
    {
      String Str = FuncCamBienBienTro();
      String BT1 = Str.substring(Str.indexOf("e") + 1, Str.indexOf("f"));
      String BT2 = Str.substring(Str.indexOf("f") + 1, Str.indexOf("g"));
      String BT3 = Str.substring(Str.indexOf("g") + 1, Str.indexOf("h"));
      String BT4 = Str.substring(Str.indexOf("h") + 1, Str.indexOf("i"));
      NhipTho = BT1.toInt();
      TheTich = BT2.toFloat();
      HitTho = BT3.toFloat();
      ApXuatMax = BT4.toFloat();
      Serial.println("Nhip Tho test = :" + String(NhipTho));
      Serial.println("The Tich test = :" + String(TheTich));
      Serial.println("Hit Tho test = :" + String(HitTho));
      Serial.println("Ap Xuat Max test = :" + String(ApXuatMax));
    }
  }
  Lan1XacNhan = 1;
}
void FuncCanhBao()
{
  if ((1 == 1) && (TrangThaiCanhBao == 0))
  {
    CanhBao = 1;
  }
}
void FuncLoa()
{
  digitalWrite(Loa, CanhBao);
}
void FuncBtnOffCanhBao()
{
  if (Lan1CanhBao != 0)
  {
    d = !d;
    CanhBao = 0;
    TrangThaiCanhBao = 1;
    FuncLoa();
  }
  Lan1CanhBao = 1;
}
void FuncDongCo(uint8_t Chieu, uint8_t DoRongXung, uint8_t VanToc)
{
  uint8_t myEraser = 7;      // 111
  TCCR2B &= ~ myEraser;   // 2 la time 2 chan 9 10
  uint8_t myPrescaler = VanToc;   // 011
  TCCR2B |= myPrescaler; // TCCR2B se bang 011
  if (Chieu == 1)
  {
    digitalWrite(dirPin, 0); // 1 Thuan
    analogWrite(stepPin, DoRongXung);
  }
  else
  {
    digitalWrite(dirPin, 1); //1 Nghich
    analogWrite(stepPin, DoRongXung);
  }
  // 16*10^6/256/8  ???
  //TCCR2B = TCCR2B & B11111000 | B00000001;      1 for PWM frequency of 31372.55 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000010;      8 for PWM frequency of  3921.16 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000011;      32 for PWM frequency of   980.39 Hz
  // TCCR2B = TCCR2B & B11111000| B00000100;     64 for PWM frequency of   490.20 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000101;      128 for PWM frequency of   245.10 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000110;      256 for PWM frequency of   122.55 Hz
  //TCCR2B = TCCR2B & B11111000 | B00000111;      1024 for PWM frequency of    30.64 Hz
}
String FuncCamBienBienTro()
{
  float CB1 = 100.1;   //analogRead(A0)
  float CB2 = 10.46;  //analogRead(A1)
  float CB3 = 44.13;  //analogRead(A2)
  float CB4 = 267.01;  //analogRead(A3)
  CB1 = map(CB1, 0, 1023, 300, 4000); //(0,1023)->(300,4000)
  CB2 = map(CB2, 0, 1023, 300, 4000);
  CB3 = map(CB3, 0, 1023, 300, 4000);
  CB4 = map(CB4, 0, 1023, 300, 4000);

  float BT1 = analogRead(A4);
  float BT2 = analogRead(A5);
  float BT3 = analogRead(A6);
  float BT4 = 267.01;  //analogRead(A7)
  BT1 = map(BT1, 0, 1023, 0, 100);
  BT2 = map(BT2, 0, 1023, 0, 100);
  BT3 = map(BT3, 0, 1023, 0, 100);
  BT4 = map(BT4, 0, 1023, 300, 4000);
  String Dulieu = "a" + String(CB1) + "b" + String(CB2) + "c" + String(CB3) + "d" + String(CB4) + "e"
                  + String(BT1) + "f" + String(BT2) + "g" + String(BT3) + "h" + String(BT4) + "i";
  return Dulieu;
}
//ISR  (TIMER1_OVF_vect)
//{
//  TCNT1 = 3036;
//  if (TrangThaiCanhBao == 1)
//  {
//    CountTime++;
//    Serial.println(CountTime);
//    if (CountTime == 10)
//    {
//      CountTime = 0;
//      TrangThaiCanhBao = 0;
//      Serial.println("Dem= 10");
//    }
//  }
//}

void Timer1s(void)
{
  if (TrangThaiCanhBao == 1)
  {
    CountTime++;
    Serial.println(CountTime);
    if (CountTime == 10)
    {
      CountTime = 0;
      TrangThaiCanhBao = 0;
      Serial.println("Dem= 10");
    }
  }
}
void Timer001s(void)
{
  if (ThuanChieu == 1)
  {
    DemThuanChieu++;
  }
  else
  {
    DemThuanChieu = 0;
  }
  ////////////////////////
  if (NguocChieu == 1)
  {
    DemNguocChieu++;
  }
  else
  {
    DemNguocChieu = 0;
  }
  //////////////////////////////
  if (DelayThuan == 1)
  {
    DemDelayThuan++;
  }
  else
  {
    DemDelayThuan = 0;
  }
  ////////////////////////////
  if (DelayNguoc == 1)
  {
    DemDelayNguoc++;
  }
  else
  {
    DemDelayNguoc = 0;
  }
}
void ResetViTri()
{
  while (PhatHien != 1)
  {
    FuncDongCo( 0, 250, 2);
  }
  Serial.println("Đã Reset Thanh công !");
  //DungIm();
}
void DungIm()
{
  FuncDongCo( 0, 0, 2);
  Serial.println("Đang Đứng im !");
}
void TinhToan( uint8_t NhipTho, float TheTich, float  HitTho)
{
  float ChuKi = 60.0 / NhipTho;
  float TimeHitTho1ChuKi = ChuKi - float(TimeDelayThuan) - float(TimeDelayNguoc);
  ThoiGianHit = HitTho * TimeHitTho1ChuKi;
  ThoiGianTho = TimeHitTho1ChuKi - ThoiGianHit;
  // từ thể tích tính quãng đường => vận tốc
  // vận tốc có các mức là 2 3 4 5 6 . Theo thứ tự giảm dần

}
