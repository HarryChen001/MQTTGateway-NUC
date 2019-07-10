#pragma once
#ifndef _MY_DATA_H_
#define _MY_DADA_H_

#include <stdint.h>
#include <string>
#include <queue>
#include <map>

#include "soft_mymodbus.h"

#define MqttConnect 1
#define SerialNums 10
#define DeviceNums 10
#define ThemeCtrlNums 1
#define ThemeUploadNums 1
#define AllVarNums 500
#define VarNumsPeriodDev 200
#define UploadVarNums 500
#define uploadperiod 50

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

enum enumdatatype {
	bool_type, uint16, uint32, uint64, int16, int32, int64, float_type, double_type, data_no_type
};
enum enumregtype {
	B0, B1, W3, W4, reg_no_type
};

//�˿ڵĲ�������
typedef struct _PortInfo_t {
	int id;					//Poqrtinfo id
	int PortId;				//�趨��PortId
	int PortNum;
	int PortType;			//�˿�����
	int baud;				//������
	char Parity[10];		//��żλ
	int DataBits;			//����λ
	int StopBits;			//ֹͣλ
	int DelayTime;			//�ȴ�ʱ��
	char PortName[20];		//��������
	int gpio;

	int portcount;
}PortInfo_t;

//����������ز���
typedef struct _ThemeCtrl_t {
	int id;					//ThemeCtrl id
	int Enable;				//ʹ�ܿ�������
	int MqttId;				//�󶨵�MqttId
	int	QosPub;				//"����"�����Qos
	int QosSub;				//"����"�����Qos
	char proto[10];
	char CtrlName[100];		//�������������
	char CtrlPub[100];		//����������
	char CtrlSub[100];		//���ĵ�����
	int uartID;
}ThemeCtrl_t;

//�ϴ�������ز���
typedef struct _ThemeUpload_t {
	int id;					//ThemeUpload Id
	int Enable;				//�����ϴ�����
	int MqttId;				//�󶨵�MqttId
	int UploadId;			//�趨�ķ�������Id
	int PubPeriod;			//������Ϣ��ʱ����
	int QosPub;				//���������Qos
	char CtrlPub[100];		//����������
	char UploadName[100];	//��ʱ�ϴ����������
	char Proto[20];			//ͨ��Э��
}ThemeUpload_t;

//�ϴ�����󶨵ı���
typedef struct _ThemeUploadList_t {
	int id;					//ThemeUploadList Id
	int Enable;				//ʹ���ϴ�
	int MqttId;				//�󶨵�MqttId
	int UploadId;			//�󶨵�UploadId
	int DevId;				//�󶨵��豸��Ϣ�е�DevInfo��id
	int VarId;				//�󶨱�����VarParam��Id
	std::string VarName;		//������

	int UploadCount;
}ThemeUploadList_t;

typedef struct _VarParam_t {
	int id;					//VarParam Id
	std::string DataType;		//��������
	int DecimalsBit;		//С��λ
	int obj;				//
	int RegAdr;				//�Ĵ�����ַ
	std::string RegType;		//�Ĵ������ͣ�B0���߼���Ȧ����B1�����������룩��W3�����ּĴ�������W4������Ĵ�����
	std::string VarName;		//��������
	int PortId;				//�󶨵Ĵ���Id
	int DevId;				//�󶨵��豸Id
	float modules;			//ת��ϵ��

	int VarCount;		//����������
}VarParam_t;
//�豸��������Ϣ
typedef struct _DeviceInfo_t {
	int id;					//devinfo id
	int PortId;				//�󶨵�PortId
	int address;				//�豸��ַ
	int regedian;			//�豸��С��
	int byteorder;			//�豸�Ĵ����ߵ�λ
	char DeviceName[100];	//�豸��

	int devcount;
	VarParam_t uploadvarparam[UploadVarNums];
	VarParam_t allvarparams[VarNumsPeriodDev];
	int allvarcount;
	int uploadvarcount;
}DeviceInfo_t;

typedef struct _ConnectInfo_t {
	int id;
	int Session;
	int Enable;
	int KeepAlive;
	int ServerPort;

	char ClientId[100];
	char UserName[100];
	char Password[100];
	int PeriodDef;
	int EnableDef;
	char TopicDef[100];
	int QosDef;
	int EnableAlarm;
	int QosAlarm;
	char TopicAlarm[100];

	char ServerLink[100];
	char MqttName[100];

	int MqttCount;

}ConnectInfo_t;
typedef struct _Allinfo_t {

	modbus_t* fdmodbus;
	PortInfo_t portinfo;

	DeviceInfo_t deviceinfo[DeviceNums];
	int devcount;
}Allinfo_t;
union doubleunion
{
	int64_t int64type;
	double doubletype;
};
union floatunion
{
	int32_t int32type;
	float floattype;
};
typedef struct _Varinfo_t {
	std::string varname;
	double value;
}Varinfo_t;
extern ConnectInfo_t MqttInfo[MqttConnect];
extern PortInfo_t PortInfo[SerialNums];
extern ThemeCtrl_t ThemeCtrl[ThemeCtrlNums];
extern ThemeUpload_t ThemeUpload[ThemeUploadNums];

extern enumdatatype datatype;
extern Allinfo_t Allinfo[15];
extern std::map<std::string, double>var;
extern std::map<std::string, double>var_write;
extern std::queue<Varinfo_t>queue_var_write;
extern Varinfo_t varinfo;

#endif // !_MY_DATA_H_
