#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include "Trade.h"
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

struct LevelInfo
{
    Price price_;
    Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderbookLevelInfos
{
public:
    OrderbookLevelInfos(const LevelInfos& bids, const LevelInfos& asks);
    
    const LevelInfos& GetBids() const;
    const LevelInfos& GetAsks() const;

private:
    LevelInfos bids_;
    LevelInfos asks_;
};

class Orderbook
{
public:
    Trades AddOrder(OrderPointer order);
    void CancelOrder(OrderId orderId);
    Trades ModifyOrder(OrderPointer order);
    std::size_t Size() const;
    OrderbookLevelInfos GetOrderInfos() const;

private:
    struct OrderEntry
    {
        OrderPointer order_;
        std::list<OrderPointer>::iterator location_;
    };

    std::map<Price, std::list<OrderPointer>, std::greater<Price>> bids_;
    std::map<Price, std::list<OrderPointer>, std::less<Price>> asks_;
    std::unordered_map<OrderId, OrderEntry> orders_;

    bool CanMatch(Side side, Price price) const;
    Trades MatchOrders();
};

#endif // ORDERBOOK_H
