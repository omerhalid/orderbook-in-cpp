#ifndef TRADE_H
#define TRADE_H

#include <cstdint>
#include <vector>

using OrderId = std::uint64_t; /**< Type alias for order ID. */
using Quantity = std::uint32_t; /**< Type alias for quantity. */
using Price = std::int32_t; /**< Type alias for price. */

/**
 * @brief Structure representing trade information.
 */
struct TradeInfo
{
    OrderId orderId_; /**< ID of the order involved in the trade. */
    Quantity quantity_; /**< Quantity traded. */
    Price price_; /**< Price at which the trade occurred. */
};

/**
 * @brief Class representing a trade.
 */
class Trade
{
public:
    /**
     * @brief Constructs a Trade.
     * @param bidTrade Information about the bid trade.
     * @param askTrade Information about the ask trade.
     */
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade);
    
    /**
     * @brief Gets the bid trade information.
     * @return Bid trade information.
     */
    const TradeInfo& GetBidTrade() const;

    /**
     * @brief Gets the ask trade information.
     * @return Ask trade information.
     */
    const TradeInfo& GetAskTrade() const;

private:
    TradeInfo bidTrade_; /**< Information about the bid trade. */
    TradeInfo askTrade_; /**< Information about the ask trade. */
};

using Trades = std::vector<Trade>; /**< Type alias for a vector of trades. */

#endif // TRADE_H
