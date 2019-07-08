#include "soft_sqlite3pp.h"
#include "MyData.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

using namespace sqlite3pp;
using std::cout;
using std::endl;


MySqlite::MySqlite(char* dbname)
{
	db = database(dbname);
	transaction xct(db, true);
}
MySqlite::~MySqlite()
{
	db.disconnect();
}
int MySqlite::selectfrom(char* tablename,char* format,...)
{
	char dest[100];
	char dbcmd[200];

	va_list	arg_list;
	va_start(arg_list, format); //һ��Ҫ��...��֮ǰ���Ǹ�����
	vsprintf(dest, format, arg_list);
	va_end(arg_list);
	sprintf(dbcmd, "SELECT %s FROM %s", dest, tablename);

	query qry(db, dbcmd);

	cout << "qry is " << qry.column_count() << endl;

	for (int i = 0; i < qry.column_count(); ++i)
	{
		cout << qry.column_name(i) << "\t";
	}
	cout << endl;

	for (query::iterator i = qry.begin(); i != qry.end(); ++i)
	{
		for (int j = 0; j < qry.column_count(); ++j)
		{
			cout << (*i).get<const char*>(j) << "\t";
		}
		cout << endl;
	}
	return 0;
}
int MySqlite::GetAllInfo()
{
	int j = 0;
	query qryDevParam(db, "SELECT * FROM DevParam");
	query qryMqttParam(db, "SELECT * FROM MqttParam");
	query qryPortParam(db, "SELECT * FROM PortParam");
	query qryThemeCtrl(db, "SELECT * FROM ThemeCtrl");
	query qryThemeUpload(db, "SELECT * FROM ThemeUpload");
	query qryThemeUploadList(db, "SELECT * FROM ThemeUploadList");
//	qryDevParam.finish();
	j = 0;
	for (query::iterator i = qryMqttParam.begin(); i != qryMqttParam.end(); ++i)
	{
		MqttInfo[j].id = (*i).get<int>(0);
		MqttInfo[j].Session = (*i).get<int>(1);
		MqttInfo[j].Enable = (*i).get<int>(2);
		MqttInfo[j].KeepAlive = (*i).get<int>(3);
		MqttInfo[j].ServerPort = (*i).get<int>(4);
		strcpy(MqttInfo[j].ClientId, (*i).get<const char*>(5));
		strcpy(MqttInfo[j].UserName, (*i).get<const char*>(8));
		strcpy(MqttInfo[j].Password, (*i).get<const char*>(9));
		MqttInfo[j].EnableDef = (*i).get<int>(11);
		strcpy(MqttInfo[j].TopicDef, (*i).get<const char*>(12));
		MqttInfo[j].QosDef = (*i).get<int>(13);
		MqttInfo[j].EnableAlarm = (*i).get<int>(14);
		strcpy(MqttInfo[j].TopicAlarm, (*i).get<const char*>(15));
		MqttInfo[j].QosAlarm = (*i).get<int>(16);
		strcpy(MqttInfo[j].ServerLink, (*i).get<const char*>(17));
		strcpy(MqttInfo[j].MqttName, (*i).get<const char*>(18));
		j++;
	}
	MqttInfo[0].MqttCount = j--;
	j = 0;
	qryMqttParam.finish();
	for (query::iterator i = qryThemeCtrl.begin(); i != qryThemeCtrl.end(); ++i)
	{
		ThemeCtrl[j].id = (*i).get<int>(0);
		strcpy(ThemeCtrl[j].proto, (*i).get<const char*>(1));
		ThemeCtrl[j].Enable = (*i).get<int>(2);
		ThemeCtrl[j].MqttId = (*i).get<int>(3);
		ThemeCtrl[j].QosPub = (*i).get<int>(4);
		ThemeCtrl[j].QosSub = (*i).get<int>(5);
		strcpy(ThemeCtrl[j].CtrlName, (*i).get<const char*>(6));
		strcpy(ThemeCtrl[j].CtrlPub, (*i).get<const char*>(7));
		strcpy(ThemeCtrl[j].CtrlSub, (*i).get<const char*>(8));
		ThemeCtrl[j].uartID = (*i).get<int>(9);
		j++;
	}
	qryThemeCtrl.finish();
	j = 0;
	for (query::iterator i = qryPortParam.begin(); i != qryPortParam.end(); ++i)
	{
		PortInfo[j].id = (*i).get<int>(0);
		PortInfo[j].PortId = (*i).get<int>(2);
		PortInfo[j].PortType = (*i).get<int>(3);
		PortInfo[j].PortNum = (*i).get<int>(4);
		switch (PortInfo[j].PortNum)
		{
		case 1:
			PortInfo[j].PortNum = 10;
			PortInfo[j].gpio = 0x2e;
			break;
		case 2:
			PortInfo[j].PortNum = 3;
			PortInfo[j].gpio = 0x67;
			break;
		case 3:
			PortInfo[j].PortNum = 9;
			PortInfo[j].gpio = 0x66;
			break;
		case 4:
			PortInfo[j].PortNum = 1;
			PortInfo[j].gpio = 0x84;
			break;
		case 5:
			PortInfo[j].PortNum = 6;
			PortInfo[j].gpio = 0x21;
			break;
		case 6:
			PortInfo[j].PortNum = 7;
			PortInfo[j].gpio = -1;
			break;
		case 7:
			PortInfo[j].PortNum = 2;
			PortInfo[j].gpio = -1;
			break;
		case 8:
			PortInfo[j].gpio = -1;
			break;
		case 9:
			PortInfo[j].PortNum = 4;
			PortInfo[j].gpio = -1;
			break;
		}
		PortInfo[j].baud = (*i).get<int>(5);
		PortInfo[j].DataBits = (*i).get<int>(7);
		PortInfo[j].StopBits = (*i).get<int>(8);
		PortInfo[j].DelayTime = (*i).get<int>(9);
		strcpy(PortInfo[j].Parity, (*i).get<const char*>(6));

		int portnums = PortInfo[j].PortNum;
		Allinfo[portnums].portinfo.id = (*i).get<int>(0);
		Allinfo[portnums].portinfo.PortId = (*i).get<int>(2);
		Allinfo[portnums].portinfo.PortType = (*i).get<int>(3);
		Allinfo[portnums].portinfo.PortNum = portnums;
		Allinfo[portnums].portinfo.baud = (*i).get<int>(5);
		strcpy(Allinfo[portnums].portinfo.Parity , (*i).get<const char*>(6));
		Allinfo[portnums].portinfo.DataBits = (*i).get<int>(7);
		Allinfo[portnums].portinfo.StopBits = (*i).get<int>(8);
		Allinfo[portnums].portinfo.DelayTime = (*i).get<int>(9);

		j++;
	}
	PortInfo[0].portcount = j--;
	qryPortParam.finish();
	j = 0;
	for (query::iterator i = qryDevParam.begin(); i != qryDevParam.end(); ++i)
	{
		DevInfo[j].id = (*i).get<int>(0);
		DevInfo[j].address = (*i).get<int>(2);
		DevInfo[j].PortId = (*i).get<int>(4);
		DevInfo[j].regedian = (*i).get<int>(5);
		DevInfo[j].byteorder = (*i).get<int>(6);

		int portid = (*i).get<int>(4);
		int portnums = -1;
		for (int i = 0; i < 11; i++)
		{
			if (portid == Allinfo[i].portinfo.PortId)
			{
				portnums = i;
				break;
			}
		}
		Allinfo[portnums].deviceinfo[Allinfo[portnums].devcount].id = (*i).get<int>(0);
		Allinfo[portnums].deviceinfo[Allinfo[portnums].devcount].address = (*i).get<int>(2);
		Allinfo[portnums].deviceinfo[Allinfo[portnums].devcount].PortId = (*i).get<int>(4);
		Allinfo[portnums].deviceinfo[Allinfo[portnums].devcount].regedian = (*i).get<int>(5);
		Allinfo[portnums].deviceinfo[Allinfo[portnums].devcount].byteorder = (*i).get<int>(6);

		Allinfo[portnums].devcount++;
		j++;
	}
	DevInfo[0].devcount = j--;
	qryDevParam.finish();
	j = 0;
	for (query::iterator i = qryThemeUpload.begin(); i != qryThemeUpload.end(); ++i)
	{
		ThemeUpload[j].id = (*i).get<int>(0);
		ThemeUpload[j].Enable = (*i).get<int>(1);
		ThemeUpload[j].MqttId = (*i).get<int>(2);
		ThemeUpload[j].UploadId = (*i).get<int>(3);
		ThemeUpload[j].PubPeriod = (*i).get<int>(4);
		ThemeUpload[j].QosPub = (*i).get<int>(5);
		strcpy(ThemeUpload[j].CtrlPub, (*i).get<const char*>(6));
		strcpy(ThemeUpload[j].UploadName, (*i).get<const char*>(7));
		strcpy(ThemeUpload[j].Proto, (*i).get<const char*>(8));
		j++;
	}
	qryThemeUpload.finish();
	j = 0;
	query qryVarParam(db, "SELECT * FROM VarParam");
	for (query::iterator i = qryVarParam.begin(); i != qryVarParam.end(); ++i)
	{
		VarParam[j].id = (*i).get<int>(0);
		VarParam[j].DecimalsBit = (*i).get<int>(2);
		VarParam[j].obj = (*i).get<int>(3);
		VarParam[j].RegAdr = (*i).get<int>(5);
		VarParam[j].PortId = (*i).get<int>(8);
		VarParam[j].DevId = (*i).get<int>(9);
		VarParam[j].modules = (*i).get<float>(11);
		strcpy(VarParam[j].DataType, (*i).get<const char*>(1));
		strcpy(VarParam[j].RegType, (*i).get<const char*>(6));
		strcpy(VarParam[j].VarName, (*i).get<const char*>(7));
		j++;
	}
	VarParam[0].VarCount = j--;
	qryVarParam.finish();
	j = 0;
	for (query::iterator i = qryThemeUploadList.begin(); i != qryThemeUploadList.end(); ++i)
	{
		ThemeUploadList[j].id = (*i).get<int>(0);
		ThemeUploadList[j].Enable = (*i).get<int>(1);
		ThemeUploadList[j].MqttId = (*i).get<int>(2);
		ThemeUploadList[j].UploadId = (*i).get<int>(3);
		ThemeUploadList[j].DevId = (*i).get<int>(4);
		ThemeUploadList[j].VarId = (*i).get<int>(5);
		memset(&ThemeUploadList[j].VarName, 0, sizeof(ThemeUploadList[j].VarName));
		strcpy(ThemeUploadList[j].VarName, (*i).get<const char*>(6));
		j++;
	}
	ThemeUploadList[0].UploadCount = j--;
	for (int i = 0; i < sizeof(ThemeUploadList) / sizeof(ThemeUploadList[0]); i++)
	{
		if (ThemeUploadList[i].id == 0)
			continue;
		int varid = ThemeUploadList[i].VarId;
		int varsubscript = -1;
		for (int i = 0; i < sizeof(VarParam) / sizeof(VarParam_t); i++)
		{
			if (varid == VarParam[i].id)
			{
				varsubscript = i;
				break;
			}
		}

		int portid = VarParam[varsubscript].PortId;
		int devid = VarParam[varsubscript].DevId;
		int allinfosubscript;
		for (int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
		{
			if (portid == Allinfo[i].portinfo.PortId)
			{
				allinfosubscript = i;
				break;
			}
		}
		int devsubscript;
		for (int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
		{
			if (devid = Allinfo[allinfosubscript].deviceinfo[i].id)
			{
				devsubscript = i;
				break;
			}
		}
		int* varcount = &Allinfo[allinfosubscript].deviceinfo[devsubscript].varcount;
		Allinfo[allinfosubscript].deviceinfo[devsubscript].varparam[*varcount].id = VarParam[varsubscript].id;
		strcpy(Allinfo[allinfosubscript].deviceinfo[devsubscript].varparam[*varcount].DataType, VarParam[varsubscript].DataType);
		Allinfo[allinfosubscript].deviceinfo[devsubscript].varparam[*varcount].DecimalsBit = VarParam[varsubscript].DecimalsBit;
		Allinfo[allinfosubscript].deviceinfo[devsubscript].varparam[*varcount].DevId = VarParam[varsubscript].DevId;
		Allinfo[allinfosubscript].deviceinfo[devsubscript].varparam[*varcount].modules = VarParam[varsubscript].modules;
		Allinfo[allinfosubscript].deviceinfo[devsubscript].varparam[*varcount].RegAdr = VarParam[varsubscript].RegAdr;
		strcpy(Allinfo[allinfosubscript].deviceinfo[devsubscript].varparam[*varcount].RegType, VarParam[varsubscript].RegType);
		strcpy(Allinfo[allinfosubscript].deviceinfo[devsubscript].varparam[*varcount].VarName, VarParam[varsubscript].VarName);
		(*varcount)++;
	}
}
int MySqlite::GetCountFromTable(char* tablename)
{
	char cmd[100];
	sprintf(cmd, "%s%s", "Select count(*) from ", tablename);
	query qry(db, cmd);
	for (query::iterator i = qry.begin(); i != qry.end(); ++i)
	{
		cout << (*i).get<int>(0) << endl;
		return (*i).get<int>(0);
	}
}
