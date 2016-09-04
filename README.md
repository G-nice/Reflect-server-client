# Reflect Server & Client

----------

This is a simple example of Linux Network programming. Reflect server will send what the client send to the server back to the client.

# Build
run `build.sh` to build the project. This will generate `reflect_server` and 'reflect_client' on the current directory.

```bash
	bash build.sh
```

# Usage
The server run on a IP address and port whitch given by the user setting the argument when start the server. The client is the same as the server.
##　Argument Format:
* `./reflect_server <IP> <port>`
* `./reflect_client <IP> <port>`
```bash
	./reflect_server 127.0.0.1 8888
	./reflect_client 127.0.0.1 8888
```


----------

----------

# 回射服务器及其客户端

----------
回射服务器是一个Linux网络编程中的简单例子。服务器会向客户端发送客户端所发送过来的所有内容。

# 构建
运行shell文件`build.sh`来构建项目。运行后会在当前目录生成 `reflect_server` 和 'reflect_client'两个可执行文件

```bash
	bash build.sh
```
# 使用方法
服务器需要在启动参数中指定运行的IP地址以及端口号，客户端运行时同样需要在参数中给定服务器的IP地址以及端口号。
## 参数格式:
* `./reflect_server <IP> <port>`
* `./reflect_client <IP> <port>`
```bash
	./reflect_server 127.0.0.1 8888
	./reflect_client 127.0.0.1 8888
```
