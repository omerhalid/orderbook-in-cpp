#ifndef TRADE_H
#define TRADE_H

#include <cstdint>
#include <vector>

using OrderId = std::uint64_t;
using Quantity = std::uint32_t;
using Price = std::int32_t;

struct TradeInfo
{
    OrderId orderId_;
    Quantity quantity_;
    Price price_;
};

class Trade
{
public:
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade);
    
    const TradeInfo& GetBidTrade() const;
    const TradeInfo& GetAskTrade() const;

private:
    TradeInfo bidTrade_;
    TradeInfo askTrade_;
};

using Trades = std::vector<Trade>;

#endif // TRADE_H
