[toc]



# 说明

本文档适用于非Ubuntu编译方式。



# 更新说明

| 版本 | 说明                 | 作者           | 日期          |
| ---- | -------------------- | -------------- | ------------- |
| V1.0 | WiFi连接的第一版文档 | 就叫我阿廖莎吧 | 2023年12月5日 |
| V1.1 | 修改硬件连接图 | 就叫我阿廖莎吧 | 2024年2月28日 |



# 硬件设置

根据自己的板子的原理图，设置usb hub芯片选择aic8800这个设备。

![image-20231205191832786](.\pictures\image-20231205191832786.png)

## 验证

在确保硬件设置没有问题后，给板子上电，等正常进入shell之后，创建如下的脚本：

```shell
#!/bin/sh
echo 4 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio4/direction
echo 1 > /sys/class/gpio/gpio4/value

echo host > /sys/devices/platform/ff3e0000.usb2-phy/otg_mode
```

- echo前3行用于设置GPIO0_A4_d引脚输出高电平，控制aic8800内部使能；最后一句用于设置usb进入host模式

最新的sdk自带的WiFi驱动在切换了usb为主机模式后不会触发枚举，这不影响下面的操作。在安装完WiFi驱动后通过其它节点查看是否成功操作。


**操作命令：**

```shell
# 命令
cd /data
vi usb_wifi.sh
chmod +x usb_wifi.sh
```


# 软件设置

> 所需的固件在群文件下载 fw.zip

由于当前SDK中，并未将WiFi驱动加载时需要的WiFi固件拷贝进rootfs，这需要在SDK做一些操作，在编译时拷贝到rootfs。

<p>
    这里将fw.zip文件解压出来的aic8800D80文件夹复制到<font color=#"FF0000"><strong>sdk_root/sysdrv/drv_ko</strong></font>目录下，并<font color=#"FF0000"><strong>修改该级目录下的Makefile文件</strong></font>。
</p>

![image-20231205193626838](.\pictures\image-20231205193626838.png)

然后整编SDK，编译完成后，可在`sdk_root/output/out/rootfs_uclibc_rv1106/oem/usr/ko`目录下确认是否拷贝成功。

然后烧录固件！（烧录固件时确保usb_sel引脚未短接到3v3）



# 驱动加载

<p>
    确保<font color=#"FF0000">硬件连接正常</font>、WiFi需要的<font color=#"FF0000">固件</font>也<font color=#"FF0000">拷贝到rootfs</font>后即可执行下面的操作
</p>

1. 执行硬件设置中的脚本，并确认有正确的打印

2. 进入`/oem/usr/ko`目录，使用ls命令可以看到下面的输出

   ```shell
   # ls
   insmod_wifi.sh       aic_btusb.ko         release_version.txt
   hpmcu_wrap.bin       rockit.ko            aic8800D80
   insmod_ko.sh         mpp_vcodec.ko
   ```

3. 加载`aic_load_fw.ko`驱动，执行下面的命令：

   ```shell
   insmod aic_load_fw.ko aic_fw_path=/oem/usr/ko/aic8800D80
   ```

   - aic_fw_path：该路径为aic8800D80所在目录，如果在rootfs其他位置，请改为正确的路径

   加载成功后会有如下打印：

   ![image-20231205200614964](.\pictures\image-20231205200614964.png)

   注意图中箭头指向的打印，如果WiFi固件不存在，则加载会报错。

4. 加载`insmod aic8800_fdrv.ko`驱动

   ```shell
   insmod aic8800_fdrv.ko
   ```

   加载成功后，控制台会输出一大堆打印，出现下图所示证明驱动加载成功，可以使用`lsmod`或者`ifconfig -a`命令查看

   ![image-20231205200839296](.\pictures\image-20231205200839296.png)



# 连接

<p>
    <font color=#"FF0000" size=5><strong>确保前面步骤成功执行</strong></font>
</p>

## 启动wpa_suppcliant

1. 使能网卡

   ```shell
   ifconfig wlan0 up
   ```

   通过ifconfig命令查看wlan0是否打开。

2. 启动wap_supplicant

   ```shell
   wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf
   ```

   成功执行后会出现如下打印：

   ```
   # wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf
   Successfully initialized wpa_supplicant
   ```

   并且在使用ps命令查看也可以看到：

   ```shell
    ...
    351 root      1232 S    wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.co
    ...
   ```



## 使用wpa_cli连接网络

1. 扫描网络，并获取结果

   ```shell
   wpa_cli scan
   wpa_cli scan_result
   ```

   以上两条命令可以反复执行，WiFi刚启动，可能扫描不出来太多热点，扫面成功后会打印：

   ![image-20231205202004519](.\pictures\image-20231205202004519.png)

2. 添加一个网络配置

   ```shell
   wpa_cli add_network
   ```

   成功执行后会返回当前创建的网络配置的id，第一次运行返回0，第二次返回1以此类推

3. 设置热点名和密码并保存当前配置

   ```shell
   wpa_cli set_network 0 ssid '"Deep_Space"'
   wpa_cli set_network 0 psk '"eihooMieChee0gez7Aeh"'
   wpa_cli save_config
   ```

   保存成功后，可以通过`cat /etc/wpa_supplicant.conf`命令查看。此处的0是刚才创建的配置文件的id

   ![image-20231205202340109](.\pictures\image-20231205202340109.png)

4. 选择刚才创建的热点进行连接

   ```shell
   wpa_cli select_work 0
   ```

   这里的0就是刚刚创建的第一个配置文件，如果有多份配置，则选择需要的。执行该命令后，控制台会出现一些打印，此时可以输入`wpa_cli status`查看当前的状态。

   如果一直出现下面的打印，说明热点扫描不到，可能的原因有：用户名、密码设置不正确。热点拒绝连接、热点距离较远。

   ![image-20231205202750443](.\pictures\image-20231205202750443.png)

   如果出现下图打印，则说明连接成功，

   ![image-20231205202919144](.\pictures\image-20231205202919144.png)

5. 获取ip地址

   ```shell
   udhcpc -i wlan0
   ```

   执行成功后，打印出申请到的Ip地址

   ![image-20231205203038311](.\pictures\image-20231205203038311.png)

   此时可以使用ping命令去测试联网情况

![image-20231205203114177](.\pictures\image-20231205203114177.png)



# 备注

<p>
    <font color=#"FF00F0">闻道有先后，术业有专攻</font>。如果有无法执行的步骤，可以在群里进行沟通，如果你又自己解决了，可以在当前文档进行补充，方便各爱好者的快速开发，大家都能够进步，提升。
</br><font style="line-height:2">最后祝大家生活愉快，工作顺利！朝日摆脱狗日的996！</font>
</p>







