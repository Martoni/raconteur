//#include "DFRobotDFPlayerMini.h"


/* Serial */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "rc_dfr0299.h"

char *buff,*buffer,*bufptr;
int wr,rd,nbytes,tries;

int serial_open(const char *pathname){
    struct termios options;
    int fserial;

    fserial=open(pathname, O_RDWR|O_NOCTTY|O_NDELAY);
    if (fserial == -1 ){
        perror("open_port: Unable to open /dev/ttyS0 – ");
    } else {
        fcntl(fserial, F_SETFL,0);
        printf("Port 1 has been sucessfully opened and %d is the file description\n",fserial);
    }
    tcgetattr(fserial, &options);
    // Turn off s/w flow ctrl
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    // Disable any special handling of received bytes
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= CREAD | CLOCAL;
    options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);
    options.c_oflag &= ~(OPOST | ONLCR);
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 10;
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    if (tcsetattr(fserial, TCSANOW, &options) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }
//    fcntl(fserial, F_SETFL, FNDELAY); // non blocking read

    return fserial;
}

int main(int argc, char *argv[]){
    int fserial;
    int ret;
    int i;
    printf("Welcome in dfr0299 test\n");

    fserial = serial_open("/dev/ttyUSB0");

    ret = rc_uart_send(fserial, CMD_RESET, 0, 0);
    ret = rc_uart_rcv(fserial, 1);
    ret = rc_uart_send(fserial, CMD_SPECVOL, 0, 15);
    ret = rc_uart_rcv(fserial, 1);
    ret = rc_uart_send(fserial, CMD_PLAYBACK, 0, 0);
    ret = rc_uart_rcv(fserial, 1);
    ret = rc_uart_send(fserial, CMD_UDISKFILESNUM, 0,0);
    ret = rc_uart_rcv(fserial, 1);
    ret = rc_uart_send(fserial, CMD_SPECTRACK, 0,0x10);
    ret = rc_uart_rcv(fserial, 1);
    sleep(1);
    //ret = rc_uart_send(fserial, CMD_PAUSE, 0, 0);
    ret = rc_uart_rcv(fserial, 1);
    printf("wrote %02d\n", ret);
    if(ret < 0){
	    printf("Error on send %02d\n", ret);
    }
//    wr=write(fserial, _frame_get_current_status, DFPLAYER_SEND_LENGTH);
    //And for reading response from device:

    //At the end, close the connection:
    close(fserial);
    printf("Goodbye");
    return 0;
}
