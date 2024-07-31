#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include "Trade.h"
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

/**
 * @brief Structure representing level information in the order book.
 */
struct LevelInfo
{
    Price price_; /**< Price level. */
    Quantity quantity_; /**< Quantity at the price level. */
};

using LevelInfos = std::vector<LevelInfo>; /**< Type alias for a vector of level information. */

/**
 * @brief Class representing order book level information.
 */
class OrderbookLevelInfos
{
public:
    /**
     * @brief Constructs OrderbookLevelInfos.
     * @param bids Bid levels information.
     * @param asks Ask levels information.
     */
    OrderbookLevelInfos(const LevelInfos& bids, const LevelInfos& asks);
    
    /**
     * @brief Gets the bid levels information.
     * @return Bid levels information.
     */
    const LevelInfos& GetBids() const;

    /**
     * @brief Gets the ask levels information.
     * @return Ask levels information.
     */
    const LevelInfos& GetAsks() const;

private:
    LevelInfos bids_; /**< Bid levels information. */
    LevelInfos asks_; /**< Ask levels information. */
};

/**
 * @brief Class representing an order book.
 */
class Orderbook
{
public:
    /**
     * @brief Adds an order to the order book.
     * @param order Order to be added.
     * @return Trades resulting from matching orders.
     */
    Trades AddOrder(OrderPointer order);

    /**
     * @brief Cancels an order in the order book.
     * @param orderId ID of the order to be canceled.
     */
    void CancelOrder(OrderId orderId);

    /**
     * @brief Modifies an existing order in the order book.
     * @param order Modified order.
     * @return Trades resulting from matching orders.
     */
    Trades ModifyOrder(OrderPointer order);

    /**
     * @brief Gets the size of the order book.
     * @return Number of orders in the order book.
     */
    std::size_t Size() const;

    /**
     * @brief Gets the order book level information.
     * @return Order book level information.
     */
    OrderbookLevelInfos GetOrderInfos() const;

private:
    struct OrderEntry
    {
        OrderPointer order_; /**< Pointer to the order. */
        std::list<OrderPointer>::iterator location_; /**< Location of the order in the list. */
    };

    std::map<Price, std::list<OrderPointer>, std::greater<Price>> bids_; /**< Bid orders sorted by price. */
    std::map<Price, std::list<OrderPointer>, std::less<Price>> asks_; /**< Ask orders sorted by price. */
    std::unordered_map<OrderId, OrderEntry> orders_; /**< Map of orders by order ID. */

    /**
     * @brief Checks if an order can be matched.
     * @param side Side of the order (buy or sell).
     * @param price Price of the order.
     * @return True if the order can be matched, otherwise false.
     */
    bool CanMatch(Side side, Price price) const;

    /**
     * @brief Matches orders in the order book.
     * @return Trades resulting from matching orders.
     */
    Trades MatchOrders();
};

#endif // ORDERBOOK_H
