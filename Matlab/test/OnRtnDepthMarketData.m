function OnRtnDepthMarketData(sender,arg)
% 行情回报

% 打印行情
disp(arg.marketData.Symbol)
disp(arg.marketData.LastPrice)

end
