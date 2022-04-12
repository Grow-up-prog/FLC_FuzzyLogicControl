/*
  METODE SUGENO
  *INPUT BERUPA HIMPUNAN FUZZY
  *OUTPUT BERUPA NILAI
  *PENENTUAN NILAI DEFUZZIFIKASI BISA DENGAN CENTER OF GRAVITY
*/
//~~~~~LCD~~~~~~
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);//16chars x 2 line display

//~~~~~Pendefinisian potensiometer input~~~~~
int potKecepatan = A0;      //potensio sisi kanan
int potJarak = A1;          //potensio sisi kiri
float nilaiKecepatan = 0;   // variabel penyimpanan nilai potensio kecepatan
float nilaiJarak = 0;       //variabel penyimpanan nilai potensio jarak
float J,K;                  //variabel penyimpan nilai jarak & kecepatan

//~~~~~FUZZY~~~~~~
float m_jarak, m_kecepatan;                                                   //variabel derajat keanggotaan
float sangatDekat_mfj, dekat_mfj, sedang_mfj, jauh_mfj, sangatJauh_mfj;       //membership function jarak (j)
float sangatLambat_mfk, lambat_mfk, sedang_mfk, cepat_mfk, sangatCepat_mfk;   //membership function kecepatan (k)
float sangatLambat_imk, lambat_imk, sedang_imk, cepat_imk, sangatCepat_imk;   //inisialisasi membership kecepatan aktif
float sangatDekat_imj, dekat_imj, sedang_imj, jauh_imj,sangatJauh_imj;        //inisialisasi membership jarak aktif
float SLmax=0,Lmax=0,Smax=0,Kmax=0,SKmax=0;                                   //variabel nilai maximum masing masing output pengereman
float num, den, output;

//RUMUS MENCARI DERAJAT KEANGGOTAAN
//~~~~~FUZZIFIKASI~~~~~~
/*rumus fungsi keanggotaan
 * segitiga(x,a,b,c)=
 * Kondisi  Rumus
 * 0        | x<a,x>c
 * a<= x <b | (x-a)/(b-a)
 * b<= x <c | (c-x)/(c-b) 
*/
float miuF_kecepatan(float a,float b,float c)                                 //mencari derajat keanggotaan membership function segitiga kecepatan FUNGSI MIU KECEPATAN!
{
  if(a<= K && K < b){m_kecepatan=(K-a) / (b-a);}
  else
  if(b<= K && K <= c){m_kecepatan=(c-K) / (c-b);}
  else (m_kecepatan=0);
}
float miuF_jarak(float a1, float b1, float c1)
{
  if(a1<=J && J < b1){m_jarak=(J-a1) / (b1-a1);}
  if(b1<= J && J <=c1){m_jarak=(c1-J) / (c1-b1);}
  if((J>c1)||(J<a1)){m_jarak=0;}
//  if((J>c1)||(J<a1)){m_jarak=0;}
}

void setup() {
  Serial.begin(9600);
  pinMode(potKecepatan, INPUT);
  pinMode(potJarak, INPUT);
  lcd.init(); //initialisasi LCD
  lcd.backlight();//menampilkan backlight pada latar LCD
  lcd.setCursor(0,0);
  lcd.print("~PROGRAM FUZZY~");  
  lcd.setCursor(1,1);
  lcd.print("SISTEM KENDALI");
  delay(500);
  lcd.setCursor(0,1);
  lcd.print("....3..2..1.....");
  delay(500);
}
void loop() {
  nilaiJarak = analogRead(potJarak);
  J= map(nilaiJarak, 0 ,695,0,40);
  nilaiKecepatan = analogRead(potKecepatan);
  K= map(nilaiKecepatan, 0 ,1023,15,35);
 
  
//~~~~~MEMBERSHIP FUNCTION~~~~~
//~~~~~~~~~~~~~~~~~~~KECEPATAN~~~~~~~~~~~~~~~~~~~~~~~~
if(K>=15 && K<=20)  
{miuF_kecepatan(15,15,20); sangatLambat_mfk=m_kecepatan; sedang_mfk=0; sangatCepat_mfk=0;}              //SANGAT LAMBAT
if(K>=15 && K<=25)
{miuF_kecepatan(15,20,25); lambat_mfk=m_kecepatan; cepat_mfk=0; sangatCepat_mfk=0;}                     //LAMBAT
if(K>=20 && K<=30)
{miuF_kecepatan(20,25,30); sangatLambat_mfk=0; sedang_mfk=m_kecepatan; sangatCepat_mfk=0;}              //SEDANG
if(K>=25 && K<=35)
{miuF_kecepatan(25,30,35); sangatLambat_mfk=0; lambat_mfk=0; cepat_mfk=m_kecepatan;}                    //CEPAT
if(K>=30 && K<=35)
{miuF_kecepatan(30,35,35); sangatLambat_mfk=0; lambat_mfk=0; sangatCepat_mfk=m_kecepatan;}              //SANGAT CEPAT
//~~~~~~~~~~~~~~~~~~~~~~JARAK~~~~~~~~~~~~~~~~~~~~~~~~~~
if(J>=0 && J<=10)  
{miuF_jarak(0,0,10); sangatDekat_mfj=m_jarak; sedang_mfj=0; sangatJauh_mfj=0;}                          //SANGAT DEKAT
if(J>=0 && J<=20)  
{miuF_jarak(0,10,20); dekat_mfj=m_jarak; jauh_mfj=0; sangatJauh_mfj=0;}                                 //DEKAT
if(J>=10 && J<=30)  
{miuF_jarak(10,20,30); sangatDekat_mfj=0; sedang_mfj=m_jarak; sangatJauh_mfj=0;}                        //SEDANG
if(J>=20 && J<=40)  
{miuF_jarak(20,30,40); sangatDekat_mfj=0; dekat_mfj=0; jauh_mfj=m_jarak;}                               //JAUH
if(J>=30 && J<=40)  
{miuF_jarak(30,40,40); sangatJauh_mfj=0; sedang_mfj=0; sangatJauh_mfj=m_jarak;}                         //SANGAT JAUH

//~~~~~MEKANISME INFERENSI~~~~~

//~~~~INISIASIALISASI MEMBERSHIP~~~~
if(sangatLambat_mfk>0) {sangatLambat_imk=1;} else {sangatLambat_imk=0;}
if(lambat_mfk>0) {lambat_imk=1;} else {lambat_imk=0;}
if(sedang_mfk>0) {sedang_imk=1;} else {sedang_imk=0;}
if(cepat_mfk>0) {cepat_imk=1;} else {cepat_imk=0;}
if(sangatCepat_mfk>0) {sangatCepat_imk=1;} else {sangatCepat_imk=0;}

if(sangatDekat_mfj>0) {sangatDekat_imj=1;} else {sangatDekat_imj=0;}
if(dekat_mfj>0) {dekat_imj=1;} else {dekat_imj=0;}
if(sedang_mfj>0) {sedang_imj=1;} else {sedang_imj=0;}
if(jauh_mfj>0) {jauh_imj=1;} else {jauh_imj=0;}
if(sangatJauh_mfj>0) {sangatJauh_imj=1;} else {sangatJauh_imj=0;}
//~~~~~TABEL BASIS ATURAN~~~~~
float SangatLemah[8]={0,1,2,3,4,5,6,7};
{
if (sangatDekat_imj==1 && sangatLambat_imk==1)    {SangatLemah[1]=min(sangatDekat_mfj,sangatLambat_mfk);}  else {SangatLemah[1]=0;}
if (dekat_imj==1 && sangatLambat_imk==1)          {SangatLemah[2]=min(dekat_mfj,sangatLambat_mfk);}        else {SangatLemah[2]=0;}
if (sedang_imj==1 && sangatLambat_imk==1)         {SangatLemah[3]=min(sedang_mfj,sangatLambat_mfk);}       else {SangatLemah[3]=0;}
if (jauh_imj==1 && sangatLambat_imk==1)           {SangatLemah[4]=min(jauh_mfj,sangatLambat_mfk);}         else {SangatLemah[4]=0;}
if (sangatJauh_imj==1 && sangatLambat_imk==1)     {SangatLemah[5]=min(sangatJauh_mfj,sangatLambat_mfk);}   else {SangatLemah[5]=0;}
if (sangatJauh_imj==1 && lambat_imk==1)           {SangatLemah[6]=min(sangatJauh_mfj,lambat_mfk);}         else {SangatLemah[6]=0;}
if (jauh_imj==1 && lambat_imk==1)                 {SangatLemah[7]=min(jauh_mfj,lambat_mfk);}               else {SangatLemah[7]=0;}
}
float Lemah[6]={0,1,2,3,4,5};
if (sangatDekat_imj==1 && lambat_imk==1)          {Lemah[1]=min(sangatDekat_mfj,lambat_mfk);}       else {Lemah[1]=0;}
if (dekat_imj==1 && lambat_imk==1)                {Lemah[2]=min(dekat_mfj,lambat_mfk);}             else {Lemah[2]=0;}
if (sedang_imj==1 && lambat_imk==1)               {Lemah[3]=min(sedang_mfj,lambat_mfk);}            else {Lemah[3]=0;}
if (jauh_imj==1 && sedang_imk==1)                 {Lemah[4]=min(jauh_mfj,sedang_mfk);}              else {Lemah[4]=0;}
if (sangatJauh_imj==1 && sedang_imk==1)           {Lemah[5]=min(sangatJauh_mfj,sedang_mfk);}        else {Lemah[5]=0;}

float Sedang[7]={0,1,2,3,4,5,6};
if (sangatDekat_imj==1 && sedang_imk==1)          {Sedang[1]=min(sangatDekat_mfj,sedang_mfk);}      else {Sedang[1]=0;}
if (dekat_imj==1 && sedang_imk==1)                {Sedang[2]=min(dekat_mfj,sedang_mfk);}            else {Sedang[2]=0;}
if (sedang_imj==1 && sedang_imk==1)               {Sedang[3]=min(sedang_mfj,sedang_mfk);}           else {Sedang[3]=0;}
if (jauh_imj==1 && cepat_imk==1)                  {Sedang[4]=min(jauh_mfj,cepat_mfk);}              else {Sedang[4]=0;}
if (sangatJauh_imj==1 && cepat_imk==1)            {Sedang[5]=min(sangatJauh_mfj,cepat_mfk);}        else {Sedang[5]=0;}
if (sangatJauh_imj==1 && sangatCepat_imk==1)      {Sedang[6]=min(sangatJauh_mfj,sangatCepat_mfk);}  else {Sedang[6]=0;}

float Kuat[5]={0,1,2,3,4};
if (sangatDekat_imj==1 && cepat_imk==1)           {Kuat[1]=min(sangatDekat_mfj,cepat_mfk);}         else {Kuat[1]=0;}
if (dekat_imj==1 && cepat_imk==1)                 {Kuat[2]=min(dekat_mfj,cepat_mfk);}               else {Kuat[2]=0;}
if (sedang_imj==1 && cepat_imk==1)                {Kuat[3]=min(sedang_mfj,cepat_mfk);}              else {Kuat[3]=0;}
if (jauh_imj==1 && sangatCepat_imk==1)            {Kuat[4]=min(jauh_mfj,sangatCepat_mfk);}          else {Kuat[4]=0;}

float SangatKuat[4]={0,1,2,3};
if (sangatDekat_imj==1 && sangatCepat_imk==1)     {SangatKuat[1]=min(sangatDekat_mfj,sangatCepat_mfk);} else {SangatKuat[1]=0;}
if (dekat_imj==1 && sangatCepat_imk==1)           {SangatKuat[2]=min(dekat_mfj,sangatCepat_mfk);}       else {SangatKuat[2]=0;}
if (sedang_imj==1 && sangatCepat_imk==1)          {SangatKuat[3]=min(sedang_mfj,sangatCepat_mfk);}      else {SangatKuat[3]=0;}

//~~~~~~~~~~~~~~~~PENCARIAN NILAI MAXIMAL DARI MASING MASING OUTPUT PENGEREMAN~~~~~~~~~~~~~~~~~
//SLmax=0,Lmax=0,Smax=0,Kmax=0,SKmax=0;                                   
SLmax=SangatLemah[0];int  index1=0;   for(int a=1;a<=7;a++){ if(SLmax<=SangatLemah[a])  {SLmax= SangatLemah[a]; index1=a;} }
Lmax=Lemah[0];       int  index2=0;   for(int b=1;b<=5;b++){ if(Lmax<=Lemah[b])         {Lmax=  Lemah[b];       index2=b;} }
Smax=Sedang[0];      int  index3=0;   for(int c=1;c<=6;c++){ if(Smax<=Sedang[c])        {Smax=  Sedang[c];      index3=c;} }
Kmax=Kuat[0];        int  index4=0;   for(int d=1;d<=4;d++){ if(Kmax<=Kuat[d])          {Kmax=  Kuat[d];        index4=d;} }
SKmax=SangatKuat[0]; int  index5=0;   for(int e=1;e<=3;e++){ if(SKmax<=SangatKuat[e])   {SKmax= SangatKuat[e];  index5=e;} }

//num, den, output;
num=SLmax*20 + Lmax*40 + Smax*60 + Kmax*80 + SKmax*100;
den=SLmax+Lmax+Smax+Kmax+SKmax;
output=num/den;

//~~~~~PRINT LCD~~~~~~~~
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("JRK=");
  lcd.setCursor(4,0);
  lcd.print(J);
//=====================
  lcd.setCursor(8,0);
  lcd.print("KCP=");
  lcd.setCursor(12,0);
  lcd.print(K);
//=====================
  lcd.setCursor(0,1);
  lcd.print("Pengereman=");
  lcd.setCursor(11,1);
  lcd.print(output);
  delay(400);
}
