//本文件定义一个毫秒级的时间戳工具类
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

//时间戳类型
typedef long long timestamp_t;

class Timestamp
{

public:
	//获取以毫秒计的时间戳
	static timestamp_t now();

};

#endif