#!/bin/sh 

# 这样直接拷贝过去取直接覆盖原来的头文件，因此需要进行备份
# 当编译不成功时，可以直接复原原来的能够通过正确编译的头文件
cp ../include/*.h ../include_bak/

# 将当前修改的头文件拷贝到指定头文件目录
cp *.h ../include/

# 拷贝客户端运行脚本
cp ./run2.sh ../clients/client/
cp ./run2.sh ../clients/client1/
cp ./.lock.db ../clients/client
cp ./.lock.db ../clients/client1

# 编译项目
make all

# 运行服务器
./server 192.168.0.106
