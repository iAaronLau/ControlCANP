// CANIOC.cpp : 定义 DLL 应用程序的导出函数。
#include "stdafx.h"
#include"cvidef.h"
#include<stdio.h>  
#include<stdlib.h>  
#include<iostream>
#include<iomanip>
#include<bitset>

#define EXTERNC		extern "C"

#pragma comment(lib,"ControlCAN.lib")

using namespace std;

// 定义结构体，相当于数据包格式
typedef struct {
	unsigned char ID[2000];
	unsigned char byte0[2000];
	unsigned char byte1[2000];
	unsigned char byte2[2000];
	unsigned char byte3[2000];
	unsigned char byte4[2000];
	unsigned char byte5[2000];
	unsigned char byte6[2000];
	unsigned char byte7[2000];
} PackedData;

// 定义归纳好的数据包格式
typedef struct {
	unsigned int ID[2000];
	unsigned char	TimeStamp[2000];
	short int current_now[2000];
	short int  voltage_now[2000];
	unsigned char  heart_beat[2000];
} SortedData;

// 置位
void setBit(unsigned char & input, const unsigned char n) {
	input |= (unsigned char)(1 << n);
}
// 清除位
void clearBit(unsigned char	 & input, const unsigned char n) {
	input &= ~(unsigned char)(1 << n);
}

void int16to2int8p(short int &int16, unsigned char &int8H, unsigned char &int8L) {
	int8H = int16 >> 8;
	int8L = int16;
}

void int8toint16p(short int &int16, unsigned char &int8H, unsigned char &int8L) {
	int16 = (int8H << 8) | int8L;
}

void uint16to2int8p(unsigned short int &int16, unsigned char &int8H, unsigned char &int8L) {
	int8H = int16 >> 8;
	int8L = int16;
}
//(bitset<16>)voltage_calibration_low;
//clearBit(voltage_calibration_low, 4);

extern "C"{
	_declspec(dllexport)
	int initDevice(unsigned long CANindex = 0) {
		VCI_CloseDevice(VCI_USBCAN2A, CANindex);
		int ret = VCI_OpenDevice(VCI_USBCAN2A, CANindex, 0);
		for (int i = 0; ret != 1 && i < 10; i++) {
			ret = VCI_OpenDevice(VCI_USBCAN2A, CANindex, 0);
		}
		return ret;
	}
}

// 初始化can卡
extern "C" {
	_declspec(dllexport)
		int initCANPort(unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char mode = 0,
			unsigned long AccCode = 0x80000000,
			unsigned long AccMask = 0xFFFFFFFF,
			unsigned char Filter = 1,
			unsigned char Timing0 = 0x01,
			unsigned char Timing1 = 0x1C) {

		VCI_INIT_CONFIG vci_initconfig;
		vci_initconfig.AccCode = AccCode;
		vci_initconfig.AccMask = AccMask;
		vci_initconfig.Reserved = 0;
		vci_initconfig.Filter = Filter;
		vci_initconfig.Timing0 = Timing0;
		vci_initconfig.Timing1 = Timing1;
		vci_initconfig.Mode = mode;
		int ret = VCI_InitCAN(VCI_USBCAN2A, CANindex, way_index, &vci_initconfig);
		for (int i = 0; ret != 1 && i < 10; i++) {
			ret = VCI_InitCAN(VCI_USBCAN2A, CANindex, way_index, &vci_initconfig);
		}
		ret = VCI_StartCAN(VCI_USBCAN2A, CANindex, way_index);
		for (int i = 0; ret != 1 && i < 10; i++) {
			ret = VCI_StartCAN(VCI_USBCAN2A, CANindex, way_index);
		}
		return ret;
	}
}
// 关闭can卡
extern "C" {
	_declspec(dllexport)
	int close(unsigned long CANindex = 0) {
		int ret = VCI_CloseDevice(VCI_USBCAN2A, CANindex);
		for (int i = 0; ret != 1 && i < 10; i++) {
			ret = VCI_CloseDevice(VCI_USBCAN2A, CANindex);
		}
		return ret;
	}
}
// 发送
extern "C" {
	_declspec(dllexport)
		int send(VCI_CAN_OBJ vci_can_obj, unsigned long CANindex = 0, unsigned long way_index = 0) {
		int ret = VCI_Transmit(VCI_USBCAN2A, CANindex, way_index, &vci_can_obj, 1);
		for (int i = 0; ret != 1 && i < 10; i++) {
			ret = VCI_Transmit(VCI_USBCAN2A, CANindex, way_index, &vci_can_obj, 1);
		}
		return ret;
	}
}
// 接收
extern "C" {
	_declspec(dllexport)
		int receive(unsigned long CANindex = 0, unsigned long way_index = 0, unsigned long size = 2000) {
		VCI_CAN_OBJ vci_can_obj[2000];
		int ret = VCI_Receive(VCI_USBCAN2A, CANindex, way_index, vci_can_obj, size, 10);
		for (int i = 0; ret != 1 && i < 10; i++) {
			ret = VCI_Receive(VCI_USBCAN2A, CANindex, way_index, vci_can_obj, size, 10);
		}
		return ret;
	}
}
// 接收0协议
extern "C" {
	_declspec(dllexport)
		int receive0(PackedData *pdata, unsigned long CANindex = 0, unsigned long way_index = 0, unsigned long size = 2000) {
		VCI_CAN_OBJ vci_can_obj[2000];
		int ret = VCI_Receive(VCI_USBCAN2A, CANindex, way_index, vci_can_obj, size, 10);
		for (int i = 0; ret < 1 && i < 10; i++) {
			ret = VCI_Receive(VCI_USBCAN2A, CANindex, way_index, vci_can_obj, size, 10);
		}
		for (int i = 0; i < 2000; i++) {
			pdata->ID[i] = vci_can_obj[i].ID;
			pdata->byte0[i] = vci_can_obj[i].Data[0];
			pdata->byte1[i] = vci_can_obj[i].Data[1];
			pdata->byte2[i] = vci_can_obj[i].Data[2];
			pdata->byte3[i] = vci_can_obj[i].Data[3];
			pdata->byte4[i] = vci_can_obj[i].Data[4];
			pdata->byte5[i] = vci_can_obj[i].Data[5];
			pdata->byte6[i] = vci_can_obj[i].Data[6];
			pdata->byte7[i] = vci_can_obj[i].Data[7];
		}
		return ret;
	}
}
// 接收2协议
extern "C" {
	_declspec(dllexport)
		int receive1(unsigned long DevID1 ,SortedData *sdata, unsigned long CANindex = 0, unsigned long way_index = 0, unsigned long size = 2000) {
		VCI_CAN_OBJ vci_can_obj[2000];
		int ret = VCI_Receive(VCI_USBCAN2A, CANindex, way_index, vci_can_obj, size, 10);
		for (int i = 0; ret < 1 && i < 10; i++) {
			ret = VCI_Receive(VCI_USBCAN2A, CANindex, way_index, vci_can_obj, size, 10);
		}
		for (int i = 0; i < ret; i++) {
			if (vci_can_obj[i].ID / DevID1 == 1) {
				unsigned char voltage_LSB = vci_can_obj[i].Data[1];
				unsigned char voltage_MSB = vci_can_obj[i].Data[0];
				unsigned char current_LSB = vci_can_obj[i].Data[3];
				unsigned char current_MSB = vci_can_obj[i].Data[2];
				unsigned char heart_beat = vci_can_obj[i].Data[7];

				sdata->ID[i] = vci_can_obj[i].ID;
				sdata->TimeStamp[i] = vci_can_obj[i].TimeStamp;
				int8toint16p(sdata->voltage_now[i], voltage_MSB, voltage_LSB);
				int8toint16p(sdata->current_now[i], current_MSB, current_LSB);
				sdata->heart_beat[i] = heart_beat;
			}
		}
		if (ret < 2000 && ret >0) {
			sdata->ID[ret] = 255;
			sdata->voltage_now[ret] = 255;
			sdata->current_now[ret] = 255;
			sdata->heart_beat[ret] = 255;
		}
		return ret;
	}
}
// 发送1协议
extern "C" {
	_declspec(dllexport)
		int send1(unsigned int ID,
			unsigned char device_reset = 0,
			unsigned char start_stop = 1,
			unsigned char special_command = 0,
			unsigned short int special_value = 0,
			unsigned char addresss = 0x10203010,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		VCI_CAN_OBJ vco;
		vco.ID = ID;
		vco.RemoteFlag = RemoteFlag;
		vco.ExternFlag = ExternFlag;
		vco.SendType = SendType;
		vco.DataLen = 8;
		vco.Data[0] = addresss;
		vco.Data[1] = 0;
		vco.Data[2] = device_reset;
		vco.Data[3] = start_stop;
		vco.Data[4] = 0;
		vco.Data[5] = special_command;
		uint16to2int8p(special_value, vco.Data[6], vco.Data[7]);
		int ret = send(vco, CANindex, way_index);
		return ret;
	}
}
// 发送2协议
extern "C" {
	_declspec(dllexport)
		int send2(unsigned int ID,
			short int voltage,
			short int current,
			short int power,
			unsigned char addresss = 0x10203010,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		VCI_CAN_OBJ vco;
		vco.ID = ID;
		vco.RemoteFlag = RemoteFlag;
		vco.ExternFlag = ExternFlag;
		vco.SendType = SendType;
		vco.DataLen = 8;
		vco.Data[0] = addresss;
		vco.Data[1] = 1;
		int16to2int8p(voltage, vco.Data[2], vco.Data[3]);
		int16to2int8p(current, vco.Data[4], vco.Data[5]);
		int16to2int8p(power, vco.Data[6], vco.Data[7]);
		int ret = send(vco, CANindex, way_index);
		return ret;
	}
}
// 发送3协议
extern "C" {
	_declspec(dllexport)
		int send3(unsigned int ID,
			short int voltage_up,
			short int voltage_down,
			short int current_up,
			unsigned char addresss = 0x10203012,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		VCI_CAN_OBJ vco;
		vco.ID = ID;
		vco.RemoteFlag = RemoteFlag;
		vco.ExternFlag = ExternFlag;
		vco.SendType = SendType;
		vco.DataLen = 8;
		vco.Data[0] = addresss;
		vco.Data[1] = 2;
		int16to2int8p(voltage_up, vco.Data[2], vco.Data[3]);
		int16to2int8p(voltage_down, vco.Data[4], vco.Data[5]);
		int16to2int8p(current_up, vco.Data[6], vco.Data[7]);
		int ret = send(vco, CANindex, way_index);
		return ret;
	}
}
// 发送4协议
extern "C" {
	_declspec(dllexport)
		int send4(unsigned int ID,
			short int voltage_calibration,
			short int current_calibration,
			char voltage_deviation,
			char current_deviation,
			unsigned char addresss = 0x10203010,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		VCI_CAN_OBJ vco;
		vco.ID = ID;
		vco.RemoteFlag = RemoteFlag;
		vco.ExternFlag = ExternFlag;
		vco.SendType = SendType;
		vco.DataLen = 8;
		vco.Data[0] = addresss;
		vco.Data[1] = 3;
		int16to2int8p(voltage_calibration, vco.Data[2], vco.Data[3]);
		int16to2int8p(current_calibration, vco.Data[4], vco.Data[5]);
		vco.Data[6] = voltage_deviation;
		vco.Data[7] = current_deviation;
		int ret = send(vco, CANindex, way_index);
		return ret;
	}
}
// 发送协议主框架
extern "C" {
	_declspec(dllexport)
		int sendmainframe(unsigned int ID,
			short int MethodAddr,
			short int value,
			short int cmd = 0x0582,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		VCI_CAN_OBJ vco;
		vco.ID = ID;
		vco.RemoteFlag = RemoteFlag;
		vco.ExternFlag = ExternFlag;
		vco.SendType = SendType;
		vco.DataLen = 8;
		vco.Data[0] = 0x5A;
		vco.Data[1] = 0xA5;
		int16to2int8p(cmd, vco.Data[2], vco.Data[3]);
		int16to2int8p(MethodAddr, vco.Data[4], vco.Data[5]);
		int16to2int8p(value, vco.Data[6], vco.Data[7]);
		int ret = send(vco, CANindex, way_index);
		return ret;
	}
}

// 设置恒流模式
extern "C" {
	_declspec(dllexport)
		int setCC(unsigned int ID,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {
		int ret = sendmainframe(ID,
			0x1001,
			0x0000,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 恒压模式
extern "C" {
	_declspec(dllexport)
		int setCV(unsigned int ID,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1001,
			0x0001,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}

}
// 恒功率
extern "C" {
	_declspec(dllexport)
		int setCP(unsigned int ID,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1001,
			0x0002,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 恒阻
extern "C" {
	_declspec(dllexport)
		int setCR(unsigned int ID,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1001,
			0x0003,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}

}
// 恒流值
extern "C" {
	_declspec(dllexport)
		int setCCValue(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1013,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 恒压值
extern "C" {
	_declspec(dllexport)
		int setCVValue(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1014,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 恒功率值
extern "C" {
	_declspec(dllexport)
		int setCPValue(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1015,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 恒阻值
extern "C" {
	_declspec(dllexport)
		int setCRValue(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1016,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 读取Ulim
extern "C" {
	_declspec(dllexport)
		int readUlimValue(unsigned int ID,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1011,
			0x0100,
			0x0483,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 正弦波
extern "C" {
	_declspec(dllexport)
		int setZX(unsigned int ID,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1002,
			0x0000,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 三角波
extern "C" {
	_declspec(dllexport)
		int setSJ(unsigned int ID,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1002,
			0x0001,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 方波
extern "C" {
	_declspec(dllexport)
		int setFB(unsigned int ID,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1002,
			0x0002,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 自定义模式
extern "C" {
	_declspec(dllexport)
		int setZDY(unsigned int ID,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1002,
			0x0003,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 正弦波频率
extern "C" {
	_declspec(dllexport)
		int setZXPL(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1006,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 正弦幅度
extern "C" {
	_declspec(dllexport)
		int setZXFD(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1007,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 正弦占空比
extern "C" {
	_declspec(dllexport)
		int setZXZKB(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1008,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 三角波频率
extern "C" {
	_declspec(dllexport)
		int setSJPL(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1016,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 三角波幅度
extern "C" {
	_declspec(dllexport)
		int setSJFD(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1017,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 三角波占空比
extern "C" {
	_declspec(dllexport)
		int setSJZKB(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1018,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 方波频率
extern "C" {
	_declspec(dllexport)
		int setFBPL(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1056,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 方波幅度
extern "C" {
	_declspec(dllexport)
		int setFBFD(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1057,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 方波占空比
extern "C" {
	_declspec(dllexport)
		int setFBZKB(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1058,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 设置Ulim
extern "C" {
	_declspec(dllexport)
		int setUlim(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1011,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 设置Ilim
extern "C" {
	_declspec(dllexport)
		int setIlim(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1012,
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 设置电压
extern "C" {
	_declspec(dllexport)
		int setU(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1041,
			value,
			0x0B82,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 设置电流
extern "C" {
	_declspec(dllexport)
		int setI(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1042,
			value,
			0x0B82,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 设置电阻
extern "C" {
	_declspec(dllexport)
		int setR(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1043,
			value,
			0x0B82,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 设置功率
extern "C" {
	_declspec(dllexport)
		int setP(unsigned int ID,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			0x1044,
			value,
			0x0B82,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 自定义方波电流
extern "C" {
	_declspec(dllexport)
		int setZDYFBCurrent(unsigned int ID,
			short int num,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			short int(0x1020 + num),
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
// 自定义方波各个T
extern "C" {
	_declspec(dllexport)
		int setZDYFBT(unsigned int ID,
			short int num,
			short int value,
			unsigned long CANindex = 0,
			unsigned long way_index = 0,
			unsigned char RemoteFlag = 0,
			unsigned char ExternFlag = 1,
			unsigned char SendType = 1
		) {

		int ret = sendmainframe(ID,
			short int(0x1030 + num),
			value,
			0x0582,
			CANindex,
			way_index,
			RemoteFlag,
			ExternFlag,
			SendType
		);
		return ret;
	}
}
//extern "C" {
//	_declspec(dllexport)
//
//}














//int main() {
//	initDevice();
//	initCANPort();
//	for (int i = 0; i < 2000; i++) {
//		VCI_CAN_OBJ vco;
//		vco.ID = 0x10203040;
//		vco.RemoteFlag = 0;
//		vco.ExternFlag = 1;
//		vco.SendType = 1;
//		vco.DataLen = 8;
//		short int v_up = -3000;
//		int16to2int8p(v_up, vco.Data[1], vco.Data[0]);
//		short int v_down = i;
//		int16to2int8p(v_down, vco.Data[3], vco.Data[2]);
//		short int c_up = -i;
//		int16to2int8p(c_up, vco.Data[5], vco.Data[4]);
//		vco.Data[6] = 0;
//		vco.Data[7] = i;
//		send(vco);
//	}
//	SortedData pc;
//
//	for (int i = 0; i < 1; i++) {
//		int ret = receive1(&pc, 0, 0, 2000);
//		for (int i = 0; i < 2000; i++) {
//			cout << "ID: " << pc.ID[i] << endl
//				<< "voltage_up: " << pc.voltage_up[i] << endl
//				<< "voltage_down: " << pc.voltage_down[i] << endl
//				<< "current_up: " << pc.current_up[i] << endl
//				<< "current_now: " << pc.current_now[i] << endl
//				<< "voltage_now: " << pc.voltage_now[i] << endl
//				<< "status: " << pc.status[i] << endl;
//			cout << "===========================" << endl;
//		}
//	}
//	cout << "Qiudoumadei!";
//	cin.get();
//	cin.get();
//	return 0;
//}