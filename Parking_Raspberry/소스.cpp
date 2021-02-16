#include<wiringPi.h>	//GPIO를 제어하기 위해 wiringPi 헤더파일을 include한다.
#include<stdio.h>		//standard input output 헤더파일을 include한다.
#include<softPwm.h>	//Servo모터를 제어할 때 필요한 Pulse를 사용하기위한 헤더를 include한다.

#define SERVO 25	//servo모터를 GPIO 25번으로 정의한다.
#define SERVO1 13	//두번째 servo모터를 GPIO 13번으로 정의한다.

#define Button 16	//PushButton을 GPIO 16번으로 정의한다.

#define TP1 18		//주차자리에 쓰이는 초음파센서 trigger를 GPIO 18번으로 정의한다.
#define EP1 27		//주차자리에 쓰이는 초음파센서 echo를 GPIO 27번으로 정의한다.

#define TP2 19		//두번째 주차자리 초음파센서 TRIGGER를 GPIO 19번으로 정의한다.
#define EP2 26		//두번째 주차자리 초음파센서 ECHO를 GPIO 19번으로 정의한다.

#define TP4   23		//출구에 사용하는 초음파센서 TRIGGER를 GPIO 23번으로 정의한다.
#define EP4   22		//출구에 사용하는 초음파센서 ECHO를 GPIO 22번으로 정의한다.

#define LED1 21		//가로등 역할을 하는 백색 LED를 GPIO 21번으로 정의한다. 

#define LED_R1 17	//주차자리 유무를 나타내는 LED의 빨간색 핀을 GPIO 17번에 정의한다.
#define LED_G1 24	//주차자리 유무를 나타내는 LED의 녹색 핀을 GPIO 24번에 정의한다.

#define LED_R2 5	//주차자리 유무를 나타내는 LED의 빨간색 핀을 GPIO 5번에 정의한다.
#define LED_G2 6	//주차자리 유무를 나타내는 LED의 빨간색 핀을 GPIO 6번에 정의한다.


const int TPTable[2] = { TP1,TP2 };	//주차 자리에 사용할 초음파센서의 Trigger값들의 배열선언.
const int EPTable[2] = { EP1,EP2 };		//주차 자리에 사용할 초음파센서의 Echo값들의 배열선언.
const int LED_RTable[2] = { LED_R1,LED_R2 };	//주차되면 led색을 빨간색으로 하기위한 배열선언
const int LED_GTable[2] = { LED_G1,LED_G2 };	//주차되면 led색을 녹색으로 하기위한 배열선언
float getDistance(int TP, int EP)
{//초음파 센서를 이용하여 거리를 계산하기 위한 함수
    float fDistance;
    int nStartTime, nEndTime;
    //초음파 신호를 Trigger에서 보내고 Echo에서 수신할 때 시간을 계산하기 위한 두 개의 변수 선언.

    digitalWrite(TP, LOW);	//매개변수 TP를 LOW로 Write한다.
    delayMicroseconds(2);	//2Micro second만큼 delay한다.
    digitalWrite(TP, HIGH);	//변수 TP를 HIGH로 Write한다.
    delayMicroseconds(10);	//10 Microsecond만큼 delay한다.
    digitalWrite(TP, LOW);	//TP를 LOW로 wirte한다.
    while (digitalRead(EP) == LOW);
    nStartTime = micros();
    //Trigger에서 보낸 초음파값을 Echo에서 받지 않았을 때의 시간을 micro단위로 계산하여 nStartTime변수에 넣는다.
    while (digitalRead(EP) == HIGH);
    nEndTime = micros();
    //Trigger에서 보낸 초음파값을 Echo에서 받을 때의 시간을 micro단위로 계산하여 nEndTime 변수에 넣는다.
    fDistance = (nEndTime - nStartTime) * 0.034 / 2;
    //fDistance 변수에 EndTime에서 StartTime을 빼주고 초단위로 바꿔준다.
    return fDistance;	//fDistance를 return한다.
}

int LED_OUT(int LED_G, int LED_R, int fDistance) {
    //차가 주차되어 있으면 빨간불, 주차되어있지 않으면 녹색불을 출력하기 위한 함수.

    int count = 0;
    //count변수는 주차장에 자리가 없을 경우, 입구에서 차단기가 열리지 않게 하기 위한 변수이다.
    if (fDistance < 5) { 	//초음파센서와 차의 거리가 5cm 미만이면
        digitalWrite(LED_G, LOW);
        //LED의 초록색을 끄고,
        digitalWrite(LED_R, HIGH);
        //LED의 빨간색을 켠다.
        count += 1;
        //차가 한 대 더 주차했으니 count에 1을 더해준다.
    }
    else {	//초음파센서와 차의 거리가 5cm 이상일 때
        digitalWrite(LED_R, LOW);
        //적색불을 끄고
        digitalWrite(LED_G, HIGH);
        //녹색불을 켠다.
        count = 0;	//count값을 0으로 초기화한다.
    }
    return count;	//count를 return한다.
}


int main(void)
{
    int lcount = 0;		//차가 들어오면 가로등역할을 하는 백색 led가 켜지게 할 때 사용하는 변수

    int i, j;			//반복문을 사용하기 위한 변수 i,j

    if (wiringPiSetupGpio() == -1) {	//wiringPiSetupGpio()의 return값이 1이면 종료한다.
        return 1;
    }


    softPwmCreate(SERVO, 0, 200);	//Servo모터를 사용하기위해 pulse를 만든다
    pinMode(Button, INPUT);	//ButtonPin을 INPUT으로 설정
    int count[2] = { 0,0 };
    for (i = 0; i < 2; i++) {
        pinMode(TPTable[i], OUTPUT);		//TP1,TP2 pinMode를 OUTPUT으로 설정
        pinMode(EPTable[i], INPUT);		//EP1,EP2 pinMode를 INPUT으로 설정
        pinMode(LED_RTable[i], OUTPUT);	//LED_RED pinMode를 OUTPUT으로 설정
        pinMode(LED_GTable[i], OUTPUT);	//LED_ERED pinMode를 OUTPUT으로 설정
    }
    softPwmCreate(SERVO1, 0, 200);
    pinMode(TP4, OUTPUT);		//TP4 pinMode를 OUTPUT으로 설정
    pinMode(EP4, INPUT);			//EP4 pinMode를 INPUT으로 설정
    pinMode(LED1, OUTPUT);		//LED1 (가로등) pinMode를 OUTPUT으로 설정
    digitalWrite(LED1, LOW);		//LED1 (가로등) 초기상태를 LOW(꺼짐으로 설정)
    while (1) {
        float fDistance[2];
        for (i = 0; i < 2; i++) {
            fDistance[i] = getDistance(TPTable[i], EPTable[i]);	//초음파센서 1, 2 거리 값을 불러옴
            printf("Distance %d : %2fcm \n", i, fDistance[i]);	//거리 값 화면에 출력
        }
        for (j = 0; j < 2; j++) {
            count[j] = LED_OUT(LED_GTable[j], LED_RTable[j], fDistance[j]);	//차량이 주차되어 있으면 빨간불, 주차되어있지 않으면 녹색불을 출력하기 위한 함수(최대 2)
        }

        int c = 0;
        for (j = 0; j < 2; j++)
            c += count[j];	//주차 되어 있는 차량의 개수 확인을 위한 반복문
        printf("count = %d\n", c);
        if (digitalRead(Button) == 1 && c != 2) {	//버튼 작동
            softPwmWrite(SERVO, 25);		//입구쪽 차단기 작동
            lcount = lcount + 1;			//가로등 점등을 위한 차량 개수 파악 (+1)
            delay(2000);			//중복 방지를 위한 딜레이 2000ms
            softPwmWrite(SERVO, 15);		//차단기 원상 복귀
            delay(500);
        }
        int distance = getDistance(TP4, EP4); 		//출구쪽 초음파 센서를 작동하기 위한 함수출력
        printf("distance = %dcm\n", distance); 	//거리값 출력
        printf("lcount = %d\n", lcount);	//가로등 점등을 위한 차량 개수

        if (distance < 7) {
            softPwmWrite(SERVO1, 25);	//출구 차단기 열림
            lcount = lcount - 1;		//출구 차단기 작동 시 가로등 감지 차량 수 감소
            delay(2000);
            softPwmWrite(SERVO1, 15);	//출구 차단기 원상 복귀
        }
        else if (distance > 10) {
            softPwmWrite(SERVO1, 15);	//출구 차단기 고정을 위한 if문
            delay(500);
        }
        if (lcount > 0) {
            digitalWrite(LED1, HIGH);	//차량이 한 대 이상 있을 경우 가로등 점등
        }
        else if (lcount == 0) {
            digitalWrite(LED1, LOW);	//차량이 없을 경우 가로등 소등
        }
        softPwmWrite(SERVO, 0);	//입구 차단기 초기화(떨림현상 제거)
        softPwmWrite(SERVO1, 0);	//출구 차단기 초기화(떨림현상 제거)
        delay(100);
    }
    return 0;
}