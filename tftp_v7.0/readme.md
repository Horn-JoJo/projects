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
- client1 另一个客户端的登录路径
- include 头文件
- log     服务器系统输出日志(守护进程来实现)
- res     文件服务器资源存放目录(目录里根据注册用户来创建新的对应的资源路径)
- src     源代码存放目录

#### 已经完成
- 实现文件的上传与下载以及显示服务器资源路径的功能
- 将服务器的循环服务器模型改为多线程技术实现的并发服务器模型
- 优化了makefile 使其更加灵活

#### 需要添加和改进的地方
- list功能已经在服务器和客户端上调通了，并且能够正常显示了，但还存在如下不足(这个功能暂时先不管了，后续优化)：
	> list显示所有的信息都是追加到一个buf中，然后一次性发送给客户端，但是如果一个目录中的文件太多时，信息可能装不下，这回导致信息list目录下的信息不全！！！
	> 目前只能显示固定目录下的文件及其信息！！！
- 先解决之前遗留的输入的命令的格式的bug！！！(目前已经解决掉！！！但是还存在隐患：就是当空格比较多是，自己设置的缓存大小会不够，如果超了程序会崩溃)
	> get + 文件名中间必须是一个空格(已经解决)
	> put + 文件名中间必须是一个空格(已经解决)
- 优化了之前的加密解密代码，使其能够输入一个字符串密钥！！！, 并且整合到客户端的代码逻辑中！！！
	> 但还存在如下bug:
	> 1. 有时候输入的加密字符串不能进行加密
	> 2. 不能确定文件的加密和解密状态，需要用户自己记住
	> 3. 加密和解密方法采用的异或运算的方式，因此，加密和解密的密码是不一样的！！！，需要用户自己记住！！！
- 增加io多路复用(已经使用poll实现io多路复用功能)服务器在监听的同时，也能有自己的功能：如打印系统日志(记录运行状态和用户登录信息！！！)
	> 已经实现io多路复用来实现同时监听多个文件描述符产生的事件，如标准输入事件和监听套接字事件
- 增加登录，注册用户功能(如果增加了用户登录后才能上传和下载文件的话，则客户端list、put和get功能必须是保证在在线的情况下才能上传和下载,这样的话就成了云盘服务器，先暂时不考虑！！！)
- 根据用户登录的情况选择对应自己的资源存放路径
- 把客户端优化到Qt上！！！(这个暂时不考虑)