#include "httplib.h"           //http库
#include <jsoncpp/json/json.h> //JSON库
#include <iostream>
#include <string>
using namespace std;

void doTestPost(const httplib::Request &req, httplib::Response &res)
{
    cout << "执行doTestPost" << endl;
    // 获取请求体
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);

    cout << "获取请求体信息：" << endl;
    cout << jsonvalue.toStyledString() << endl;

    // 处理逻辑...

    Json::Value resjson;
    resjson["result"] = "处理成功";
    resjson["info"] = jsonvalue;

    res.set_content(resjson.toStyledString(), "json");
}

int main()
{
    using namespace httplib;
    Server server;

    // Post请求
    server.Post("/test/post", doTestPost);
    cout << "post请求设置成功" << endl;
    // 设置监听端口
    server.listen("0.0.0.0", 9999);
    cout << "正在监听中..." << endl;
}