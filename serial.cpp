#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/epoll.h>

#define ttyDevice "/dev/ttyS0"
#define MAX_EVENTS 10           // epoll_wait() 一次最多处理的事件数

class SerialPort{

private:
	int serial_fd;
	int buoundRate;
public:
	SerialPort() : serial_fd(-1), buoundRate(115200) {     //构造函数1
		serial_fd = -1;
	}
	
	SerialPort(int rate) : buoundRate(rate) { //构造函数2 提供给外部重载参数用

	}
		
	int openPort(void)
	{
        //打开设备
		serial_fd = open(ttyDevice , O_RDWR| O_NOCTTY|O_NONBLOCK); // O_NONBLOCK 表示非阻塞, 跟O_NDELAY是一样的，表示read时没数据直接返回错误 而不是阻塞   使用 O_NOCTTY 选项，可以避免串口设备成为进程的控制终端，这样可以防止串口的输入输出干扰进程的交互 

        if(serial_fd == -1) {
            std::cerr << "无法打开串口设备" << std::endl;
            return false;
        }  
        
        if (isatty(serial_fd) == 0) {
            std::cerr << "非串口设备，检查设备路径" << std::endl;
            return false;
        }

        //配置tty参数
        struct termios tty;
        memset(&tty, 0, sizeof(tty));

        if (tcgetattr(serial_fd, &tty) != 0) {
            std::cerr << "无法获取串口参数" << std::endl;
            closePort();
            return false;
        }

        cfsetospeed(&tty, buoundRate);  //设置输出波特率
        cfsetispeed(&tty, buoundRate);  //设置输入波特率
#if 0 
        //也可以用ioctl + terimos方法设置波特率, 如下：
        {
            struct termios2 tio;

            if (ioctl(serial_fd, TCGETS2, &tio)) {
                perror("TCGETS2");
                return -1;
            }

            tio.c_cflag &= ~CBAUD;
            tio.c_cflag |= BOTHER;
            tio.c_ispeed = baudrate;
            tio.c_ospeed = baudrate;

            if (ioctl(serial_fd, TCSETS2, &tio)) {
                perror("TCSETS2");
                return -1;
            }

            if (ioctl(serial_fd, TCGETS2, &tio)) {
                perror("TCGETS2");
                return -1;
            }
        }
#endif

        tty.c_cflag |= CLOCAL | CREAD; // CLOCAL：该标志用于表示本地连接  CREAD：该标志用于表示使能串口接收功能

        tty.c_cflag &= ~CSIZE; CSIZE; //清除 c_cflag 中的 CSIZE 位  #define CSIZE   (CS5 | CS6 | CS7 | CS8)  /* 字符大小掩码 */
        tty.c_cflag |= CS8;   // 将 CS8 标志位设置到 c_cflag 中  表示数据位8位

        tty.c_cflag &= ~PARENB; //禁用奇偶校验
        tty.c_cflag &= ~CRTSCTS; //禁用RTS/CTS硬件流控


        tty.c_lflag = 0;
        tty.c_lflag = ~NOFLSH; // 输入的特殊字符（如 Ctrl+C）不会产生相应的信号
        // tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //禁用了这几个输入模式的特性 不关键

        tty.c_iflag &= ~INPCK;  //禁用输入奇偶校验
        // tty.c_iflag &= ~(IXON | IXOFF | IXANY);  //用于控制终端的输入模式 不关键

        tty.c_oflag &= ~OPOST; //直接输出原始的字符  不关键

        tty.c_cc[VMIN] = 1; //要求读取最小字符数  用来触发fd的事件

        tty.c_cc[VTIME] = 0; // 设置为0表示非阻塞等待

        tcflush(serial_fd, TCIOFLUSH); //刷新输入输出缓冲

        if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {  //设置属性,立即生效  TCSANOW TCSADRAIN：等待所有输出完成后生效  TCSAFLUSH：清空输入输出队列后生效
            std::cerr << "无法设置串口参数" << std::endl; 
            close(serial_fd);
            return false;
        }

        return true;
	}

	int closePort(void) {
        if (serial_fd != -1) {
            close(serial_fd);
            serial_fd = -1;
        }
        return 0;
	}

    int getFd() const {
        return serial_fd;
    }

    int readData(char *buffer, int size) {
        int bytes_read = read(serial_fd, buffer, size);
        if (bytes_read == -1) {
            std::cerr << "读取串口数据时发生错误" << std::endl;
        }
        return bytes_read;
    }

    int writeData(const char *data, int size) {
        int bytes_written = write(serial_fd, data, size);
        if (bytes_written == -1) {
            std::cerr << "写入串口数据时发生错误" << std::endl;
        }
        return bytes_written;
    }
};


int main() {

    SerialPort serialPort(115200);

    if (!serialPort.openPort()) {
        return -1;
    }

    int epoll_fd = epoll_create1(0); // 动态监听不同个数的文件描述符产生的事件 可以用epoll_create(1024)代替
    if (epoll_fd == -1) {
        std::cerr << "无法创建 epoll 文件描述符" << std::endl;
        return -1;
    }

    struct epoll_event event;
    event.events = EPOLLIN; // 监听可读事件 跟串口设置中的tty.c_cc[VMIN]关联
    event.data.fd = serialPort.getFd();
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serialPort.getFd(), &event) == -1) {
        std::cerr << "无法添加串口文件描述符到 epoll" << std::endl;
        return -1;
    }

    struct epoll_event events[MAX_EVENTS];
    char receive_buffer[256];

    while (true) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            std::cerr << "epoll_wait 错误" << std::endl;
            break;
        }

        for (int i = 0; i < num_events; ++i) {
            if (events[i].data.fd == serialPort.getFd()) {
                // 从串口读取数据
                int bytes_read = serialPort.readData(receive_buffer, sizeof(receive_buffer));
                if (bytes_read > 0) {
                    // 在这里处理接收到的数据
                    // 例如打印数据、解析数据等
                    std::cout << "接收到数据: " << std::string(receive_buffer, bytes_read) << std::endl;
                }
            }
        }
    }

    close(epoll_fd);
    serialPort.closePort();
    return 0;
}

/*
说明:

在UNIX和类UNIX系统中，每个进程都有一个控制终端，通常是一个终端设备，它用于处理标准输入（stdin）、标准输出（stdout）和标准错误输出（stderr）。
当一个终端设备成为进程的控制终端时，这个进程就可以与用户进行交互，例如读取用户的输入和输出结果。
而一旦一个进程打开了一个终端设备作为文件描述符，并且没有指定 O_NOCTTY 选项，那么该终端设备将成为该进程的控制终端。


*/


/*
`cfsetospeed` 和 `ioctl` 是用于设置串口波特率的两种不同方法。它们的区别主要在于操作的对象和实现方式。

1. `cfsetospeed`：
   - `cfsetospeed` 是一个函数，用于设置终端（包括串口）的输出波特率（Output Baud Rate）。
   - 它是 POSIX 标准中定义的串口编程函数，在 Linux 和其他 POSIX 系统中使用。
   - 使用 `cfsetospeed` 函数时，需要事先获取终端的属性信息，并将波特率设置到 `termios` 结构体中的 `c_ispeed` 和 `c_ospeed` 成员中，然后再使用 `tcsetattr` 函数将修改后的终端属性写回设备。

2. `ioctl`：
   - `ioctl` 是一个系统调用，用于执行控制命令（IO Control）。在串口编程中，它常用于设置和获取终端的属性信息。
   - 使用 `ioctl` 时，需要指定要执行的控制命令以及相应的参数，通常传递一个 `termios` 或 `termios2` 结构体作为参数。
   - `cfsetospeed` 其实是 `ioctl` 的一个特定用法，用于设置终端的输出波特率。它相当于执行了 `ioctl(fd, TCSETS2, &tio)` 中的 `TCSETS2` 控制命令，并将波特率设置到 `termios2` 结构体中。

综上所述，`cfsetospeed` 和 `ioctl` 本质上都是用于设置终端属性的，`cfsetospeed` 是一种更高级、更方便的设置波特率的函数，而 `ioctl` 则是一种更通用的控制命令执行接口，可以用于多种设置和操作终端的场景。

在 POSIX 系统中，如果只需要设置波特率，使用 `cfsetospeed` 是更简单的选择。但如果需要进行更复杂的终端属性设置或其他控制操作，可以使用 `ioctl` 并结合相应的控制命令进行实现。
*/
