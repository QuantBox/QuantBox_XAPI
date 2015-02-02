function OnMdConnectionStatus(sender,arg)

import QuantBox.XAPI.*;

global md;

disp('MD')
% 交易连接回报
disp(arg.status);

switch arg.status
    case QuantBox.ConnectionStatus.Disconnected
        % 打印错误信息
        disp(Extensions_GBK.ErrorMsg(arg.userLogin));
    case QuantBox.ConnectionStatus.Done
        % 订阅行情，支持","和";"分隔
        md.Subscribe('IF1502;IF1503,IF1506','');
end

end
