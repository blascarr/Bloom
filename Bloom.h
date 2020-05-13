/*
  Design and created by Blascarr
  Bloom
  Name    : Bloom
  Description: Bloom.h
  version : 1.0
  Bloom is a library used for Bluetooth connection stablishment for robots with BT modules. HC-05
  The main objective is simplify the communication between robots in a customized game.
  for simple robotic applications.
  
  AT Commands operations.
  Master and Slave definitions.
  Configuration for pairing and binding robots.
  Communication with AT Commands
  
  This library is  created by Blascarr with the purpose of create a first overview for Clumsee robots
  and extend for other applications programmable through a blocking interface.
    
    
    Blascarr invests time and resources providing this open source code like some other libraries, please
    respect the job and support open-source software.
    
    Written by Adrian for Blascarr
*/

#ifndef _BLOOM_H
#define _BLOOM_H

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "Arduino.h"
	#else
		#include "WProgram.h"
	#endif

	#include "SoftwareSerial.h"
	//#include "EEPROM.h"

	#define BLOOM_SS

	#define SIZENAME 15
	#define SIZEADDRESS 15

	#ifdef MASTER
	    #define AT_MODE true
	#endif
	
	#ifdef SLAVE
		#define AT_MODE false
	#else
		#define AT_MODE true
	#endif

	typedef struct{
		char name [SIZENAME];
		char addr [SIZEADDRESS];
	} BLAddress;

	// Player List fot BT modules in game
	#define MAXADDRESS 10

class Bloom {

	public:
		uint32_t _bps;
		int _nEEPROM = 0;
		uint8_t _ATPIN;
		BLAddress _dev [MAXADDRESS];
		int _actualDev;
		BLAddress _actualAddr;

		#ifdef BLOOM_SS
		  SoftwareSerial *m_puart; /* The UART to communicate with Bloom */
		  Bloom(SoftwareSerial &uart, uint32_t bps =38400);
		  Bloom(SoftwareSerial &uart, uint8_t pinRESET, uint32_t bps =38400);
		#else
		  HardwareSerial *m_puart; /* The UART to communicate with Bloom */
		  Bloom(HardwareSerial &uart, uint32_t bps = 38400);
		  Bloom(HardwareSerial &uart, uint8_t pinRESET, uint32_t bps = 38400);
		#endif

		void begin(){
			m_puart->begin(_bps);
			pinMode( _ATPIN, OUTPUT );
			ATMODE( AT_MODE );
		}
		void init();
		// ---------------------------------------------------------------- //
		// ----------------------- GET Methods  --------------------------- //
		// ---------------------------------------------------------------- //

		String getName(void);
		String getPassword(void);
		String getState(void);
		String getVersion(void);
		String getAddress(void);
		String getRole(void);
		int    getMode(void);
		String getUART(void);
		String getIAC(void);
		String getBindAddress(void);

		String getDevCount(void);
		String getRecent(void);

		//Communication

		void search(void);
		void readConfig(void);

		bool seekDevice(String address);

		// ---------------------------------------------------------------- //
		// ----------------------- AT LINK Methods  ----------------------- //
		// ---------------------------------------------------------------- //

		void ATMODE(bool mode);
		
		String seekname(String address);
		void setBindAddress(String address);
		void unbind(void);
		void unpair(void);
		void unpair(String address);
		bool pair(String address, int timeout = 10); //Return if address is paired
		bool link(String address);
		bool link(uint8_t pos);

		void removePairList(void);
		void removeDeviceOnList(String address);
		void disconnect(void);
		void cancelBT(void);

		void connect(String address, int timeout = 10);
		//bool send(const uint8_t *buffer, uint32_t len);

		void sendMSG( String msg );
		void sendMSG( String msg, uint8_t pos);
		void sendMSG( String msg, String addr);

		// ---------------------------------------------------------------- //
		// ----------------------- SET Methods  --------------------------- //
		// ---------------------------------------------------------------- //

		void setPIN(uint8_t PIN);
		void resetAT(void);
		void setDefaults(void);
		bool setUART(uint32_t baudrate, uint8_t stopbit = 0, uint8_t parity = 0);

		void setRole(int mode);
		void setSlave(){ setRole(0); };
		void setMaster(){ setRole(1); };

		void setMode(uint8_t mode);
		void setINQMode(uint8_t inquiryMode, uint8_t maxDev, uint8_t inquiryTime);
		void setPassword(String PSWD);
		void setName(String NAME);

		// ---------------------------------------------------------------- //
		// ------------------ BLUETOOTH LIST MANAGEMENT  ------------------ //
		// ---------------------------------------------------------------- //
		void insertBL( String addr, uint8_t pos = 0, String name = "");
		void insertBL( BLAddress BLDevice, uint8_t pos = 0 );
		void saveBL();
		void loadBL();
	private:
		// AT Generic Commands
		bool AT_cmd(String cmd);
		bool AT_cmd(String cmd, String &list); //  GET Commands
		bool AT_SET_cmd(String cmd, String data, String &list); //  SET Commands
		bool AT_SET_cmd(String cmd, uint8_t mode); //  SET Commands

		// AT Commands Methods
		bool AT(void);
		bool ATINIT(void);
		bool ATRESET(void);
		bool ATORGL(void);
		bool ATRMAAD(void);
		
		bool ATINQ(void);
		bool ATINQC(void);

		// AT GET Methods
		bool ATSTATE(String &list);
		bool ATADDR(String &list);
		bool ATVERSION(String &list);
		bool ATNAME(String &list);
		bool ATUART(String &list);
		bool ATCMODE(String &list);
		bool ATROLE(String &list);
		bool ATADCN(String &list);
		bool ATMRAD(String &list);
		bool ATIAC(String &list);
		bool ATINQM(String &list);
		bool ATDISC(String &list);
		bool ATCLASS(String &list);
		bool ATPSWD(String &list);
		bool ATBIND(String &list);
		bool ATRNAME(String &list, String address);

		// AT SET Methods
		bool ATSETUART(String &list, String UART);
		bool ATSETNAME(String &list, String name);
		bool ATSETINQM(String &list, String INQ);
		bool ATSETROLE(uint8_t mode);
		bool ATSETCMODE(uint8_t mode);
		bool ATFSAD(String &list, String address);
		bool ATPMSAD(String &list, String address);
		bool ATSETIAC(String &list, String IAC);
		bool ATSETCLASS(String &list, String CLASS);
		bool ATSETPSWD(String &list, String PSWD);

		bool ATSETBIND(String &list, String address);
		bool ATPAIR(String &list, String address);
		bool ATLINK(String &list, String address);
		// Buffer Send and Transmit Data
		void rx_empty(void);

		/* 
		* Recvive data from uart. Return all received data if target found or timeout. 
		*/
		String recvString(String target, uint32_t timeout = 1000);

		/* 
		* Recvive data from uart. Return all received data if one of target1 and target2 found or timeout. 
		*/
		String recvString(String target1, String target2, uint32_t timeout = 1000);

		/* 
		* Recvive data from uart. Return all received data if one of target1, target2 and target3 found or timeout. 
		*/
		String recvString(String target1, String target2, String target3, uint32_t timeout = 1000);

		/* 
		* Recvive data from uart and search first target. Return true if target found, false for timeout.
		*/
		bool recvFind(String target, uint32_t timeout = 1000);
		/* 
		* Recvive data from uart and search first target and cut out the substring between begin and end(excluding begin and end self). 
		* Return true if target found, false for timeout.
		*/
		bool recvFindAndFilter(String target, String begin, String end, String &data, uint32_t timeout = 1000);
		bool recvFindAndFilter(String begin, String end, String &data, uint32_t timeout = 1000);
		void setEEPROMaddress( int nEEPROM );
};

#endif