## linux c编程综合项目：**基于linux系统的网络文件传输服务器**

#### 设计技术点：自动化编译工具、io多路复用、网络通信、进线程高并发、sqlite3数据库数据存储
> 首先使用自动化编译工具：makefile、shell等工具来实现自动化编译源码。  
> 然后结合io技术以及网络通信技术实现服务器与客户端的上传与下载以及实现加密操作  
> 通过进线程技术使得服务器能够实现同时处理多个不同客户端的请求  
> 然后数据库实现用户的注册与登录，对已经登录用户文件或者文件夹下的所有文件进行加密解密操作,  
> 并且加密解密密码都会保存到对应的后台数据库中


