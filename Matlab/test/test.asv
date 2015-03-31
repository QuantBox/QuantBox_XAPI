%% 导入C#库，请按自己目录进行调整
cd 'd:\wukan\Documents\GitHub\QuantBox_XAPI\Matlab\test'
NET.addAssembly(fullfile(cd,'QuantBox.XAPI.dll'));

import QuantBox.XAPI.Event.*;

%% 行情
global md;
md = XApiWrapper('QuantBox_CTP_Quote.dll');
md.Server.BrokerID = '1017';
md.Server.Address = 'tcp://ctpmn1-front1.citicsf.com:51213;';

md.User.UserID = '00000015';
md.User.Password = '123456';

addlistener(md,'OnConnectionStatus',@OnMdConnectionStatus);
addlistener(md,'OnRtnDepthMarketData',@OnRtnDepthMarketData);

md.Connect();

%% 交易
global td;
td = XApiWrapper('QuantBox_CTP_Trade.dll');
td.Server.BrokerID = '1017';
td.Server.Address = 'tcp://ctpmn1-front1.citicsf.com:51205;';

td.User.UserID = '00000015';
td.User.Password = '123456';

addlistener(td,'OnConnectionStatus',@OnTdConnectionStatus);
addlistener(td,'OnRtnOrder',@OnRtnOrder);

td.Connect();

%% 下单
order1 = BuyLimit('IF1502',1,3000)
disp(order1)

%% 撤单
td.CancelOrder(order1);

%% 退出
% md.Dispose() %行情退出
% td.Dispose() %交易退出