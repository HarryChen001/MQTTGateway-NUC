//���ļ�������һ��������
#ifndef SERIAL_H
#define SERIAL_H

#include "Stream.h"
#include <pthread.h>

//������
class Serial
{

public:
	//��У��
	static const int PARITY_NONE = 0;
	//��У��
	static const int PARITY_ODD = 1;
	//żУ��
	static const int PARITY_EVEN = 2;
	//�����ɹ�
	static const int OK = 1;
	//�豸δ�ҵ�
	static const int DEV_NOT_FOUND = -1;
	//��֧�ָò�����
	static const int BAUD_NOT_SUPPORTED = -2;
	//��֧�ָ�����λ��
	static const int DATABITS_NOT_SUPPORTED = -3;
	//��֧�ָ�У��ģʽ
	static const int PARITYMODE_NOT_SUPPORTED = -4;
	//��֧�ָ�ֹͣλ��
	static const int STOPBITS_NOT_SUPPORTED = -5;
	//δ֪���ô���
	static const int CONFIG_FAIL = -6;
	//�����̳߳���
	static const int NEW_THREAD_FAIL = -7;
	//�ɹ�������β��
	static const int READ_END = 1;
	//��ȡ��ʱ
	static const int READ_TIMEOUT = -1;
	//��ȡʱ��������
	static const int READ_BUFFER_FULL = -2;

private:
	//�����豸�ļ�������
	int fd;
	//�ַ���
	Stream stream;
	//��̨�����߳�
	pthread_t tid;
	//���ַ����ӵ���
	pthread_mutex_t mutex;

public:
	Serial();
	Serial(char* dev);
	~Serial();
	//�������ڣ�����Ϊ���豸���������ʡ�����λ����У��ģʽ��ֹͣλ�������غ���ִ�н��
	int open(const char* dev, int baud, int dataBits, char parityMode, int stopBits);
	//�رմ���
	int close();
	//д���ڣ�����Ϊ�����ݡ����ȣ�����ʵ��д�볤��
	int write(const char* data, int len);
	//��ȡ�ɶ�����
	int available();
	//�����ڣ������Ƴ����ݣ�����ʵ�ʶ�ȡ����
	int peek(char* buf, int len);
	//�����ڣ�ֱ���յ�Ԥ�ڳ��ȵ����ݻ�ʱ������Ϊ�����ջ�������Ԥ�ڽ��ճ��ȡ���ʱ�����룩,����ʵ�ʶ�ȡ����
	int read(char* buf, int len, int timeout);
	//�����ڣ�ֱ������Ԥ�ڵĽ�β���򻺳�������ʱ������Ϊ�����ջ���������󳤶ȡ�Ԥ�ڽ�β������ʱ�����룩��ʵ�ʽ��ճ��ȣ�����READ_END��READ_TIMEOUT��READ_BUFFER_FULL
	int read(char* buf, int maxLen, const char* end, int timeout, int* recvLen);

	int openreadthread();

	int read_wait(char* buff,int len,int timeout);

private:
	//�������Ͳ�����ת��Ϊϵͳ���ò���
	int transformBaud(int baud);
	//������������λ��ת��Ϊϵͳ���ò���
	int transformDataBits(int dataBits);
	long long getTimestamp();
	//�ж��ַ���str�Ƿ����ַ���end��β
	bool endsWith(const char* str, int strLen, const char* end, int endLen);

	//��̨�����̺߳���
	friend void* receiveThread(void* arg);

};

#endif
