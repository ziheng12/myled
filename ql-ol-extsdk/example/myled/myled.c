#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>



/*
  *  ledtest <dev> <on|off>
  */
void print_usage(char *file)
{
    printf("Usage:\n");
    printf("%s <dev> <on|off>\n",file);
    printf("eg. \n");
    printf("%s /dev/leds on\n", file);
    printf("%s /dev/leds off\n", file);
    printf("%s /dev/led1 on\n", file);
    printf("%s /dev/led1 off\n", file);
}


int main(int argc, char **argv)
{
    int fd;
    char* filename=NULL;
    char val;
    
    filename = argv[1];
    
    fd = open(filename, O_RDWR);//打开dev/firstdrv设备文件
    if (fd < 0)//小于0说明没有成功
    {
        printf("error, can't open %s\n", filename);
        return 0;
    }
    
    if(argc !=3)
    {
        print_usage( argv[1]);//打印用法
    }

    if(!strcmp(argv[2], "on"))
        val = 1;
   else
       val = 0;
   
    write(fd, &val, 1);//操作LED
    
   return 0;
}
