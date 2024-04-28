#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>
#include <deque>
#include <format>
#include <list>

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
    
    OrderPointer ToOrderPointer(Order type)const
    {
        return std::make_shared<Order>(type, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
    } 
    
private:
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity quantity_;
};

int main()
{

}