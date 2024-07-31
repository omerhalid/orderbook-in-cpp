#include "Orderbook.h"
#include <numeric>

OrderbookLevelInfos::OrderbookLevelInfos(const LevelInfos& bids, const LevelInfos& asks)
    : bids_{bids}, asks_{asks} {}

const LevelInfos& OrderbookLevelInfos::GetBids() const { return bids_; }
const LevelInfos& OrderbookLevelInfos::GetAsks() const { return asks_; }

bool Orderbook::CanMatch(Side side, Price price) const
{
    if (side == Side::Buy)
    {
        if (asks_.empty())
        {
            return false;
        }

        const auto& [bestAsk, _] = *asks_.begin();

        return price >= bestAsk;
    }
    else
    {
        if (bids_.empty())
        {
            return false;
        }

        const auto& [bestBid, _] = *bids_.begin();

        return price <= bestBid;
    }
}

Trades Orderbook::MatchOrders()
{
    Trades trades;
    trades.reserve(orders_.size());

    while (true)
    {
        if (bids_.empty() || asks_.empty())
        {
            break;
        }
        auto& [bidPrice, bids] = *bids_.begin();
        auto& [askPrice, asks] = *asks_.begin();

        if (bidPrice < askPrice)
        {
            break;
        }

        while (bids.size() && asks.size())
        {
            auto& bidOrder = bids.front();
            auto& askOrder = asks.front();

            auto tradeQuantity = std::min(bidOrder->GetRemainingQuantity(), askOrder->GetRemainingQuantity());
            bidOrder->Fill(tradeQuantity);
            askOrder->Fill(tradeQuantity);

            if (bidOrder->IsFilled())
            {
                bids.pop_front();
                orders_.erase(bidOrder->GetOrderId());
            }
            if (askOrder->IsFilled())
            {
                asks.pop_front();
                orders_.erase(askOrder->GetOrderId());
            }

            if (bids.empty()) bids_.erase(bidPrice);
            if (asks.empty()) asks_.erase(askPrice);

            trades.push_back(Trade{
                TradeInfo{bidOrder->GetOrderId(), tradeQuantity, bidOrder->GetPrice()},
                TradeInfo{askOrder->GetOrderId(), tradeQuantity, askOrder->GetPrice()}});
        }
    }

    if (!bids_.empty())
    {
        auto& [_, bids] = *bids_.begin();
        auto& order = bids.front();
        if (order->GetOrderType() == OrderType::FillAndKill)
        {
            CancelOrder(order->GetOrderId());
        }
    }
    if (!asks_.empty())
    {
        auto& [_, asks] = *asks_.begin();
        auto& order = asks.front();
        if (order->GetOrderType() == OrderType::FillAndKill)
        {
            CancelOrder(order->GetOrderId());
        }
    }
    return trades;
}

Trades Orderbook::AddOrder(OrderPointer order)
{
    if (orders_.contains(order->GetOrderId()))
    {
        return {};
    }
    if (order->GetOrderType() == OrderType::FillAndKill && !CanMatch(order->GetSide(), order->GetPrice()))
    {
        return {};
    }
    std::list<OrderPointer>::iterator iterator;

    if (order->GetSide() == Side::Buy)
    {
        auto& orders = bids_[order->GetPrice()];
        orders.push_back(order);
        iterator = std::next(orders.begin(), orders.size() - 1);
    }
    else
    {
        auto& orders = asks_[order->GetPrice()];
        orders.push_back(order);
        iterator = std::next(orders.begin(), orders.size() - 1);
    }

    orders_.insert({order->GetOrderId(), OrderEntry{order, iterator}});

    return MatchOrders();
}

void Orderbook::CancelOrder(OrderId orderId)
{
    if (!orders_.contains(orderId))
    {
        return;
    }
    const auto& [order, orderIterator] = orders_.at(orderId);
    orders_.erase(orderId);

    if (order->GetSide() == Side::Sell)
    {
        auto price = order->GetPrice();
        auto& orders = asks_[price];
        orders.erase(orderIterator);
        if (orders.empty())
        {
            asks_.erase(price);
        }
    }
    else
    {
        auto price = order->GetPrice();
        auto& orders = bids_[price];
        orders.erase(orderIterator);
        if (orders.empty())
        {
            bids_.erase(price);
        }
    }
}

Trades Orderbook::ModifyOrder(OrderPointer order)
{
    if (!orders_.contains(order->GetOrderId()))
    {
        return {};
    }
    auto& [existingOrder, _] = orders_.at(order->GetOrderId());
    CancelOrder(order->GetOrderId());
    return AddOrder(order);
}

std::size_t Orderbook::Size() const { return orders_.size(); }

OrderbookLevelInfos Orderbook::GetOrderInfos() const
{
    LevelInfos bidInfos, askInfos;
    bidInfos.reserve(bids_.size());
    askInfos.reserve(asks_.size());

    auto CreateLevelInfos = [](Price price, const std::list<OrderPointer>& orders)
    {
        return LevelInfo{price, std::accumulate(orders.begin(), orders.end(), Quantity{0}, [](Quantity runningSum, const OrderPointer& order)
        {
            return runningSum + order->GetRemainingQuantity();
        })};
    };

    for (const auto& [price, orders] : bids_)
    {
        bidInfos.push_back(CreateLevelInfos(price, orders));
    }
    for (const auto& [price, orders] : asks_)
    {
        askInfos.push_back(CreateLevelInfos(price, orders));
    }

    return OrderbookLevelInfos{bidInfos, askInfos};
}
