function OnTdConnectionStatus(sender,arg)

import QuantBox.XAPI.*;

global td;

disp('TD')
% 交易连接回报
disp(arg.status);

end
