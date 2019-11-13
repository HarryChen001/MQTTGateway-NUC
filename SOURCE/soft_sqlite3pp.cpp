#include "soft_sqlite3pp.h"
#include "MyData.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "glog/logging.h"

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
int MySqlite::selectfrom(char* tablename, char* format, ...)
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
	query qryDevParam(db, "SELECT * FROM DevParam");
	query qryMqttParam(db, "SELECT * FROM MqttParam");
	query qryPortParam(db, "SELECT * FROM PortParam");
	query qryThemeCtrl(db, "SELECT * FROM ThemeCtrl");
	query qryThemeUpload(db, "SELECT * FROM ThemeUpload");
	query qryThemeUploadList(db, "SELECT * FROM ThemeUploadList");

	for (query::iterator i = qryMqttParam.begin(); i != qryMqttParam.end(); ++i)
	{
		int* count = &MqttInfo[0].MqttCount;
		MqttInfo[*count].id = (*i).get<int>(0);
		MqttInfo[*count].Session = (*i).get<int>(1);
		MqttInfo[*count].Enable = (*i).get<int>(2);
		MqttInfo[*count].KeepAlive = (*i).get<int>(3);
		MqttInfo[*count].ServerPort = (*i).get<int>(4);
		strcpy(MqttInfo[*count].ClientId, (*i).get<const char*>(5));
		strcpy(MqttInfo[*count].UserName, (*i).get<const char*>(8));
		strcpy(MqttInfo[*count].Password, (*i).get<const char*>(9));
		MqttInfo[*count].EnableDef = (*i).get<int>(11);
		strcpy(MqttInfo[*count].TopicDef, (*i).get<const char*>(12));
		MqttInfo[*count].QosDef = (*i).get<int>(13);
		MqttInfo[*count].EnableAlarm = (*i).get<int>(14);
		strcpy(MqttInfo[*count].TopicAlarm, (*i).get<const char*>(15));
		MqttInfo[*count].QosAlarm = (*i).get<int>(16);
		strcpy(MqttInfo[*count].ServerLink, (*i).get<const char*>(17));
		strcpy(MqttInfo[*count].MqttName, (*i).get<const char*>(18));
		(*count)++;
	}
	for (query::iterator i = qryThemeCtrl.begin(); i != qryThemeCtrl.end(); ++i)
	{
		int* count = &ThemeCtrl[0].Ctrlcount;
		ThemeCtrl[*count].id = (*i).get<int>(0);
		strcpy(ThemeCtrl[*count].proto, (*i).get<const char*>(1));
		ThemeCtrl[*count].Enable = (*i).get<int>(2);
		ThemeCtrl[*count].MqttId = (*i).get<int>(3);
		ThemeCtrl[*count].QosPub = (*i).get<int>(4);
		ThemeCtrl[*count].QosSub = (*i).get<int>(5);
		strcpy(ThemeCtrl[*count].CtrlName, (*i).get<const char*>(6));
		strcpy(ThemeCtrl[*count].CtrlPub, (*i).get<const char*>(7));
		strcpy(ThemeCtrl[*count].CtrlSub, (*i).get<const char*>(8));
		ThemeCtrl[*count].uartID = (*i).get<int>(9);
		(*count)++;
	}
	for (query::iterator i = qryPortParam.begin(); i != qryPortParam.end(); ++i)
	{
		static int subscribe = 0;
		int portnums = (*i).get<int>(4);
		int gpio = -1;
		switch (portnums)
		{
		case 1:
			portnums = 10;
			gpio = 0x2e;
			break;
		case 2:
			portnums = 3;
			gpio = 0x67;
			break;
		case 3:
			portnums = 9;
			gpio = 0x66;
			break;
		case 4:
			portnums = 1;
			gpio = 0x84;
			break;
		case 5:
			portnums = 6;
			gpio = 0x21;
			break;
		case 6:
			portnums = 7;
			break;
		case 7:
			portnums = 2;
			break;
		case 8:
			break;
		case 9:
			portnums = 4;
			break;
		}

		Allinfo[subscribe].portinfo.id = (*i).get<int>(0);
		Allinfo[subscribe].portinfo.PortId = (*i).get<int>(2);
		Allinfo[subscribe].portinfo.PortType = (*i).get<int>(3);
		Allinfo[subscribe].portinfo.PortNum = portnums;
		Allinfo[subscribe].portinfo.baud = (*i).get<int>(5);
		strcpy(Allinfo[subscribe].portinfo.Parity, (*i).get<const char*>(6));
		Allinfo[subscribe].portinfo.DataBits = (*i).get<int>(7);
		Allinfo[subscribe].portinfo.StopBits = (*i).get<int>(8);
		Allinfo[subscribe].portinfo.DelayTime = (*i).get<int>(9);
		Allinfo[subscribe].portinfo.gpio = gpio;
		Allinfo[subscribe].portinfo.isSerial = (*i).get<int>(10);
		strcpy(Allinfo[subscribe].portinfo.ipaddr, (*i).get<const char*>(11));
		Allinfo[subscribe].portinfo.ipport = (*i).get<int>(12);
		subscribe++;
	}
	for (query::iterator i = qryDevParam.begin(); i != qryDevParam.end(); ++i)
	{
		LOG(INFO) << "GET Device Info" << endl;
		int portid = (*i).get<int>(4);
		int portnums = -1;
		for (int i = 0; i < SerialNums; i++)
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
	}
	for (query::iterator i = qryThemeUpload.begin(); i != qryThemeUpload.end(); ++i)
	{
		LOG(INFO) << "GET ThemeUpload Info" << endl;
		int* j = &ThemeUpload[0].UploadThemeqCount;
		ThemeUpload[*j].id = (*i).get<int>(0);
		ThemeUpload[*j].Enable = (*i).get<int>(1);
		ThemeUpload[*j].MqttId = (*i).get<int>(2);
		ThemeUpload[*j].UploadId = (*i).get<int>(3);
		ThemeUpload[*j].PubPeriod = (*i).get<int>(4);
		ThemeUpload[*j].QosPub = (*i).get<int>(5);
		strcpy(ThemeUpload[*j].CtrlPub, (*i).get<const char*>(6));
		strcpy(ThemeUpload[*j].UploadName, (*i).get<const char*>(7));
		strcpy(ThemeUpload[*j].Proto, (*i).get<const char*>(8));

		(*j)++;
	}
	query qryVarParam(db, "SELECT * FROM VarParam");
	for (query::iterator i = qryVarParam.begin(); i != qryVarParam.end(); ++i)
	{
		int id = 0;
		id = (*i).get<int>(0);
		int DecimalsBit = (*i).get<int>(2);
		int obj = (*i).get<int>(3);
		int RegAdr = (*i).get<int>(5);
		int portid = (*i).get<int>(8);
		int devid = (*i).get<int>(9);
		int modules = (*i).get<int>(11);
		std::string datatype = (*i).get<const char*>(1);
		std::string regtype = (*i).get<const char*>(6);
		std::string varname = (*i).get<const char*>(7);

		int allinfosubscript = 0;
		int devsubscript = 0;
		for (int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
		{
			if (portid == Allinfo[i].portinfo.PortId)
			{
				allinfosubscript = i;
				break;
			}
		}
		for (int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
		{
			if (devid == Allinfo[allinfosubscript].deviceinfo[i].id)
			{
				devsubscript = i;
				break;
			}
		}
		int* allvarcount = &Allinfo[allinfosubscript].deviceinfo[devsubscript].allvarcount;
		VarParam_t* varparamtemp = &Allinfo[allinfosubscript].deviceinfo[devsubscript].allvarparams[*allvarcount];

		varparamtemp->id = id;
		varparamtemp->DecimalsBit = DecimalsBit;
		varparamtemp->obj = obj;
		varparamtemp->RegAdr = RegAdr;
		varparamtemp->PortId = portid;
		varparamtemp->DevId = devid;
		varparamtemp->modules = modules;

		varparamtemp->VarName = varname;
		varparamtemp->RegType = regtype;
		varparamtemp->DataType = datatype;

		(*allvarcount)++;
	}
	LOG(INFO) << "Already Get Varparams" << endl << endl;
	for (query::iterator i = qryThemeUploadList.begin(); i != qryThemeUploadList.end(); ++i)
	{
		int uploadid = (*i).get<int>(3);
		int devid = (*i).get<int>(5);
		int varid = (*i).get<int>(6);
		std::string varname = (*i).get<const char*>(7);

		for (int i = 0; i < ThemeUpload[0].UploadThemeqCount; i++)
		{
			int* count = &ThemeUpload[i].varcount;
			if (ThemeUpload[i].UploadId == uploadid)
			{
				ThemeUpload[i].varname[(*count)++] = varname;
				break;
			}
		}

		for (int i = 0; i < sizeof(Allinfo) / sizeof(Allinfo_t); i++)
		{
			if (Allinfo[i].portinfo.id == 0)
			{
				continue;
			}
			for (int j = 0; j < Allinfo[i].devcount; j++)
			{
				if (Allinfo[i].deviceinfo[j].id != devid)
				{
					continue;
				}
				int* uploadcount = &Allinfo[i].deviceinfo[j].uploadvarcount;
				VarParam_t* dest = &Allinfo[i].deviceinfo[j].uploadvarparam[*uploadcount];
				for (int k = 0; k < Allinfo[i].deviceinfo[j].allvarcount; k++)
				{
					VarParam_t* temp = &Allinfo[i].deviceinfo[j].allvarparams[k];
					if (temp->id == 0)
					{
						continue;
					}
					if (temp->id == varid)
					{
						memcpy(dest, temp, sizeof(temp[k]));
						(*uploadcount)++;
						break;
					}
				}
			}
		}
	}
	qryDevParam.finish();
	qryMqttParam.finish();
	qryPortParam.finish();
	qryThemeCtrl.finish();
	qryThemeUpload.finish();
	qryThemeUploadList.finish();
	return 0;
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
	return -1;
}
