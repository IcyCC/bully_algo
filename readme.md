# bully 算法

## 网络库

### 事件

1. Task

代表一个任务, (选做 可以设置, 结果, 设置完成时候的回调, 查看完成状态)

``` 
注意 nocopy
class Task {

data: 
    cb, // call back
method:
    SetDone();
    IsDone();
    AddDoneCallBack(Task);
};
```

2. EventLoop

```
注意是一个单例

class EventLoop{
data:
    _poller
method:
    CreateTask(cb);
    RunLoop();
    GetInstance();
}

```

3. Channel

```

class Channel {
 data:
    int _fd;
    write_cb;
    read_cb;
}

```


4. TcpConn && TcpServer
参考
[tcp](https://github.com/yedf/handy/blob/master/handy/conn.h)



总体流程:

建立 Eventloop , EventLoop 驱动poller 和 timer , poller 里有channels, 用来确定fd-事件和
对应回调的关系. 每个socket会绑定一个channel, tcpconn 和 tcpserve 提供到channel 绑定的一个封装


客户端
```c++
TcpConnPtr con = TcpConn::createConnection(&base, host, port);
con->onState([=](const TcpConnPtr& con) {
    info("onState called state: %d", con->getState());
});
con->onRead([](const TcpConnPtr& con){
    info("recv %lu bytes", con->getInput().size());
    con->send("ok");
    con->getInput().clear();
});
```

服务端
```c++
TcpConnPtr con = TcpConn::createConnection(&base, host, port);
con->onState([=](const TcpConnPtr& con) {
    info("onState called state: %d", con->getState());
});
con->onRead([](const TcpConnPtr& con){
    info("recv %lu bytes", con->getInput().size());
    con->send("ok");
    con->getInput().clear();
});
```

###

## 实体

```
class Message {
    from
    to
    type
    msg
}
```
```
class Node {
    data:
    
    id
    state
    neighbors
    
    method:
    Serve() // 运行服务
    Send()
    
       
}   
```



流程
```
   node = Node()
   node.Serve() 
```
