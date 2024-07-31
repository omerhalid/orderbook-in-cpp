#include "Order.h"

Order::Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
    : orderType_{orderType}, orderId_{orderId}, side_{side}, price_{price}, initialQuantity_{quantity}, remainingQuantity_{quantity} {}

OrderId Order::GetOrderId() const { return orderId_; }
OrderType Order::GetOrderType() const { return orderType_; }
Side Order::GetSide() const { return side_; }
Price Order::GetPrice() const { return price_; }
Quantity Order::GetInitialQuantity() const { return initialQuantity_; }
Quantity Order::GetRemainingQuantity() const { return remainingQuantity_; }
Quantity Order::GetFilledQuantity() const { return initialQuantity_ - remainingQuantity_; }
bool Order::IsFilled() const { return GetRemainingQuantity() == 0; }

void Order::Fill(Quantity quantity)
{
    if (quantity > GetRemainingQuantity())
    {
        throw std::logic_error(std::format("Order ({}) cannot be filled for more than remaining quantity", GetOrderId()));
    }
    remainingQuantity_ -= quantity;
}
