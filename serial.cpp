#include<iostream>
#include<stdlib.h>     /*标准函数库定义*/    
#include<unistd.h>     /*Unix 标准函数定义*/    
#include<sys/types.h>     
#include<sys/stat.h>       
#include<fcntl.h>      /*文件控制定义*/    
#include<termios.h>    /*PPSIX 终端控制定义*/    
#include<errno.h>      /*错误号定义*/    
#include<string.h>  

using namespace std;

/**
* @brief 打开串口
*
* @param const char* port 串口名称
*
* @return int fd 串口打开返回的数值
*/
int open_port (const char* port){
	int fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY); 
		if (-1 == fd)
		{ 
            perror("Can't Open Serial Port"); 
			return(-1); 
		} 
    
     //恢复串口为阻塞状态
    if(fcntl(fd, F_SETFL, 0)<0) {
        printf("fcntl failed!\n"); 
    }
    else {
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0)); 
    }
		
    //测试是否为终端设备
    if(isatty(STDIN_FILENO)==0) {
        printf("standard input is not a terminal device\n"); 
    }
    else {
        printf("isatty success!\n"); 
    }
		
    printf("fd-open=%d\n",fd); 
    return fd; 
}



/**
* @brief 串口初始化
*
* @param int fd 第一步返回的数值
* @param int nSpeed 波特率
* @param int nBits 传输位数 填写7或8
* @param char nEvent 奇偶校验位，o奇数，e偶数，n无
* @param int nStop 停止位 填1或者2
*
* @return int 表示是否成功的数
*/
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop) 
{ 
	//定义两个结构体
    struct termios newtio,oldtio; 

	//保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息
    if  ( tcgetattr( fd,&oldtio)  !=  0) {  
        perror("SetupSerial 1");
	    printf("tcgetattr( fd,&oldtio) -> %d\n",tcgetattr( fd,&oldtio)); 
        return -1; 
    } 
    bzero( &newtio, sizeof( newtio ) ); 

    newtio.c_cflag  |=  CLOCAL | CREAD;  

    //设置传输位数
    switch( nBits ) 
    { 
        case 7: 
            newtio.c_cflag &= ~CSIZE;  
            newtio.c_cflag |= CS7; 
            break; 
        case 8: 
            newtio.c_cflag &= ~CSIZE;  
            newtio.c_cflag |= CS8; 
            break; 
    } 


    //设置奇偶校验位
    switch( nEvent ) 
    { 
        case 'o':
        case 'O': //奇数 
            newtio.c_cflag |= PARENB; 
            newtio.c_cflag |= PARODD; 
            newtio.c_iflag |= (INPCK | ISTRIP); 
            break; 
        case 'e':
        case 'E': //偶数 
            newtio.c_cflag |= PARENB; 
            newtio.c_cflag &= ~PARODD; 
            newtio.c_iflag |= (INPCK | ISTRIP); 
            break;
        case 'n':
        case 'N':  //无奇偶校验位 
            newtio.c_cflag &= ~PARENB; 
            break;
            default:
            break;
    } 


    //设置波特率
    switch( nSpeed ) 
    { 
        case 2400: 
            cfsetispeed(&newtio, B2400); 
            cfsetospeed(&newtio, B2400); 
            break; 
        case 4800: 
            cfsetispeed(&newtio, B4800); 
            cfsetospeed(&newtio, B4800); 
            break; 
        case 9600: 
            cfsetispeed(&newtio, B9600); 
            cfsetospeed(&newtio, B9600); 
            break; 
        case 115200: 
            cfsetispeed(&newtio, B115200); 
            cfsetospeed(&newtio, B115200); 
            break; 
        case 460800: 
            cfsetispeed(&newtio, B460800); 
            cfsetospeed(&newtio, B460800); 
            break; 
        default: 
            cfsetispeed(&newtio, B9600); 
            cfsetospeed(&newtio, B9600); 
            break; 
    } 
	//设置停止位
    if( nStop == 1 ) {
        newtio.c_cflag &=  ~CSTOPB; 
    }
    else if ( nStop == 2 ) {
        newtio.c_cflag |=  CSTOPB; 
    }
    
	//设置等待时间和最小接收字符
    newtio.c_cc[VTIME]  = 0; 
    newtio.c_cc[VMIN] = 0; 

	//处理未接收字符
    tcflush(fd,TCIFLUSH); 

	//应用新配置
    if((tcsetattr(fd,TCSANOW,&newtio))!=0) 
    { 
        perror("com set error"); 
        return -1; 
    } 
    
    printf("set done!\n"); 
    return 0; 
} 


int main(){
    int fd=open_port("/dev/ttyUSB0");
    set_opt(fd, 115200, 8, 'O', 1) ;
    return 0;
}
