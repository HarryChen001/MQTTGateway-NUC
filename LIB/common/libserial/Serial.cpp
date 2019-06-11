#include "Serial.h"
#include "Timestamp.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include "sqlite3pp/sqlite3pp.h"
#include "sqlite3pp/sqlite3ppext.h"
int test0()
{
	return 100;
}
Serial::Serial(char* dev) :stream()
{
	printf("create database");
	open(dev, 115200, 8, 'N', 1);
	
	pthread_mutex_init(&mutex, 0);
}
Serial::Serial() : stream()
{
	pthread_mutex_init(&mutex, 0);
}
Serial::~Serial()
{
	close();
}

void* receiveThread(void* arg)
{
	Serial* serial = (Serial*)arg;
	char buf[1024];
	printf("%s\n", "created thread to read serial");
	while (true)
	{
		pthread_testcancel();
		int len = read(serial->fd, buf, sizeof(buf));
		if (len > 0)
		{
			pthread_mutex_lock(&(serial->mutex));
			serial->stream.append(buf, len);
			pthread_mutex_unlock(&(serial->mutex));
			puts(buf);
			memset(buf, 0, sizeof(buf));
		}
		usleep(1000);
	}
}

int Serial::open(const char* dev, int baud, int dataBits, char parityMode, int stopBits)
{
	fd = ::open(dev, O_RDWR, O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
	{
		printf("open serial port fail!\n");
		return DEV_NOT_FOUND;
	}
	struct termios newtio;

	memset(&newtio, 0, sizeof(newtio));

	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	//���ò����ʡ�
	int baudspeed = transformBaud(baud);
	if (baudspeed < 0)
	{
		printf("Unsupport baud!\n");
		return BAUD_NOT_SUPPORTED;
	}
	cfsetispeed(&newtio, baudspeed);
	cfsetospeed(&newtio, baudspeed);

	int databits = transformDataBits(dataBits);
	if (databits < 0)
	{
		printf("Unsupport databits!\n");
		return DATABITS_NOT_SUPPORTED;
	}
	newtio.c_cflag |= databits;

	//����У��λ
	switch (parityMode)
	{
		case 'N':
		{
			newtio.c_cflag &= ~PARENB;
			newtio.c_iflag &= ~INPCK;
			break;
		}
		case 'O':
		{
			newtio.c_cflag |= (PARODD | PARENB);
			newtio.c_iflag |= INPCK;
			break;
		}
		case 'E':
		{
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			newtio.c_iflag |= INPCK;
			break;
		}
		default:
		{	printf("Unsupported Parity.\n");
			return PARITYMODE_NOT_SUPPORTED;
		}
	}

	//����ֹͣλ��ֵΪ1 or 2
	switch (stopBits)
	{
		case 1:
		{
			newtio.c_cflag &= ~CSTOPB;
			break;
		}
		case 2:
		{
			newtio.c_cflag |= CSTOPB;
			break;
		}
		default:
		{
			printf("Unsupported Stop_bits.\n");
			return STOPBITS_NOT_SUPPORTED;
		}

	}
	//ˢ��������������
	tcflush(0, TCIOFLUSH);
	//�������ã�TCSANOW��ʾ���ĺ�������Ч��
	if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
	{
		//�ж��Ƿ񼤻�ɹ���
		printf("Com set error\n");
		return CONFIG_FAIL;
	}
	else
	{
		printf("setting serial success!\n");
	}

	return OK;
}
int Serial::openreadthread()
{
	if (pthread_create(&tid, 0, receiveThread, this) != 0)
	{
		printf("created thread fail!\n");
		return NEW_THREAD_FAIL;
	}
	return 0;
}

int Serial::close()
{
	if (fd >= 0)
	{
		::close(fd);
		if(tid)
		{
			pthread_cancel(tid);
			pthread_mutex_destroy(&mutex);
		}
		fd = -1;
	}
	return 0;
}

int Serial::write(const char* data, int len)
{
	return ::write(fd, data, len);
}

int Serial::available()
{
	int len = stream.getLength();
	return len;
}

int Serial::peek(char* buf, int len)
{
	len = stream.peek(buf, len);
	return len;
}

int Serial::read(char* buf, int len, int timeout)
{
	timestamp_t start = Timestamp::now();
	int total = 0;
	while (total < len)
	{
		pthread_mutex_lock(&mutex);
		int readLen = stream.take(buf + total, len - total);
		pthread_mutex_unlock(&mutex);
		if (readLen > 0)
			total += readLen;
		timestamp_t now = Timestamp::now();
		if (now >= start + timeout)
			break;
		usleep(1000);
	}
	return total;
}

int Serial::read(char* buf, int maxLen, const char* end, int timeout, int* recvLen)
{
	int endLen = strlen(end);
	timestamp_t start = Timestamp::now();
	int total = 0;
	while (total < maxLen)
	{
		pthread_mutex_lock(&mutex);
		int readLen = stream.take(buf + total, 1);
		pthread_mutex_unlock(&mutex);
		if (readLen > 0)
		{
			total += readLen;
			if (endsWith(buf, total, end, endLen))
			{
				if (recvLen != 0)
					* recvLen = total;
				return READ_END;
			}
		}
		timestamp_t now = Timestamp::now();
		if (now >= start + timeout)
			return READ_TIMEOUT;
		usleep(1000);
	}
	return READ_BUFFER_FULL;
}

int Serial::transformBaud(int baud)
{
	int map[][2] = { {2400,B2400},{4800,B4800},{9600,B9600},
				  {19200,B19200},{38400,B38400},{57600,B57600},
				  {115200,B115200} };
	for (int i = 0; i < sizeof(map) / sizeof(int) / 2; i++)
		if (map[i][0] == baud)
			return map[i][1];
	return -1;
}

int Serial::transformDataBits(int dataBits)
{
	int map[][2] = { {5,CS5},{6,CS6},{7,CS7},{8,CS8} };
	for (int i = 0; i < sizeof(map) / sizeof(int) / 2; i++)
		if (map[i][0] == dataBits)
			return map[i][1];
	return -1;
}

bool Serial::endsWith(const char* str, int strLen, const char* end, int endLen)
{
	if (strLen < endLen)
		return false;
	for (int i = endLen - 1; i >= 0; i--)
		if (end[i] != str[strLen - endLen + i])
			return false;
	return true;
}
