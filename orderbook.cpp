#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>
#include <deque>
#include <format>
#include <list>
#include <map>
#include <unordered_map>
#include <numeric>

enum class OrderType
{
    GoodTollCancel,
    FillandKill
};

enum class Side
{
    Buy,
    Sell
};

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

struct LevelInfo
{
    Price price_;
    Quantity quantity_;
};

using LevelInfo = std::vector<LevelInfo>;

class OrderbookLevelInfos
{
public:
    OrderbookLevelInfos(const LevelInfo& bids, const LevelInfo& asks)
        : bids_{bids}, asks_{asks} {}
    
    const LevelInfo& GetBids() const { return bids_; }
    const LevelInfo& GetAsks() const { return asks_; }
private:
    LevelInfo bids_;
    LevelInfo asks_;
};

class Order
{
public:
    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
        : orderType_{orderType}, 
        orderId_{orderId}, 
        side_{side}, 
        price_{price}, 
        initialQuantity_{quantity}, 
        remaningQuantity_{quantity} 
        {}
    
    OrderId GetOrderId() const { return orderId_; }
    OrderType GetOrderType() const { return orderType_; }
    Side GetSide() const { return side_; }
    Price GetPrice() const { return price_; }
    Quantity GetInitialQuantity() const { return initialQuantity_; } // we might not need this
    Quantity GetRemainingQuantity() const { return remaningQuantity_; } // we care about remaining quantity
    Quantity GetFilledQuantity() const { return initialQuantity_ - remaningQuantity_; }
    bool IsFilled() const { return GetRemainingQuantity() == 0; }
    /**
     * Fills the order with the specified quantity.
     * 
     * @param quantity The quantity to fill the order with.
     * @throws std::logic_error If the quantity is greater than the remaining quantity of the order.
     */
    void Fill(Quantity quantity)
    {
        if (quantity > GetRemainingQuantity())
        {
            throw std::logic_error(std::format("Order ({}) cannot be filled for more than remaining quantity", GetOrderId()));
        }
        remaningQuantity_ -= quantity;
    }
private:
    OrderType orderType_;
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity initialQuantity_;
    Quantity remaningQuantity_;

};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

class OrderModify
{
public:
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity)
        : orderId_{orderId}, 
        side_{side},
        price_{price}, 
        quantity_{quantity} 
        {}

    OrderId GetOrderId() const { return orderId_; }
    Side GetSide() const { return side_; }
    Price GetPrice() const { return price_; }
    Quantity GetQuantity() const { return quantity_; }
    
    OrderPointer ToOrderPointer(OrderType type) const
    {
        return std::make_shared<Order>(type, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
    } 

private:
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity quantity_;
};

struct TradeInfo
{
    OrderId orderId_;
    Quantity quantity_;
    Price price_;
};

class Trade
{
public:
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade)
        : bidTrade_{bidTrade}, 
        askTrade_{askTrade} 
        {}
    
    const TradeInfo& GetBidTrade() const { return bidTrade_; }
    const TradeInfo& GetAskTrade() const { return askTrade_; }

private:
    TradeInfo bidTrade_;
    TradeInfo askTrade_;

};

using Trades = std::vector<Trade>;

class Orderbook
{
private:

    struct OrrderEntry
    {
        OrderPointer order_ {nullptr};
        OrderPointers::iterator location_;
    };

    std::map<Price, OrderPointers, std::greater<Price>> bids_;
    std::map<Price, OrderPointers, std::less<Price>> asks_;
    std::unordered_map<OrderId, OrrderEntry> orders_;

    bool CanMatch(Side side, Price price) const
    {
        if (side == Side::Buy)
        {
            if(asks_.empty())
            {
                return false;
            }

            const auto& [bestAsk, _] = *asks_.begin();

            return price >= bestAsk;
        }
        else
        {
            if(bids_.empty())
            {
                return false;
            }

            const auto& [bestBid, _] = *bids_.begin();

            return price <= bestBid;
        }
    }

    Trades MatchOrders()
    {
        Trades trades;
        trades.reserve(orders_.size());

        while (true)
        {
            if(bids_.empty() || asks_.empty())
            {
                break;
            }
            auto& [bidPrice, bids] = *bids_.begin();
            auto& [askPrice, asks] = *asks_.begin();

            if(bidPrice < askPrice)
            {
                break;
            }

            while(bids.size() && asks.size())
            {
                auto& bidOrder = bids.front();
                auto& askOrder = asks.front();

                auto tradeQuantity = std::min(bidOrder->GetRemainingQuantity(), askOrder->GetRemainingQuantity());
                bidOrder->Fill(tradeQuantity);
                askOrder->Fill(tradeQuantity);

                if(bidOrder->IsFilled())
                {
                    bids.pop_front();
                    orders_.erase(bidOrder->GetOrderId());
                }
                if(askOrder->IsFilled())
                {
                    asks.pop_front();
                    orders_.erase(askOrder->GetOrderId());
                }

                if(bids.empty()) bids_.erase(bidPrice);
                if(asks.empty()) asks_.erase(askPrice); 

                trades.push_back(Trade{ 
                    TradeInfo{bidOrder->GetOrderId(), tradeQuantity, bidOrder->GetPrice()},
                    TradeInfo{askOrder->GetOrderId(), tradeQuantity, askOrder->GetPrice()}});
            }
        }

        if(!bids_.empty())
        {
            auto& [_, bids] = *bids_.begin();
            auto& order = bids.front();
            if(order->GetOrderType() == OrderType::FillandKill)
            {
                CancelOrder(order->GetOrderId());
            }
        }
        if(!asks_.empty())
        {
            auto& [_, asks] = *asks_.begin();
            auto& order = asks.front();
            if(order->GetOrderType() == OrderType::FillandKill)
            {
                CancelOrder(order->GetOrderId());
            }
        }
        return trades;
    }

public:

    Trades AddOrder(OrderPointer order)
    {
        if(orders_.contains(order->GetOrderId()))
        {
            return {};
        }
        if(order->GetOrderType() == OrderType::FillandKill && !CanMatch(order->GetSide(), order->GetPrice()))
        {
            return {};
        }
        OrderPointers::iterator iterator;

        if(order->GetSide() == Side::Buy)
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

        orders_.insert({order->GetOrderId(), OrrderEntry{order, iterator}});

        return MatchOrders();
    }

    void CancelOrder(OrderId orderId)
    {
        if(!orders_.contains(orderId))
        {
            return;
        }
        const auto& [order, orderIterator] = orders_.at(orderId);
        orders_.erase(orderId);

        if(order->GetSide() == Side::Sell)
        {
            auto price = order->GetPrice();
            auto& orders = asks_[price];
            orders.erase(orderIterator);
            if(orders.empty())
            {
                asks_.erase(price);
            }
        }
        else
        {
            auto price = order->GetPrice();
            auto& orders = bids_[price];
            orders.erase(orderIterator);
            if(orders.empty())
            {
                bids_.erase(price);
            }
        }
    }

    Trades MatchOrder(OrderModify order)
    {
        if(!orders_.contains(order.GetOrderId()))
        {
            return {};
        }
        auto& [existingOrder, _] = orders_.at(order.GetOrderId());
        CancelOrder(order.GetOrderId());
        return AddOrder(order.ToOrderPointer(existingOrder->GetOrderType()));
    }

    std::size_t Size() const { return orders_.size(); }

    OrderbookLevelInfos GetorderInfos() const
    {
        LevelInfo bidInfos, askInfos;
        bidInfos.reserve(bids_.size());
        askInfos.reserve(asks_.size());

        auto CreateLevelInfos = [](Price price, const OrderPointers& orders)
        {
            return LevelInfo{price, std::accumulate(orders.begin(), orders.end(), Quantity{0}, [](Quantity runningSum, const OrderPointer& order) // bug here
            {
                return runningSum + order->GetRemainingQuantity();
            })};
        };

        for(const auto& [price, orders] : bids_)
        {
            bidInfos.push_back(CreateLevelInfos(price, orders));
        }
        for(const auto& [price, orders] : asks_)
        {
            askInfos.push_back(CreateLevelInfos(price, orders));
        }

        return OrderbookLevelInfos{bidInfos, askInfos};

};

int main()
{
    Orderbook orderbook;
    const OrderId orderId = 1;
    orderbook.AddOrder(std::make_shared<Order>(OrderType::GoodTollCancel, orderId, Side::Buy, 100, 10));
    std::cout << orderbook.Size() << std::endl; // 1
    orderbook.CancelOrder(orderId);
    std::cout << orderbook.Size() << std::endl; // 0
    return 0;
}
