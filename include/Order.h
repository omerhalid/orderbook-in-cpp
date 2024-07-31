#ifndef ORDER_H
#define ORDER_H

#include <cstdint>
#include <stdexcept>
#include <format>
#include <memory>

/**
 * @brief Enum class for order types.
 */
enum class OrderType
{
    GoodTillCancel, /**< Order remains active until it is canceled. */
    FillAndKill /**< Order is either fully filled immediately or canceled. */
};

/**
 * @brief Enum class for order sides.
 */
enum class Side
{
    Buy, /**< Buy side of the order book. */
    Sell /**< Sell side of the order book. */
};

using Price = std::int32_t; /**< Type alias for price. */
using Quantity = std::uint32_t; /**< Type alias for quantity. */
using OrderId = std::uint64_t; /**< Type alias for order ID. */

/**
 * @brief Class representing an order.
 */
class Order
{
public:
    /**
     * @brief Constructs an Order.
     * @param orderType Type of the order.
     * @param orderId ID of the order.
     * @param side Side of the order (buy or sell).
     * @param price Price of the order.
     * @param quantity Quantity of the order.
     */
    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity);
    
    /**
     * @brief Gets the order ID.
     * @return Order ID.
     */
    OrderId GetOrderId() const;

    /**
     * @brief Gets the order type.
     * @return Order type.
     */
    OrderType GetOrderType() const;

    /**
     * @brief Gets the side of the order.
     * @return Side of the order.
     */
    Side GetSide() const;

    /**
     * @brief Gets the price of the order.
     * @return Price of the order.
     */
    Price GetPrice() const;

    /**
     * @brief Gets the initial quantity of the order.
     * @return Initial quantity of the order.
     */
    Quantity GetInitialQuantity() const;

    /**
     * @brief Gets the remaining quantity of the order.
     * @return Remaining quantity of the order.
     */
    Quantity GetRemainingQuantity() const;

    /**
     * @brief Gets the filled quantity of the order.
     * @return Filled quantity of the order.
     */
    Quantity GetFilledQuantity() const;

    /**
     * @brief Checks if the order is completely filled.
     * @return True if the order is completely filled, otherwise false.
     */
    bool IsFilled() const;

    /**
     * @brief Fills the order with the specified quantity.
     * @param quantity The quantity to fill the order with.
     * @throws std::logic_error If the quantity is greater than the remaining quantity of the order.
     */
    void Fill(Quantity quantity);

private:
    OrderType orderType_; /**< The type of the order. */
    OrderId orderId_; /**< The ID of the order. */
    Side side_; /**< The side of the order (buy or sell). */
    Price price_; /**< The price of the order. */
    Quantity initialQuantity_; /**< The initial quantity of the order. */
    Quantity remainingQuantity_; /**< The remaining quantity of the order. */
};

using OrderPointer = std::shared_ptr<Order>; /**< Type alias for a shared pointer to an Order. */

#endif // ORDER_H
