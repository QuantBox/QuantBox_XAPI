# Matlab接.Net版的接口

目标是能用Matlab也进行交易，已经测试通过。能接收行情，能下单，能撤单。<br/>
用户要使用时应当要在Matlab中对一些API进行封装，否则用起来太麻烦。<br/>
后期考虑模仿Trading Toolbox的调用方式<br/>
指定要加载的库即可使用对应版本的API

1. 将C#接口的回调函数的方式改成了事件方式
2. 在Matlab中用addlistener来注册事件处理函数
3. 用global来实现变量引用