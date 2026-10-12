/*============================================================================
w 打开只写文件，若文件存在则文件长度清为0，即该文件内容会消失。若文件不存在则建立该文件。
w+ 打开可读写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
a 以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留。（EOF符保留）
a+ 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留。 （原来的EOF符不保留)

在进行V4L2开发中，常用的命令标志符如下(some are optional)：
?    VIDIOC_REQBUFS：分配内存
?    VIDIOC_QUERYBUF：把VIDIOC_REQBUFS中分配的数据缓存转换成物理地址
?    VIDIOC_QUERYCAP：查询驱动功能
?    VIDIOC_ENUM_FMT：获取当前驱动支持的视频格式
?    VIDIOC_S_FMT：设置当前驱动的频捕获格式
?    VIDIOC_G_FMT：读取当前驱动的频捕获格式
?    VIDIOC_TRY_FMT：验证当前驱动的显示格式
?    VIDIOC_CROPCAP：查询驱动的修剪能力
?    VIDIOC_S_CROP：设置视频信号的边框
?    VIDIOC_G_CROP：读取视频信号的边框
?    VIDIOC_QBUF：把数据从缓存中读取出来
?    VIDIOC_DQBUF：把数据放回缓存队列
?    VIDIOC_STREAMON：开始视频显示函数
?    VIDIOC_STREAMOFF：结束视频显示函数
?    VIDIOC_QUERYSTD：检查当前视频设备支持的标准，例如PAL或NTSC。

=============================================================================*/
#ifndef __VIDEO_CAPA_H__
#define __VIDEO_CAPA_H__
//#include <unistd.h>
#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <stdio.h>
//#include <sys/ioctl.h>
//#include <stdlib.h>
#include <linux/types.h>
#include <linux/videodev2.h>
//#include <malloc.h>
//#include <math.h>
//#include <string.h>
//#include <sys/mman.h>
//#include <errno.h>
//#include <assert.h>



#ifdef __cplusplus
extern "C" {
#endif

enum io_method {
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR,
    IO_METHOD_DMABUF,
};




struct buffer {
    void *start;
    size_t length;
    struct v4l2_buffer v4l2_buf;
};

static int io = IO_METHOD_MMAP;
static enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

static unsigned int n_buffer = 0;
int init_mmap(int fd);
int init_camera_device(int fd);
int open_camera_device();
int start_capture(int fd);
int process_image_video(void *addr, int length);
int read_frame(int fd);
int mainloop(int fd);
void stop_capture(int fd);
void close_camera_device(int fd);
int cap();
int n();

#ifdef __cplusplus
}
#endif
#endif /*__DRM_FUNC_H__*/
