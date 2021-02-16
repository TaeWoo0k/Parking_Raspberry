#include<wiringPi.h>	//GPIO�� �����ϱ� ���� wiringPi ��������� include�Ѵ�.
#include<stdio.h>		//standard input output ��������� include�Ѵ�.
#include<softPwm.h>	//Servo���͸� ������ �� �ʿ��� Pulse�� ����ϱ����� ����� include�Ѵ�.

#define SERVO 25	//servo���͸� GPIO 25������ �����Ѵ�.
#define SERVO1 13	//�ι�° servo���͸� GPIO 13������ �����Ѵ�.

#define Button 16	//PushButton�� GPIO 16������ �����Ѵ�.

#define TP1 18		//�����ڸ��� ���̴� �����ļ��� trigger�� GPIO 18������ �����Ѵ�.
#define EP1 27		//�����ڸ��� ���̴� �����ļ��� echo�� GPIO 27������ �����Ѵ�.

#define TP2 19		//�ι�° �����ڸ� �����ļ��� TRIGGER�� GPIO 19������ �����Ѵ�.
#define EP2 26		//�ι�° �����ڸ� �����ļ��� ECHO�� GPIO 19������ �����Ѵ�.

#define TP4   23		//�ⱸ�� ����ϴ� �����ļ��� TRIGGER�� GPIO 23������ �����Ѵ�.
#define EP4   22		//�ⱸ�� ����ϴ� �����ļ��� ECHO�� GPIO 22������ �����Ѵ�.

#define LED1 21		//���ε� ������ �ϴ� ��� LED�� GPIO 21������ �����Ѵ�. 

#define LED_R1 17	//�����ڸ� ������ ��Ÿ���� LED�� ������ ���� GPIO 17���� �����Ѵ�.
#define LED_G1 24	//�����ڸ� ������ ��Ÿ���� LED�� ��� ���� GPIO 24���� �����Ѵ�.

#define LED_R2 5	//�����ڸ� ������ ��Ÿ���� LED�� ������ ���� GPIO 5���� �����Ѵ�.
#define LED_G2 6	//�����ڸ� ������ ��Ÿ���� LED�� ������ ���� GPIO 6���� �����Ѵ�.


const int TPTable[2] = { TP1,TP2 };	//���� �ڸ��� ����� �����ļ����� Trigger������ �迭����.
const int EPTable[2] = { EP1,EP2 };		//���� �ڸ��� ����� �����ļ����� Echo������ �迭����.
const int LED_RTable[2] = { LED_R1,LED_R2 };	//�����Ǹ� led���� ���������� �ϱ����� �迭����
const int LED_GTable[2] = { LED_G1,LED_G2 };	//�����Ǹ� led���� ������� �ϱ����� �迭����
float getDistance(int TP, int EP)
{//������ ������ �̿��Ͽ� �Ÿ��� ����ϱ� ���� �Լ�
    float fDistance;
    int nStartTime, nEndTime;
    //������ ��ȣ�� Trigger���� ������ Echo���� ������ �� �ð��� ����ϱ� ���� �� ���� ���� ����.

    digitalWrite(TP, LOW);	//�Ű����� TP�� LOW�� Write�Ѵ�.
    delayMicroseconds(2);	//2Micro second��ŭ delay�Ѵ�.
    digitalWrite(TP, HIGH);	//���� TP�� HIGH�� Write�Ѵ�.
    delayMicroseconds(10);	//10 Microsecond��ŭ delay�Ѵ�.
    digitalWrite(TP, LOW);	//TP�� LOW�� wirte�Ѵ�.
    while (digitalRead(EP) == LOW);
    nStartTime = micros();
    //Trigger���� ���� �����İ��� Echo���� ���� �ʾ��� ���� �ð��� micro������ ����Ͽ� nStartTime������ �ִ´�.
    while (digitalRead(EP) == HIGH);
    nEndTime = micros();
    //Trigger���� ���� �����İ��� Echo���� ���� ���� �ð��� micro������ ����Ͽ� nEndTime ������ �ִ´�.
    fDistance = (nEndTime - nStartTime) * 0.034 / 2;
    //fDistance ������ EndTime���� StartTime�� ���ְ� �ʴ����� �ٲ��ش�.
    return fDistance;	//fDistance�� return�Ѵ�.
}

int LED_OUT(int LED_G, int LED_R, int fDistance) {
    //���� �����Ǿ� ������ ������, �����Ǿ����� ������ ������� ����ϱ� ���� �Լ�.

    int count = 0;
    //count������ �����忡 �ڸ��� ���� ���, �Ա����� ���ܱⰡ ������ �ʰ� �ϱ� ���� �����̴�.
    if (fDistance < 5) { 	//�����ļ����� ���� �Ÿ��� 5cm �̸��̸�
        digitalWrite(LED_G, LOW);
        //LED�� �ʷϻ��� ����,
        digitalWrite(LED_R, HIGH);
        //LED�� �������� �Ҵ�.
        count += 1;
        //���� �� �� �� ���������� count�� 1�� �����ش�.
    }
    else {	//�����ļ����� ���� �Ÿ��� 5cm �̻��� ��
        digitalWrite(LED_R, LOW);
        //�������� ����
        digitalWrite(LED_G, HIGH);
        //������� �Ҵ�.
        count = 0;	//count���� 0���� �ʱ�ȭ�Ѵ�.
    }
    return count;	//count�� return�Ѵ�.
}


int main(void)
{
    int lcount = 0;		//���� ������ ���ε���� �ϴ� ��� led�� ������ �� �� ����ϴ� ����

    int i, j;			//�ݺ����� ����ϱ� ���� ���� i,j

    if (wiringPiSetupGpio() == -1) {	//wiringPiSetupGpio()�� return���� 1�̸� �����Ѵ�.
        return 1;
    }


    softPwmCreate(SERVO, 0, 200);	//Servo���͸� ����ϱ����� pulse�� �����
    pinMode(Button, INPUT);	//ButtonPin�� INPUT���� ����
    int count[2] = { 0,0 };
    for (i = 0; i < 2; i++) {
        pinMode(TPTable[i], OUTPUT);		//TP1,TP2 pinMode�� OUTPUT���� ����
        pinMode(EPTable[i], INPUT);		//EP1,EP2 pinMode�� INPUT���� ����
        pinMode(LED_RTable[i], OUTPUT);	//LED_RED pinMode�� OUTPUT���� ����
        pinMode(LED_GTable[i], OUTPUT);	//LED_ERED pinMode�� OUTPUT���� ����
    }
    softPwmCreate(SERVO1, 0, 200);
    pinMode(TP4, OUTPUT);		//TP4 pinMode�� OUTPUT���� ����
    pinMode(EP4, INPUT);			//EP4 pinMode�� INPUT���� ����
    pinMode(LED1, OUTPUT);		//LED1 (���ε�) pinMode�� OUTPUT���� ����
    digitalWrite(LED1, LOW);		//LED1 (���ε�) �ʱ���¸� LOW(�������� ����)
    while (1) {
        float fDistance[2];
        for (i = 0; i < 2; i++) {
            fDistance[i] = getDistance(TPTable[i], EPTable[i]);	//�����ļ��� 1, 2 �Ÿ� ���� �ҷ���
            printf("Distance %d : %2fcm \n", i, fDistance[i]);	//�Ÿ� �� ȭ�鿡 ���
        }
        for (j = 0; j < 2; j++) {
            count[j] = LED_OUT(LED_GTable[j], LED_RTable[j], fDistance[j]);	//������ �����Ǿ� ������ ������, �����Ǿ����� ������ ������� ����ϱ� ���� �Լ�(�ִ� 2)
        }

        int c = 0;
        for (j = 0; j < 2; j++)
            c += count[j];	//���� �Ǿ� �ִ� ������ ���� Ȯ���� ���� �ݺ���
        printf("count = %d\n", c);
        if (digitalRead(Button) == 1 && c != 2) {	//��ư �۵�
            softPwmWrite(SERVO, 25);		//�Ա��� ���ܱ� �۵�
            lcount = lcount + 1;			//���ε� ������ ���� ���� ���� �ľ� (+1)
            delay(2000);			//�ߺ� ������ ���� ������ 2000ms
            softPwmWrite(SERVO, 15);		//���ܱ� ���� ����
            delay(500);
        }
        int distance = getDistance(TP4, EP4); 		//�ⱸ�� ������ ������ �۵��ϱ� ���� �Լ����
        printf("distance = %dcm\n", distance); 	//�Ÿ��� ���
        printf("lcount = %d\n", lcount);	//���ε� ������ ���� ���� ����

        if (distance < 7) {
            softPwmWrite(SERVO1, 25);	//�ⱸ ���ܱ� ����
            lcount = lcount - 1;		//�ⱸ ���ܱ� �۵� �� ���ε� ���� ���� �� ����
            delay(2000);
            softPwmWrite(SERVO1, 15);	//�ⱸ ���ܱ� ���� ����
        }
        else if (distance > 10) {
            softPwmWrite(SERVO1, 15);	//�ⱸ ���ܱ� ������ ���� if��
            delay(500);
        }
        if (lcount > 0) {
            digitalWrite(LED1, HIGH);	//������ �� �� �̻� ���� ��� ���ε� ����
        }
        else if (lcount == 0) {
            digitalWrite(LED1, LOW);	//������ ���� ��� ���ε� �ҵ�
        }
        softPwmWrite(SERVO, 0);	//�Ա� ���ܱ� �ʱ�ȭ(�������� ����)
        softPwmWrite(SERVO1, 0);	//�ⱸ ���ܱ� �ʱ�ȭ(�������� ����)
        delay(100);
    }
    return 0;
}