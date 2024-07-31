#include <gtest/gtest.h>
#include "Order.h"
#include "Orderbook.h"

class OrderbookTest : public ::testing::Test {
protected:
    Orderbook orderbook;

    void SetUp() override {
        
    }

    void TearDown() override {
        
    }
};

TEST_F(OrderbookTest, AddOrder) {
    OrderPointer order = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 100, 10);
    Trades trades = orderbook.AddOrder(order);
    EXPECT_EQ(orderbook.Size(), 1);
    EXPECT_TRUE(trades.empty());
}

TEST_F(OrderbookTest, CancelOrder) {
    OrderPointer order = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 100, 10);
    orderbook.AddOrder(order);
    EXPECT_EQ(orderbook.Size(), 1);
    orderbook.CancelOrder(order->GetOrderId());
    EXPECT_EQ(orderbook.Size(), 0);
}

TEST_F(OrderbookTest, MatchOrders) {
    OrderPointer buyOrder = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 100, 10);
    OrderPointer sellOrder = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Sell, 100, 10);
    orderbook.AddOrder(buyOrder);
    Trades trades = orderbook.AddOrder(sellOrder);
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].GetBidTrade().quantity_, 10);
    EXPECT_EQ(trades[0].GetAskTrade().quantity_, 10);
    EXPECT_EQ(orderbook.Size(), 0);
}

TEST_F(OrderbookTest, FillAndKillOrder) {
    OrderPointer buyOrder = std::make_shared<Order>(OrderType::FillAndKill, 1, Side::Buy, 100, 10);
    Trades trades = orderbook.AddOrder(buyOrder);
    EXPECT_EQ(orderbook.Size(), 0);
    EXPECT_TRUE(trades.empty());
}

TEST_F(OrderbookTest, GetOrderInfos) {
    OrderPointer buyOrder = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 100, 10);
    orderbook.AddOrder(buyOrder);
    OrderbookLevelInfos infos = orderbook.GetOrderInfos();
    EXPECT_EQ(infos.GetBids().size(), 1);
    EXPECT_EQ(infos.GetAsks().size(), 0);
    EXPECT_EQ(infos.GetBids()[0].price_, 100);
    EXPECT_EQ(infos.GetBids()[0].quantity_, 10);
}
