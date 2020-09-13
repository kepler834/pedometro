#include<Wire.h>
#include <SD.h>
#include <SPI.h>

#define limiar 15400
#define limiar_superior 17000
#define lenghVetor 40
#define lenghVetor_media 10
File arquivo;

int passos_counter, restritor, avrg, vetor[lenghVetor],
    eixo, vetor_media[lenghVetor_media], aux, variacao;

void setup() {
  Serial.begin(9600);
  inicializar_MPU();
  inicializar_SD();
  while(digitalRead(6)==0);
}
void loop() {
  eixo = MPU();
  avrg = media(eixo);
  guardar(avrg);
  armazenar_SD(avrg);
  if( verificar()==true and restritor <= 0){
    passos_counter++;
    restritor = lenghVetor - aux;
  }
  restritor --;
}
void inicializar_MPU(void){
  Wire.begin();
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}
int MPU(void){
  Wire.beginTransmission(0x68);
  Wire.write(0x3D);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,14,true);
  int y = Wire.read()<<8|Wire.read();
  return abs(y);
}
void guardar(int x){
  for (int i = (lenghVetor-2); i>=0; i--){
    aux = vetor[i];
    vetor[i+1] = aux;
  }
  vetor[0] = x;
}
int media(int x){
  for (int i = (lenghVetor_media-2); i>=0; i--){
    aux = vetor_media[i];
    vetor_media[i+1] = aux;
  }
  vetor_media[0] = x;
  double acumulador = 0;
  for(int i=0;i<lenghVetor_media; i++){
    acumulador = acumulador + vetor_media[i];
  }
  return acumulador/lenghVetor_media;
}
bool verificar (void){
  bool verificacao = false;
  for(int i=lenghVetor-1; i>=0; i--){
    if(vetor[i]<=limiar and verificacao == false){
      verificacao = true;
    }
    if(vetor[i]>=limiar_superior and verificacao == true){
      aux = i;
      return true;
    }
  }
  return false;
}
void inicializar_SD(void){
  pinMode(6,INPUT);
  pinMode(10,OUTPUT);
  SD.begin();
}
void armazenar_SD(int x){
  arquivo = SD.open("final16.txt", FILE_WRITE);
  arquivo.print(x);
  arquivo.print(',');
  arquivo.println(passos_counter);
  arquivo.close();
}
