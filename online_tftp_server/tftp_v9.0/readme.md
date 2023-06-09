## linux c编程综合项目：**基于linux系统的可加密的网络文件传输服务器**

#### 一、设计技术点：自动化编译工具、io多路复用、网络通信、进线程高并发、sqlite3数据库数据存储
> 首先使用自动化编译工具：makefile、shell等工具来实现自动化编译源码。  
> 然后结合io技术以及网络通信技术实现服务器与客户端的上传与下载以及实现加密操作  
> 通过进线程技术使得服务器能够实现同时处理多个不同客户端的请求  
> 然后数据库实现用户的注册与登录，对已经登录用户文件或者文件夹下的所有文件进行加密解密操作,  
> 并且加密解密密码都会保存到对应的后台数据库中


#### 二、目录说明
- bin     存放已经编译成的二进制文件
- client  客户端的登录路径
- include 头文件
- res     文件服务器资源存放目录(目录里根据注册用户来创建新的对应的资源路径)
- src     源代码存放目录

#### 已经完成
- 实现了文件的上传和下载以及list以及清屏的功能
- 客户端添加了文件加密和解密的功能
- 使用多线程技术来使得服务器能并发处理多个套接字连接
- 实现了epoll来实现IO复用实现服务器能够几乎同时监听不同文件描述符产生的事件并作相应的处理
- 在客户端使用线程池来处理来自客户端的请求，从而达到了高并发的效果
- 实现makefile和shell脚本自动化编译和运行项目代码

#### 需要添加和改进的地方（暂时over）
- 资源路径问题解决办法(这个具体修改需要等到实现了用户登录注册的基础上才能真正落实！！！)：
  > 资源根路径固定，对于不同的用户登录时，则需要进入到独立的资源路径中，然后接收客户端的命令然后完成相应的操作
  > 后续需要修改的地方：server.c +22 +54 +144 
- 客户端加密解密文件功能基本实现，还存在小的bug
  > 当服务器资源被共享给多个用户时，加密解密逻辑会出问题（解决办法：不共享，每个用户都有各自的资源路径）
