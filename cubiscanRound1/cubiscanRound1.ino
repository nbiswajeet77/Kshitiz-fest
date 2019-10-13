#define  S0     A8
#define  S1     A7
#define  S2     A9
#define  S3     A6
#define  S4     A10
#define  S5     A5
#define  S6     A11
#define  S7     A0
#define  a1     6
#define  a2     5
#define  b1     4
#define  b2     3
#define  a_pwm  7
#define  b_pwm  2

int Pin0 = 52;//definition digital 8 pins as pin to control the IN1 (ULN24L01)
int Pin1 = 53;//definition digital 9 pins as pin to control the IN2 (ULN24L01)
int Pin2 = 51;//definition digital 10 pins as pin to control the IN3 (ULN24L01)
int Pin3 = 49;//definition digital 11 pins as pin to control the IN4 (ULN24L01)
int Irhpin = A12;
int Pn0 = 41;//definition digital 8 pins as pin to control the IN1 (ULN24L01)
int Pn1 = 43;//definition digital 9 pins as pin to control the IN2 (ULN24L01)
int Pn2 = 45;//definition digital 10 pins as pin to control the IN3 (ULN24L01)
int Pn3 = 47;//definition digital 11 pins as pin to control the IN4 (ULN24L01)
int Irvpin=A13;
int _speed = 1;
int thres=200;
int thres2=200; 
int j=0;
int k=0;
int t=-1;
int i=0;
int m=0;
int THRESHOLD=62;
int sig=0;
int frnt;
int T=0;
float error=0;
float prev_error=0;
float cumm_error=0;
float kp =0.002798; //0.003167, 0.002867, 0.002817, 0.002800,0.002789,0.002798
float kd=0.00188; //0.001593, 0.001793,0.00185
float ki=0;
int base = 255;
int s[7]={0,0,0,0,0,0,0};
int s_val[7];
String cmp_value;
int maxi=100;
int mini=900;

struct blocks
{
  int pos;
  long vol;
  };

  struct blocks B[4];

void setup()
   {
    pinMode(a1,OUTPUT);
    pinMode(a2,OUTPUT);
    pinMode(b1,OUTPUT);
    pinMode(b2,OUTPUT);
    pinMode(a_pwm,OUTPUT);
    pinMode(b_pwm,OUTPUT);
    pinMode(Pin0,OUTPUT);
    pinMode(Pin1,OUTPUT);
    pinMode(Pin2,OUTPUT);
    pinMode(Pin3,OUTPUT);
    pinMode(Pn0,OUTPUT);
    pinMode(Pn1,OUTPUT);
    pinMode(Pn2,OUTPUT);
    pinMode(Pn3,OUTPUT);
    cmp_value="";
    Serial.begin(9600);
   // callibrate();
    }
    
   
   
   void loop()
   {
     readInput();
     follow_line();
     delay(100);
   }
   
   void readInput()
   {
     cmp_value="";
      s[0]=analogRead(S1);
      s[1]=analogRead(S2);
      s[2]=analogRead(S3);
      s[3]=analogRead(S4);
      s[4]=analogRead(S5);
      s[5]=analogRead(S6);
      s[6]=analogRead(S7);
       for(int i =0;i<7;i++)
        {
          if (s[i]<THRESHOLD)
                  {s[i]=1;cmp_value+="W";}
                  else {s[i]=0;cmp_value+="B";}
        }
        Serial.println(T);
        Serial.println(cmp_value);
        frnt=analogRead(A13);
   }
void follow_line()
{
  while (1)
  {
    readInput();
    for(int i =0;i<7;i++)
              s_val[i]=i-3;
    
    for(int i =0;i<7;i++)
        error+=(s_val[i]*s_val[i]*s_val[i]+1773*s_val[i])*s[i];
    cumm_error+=error;
 //Serial.println(error);
 k=kp*error + kd*(prev_error - error) + ki*(cumm_error);
//Serial.println(k);
//Serial.println(cumm_error); 
     digitalWrite(b2,0);
     digitalWrite(b1,1);
     digitalWrite(a2,0);
     digitalWrite(a1,1);
     if(frnt>thres)
     {
      stopM();
      delay(500);
      scanblock();
      }
     
      if(cmp_value=="BBBBBBB")
              stopM();
      else if(cmp_value=="BBWWWWW"||cmp_value=="BBBWWWW"||cmp_value=="BBBBWWW")
            {
              delay(50);
              stopM();
              Serial.println("GO LEFTT");
              pid_goleft();
              }
      else if(cmp_value == "WWWWWBB"||cmp_value=="WWWWBBB"||cmp_value=="WWWBBBB")
      {
        delay(100);
        stopM();
        Serial.println("GO Right");
        pid_goright();
        }  
      else if(cmp_value=="WWWWWWW")
            {
              delay(80);
              stopM();
              delay(300);
              readInput();
              delay(100);
              Serial.println("asila");
              if(cmp_value=="BBBBBBB")
              {
              //Serial.println("GO LEFT");
              if(T==0)
               {
                Serial.println("2nd tym");
                T++;
                //Serial.println(T);
                Serial.println("Going Left");
                pid_goleft();
                }
              else 
               {
                Serial.println("done right");
                Serial.println("Going right");
                Serial.println(T);
                T++;
                pid_goright();
               }
              }
              else
              {
                if(t<3)
                {
                Serial.println("eta else ra");
                 pid_goleft();              
                }
                else
                {
                  stopM();
                  led_blink_order();
                  }
                }
            }
      else if(error>0)
        {
          analogWrite(a_pwm,130);
          analogWrite(b_pwm,130-abs(k));
        }
      else if(error<=0)
            {
          analogWrite(b_pwm,130);
          analogWrite(a_pwm,130-abs(k));
        }
        prev_error=error;
        error=0;
        delay(25);
     }
     }

     void led_blink_order()
     {
      for(i=0;i<4;i++)
      {
        int f=B[i].pos;
        for(int h=0;h<f;h++)
        {
          digitalWrite(13,HIGH);
          delay(300);
          digitalWrite(13,LOW);
          delay(300);
          }
        
        }
      }

     void scanblock()
     {
      if(analogRead(Irvpin)>thres)
      {
      Stepv(700);//Stepper motor forward 512 steps ---- 360 angle    
      delay(200);// delay 2S
      Step(700);
      delay(200);
      Step(-700);
       delay(200);  
       Stepv(-k);//Stepper motor backward 512 steps ---- 360 angle    
       delay(2000);//delay 2S
       led_blink(t+2);
       t++;
       B[t].pos=t+1;
       B[t].vol=j*k;
       j=0;
       k=0;
       pid_goleft();
       }

      }

      void sortvol()
      {
        struct blocks temp;
        for(i=0;i<4;i++)
        {
          for(int l=0;l<4-i-1;l++)
          {
            if(B[l].vol<B[l+1].vol)
            {
              temp=B[l];
              B[l]=B[l+1];
              B[l+1]=temp;
              }
          }
          }
        }

     void led_blink(int n)
     {
      for(i=0;i<n;i++)
      {
        digitalWrite(13,HIGH);
        delay(300);
        digitalWrite(13,LOW);
        delay(300);
      }
      }
     
void Stepv(int _step)//Stepper motor rotation
{
  if(_step>=0){  // Stepper motor forward
    for(int i=0;i<_step;i++){   
     setStepv(0, 0, 0, 1);
      delay(_speed);
      setStepv(0, 0, 1, 1);
      delay(_speed);
      setStepv(0, 0, 1, 0);
      delay(_speed);
      setStepv(0, 1, 1, 0);
      delay(_speed);
      setStepv(0, 1, 0, 0);
      delay(_speed);
      setStepv(1, 1, 0, 0);
      delay(_speed);
      setStepv(1, 0, 0, 0);
      delay(_speed);
      setStepv(1, 0, 0, 1);
      delay(_speed);
       int frnt=analogRead(Irvpin);
      Serial.println(frnt);
      m++;
      if(frnt>thres)
      {
        k++;
      }
      else if(m-k>70)
      break;
    }
  }else{ // Stepper motor backward
     for(int i=_step;i<0;i++){  
      setStepv(1, 0, 0, 1);
      delay(_speed); 
      setStepv(1, 0, 0, 0);
      delay(_speed);
      setStepv(1, 1, 0, 0);
      delay(_speed);
      setStepv(0, 1, 0, 0);
      delay(_speed);
      setStepv(0, 1, 1, 0);
      delay(_speed);
      setStepv(0, 0, 1, 0);
      delay(_speed);
      setStepv(0, 0, 1, 1);
      delay(_speed); 
      setStepv(0, 0, 0, 1);
      delay(_speed);
      
    }
   }
}

void Step(int _step)//Stepper motor rotation
{
  if(_step>=0){  // Stepper motor forward
    for(int i=0;i<_step;i++){   
     setStep(0, 0, 0, 1);
      delay(_speed);
      setStep(0, 0, 1, 1);
      delay(_speed);
      setStep(0, 0, 1, 0);
      delay(_speed);
      setStep(0, 1, 1, 0);
      delay(_speed);
      setStep(0, 1, 0, 0);
      delay(_speed);
      setStep(1, 1, 0, 0);
      delay(_speed);
      setStep(1, 0, 0, 0);
      delay(_speed);
      setStep(1, 0, 0, 1);
      delay(_speed);
       int a=analogRead(Irhpin);
      Serial.println(a);
      if(a>thres2)
      {
        j++;
      }
    }
  }else{ // Stepper motor backward
     for(int i=_step;i<0;i++){  
      setStep(1, 0, 0, 1);
      delay(_speed); 
      setStep(1, 0, 0, 0);
      delay(_speed);
      setStep(1, 1, 0, 0);
      delay(_speed);
      setStep(0, 1, 0, 0);
      delay(_speed);
      setStep(0, 1, 1, 0);
      delay(_speed);
      setStep(0, 0, 1, 0);
      delay(_speed);
      setStep(0, 0, 1, 1);
      delay(_speed); 
      setStep(0, 0, 0, 1);
      delay(_speed);
      
    }
   }
}
void setStep(int a, int b, int c, int d)  
{  
    digitalWrite(Pin0, a);     
    digitalWrite(Pin1, b);     
    digitalWrite(Pin2, c);     
    digitalWrite(Pin3, d);     
}

void setStepv(int a, int b, int c, int d)  
{  
    digitalWrite(Pn0, a);     
    digitalWrite(Pn1, b);     
    digitalWrite(Pn2, c);     
    digitalWrite(Pn3, d);     
}

     
     void callibrate()
    {
      for(int i=0;i<40;i++)
    {
      digitalWrite(b2,0);
     digitalWrite(b1,1);
     digitalWrite(a2,1);
     digitalWrite(a1,0);
     analogWrite(a_pwm,100);
     analogWrite(b_pwm,100);
     delay(25);
     findthreshold();
    }
    Serial.println("Out of 1st callibrate");
    for(int i=0;i<75;i++)
    {
      digitalWrite(b2,1);
     digitalWrite(b1,0);
     digitalWrite(a2,0);
     digitalWrite(a1,1);
     analogWrite(a_pwm,100);
     analogWrite(b_pwm,100);
     delay(25);
     findthreshold();
    }
    for(int i=0;i<40;i++)
    {
      digitalWrite(b2,0);
     digitalWrite(b1,1);
     digitalWrite(a2,1);
     digitalWrite(a1,0);
     analogWrite(a_pwm,100);
     analogWrite(b_pwm,100);
     delay(25);
     findthreshold();
    }
    Serial.println("Out of 2nd callibrate"); 
    THRESHOLD=(maxi+mini)/2;
    Serial.print(THRESHOLD);
    }
    
    void findthreshold()
    {
        s[0]=analogRead(S0);
        s[1]=analogRead(S1);
        s[2]=analogRead(S2);
        s[3]=analogRead(S3);
        s[4]=analogRead(S4);
       for(int i =0;i<5;i++)
       {
         Serial.print(s[i]);
         Serial.print("\t");
         if (maxi>s[i])
                 maxi=s[i];
         if(s[i]>mini)
                 mini=s[i];        
       }
       Serial.print("\n");
    }
void stopM()
   {
     digitalWrite(a1,LOW);
     digitalWrite(a2,LOW);
     digitalWrite(b1,LOW);
     digitalWrite(b2,LOW);
     analogWrite(a_pwm,0);
     analogWrite(b_pwm,0);
     delay(100);
     Serial.print("stop\n");
   }
   
   void goright()
     {
       /*digitalWrite(a1,HIGH);
       digitalWrite(a2,LOW);
       digitalWrite(b1,HIGH);
       digitalWrite(b2,LOW);
       analogWrite(a_pwm,150);
       analogWrite(b_pwm,150);
       delay(60);*/
        Serial.print("GOING SHARP RIGHT\n");
       //do
       //{
         digitalWrite(a1,LOW);
     digitalWrite(a2,HIGH);
     digitalWrite(b1,HIGH);
     digitalWrite(b2,LOW);
     analogWrite(a_pwm,145);
     analogWrite(b_pwm,120);
     delay(400);
     readInput();
    
      /*digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     digitalWrite(b1,HIGH);
     digitalWrite(b2,LOW);
     analogWrite(a_pwm,0);
     analogWrite(b_pwm,0);*/
     delay(50);
    /* if(cmp_value=="BWWWB"||cmp_value=="BBWBB")
           break;
       }while (1);
      //allign();*/
      stopM();
      follow_line();
     }
     
     void goleft()
     {
       /*digitalWrite(a1,HIGH);
       digitalWrite(a2,LOW);
       digitalWrite(b1,HIGH);
       digitalWrite(b2,LOW);
       analogWrite(a_pwm,70);
       analogWrite(b_pwm,70);
       delay(60);*/
       Serial.print("GOING SHARP LEFT 3\n");
       
         digitalWrite(b1,LOW);
     digitalWrite(b2,HIGH);
     digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     analogWrite(a_pwm,155);
     analogWrite(b_pwm,120);
     delay(200);
     stopM();
     
    /* digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     digitalWrite(b1,HIGH);
     digitalWrite(b2,LOW);
     analogWrite(a_pwm,0);
     analogWrite(b_pwm,0);*/
     //delay(50);
     //if(cmp_value=="BBWWWBB" || cmp_value=="BBBWBBB")
        //   break;
       //}while (1);
       //allign();
     }
     void pid_goleft()
     {
       /*digitalWrite(a1,HIGH);
       digitalWrite(a2,LOW);
       digitalWrite(b1,HIGH);
       digitalWrite(b2,LOW);
       analogWrite(a_pwm,70);
       analogWrite(b_pwm,70);
       delay(60);*/

       delay(300);
       Serial.print("GOING SHARP LEFT 1\n");
       do{
        Serial.println(T);
         digitalWrite(b1,LOW);
     digitalWrite(b2,HIGH);
     digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     analogWrite(a_pwm,130);
     analogWrite(b_pwm,110);
     Serial.println(T);
     delay(10);
     readInput();
     
    /* digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     digitalWrite(b1,HIGH);
     digitalWrite(b2,LOW);
     analogWrite(a_pwm,0);
     analogWrite(b_pwm,0);*/
     //delay(50);
     if(cmp_value=="BBWWWBB" || cmp_value=="BBBWWWB"||cmp_value=="BBBBWWW")
           break;
       }while (1);
       //allign();
       stopM();
       delay(300);
       readInput();
      follow_line();
     }

void pid_goleft2()
     {
       /*digitalWrite(a1,HIGH);
       digitalWrite(a2,LOW);
       digitalWrite(b1,HIGH);
       digitalWrite(b2,LOW);
       analogWrite(a_pwm,70);
       analogWrite(b_pwm,70);
       delay(60);*/

       delay(300);
       Serial.print("GOING SHARP LEFT 2\n");
       do{
         digitalWrite(b1,LOW);
     digitalWrite(b2,HIGH);
     digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     analogWrite(a_pwm,130);
     analogWrite(b_pwm,110);
     Serial.println(T);
     delay(10);
     readInput();
     
    /* digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     digitalWrite(b1,HIGH);
     digitalWrite(b2,LOW);
     analogWrite(a_pwm,0);
     analogWrite(b_pwm,0);*/
     //delay(50);
     if(cmp_value=="BBWWWBB")
           break;
       }while (1);
       //allign();
       stopM();
       delay(300);
       readInput();
      follow_line();
     }

      void pid_goright()
     {
       /*digitalWrite(a1,HIGH);
       digitalWrite(a2,LOW);
       digitalWrite(b1,HIGH);
       digitalWrite(b2,LOW);
       analogWrite(a_pwm,70);
       analogWrite(b_pwm,70);
       delay(60);*/
       delay(300);
       Serial.print("GOING SHARP RIGHT\n");
       do{
         digitalWrite(b1,HIGH);
     digitalWrite(b2,LOW);
     digitalWrite(a1,LOW);
     digitalWrite(a2,HIGH);
     analogWrite(a_pwm,155);
     analogWrite(b_pwm,140);
     delay(10);
     readInput();
     
    /* digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     digitalWrite(b1,HIGH);
     digitalWrite(b2,LOW);
     analogWrite(a_pwm,0);
     analogWrite(b_pwm,0);*/
     //delay(50);
     if(cmp_value=="BBWWWBB" || cmp_value=="WWWBBBB"||cmp_value=="BWWWBBB")
           break;
       }while (1);
       //allign();
       stopM();
       delay(300);
       readInput();
      follow_line();
     }
    /*void allign()
     {
       readInput();
       if(cmp_value=="BBWBB" || cmp_value=="BWWWB" || cmp_value=="WWWWW")
       {
         digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     digitalWrite(b1,HIGH);
     digitalWrite(b2,LOW);
     analogWrite(a_pwm,75);
     analogWrite(b_pwm,75);
     delay(50);
       }
     else if(cmp_value=="BBWWW" || cmp_value=="BBBWW" || cmp_value=="BBBWB"|| cmp_value=="BBWWB"||cmp_value=="BWWWW")
       {
         digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     digitalWrite(b1,HIGH);
     digitalWrite(b2,LOW);
     analogWrite(a_pwm,75);
     analogWrite(b_pwm,50);
     delay(50);
       }
     else if(cmp_value=="WWWBB"|| cmp_value=="WWBBB" || cmp_value=="BWBBB"|| cmp_value=="BWWBB"|| cmp_value=="WWWWB")
       {
         digitalWrite(a1,HIGH);
     digitalWrite(a2,LOW);
     digitalWrite(b1,HIGH);
     digitalWrite(b2,LOW);
     analogWrite(a_pwm,50);
     analogWrite(b_pwm,75);
     delay(50);
       } 
     }*/
