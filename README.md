# QuantBox_XAPI 统一行情交易接口

##介绍
现在市场上的交易与行情API太多，对每家都接特别麻烦，如果能统一用一套API接入就好了。<br/>
如果进行接口的统一，要实现以下几点：

1. 统一的结构体
2. 统一的调用方式
3. 灵活的加载方式

实现统一的结构体必然会有些API的特殊字段没法照顾到;调用方式统一则有些功能将不会实现，而有些功能需要几个请求，或本机模拟的方法来实现;加载方式以前C#层是用的P/Invoke,现在改用LoadLibraryEx/dlopen来实现。

## 如何使用C#版
编译QuantBox.XAPI，有两种输出方式

1. 生成QuantBox.XAPI.dll,设置输出类型为类库，可以在其它项目中引用
2. 生成QuantBox.XAPI.exe,设置输出类型为控制台程序，将运行项目中的Program.cs文件，是API的使用示例

### 在Windows .Net项目中使用
1. 使用Visual Studio 2013打开QuantBox\_XAPI\_Windows.sln进行编译，会生成
> **QuantBox\.XAPI\.dll**:C#接口，由它进行C接口的加载<br/>
> **QuantBox\_Queue.dll**:C接口，消息队列，行情与交易的数据先进入队列然后，由内部的线程进行触发<br/>
> **QuantBox\_CTP\_Quote.dll**:C接口,调用CTP的行情API<br/>
> **QuantBox\_CTP\_Trade.dll**:C接口,调用CTP的交易API<br/>
> 其它如XSpeed/LTS等行情与交易接口

### 在Linux Mono项目中使用
1. 使用Mono Develop打开QuantBox\_XAPI\_CSharp\_Linux.sln进行编译，会生成
> **QuantBox\.XAPI\.dll**:C#接口，由它进行C接口的加载

2. 使用Code::Blocks打开QuantBox\_XAPI\_C\_Linux.workspace进行编译，注意得使用Clang编译器，会生成
> **libQuantBox\_Queue.so**:C接口，消息队列，行情与交易的数据先进入队列然后，由内部的线程进行触发<br/>
> **libQuantBox\_CTP\_Quote.so**:C接口,调用CTP的行情API<br/>
> **libQuantBox\_CTP\_Trade.so**:C接口,调用CTP的交易API<br/>
> 其它如XSpeed/LTS等行情与交易接口

3. 将所有生成的lib*.so文件cp命令复制到/usr/lib下

4. 将CTP的两个文件\*.so前面加lib复制到/usr/lib下

## 如何使用C++版
直接参考QuantBox_XAPI_TEST项目即可，简单的行情订阅，下单撤单

## 如何使用C版
参考QuantBox_XAPI这个项目中的CXApi与CXSpi是如何实现的

## 如何开发新的接口
1. 模仿已经提供的几个项目即可

