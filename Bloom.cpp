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

#include <Bloom.h>

#define  BLDEBUG 0
#define  BLINFO 1
    #if  BLINFO
      #define  INFODUMP(s, v)  { Serial.print(F(s)); Serial.print(v); }
    #else
      #define  INFODUMP(s, v) 
    #endif

    #if  BLDEBUG
      #define  BLDUMP(s, v)  { Serial.print(F(s)); Serial.print(v); }
      #define  BLDUMPS(s)    Serial.print(F(s))
    #else
      #define  BLDUMP(s, v)
      #define  BLDUMPS(s)
    #endif

#ifdef BLOOM_SS
    Bloom::Bloom(SoftwareSerial &uart, uint32_t bps): m_puart(&uart), _bps(bps){
        m_puart->begin(bps);
        rx_empty();
    }

    Bloom::Bloom(SoftwareSerial &uart, uint8_t pinRESET, uint32_t bps): m_puart(&uart), _bps(bps), _ATPIN( pinRESET ){
        m_puart->begin(bps);
        rx_empty();
    }

#else
  Bloom::Bloom(HardwareSerial &uart, uint32_t bps): m_puart(&uart), _bps(bps){
        m_puart->begin(bps);
        rx_empty();
  }
#endif

void Bloom::init(){
    
    if (ATINIT()){
        INFODUMP("\nINIT SUCCESS - ", getState() );
    }else{
        INFODUMP("\nINIT FAIL ERROR (17) Means Already Initialized - ", getState() );
    }
}


// ---------------------------------------------------------------- //
// ----------------------- GET Methods  --------------------------- //
// ---------------------------------------------------------------- //

String Bloom::getName(void)
{
    String name;
    ATNAME(name);
    return name;
}

String Bloom::getState(void){
    String state;
    ATSTATE(state);
    return state;
}

String Bloom::getVersion(void){
    String version;
    ATVERSION(version);
    return version;
}

String Bloom::getAddress(void){
    String address;
    ATADDR(address);
    return address;
}

String Bloom::getRole(void){
    String role;
    ATROLE(role);
    return role;
}

String Bloom::getUART(void){
    String uart;
    ATUART(uart);
    return uart;  
}

String Bloom::getIAC(void){
    String iac;
    ATIAC(iac);
    return iac; 
}

String Bloom::getDevCount(void){
    String count;
    ATADCN(count);
    return count; 
}

String Bloom::getRecent(void){
    String recent;
    ATMRAD(recent);
    return recent; 
}

int   Bloom::getMode(void){
    String cmode;
    ATCMODE(cmode);
    return cmode.toInt(); 
}

String Bloom::getPassword(void){
    String pswd;
    ATPSWD(pswd);
    return pswd;
}

String Bloom::getBindAddress(void){
    String bind;
    ATBIND(bind);
    return bind;
}

// ---------------------------------------------------------------- //
// ----------------------- SET Methods  --------------------------- //
// ---------------------------------------------------------------- //

void   Bloom::setMode(uint8_t mode){
    ATSETCMODE(mode);
}

void   Bloom::setINQMode(uint8_t inquiryMode, uint8_t maxDev, uint8_t inquiryTime){
    String cmode;
    String entry = String(inquiryMode) + ","+ String(maxDev) + ","+ String(inquiryTime);
    ATSETINQM(cmode, entry);
}

bool   Bloom::setUART(uint32_t baudrate, uint8_t stopbit, uint8_t parity){
    String cmode;
    String entry = String(baudrate) + ","+ String(stopbit) + ","+ String(parity);
    ATSETUART(cmode, entry);

    if(recvFind("OK",5000)){
      m_puart->begin(baudrate);
      return true;
    }else{
      return false;
    }
}

void Bloom::setBindAddress(String address){
    String bind;
    ATSETBIND(bind, address);
}

// ---------------------------------------------------------------- //
// ----------------------- AT LINK Methods  ----------------------- //
// ---------------------------------------------------------------- //
// Search name of BT module with address
String Bloom::seekname (String address){
    String rname;
    if(ATRNAME( rname, address )){
        return rname;
    }
}

void Bloom::connect(String address, int timeout){

    if (pair(address, timeout)){

        if(address != String(_actualAddr.addr)){
            disconnect();   
            address.toCharArray( _actualAddr.addr,SIZENAME ); 
        }else{
            //    
        } 
        
        INFODUMP("\n PAIRED WITH : ", address);
        link(address);
    }else{
        INFODUMP("\nNOT PAIRED WITH : ", address);
        
    }
}

void  Bloom::search(void){
    setMode(1);
    ATMODE(false);
    ATINQ();
}


void Bloom::unbind(void){
    setBindAddress("0");
}

bool Bloom::pair(String address, int timeout ){
    String pair;
    String entry = address + ","+ String(timeout);
    return ATPAIR(pair, entry);
}

void Bloom::unpair(void){
    ATRMAAD();
}

void Bloom::unpair(String address){
    String unpair;
    ATPMSAD( unpair,  address);
}

/*void Bloom::link(String address){
    
    if(address != String(_actualAddr.addr)){
        
        init();
        ATMODE(false);
        String link;
        if(ATLINK(link, address)){

            if( link.startsWith("ERROR") ){
                INFODUMP("EXCEPTION  : ", link);
            }else{
                INFODUMP("ESTADO CONEXION", link);
                INFODUMP("\nConnected to : ", address);
                address.toCharArray( _actualAddr.addr,SIZENAME );
            }
        }else{
            INFODUMP("\nNOT connected to : ", address);
            address.toCharArray( _actualAddr.addr,SIZENAME );
        }
    }else{
        //disconnect();
    }
    
}*/

bool Bloom::link(String address){
    String link;
    ATLINK(link, address);

    if( link.startsWith("ERROR") ){
        INFODUMP("EXCEPTION  : ", link);
        return false;
    }else{
        INFODUMP("ESTADO CONEXION ", link);
        
        if( link.indexOf("FAIL") >=0 ){
            INFODUMP("\nNot Connected to : ", address);
        }else{
            INFODUMP("\nConnected to : ", address);
        }
        
        address.toCharArray( _actualAddr.addr,SIZENAME );
        return true;
    }
    
}

bool Bloom::link(uint8_t pos){
    return link(String(_dev[pos].addr));
}

void Bloom::readConfig(void){
    String config;
    INFODUMP("\nName: ",getName());
    INFODUMP("\nAddress: ",getAddress());
    INFODUMP("\nPassword: ",getPassword());
    INFODUMP("\nRole ( 0 Slave, 1 Master, 2 Slave-Loop ): ",getRole());
    INFODUMP("\nMode: ",getMode());
    INFODUMP("\nUART Configuration: ",getUART());
    INFODUMP("\nDevices counter: ",getDevCount());
    INFODUMP("\nBind Address: ",getBindAddress());
    INFODUMP("\nMost Recent Authenticated Device: ",getRecent());
    INFODUMP("\nIAC( Inquiry Access Code ): ",getIAC());
    ATINQM(config);
    INFODUMP("\nComm Info - ", config);
}

void Bloom::disconnect(void){
    //ATMODE(true);
    String disc;
    String state= getState();
    if(state !="DISCONNECTED" ){
        if(ATDISC(disc)){
            INFODUMP("\nDisconnected - ", disc);
        }else{
            INFODUMP("\nProblem - ", disc);
        } 
    }
}

void Bloom::cancelBT(void){
    
    ATINIT();
    ATINQC();
}

// SET Methods
void Bloom::setPIN(uint8_t PIN){
    _ATPIN = PIN;
    pinMode(_ATPIN, OUTPUT);
    ATMODE(true);
}

void Bloom::ATMODE(bool mode){
    digitalWrite(_ATPIN, mode);
}

void Bloom::resetAT(){
    //Hacer un Reset implica conectar y desconectar la placa
    ATMODE(true);
    ATMODE(false);
}

void Bloom::setDefaults(void){
    ATORGL();
}

void Bloom::setPassword(String PSWD){
    String pswd;
    ATSETPSWD(pswd, PSWD);
}

void Bloom::setName(String NAME){
    String name;
    ATSETNAME( name, NAME );
}

void Bloom::setRole(int mode){
    switch (mode){
        case 0:
          ATSETROLE(mode);
        break;

        case 1:
          ATSETROLE(mode);
        break;

        case 2:
          ATSETROLE(mode);
        break;

        default:
            Serial.println("Only 0-2 values accepted");
        break;

    }
}

void Bloom::removePairList(void){
    if(ATRMAAD()){
      BLDUMPS("Authenticated Devices Deleted (Check getDevCount() Method)");
    }else{
      BLDUMPS("Something went wrong");
    }
}

void Bloom::removeDeviceOnList(String address){
    String rmDev;
    if( ATPMSAD(rmDev, address) ){
      BLDUMPS(" Devices Deleted Successfull");
      BLDUMP(" Check ", rmDev);
    }else{
      BLDUMPS("Something went wrong");
    }
}

void Bloom::insertBL(String addr, uint8_t pos , String name){
    BLAddress dev;
    name.toCharArray(dev.name, SIZENAME);
    addr.toCharArray(dev.addr, SIZEADDRESS);
    insertBL(dev, pos);
}

void Bloom::insertBL( BLAddress BLDevice, uint8_t pos){
    _dev[pos] = (BLDevice);
}

void Bloom::sendMSG( String msg ){
    //ATMODE(false);
    //delay(100);
    m_puart->print(msg);
    //delay(100);
    //ATMODE(true);
}

void Bloom::sendMSG( String msg, uint8_t pos ){
    connect(String(_dev[pos].addr));
    sendMSG(msg);
}

void Bloom::sendMSG( String msg, String addr ){
    connect(addr);
    sendMSG(msg);
}

//-----AT Generic Commands----------------------------------//
bool Bloom::AT_cmd(String cmd){
    rx_empty();
    BLDUMP(" AT+",cmd);
    m_puart->println(cmd);
    return recvFind("OK");
}

bool Bloom::AT_cmd(String cmd, String &list){
    rx_empty();
    BLDUMP(" AT+",cmd);
    m_puart->print(F("AT+"));
    m_puart->println(cmd);
    return recvFindAndFilter("OK", ":", "\r\nOK", list);
}

bool Bloom::AT_SET_cmd(String cmd, uint8_t mode){
    rx_empty();
    BLDUMP(" AT+",cmd);
    m_puart->print(F("AT+"));
    m_puart->print(cmd);
    m_puart->print(F("="));
    m_puart->println(mode);

    String data = recvString("OK", "no change");

    if (data.indexOf("OK") != -1 || data.indexOf("no change") != -1) {
        return true;
    }
    return false;
}

bool Bloom::AT_SET_cmd(String cmd, String data, String &list){
    rx_empty();
    BLDUMP(" AT+",cmd);
    m_puart->print(F("AT+"));
    m_puart->print(cmd);
    m_puart->print(F("="));
    m_puart->println(data);

    return recvFindAndFilter("OK", ":", "\r\nOK", list);
 
} 


// AT Commands Methods

bool Bloom::AT(void){
    return AT_cmd("AT");
}

bool Bloom::ATRESET(void) {
    return AT_cmd("RESET");
}

bool Bloom::ATORGL(void) {
    return AT_cmd("ORGL");
}

bool Bloom::ATINQC(void){
    return AT_cmd("INQC");
}

bool Bloom::ATRMAAD(void){
    return AT_cmd("RMAAD");
}

bool Bloom::ATINIT(void){
    rx_empty();
    String state = getState();
    if ( state == "INITIALIZED" || state == "READY" || state != "INQUIRING" || state == "DISCONNECTED"){
        BLDUMPS(" AT+INIT");
        m_puart->println(F("AT+INIT"));
        return true;
    }else{
        return false;
    }
    
    //return recvFindAndFilter("OK", ":", "\r\nOK", list);
    //return AT_cmd("INIT");
}

bool Bloom::ATINQ(void){
    return AT_cmd("INQ");
}

/* -- AT+STATE Values -- 
    INITIALIZED
    READY
    PAIRABLE
    PAIRED
    INQUIRING
    CONNECTING
    CONNECTED
    DISCONNECTED
    UNKNOWN
*/
bool Bloom::ATSTATE(String &list){
    return AT_cmd("STATE", list);
}

bool Bloom::ATADDR(String &list){
    return AT_cmd("ADDR", list);
}

bool Bloom::ATVERSION(String &list){
    return AT_cmd("VERSION", list);
}

bool Bloom::ATNAME(String &list){
    return AT_cmd("NAME", list);
}

bool Bloom::ATUART(String &list){
    return AT_cmd("UART", list);
}

bool Bloom::ATCMODE(String &list){
    return AT_cmd("CMODE", list);
}

bool Bloom::ATADCN(String &list){
    return AT_cmd("ADCN", list);
}

bool Bloom::ATMRAD(String &list){
    return AT_cmd("MRAD", list);
}

/*  AT+ROLE 
    0---- Slave role 
    1---- Master role 
    2---- Slave-Loop role 
    Default: 0 
*/
bool Bloom::ATROLE(String &list){
    return AT_cmd("ROLE", list);
}

bool Bloom::ATIAC(String &list){
    return AT_cmd("IAC", list);
}

bool Bloom::ATINQM(String &list){
    return AT_cmd("INQM", list);
}

bool Bloom::ATDISC(String &list){
    return AT_cmd("DISC", list);
}

bool Bloom::ATCLASS(String &list){
    return AT_cmd("CLASS", list);
}

bool Bloom::ATPSWD(String &list){
    return AT_cmd("PSWD", list);
}

bool Bloom::ATBIND(String &list){
    return AT_cmd("BIND", list);
}

bool Bloom::ATRNAME(String &list, String address){
    rx_empty();
    BLDUMPS(" AT+RNAME? ");
    m_puart->print(F("AT+RNAME?"));
    m_puart->println(address);

    return recvFindAndFilter("OK", ":", "\r\nOK", list);

}

/*  AT SET Methods*/
bool Bloom::ATFSAD(String &list, String address){
    return AT_SET_cmd("FSAD", address, list);
}

bool Bloom::ATPMSAD(String &list, String address){
    return AT_SET_cmd("PMSAD", address, list);
}

bool Bloom::ATSETINQM(String &list, String INQ){
    return AT_SET_cmd("INQM", INQ, list);
}

bool Bloom::ATSETROLE(uint8_t mode){
    return AT_SET_cmd("ROLE", mode);
}

bool Bloom::ATSETCMODE(uint8_t mode){
    return AT_SET_cmd("CMODE", mode);
}

bool Bloom::ATSETNAME(String &list, String name){
    return AT_SET_cmd("NAME", name, list);
}

bool Bloom::ATSETUART(String &list, String UART){
    return AT_SET_cmd("UART", UART, list);
}

bool Bloom::ATSETCLASS(String &list, String CLASS){
    return AT_SET_cmd("CLASS", CLASS, list);
}

bool Bloom::ATSETPSWD(String &list, String PSWD){
    return AT_SET_cmd("PSWD", PSWD, list);
}

bool Bloom::ATSETBIND(String &list, String address){
    return AT_SET_cmd("BIND", address, list);
}

bool Bloom::ATPAIR(String &list, String address){
    return AT_SET_cmd("PAIR", address, list);
}

bool Bloom::ATLINK(String &list, String address){
    rx_empty();
    BLDUMP(" AT+LINK= ", address);
    m_puart->print(F("AT+LINK="));
    m_puart->println(address);

    return recvFindAndFilter("OK", ":", "\r\nOK", list);
}

/*----------------------------------------------*/
void Bloom::rx_empty(void) {
    while(m_puart->available() > 0) {
        m_puart->read();
    }
}

bool Bloom::recvFind(String target, uint32_t timeout){
    String data_tmp;
    data_tmp = recvString(target, timeout);
    
    if (data_tmp.indexOf(target) != -1) {
        BLDUMPS("OK\n");
        return true;
    }
    return false;
}

String Bloom::recvString(String target, uint32_t timeout)
{
    String data;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(m_puart->available() > 0) {
            a = m_puart->read();
            if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target) != -1) {
            break;
        }   
    }
    
    return data;
}

String Bloom::recvString(String target1, String target2, uint32_t timeout)
{
    String data;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(m_puart->available() > 0) {
            a = m_puart->read();
            if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            break;
        } else if (data.indexOf(target2) != -1) {
            break;
        }
    }
    return data;
}

bool Bloom::recvFindAndFilter(String target, String begin, String end, String &data, uint32_t timeout)
{
    String data_tmp;
    data_tmp = recvString(target, timeout);

    if (BLDEBUG) Serial.println(data_tmp);

    if (data_tmp.indexOf(target) != -1) {
        int32_t index1 = data_tmp.indexOf(begin);
        int32_t index2 = data_tmp.indexOf(end);
        
        /*BLDUMP("Index 1 - ",index1);
        BLDUMPS("\n");
        BLDUMP("Index 2 - ",index2);
        BLDUMPS("\n");*/

        if (index1 != -1 && index2 != -1) {
            index1 += begin.length();
            data = data_tmp.substring(index1, index2);
            return true;
        }
    }else{
        data = data_tmp;
        return false;
    }
    
}

/*bool Bloom::recvFindAndFilter( String begin, String end, String &data, uint32_t timeout)
{
    String data_tmp;
    data_tmp = recvString(target, timeout);

    if (BLDEBUG) Serial.println(data_tmp);

    int32_t index1 = data_tmp.indexOf(begin);
    int32_t index2 = data_tmp.indexOf(end);
    if (BLDEBUG) Serial.println(index1);
    if (BLDEBUG) Serial.println(index2);
    if (index1 != -1 && index2 != -1) {
        index1 += begin.length();
        data = data_tmp.substring(index1, index2);
        return true;
    }else{
      data = data_tmp;
      return false;
    }

}*/

void  Bloom::setEEPROMaddress( int nEEPROM ){
  Bloom::_nEEPROM = nEEPROM;
}