#pragma once
#ifndef _MY_DATA_H_
#define _MY_DADA_H_

#include <stdint.h>
#include <string>
#include <queue>
#include <map>

#include "soft_mymodbus.h"

typedef void (rts)(modbus_t*, int);

#define MqttConnect 1
#define SerialNums 10
#define DeviceNums 10
#define ThemeCtrlNums 1
#define ThemeUploadNums 1
#define AllVarNums 800
#define VarNumsPeriodDev 200
#define UploadVarNums 800
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

//端口的参数设置
typedef struct _PortInfo_t {
	int id;					//Portinfo id
	int PortId;				//设定的PortId
	int PortNum;
	int PortType;			//端口类型
	int baud;				//波特率
	char Parity[10];		//奇偶位
	int DataBits;			//数据位
	int StopBits;			//停止位
	int DelayTime;			//等待时间
	char PortName[20];		//串口命名
	int gpio;

	int portcount;
}PortInfo_t;

//控制主题相关参数
typedef struct _ThemeCtrl_t {
	int id;					//ThemeCtrl id
	int Enable;				//使能控制主题
	int MqttId;				//绑定的MqttId
	int	QosPub;				//"发布"主题的Qos
	int QosSub;				//"订阅"主题的Qos
	char proto[10];
	char CtrlName[100];		//控制主题的命名
	char CtrlPub[100];		//发布的主题
	char CtrlSub[100];		//订阅的主题
	int uartID;
}ThemeCtrl_t;

//上传主题相关参数
typedef struct _ThemeUpload_t {
	int id;					//ThemeUpload Id
	int Enable;				//是能上传主题
	int MqttId;				//绑定的MqttId
	int UploadId;			//设定的发布主题Id
	int PubPeriod;			//发布消息的时间间隔
	int QosPub;				//发布主题的Qos
	char CtrlPub[100];		//发布的主题
	char UploadName[100];	//定时上传主题的命名
	char Proto[20];			//通信协议
}ThemeUpload_t;

//上传主题绑定的变量
typedef struct _ThemeUploadList_t {
	int id;					//ThemeUploadList Id
	int Enable;				//使能上传
	int MqttId;				//绑定的MqttId
	int UploadId;			//绑定的UploadId
	int DevId;				//绑定的设备信息中的DevInfo的id
	int VarId;				//绑定变量表VarParam的Id
	std::string VarName;	//变量名

	int UploadCount;
}ThemeUploadList_t;

typedef struct _VarParam_t {
	int id;					//VarParam Id
	std::string DataType;	//变量类型
	int DecimalsBit;		//小数位
	int obj;				//
	int RegAdr;				//寄存器地址
	std::string RegType;	//寄存器类型：B0（逻辑线圈），B1（开关量输入），W3（保持寄存器），W4（输入寄存器）
	std::string VarName;	//变量命名
	int PortId;				//绑定的串口Id
	int DevId;				//绑定的设备Id
	float modules;			//转换系数
	enumdatatype datatype;

	int VarCount;			//变量的数量
}VarParam_t;
//设备的设置信息
typedef struct _DeviceInfo_t {
	int id;					//devinfo id
	int PortId;				//绑定的PortId
	int address;				//设备地址
	int regedian;			//设备大小端
	int byteorder;			//设备寄存器高低位
	char DeviceName[100];	//设备名

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
	bool write_flag;
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

extern Allinfo_t Allinfo[15];
extern std::map<std::string, double>var;	//store the data from modbus(read from modbus);
extern std::queue<Varinfo_t>queue_var_write;	//store the queue to modbus(write to modbus);
extern Varinfo_t varinfo;					//store the info to queue(write to modbus);

#endif // !_MY_DATA_H_
