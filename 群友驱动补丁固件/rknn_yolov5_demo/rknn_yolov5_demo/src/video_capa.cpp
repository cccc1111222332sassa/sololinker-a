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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <assert.h>

#define LOG_NDEBUG 0
#undef LOG_TAG
#define LOG_TAG "rga_resize_demo"
#include "iostream"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <math.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <linux/stddef.h>
#include <string.h>
#include "RgaUtils.h"
#include "im2d.hpp"
#include "opencv2/core/core.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "utils.h"
#include "dma_alloc.h"
#include "rga.h"

#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "types_c.h"
#include "video_capa.h"

#include <iostream>
#include <chrono>
#include <thread>

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif
#include <linux/videodev2.h>
#define FILE_VIDEO  "/dev/video1"
#define JPG       "./image%d.yuv"
#define WIDTH  1920
#define HEIGHT 1080
#define FMT_NUM_PLANES 1


typedef struct{
    void *start;
    int length;
}   BUFTYPE;
BUFTYPE *usr_buf;
extern cv::Mat yolov5_img;
extern int key;

//set video capture ways(mmap)
int init_mmap(int fd)
{
    //to request frame cache, contain requested counts
    struct v4l2_requestbuffers reqbufs;
    //request V4L2 driver allocation video cache
    //this cache is locate in kernel and need mmap mapping
    memset(&reqbufs, 0, sizeof(reqbufs));
    reqbufs.count = 3;
    reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    reqbufs.memory = V4L2_MEMORY_MMAP;
    if(-1 == ioctl(fd,VIDIOC_REQBUFS,&reqbufs)){
        perror("Fail to ioctl 'VIDIOC_REQBUFS'");
        exit(EXIT_FAILURE);
    }
    n_buffer = reqbufs.count;
    printf("n_buffer = %d\n", n_buffer);
    usr_buf = (BUFTYPE*)calloc(reqbufs.count, sizeof(BUFTYPE));
    if(usr_buf == NULL){
        printf("Out of memory\n");
        exit(-1);
    }
    //map kernel cache to user process
    for(n_buffer = 0; n_buffer < reqbufs.count; ++n_buffer){
        //stand for a frame
        struct v4l2_buffer buf;
        struct v4l2_plane planes[FMT_NUM_PLANES];
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffer;
        if(V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf.type){
            buf.m.planes = planes;
            buf.length = FMT_NUM_PLANES;
        }
        //check the information of the kernel cache requested检查所请求的内核缓存的信息
        if(-1 == ioctl(fd,VIDIOC_QUERYBUF,&buf))
        {
            perror("Fail to ioctl : VIDIOC_QUERYBUF");
            exit(EXIT_FAILURE);
        }
        if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE ==  buf.type) {
               usr_buf[n_buffer].length = buf.m.planes[0].length;
               usr_buf[n_buffer].start =
                   mmap(NULL /* start anywhere */,
                         buf.m.planes[0].length,
                         PROT_READ | PROT_WRITE /* required */,
                         MAP_SHARED /* recommended */,
                         fd, buf.m.planes[0].m.mem_offset);
       } else {
           usr_buf[n_buffer].length = buf.length;
           usr_buf[n_buffer].start =
               mmap(NULL /* start anywhere 开始的地方*/,
                     buf.length,
                     PROT_READ | PROT_WRITE /* required */,
                     MAP_SHARED /* recommended */,
                     fd, buf.m.offset);
       }
        printf("usr_buf[%d].start=0x%x\n",n_buffer,usr_buf[n_buffer].start);

        if(MAP_FAILED == usr_buf[n_buffer].start)
        {
            perror("Fail to mmap");
            exit(EXIT_FAILURE);
        }
        printf("usr_buf %d: address=0x%x, length=%d\n", n_buffer, (unsigned int)usr_buf[n_buffer].start, usr_buf[n_buffer].length);
    }
    return 0;
}

//initial camera device
int init_camera_device(int fd)
{
    //decive fuction, such as video input
    struct v4l2_capability cap;
    //video standard,such as PAL,NTSC
    struct v4l2_standard std;
    //frame format
    struct v4l2_format tv_fmt;
    //check control
    struct v4l2_queryctrl query;
    //detail control value
    struct v4l2_fmtdesc fmt;
    int ret;
    //get the format of video supply  //获取视频格式
    memset(&fmt, 0, sizeof(fmt));
    fmt.index = 0;
    //supply to image capture  //提供图像采集
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    // show all format of supply  显示所有格式的供应
    printf("Support format:\n");
    /* 3.select the current video input 选择当前的视频输入*/
    struct v4l2_input inp;           /* select the current video input选择当前的视频输入*/
    memset(&inp, 0, sizeof(inp));
    inp.index = 0;
    inp.type = V4L2_INPUT_TYPE_CAMERA;
//    if (ioctl(fd, VIDIOC_S_INPUT, &inp) < 0) {
//         printf(" VIDIOC_S_INPUT failed! s_input: %d\n", inp.index);
//        close(fd); //关闭文件
//        return -1;
//    }
    //VIDIOC_ENUM_FMT用于枚举视频设备支持的视频格式
    while(ioctl(fd, VIDIOC_ENUM_FMT, &fmt) == 0)
    {
        fmt.index++;
        printf("pixelformat = ''%c%c%c%c''\ndescription = ''%s''\n",fmt.pixelformat & 0xFF, (fmt.pixelformat >> 8) & 0xFF,(fmt.pixelformat >> 16) & 0xFF,                   (fmt.pixelformat    >> 24) & 0xFF,fmt.description);
    }
    //check video decive driver capability  ，//用于查询视频设备的能力
    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if(ret < 0){
        perror("Fail to ioctl VIDEO_QUERYCAP");
        exit(EXIT_FAILURE);
    }
    // Print capability infomations
    printf("Capability Informations:\n");
    printf(" driver: %s\n", cap.driver);
    printf(" card: %s\n", cap.card);
    printf(" bus_info: %s\n", cap.bus_info);
    printf(" version: %08X\n", cap.version);
    printf(" capabilities: %08X\n", cap.capabilities);
    printf("----------------------------\n");
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
            !(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
        printf("The Current device is not a video capture device, capabilities: %x\n", cap.capabilities);
            exit(EXIT_FAILURE);
    }

    //judge whether or not to supply the form of video stream  判断是否提供视频流的格式
    if(!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        printf("The Current device does not support streaming i/o\n");
        exit(EXIT_FAILURE);
    }

    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
        buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    else if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)
        buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    //set the form of camera capture data 设置相机捕捉数据的格式
    memset(&fmt, 0, sizeof(   struct v4l2_fmtdesc));    // very important!must clear 0
    tv_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
//    tv_fmt.fmt.pix.width = WIDTH;     //1280;    //680;  //1920
//    tv_fmt.fmt.pix.height = HEIGHT;    //720;    //480;  //1080
//    2304×1296
    tv_fmt.fmt.pix.width = 1920;     //1280;    //680;  //1920
    tv_fmt.fmt.pix.height = 1080;    //720;    //480;  //1080

//    tv_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV21; //V4L2_PIX_FMT_NV12;
    tv_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV21; //V4L2_PIX_FMT_NV12;
    tv_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

//    tv_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE; //摄像头采集
//    tv_fmt.fmt.pix.width = 1920; //设置摄像头采集参数，不可以任意设置
//    tv_fmt.fmt.pix.height = 1088;
//    tv_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV21;

    if (ioctl(fd, VIDIOC_S_FMT, &tv_fmt)< 0) {
        printf("VIDIOC_S_FMT FAIL!\n");
        exit(-1);
        close(fd);
    }
    if(ioctl(fd, VIDIOC_G_FMT, &tv_fmt) < 0)
    {
        printf("VIDIOC_G_FMT FAIL!\n");
        exit(-1);
        close(fd);
    }

    // Print Stream Format  打印流格式
    printf("Stream Format Informations:\n");
    printf(" type: %d\n", tv_fmt.type);
    printf(" width: %d\n", tv_fmt.fmt.pix.width);
    printf(" height: %d\n", tv_fmt.fmt.pix.height);
    char fmtstr[8];
    memset(fmtstr, 0, 8);
    memcpy(fmtstr, &tv_fmt.fmt.pix.pixelformat, 4);
    printf(" pixelformat: %s\n", fmtstr);
    printf(" field: %d\n", tv_fmt.fmt.pix.field);
    printf(" bytesperline: %d\n", tv_fmt.fmt.pix.bytesperline);
    printf(" sizeimage: %d\n", tv_fmt.fmt.pix.sizeimage);
    printf(" colorspace: %d\n", tv_fmt.fmt.pix.colorspace);
    printf(" priv: %d\n", tv_fmt.fmt.pix.priv);
    printf(" raw_date: %s\n", tv_fmt.fmt.raw_data);
    //initial video capture way(mmap)  初始视频捕获方式(mmap)
    init_mmap(fd);
    return 0;
}

//char str[100];
//strcpy(str, "Hello, World!");
int open_camera_device()
{
    int fd;
    //open video device with block //用块打开视频设备
    fd = open("/dev/video11", O_RDWR | O_NONBLOCK, 0);
    if(fd < 0){
        perror(FILE_VIDEO);
        exit(EXIT_FAILURE);
    }
    return fd;
}

int start_capture(int fd)
{
    unsigned int i;
    enum v4l2_buf_type type;

    //place the kernel cache to a queue将内核缓存放置到一个队列
    for(i = 0; i < n_buffer; i++){
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type = buf_type;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf.type) {
            struct v4l2_plane planes[FMT_NUM_PLANES];

            buf.m.planes = planes;
            buf.length = FMT_NUM_PLANES;
        }

        if(-1 == ioctl(fd, VIDIOC_QBUF, &buf)){
            perror("Fail to ioctl 'VIDIOC_QBUF'");
            exit(EXIT_FAILURE);
        }
    }

    //start capture data
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if(-1 == ioctl(fd, VIDIOC_STREAMON, &type)){
        printf("i=%d.\n", i);
        perror("VIDIOC_STREAMON");
        close(fd);
        exit(EXIT_FAILURE);
    }
    return 0;
}



int process_image_video(void *addr, int length)
{

    int ret = 0;
    int src_width, src_height, src_format;
    int dst_width, dst_height, dst_format;
    char *src_buf, *dst_buf;
    int src_buf_size, dst_buf_size;
    int src_dma_fd, dst_dma_fd;
    rga_buffer_t src = {};
    rga_buffer_t dst = {};
    im_rect src_rect = {};
    im_rect dst_rect = {};
    rga_buffer_handle_t src_handle, dst_handle;
    int64_t ts;
#if 0
//    cv::Mat image_src =cv::imread("v4l2.jpg");
    Tbegin = std::chrono::steady_clock::now();//开始时间
    cv::Mat yuv(1080* 1.5, 1920, CV_8UC1, addr);
    cv::Mat image_src;
    cv::Mat resize_img;
    cv::cvtColor(yuv, image_src, CV_YUV2BGR_NV21);
    cv::resize(image_src,resize_img,cv::Size(640,640));
    extern cv::Mat yolov5_img;
    yolov5_img=resize_img;
    Tend = std::chrono::steady_clock::now();//结束时间
    f = std::chrono::duration_cast <std::chrono::milliseconds> (Tend - Tbegin).count();
    std::cout << "time读取摄像头process_image_video111 : " << f/1000.0 << " Sec" << std::endl;
    src_width = image_src.cols;
    src_height = image_src.rows;
    cv::imwrite("resize.jpg",resize_img);
#endif
//    Tbegin = std::chrono::steady_clock::now();//开始时间
    src_width = 1920;
    src_height = 1080;
//    std::cout <<"src_width=" <<src_width <<std::endl;
    src_format = RK_FORMAT_YCrCb_420_SP;
    dst_width = 640;
    dst_height =640;
    dst_format = RK_FORMAT_RGB_888;
//    std::cout <<"src_width=" <<src_width <<std::endl;
    cv::Mat resize_img;
    src_buf_size = src_width * src_height * get_bpp_from_format(src_format);
    dst_buf_size = dst_width * dst_height * get_bpp_from_format(dst_format);
    /* Allocate dma_buf from CMA, return dma_fd and virtual address */
    ret = dma_buf_alloc(RV1106_CMA_HEAP_PATH, src_buf_size, &src_dma_fd, (void **)&src_buf);
    if (ret < 0) {
        printf("alloc src CMA buffer failed!\n");
        return -1;
    }
    ret = dma_buf_alloc(RV1106_CMA_HEAP_PATH, dst_buf_size, &dst_dma_fd, (void **)&dst_buf);
    if (ret < 0) {
        printf("alloc dst CMA buffer failed!\n");
        dma_buf_free(src_buf_size, &src_dma_fd, src_buf);
        return -1;
    }
    memcpy(src_buf, addr,src_buf_size);//这段代码使用了 memcpy 和 memset 函数，分别用于内存数据的复制和填充
    memset(dst_buf, 0x33, dst_buf_size);
    /*
     * Import the allocated dma_fd into RGA by calling
     * importbuffer_fd, and use the returned buffer_handle
     * to call RGA to process the image.
     */
     src_handle = importbuffer_fd(src_dma_fd, src_buf_size);
     dst_handle = importbuffer_fd(dst_dma_fd, dst_buf_size);
//     src_handle = importbuffer_virtualaddr(addr, src_buf_size);
//     dst_handle = importbuffer_fd(dst_dma_fd, dst_buf_size);
    if (src_handle == 0 || dst_handle == 0) {
        printf("import dma_fd error!\n");
        ret = -1;
       // goto free_buf;
    }
    src = wrapbuffer_handle(src_handle, src_width, src_height, src_format);// 快速封装图像缓冲区结构（rga_buffer_t）。
    dst = wrapbuffer_handle(dst_handle, dst_width, dst_height, dst_format); //快速封装图像缓冲区结构（rga_buffer_t）。
    /*
     * Scale up the src image to 1920*1080.
        --------------    ---------------------
        |            |    |                   |
        |  src_img   |    |     dst_img       |
        |            | => |                   |
        --------------    |                   |
                          |                   |
                          ---------------------
     */
    ret = imcheck(src, dst,src_rect, dst_rect);//： 校验参数是否合法，以及当前硬件是否支持该操作。
    if (IM_STATUS_NOERROR != ret) {
        printf("%d, check error! %s", __LINE__, imStrError((IM_STATUS)ret));
        return -1;
    }
    ret = imresize(src, dst);//图像缩放、图像金字塔
    if (ret == IM_STATUS_SUCCESS) {
//        printf("%s running success!\n", LOG_TAG);
//         printf("%s RGA RESIZE! cost %ld us\n", LOG_TAG, get_cur_us() - ts);
    } else {
        printf("%s running failed,l %s\n", LOG_TAG, imStrError((IM_STATUS)ret));
       // goto release_buffer;
    }

    cv::Mat _img = cv::Mat(cv::Size(dst_width, dst_height), CV_8UC3, dst_buf);
   // cv::Mat _img2 = cv::Mat(cv::Size(src_width, src_height), CV_8UC3, src_buf);
//    Tend = std::chrono::steady_clock::now();//结束时间
//    f = std::chrono::duration_cast <std::chrono::milliseconds> (Tend - Tbegin).count();
//    std::cout << "time读取摄像头process_image_video111 : " << f/1000.0 << " Sec" << std::endl;
    cv::cvtColor(_img, _img, cv::COLOR_RGB2BGR);
//    yolov5_img=_img;
//    cv::Mat image_src1 =cv::imread("v4l2.jpg");
    _img.copyTo(yolov5_img);
//    yolov5_img=_img;
//    cv::imwrite("yolov5.jpg",yolov5_img);
//    cv::imwrite("resize.jpg",yolov5_img);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    dma_buf_free(src_buf_size, &src_dma_fd, src_buf);
    dma_buf_free(dst_buf_size, &dst_dma_fd, dst_buf);
release_buffer:
    if (src_handle)
        releasebuffer_handle(src_handle);
    if (dst_handle)
        releasebuffer_handle(dst_handle);

//    if (src_buf)
//        free(src_buf);
//    if (dst_buf)
//        free(dst_buf);

    return ret;




}
int read_frame(int fd)
{
    struct v4l2_buffer buf;
    unsigned int i, bytesused;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE ==  buf.type) {
        struct v4l2_plane planes[FMT_NUM_PLANES];
        buf.m.planes = planes;
        buf.length = FMT_NUM_PLANES;
    }
    //put cache from queue将缓存从队列中移除
    if(-1 == ioctl(fd, VIDIOC_DQBUF,&buf)){
        perror("Fail to ioctl 'VIDIOC_DQBUF'");
        exit(EXIT_FAILURE);
    }

    assert(buf.index < n_buffer);
//    printf("buf.index=%d,n_buffer=%d\n",buf.index,n_buffer);
//    printf(" usr_buf[%d].start=0x%x,usr_buf[%d].length=0x%x\n", buf.index,usr_buf[buf.index].start,buf.index,usr_buf[buf.index].length);
    if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf.type)
        bytesused = buf.m.planes[0].bytesused;
    else
        bytesused = buf.bytesused;

//     printf("read_frame bytesused=%d\n",bytesused);
    //read process space's data to a file //读取进程空间的数据到一个文件
    process_image_video(usr_buf[buf.index].start, bytesused);

    if(-1 == ioctl(fd, VIDIOC_QBUF,&buf))
    {
        perror("Fail to ioctl 'VIDIOC_QBUF'");
        exit(EXIT_FAILURE);
    }
    return 1;
}
int mainloop(int fd)
{
    int count =1000000;
//    while(1)
//    {
        for(;;)
        {
            fd_set fds;
            struct timeval tv;
            int r;
            FD_ZERO(&fds);
            FD_SET(fd,&fds);

            /*Timeout*/
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            r = select(fd + 1,&fds,NULL,NULL,&tv);

            if(-1 == r)
            {
                if(EINTR == errno)
                    continue;
                perror("Fail to select");
                //exit(EXIT_FAILURE);
            }

            if(0 == r)
            {
                fprintf(stderr,"select Timeout\n");
                exit(-1);
            }

            if(read_frame(fd))
                break;
        }
//    }
    return 0;
}

void stop_capture(int fd)
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if(-1 == ioctl(fd,VIDIOC_STREAMOFF,&type))
    {
        perror("Fail to ioctl 'VIDIOC_STREAMOFF'");
        exit(EXIT_FAILURE);
    }
    return;
}

void close_camera_device(int fd)
{
    unsigned int i;
    for(i = 0;i < n_buffer; i++)
    {
        if(-1 == munmap(usr_buf[i].start,usr_buf[i].length)){
            exit(-1);
        }
    }
    free(usr_buf);

    if(-1 == close(fd))
    {
        perror("Fail to close fd");
        exit(EXIT_FAILURE);
    }
    return;
}

int fd;
#define LOCAL_FILE_PATH "/data"
int cap(int argc, char *argv[])
{

//    int ret;
//    char str[100];
//    strcpy(str, argv[1]);
//    fd = open_camera_device(str);
//    printf("-------open_camera_device-------\n");
//    init_camera_device(fd);
//    //usleep(1000000); //usleep(10000); //单位是微秒 1000000us = 1s
//    printf("-------init_camera_device-------\n");
//    start_capture(fd);
//    printf("-------start_capture-------\n");
//    mainloop(fd);
//    printf("-------mainloop-------\n");
//    stop_capture(fd);
//    printf("-------stop_capture-------\n");
//    close_camera_device(fd);
//    printf("-------close_camera_device-------\n");

    return 0;
}



