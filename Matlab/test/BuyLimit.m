function OrderRef = BuyLimit(Instrument,Qty,Price)

import QuantBox.XAPI.*;

global td;

% 下单
order = QuantBox.XAPI.OrderField;
order.InstrumentID = Instrument;
order.Type = QuantBox.OrderType.Limit;
order.Side = QuantBox.OrderSide.Buy;
order.Qty = Qty;
order.Price = Price;
order.OpenClose = QuantBox.OpenCloseType.Open;
order.HedgeFlag = QuantBox.HedgeFlagType.Speculation;

OrderRef = td.SendOrder(...
    -1,... %强行指定报单引用,-1表示由底层生成
    order);    

end
