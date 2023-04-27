#include <gtest/gtest.h>
extern "C" {
    #include "ipv6_cidr.h"
}

TEST(ipv6_cidr_table, Insert) {
    ipv6_cidr_table_t tab;
    ipv6_cidr_table_init(&tab);
    ASSERT_TRUE(ipv6_cidr_table_is_empty(&tab));

    // insert invalid cidr string
    ASSERT_EQ(-1, ipv6_cidr_put_to_table(&tab, "abcdef"));
    ASSERT_TRUE(ipv6_cidr_table_is_empty(&tab));

    ASSERT_EQ(-1, ipv6_cidr_put_to_table(&tab, "10:aabb::10/129"));
    ASSERT_TRUE(ipv6_cidr_table_is_empty(&tab));

    ASSERT_EQ(-1, ipv6_cidr_put_to_table(&tab, "10:aabb::10/-1"));
    ASSERT_TRUE(ipv6_cidr_table_is_empty(&tab));

    ASSERT_EQ(-1, ipv6_cidr_put_to_table(&tab, "10:aabbff::10/1"));
    ASSERT_TRUE(ipv6_cidr_table_is_empty(&tab));

    // insert normal
    ASSERT_EQ(0, ipv6_cidr_put_to_table(&tab, "10:10::8888/16"));
    ASSERT_FALSE(ipv6_cidr_table_is_empty(&tab));
    ASSERT_TRUE(ipv6_in_cidr_table(&tab, "10:10::8888"));
    ASSERT_TRUE(ipv6_in_cidr_table(&tab, "10:10::"));
    ASSERT_TRUE(ipv6_in_cidr_table(&tab, "10:11::"));
    ASSERT_FALSE(ipv6_in_cidr_table(&tab, "11::"));
    ASSERT_FALSE(ipv6_in_cidr_table(&tab, "1::8888"));

    ASSERT_EQ(0, ipv6_cidr_put_to_table(&tab, "f:8888::f/20"));
    ASSERT_TRUE(ipv6_in_cidr_table(&tab, "10:10::"));
    ASSERT_TRUE(ipv6_in_cidr_table(&tab, "f:8888::"));
    ASSERT_TRUE(ipv6_in_cidr_table(&tab, "f:8000:111:11::"));
    ASSERT_FALSE(ipv6_in_cidr_table(&tab, "1::"));
    ASSERT_FALSE(ipv6_in_cidr_table(&tab, "f:1:11::"));
    ASSERT_FALSE(ipv6_in_cidr_table(&tab, "f:1888::"));

    ASSERT_EQ(0, ipv6_cidr_put_to_table(&tab, "aa::ff/128"));
    ASSERT_TRUE(ipv6_in_cidr_table(&tab, "aa::ff"));
    ASSERT_FALSE(ipv6_in_cidr_table(&tab, "aa::fe"));

    ASSERT_EQ(0, ipv6_cidr_put_to_table(&tab, "aa::ff/120"));
    ASSERT_TRUE(ipv6_in_cidr_table(&tab, "aa::ff"));
    ASSERT_TRUE(ipv6_in_cidr_table(&tab, "aa::fe"));

    // insert single ip without mask
    ASSERT_EQ(0, ipv6_cidr_put_to_table(&tab, "aa:bbbb:10::10"));
    ASSERT_TRUE(ipv6_in_cidr_table(&tab, "aa:bbbb:10::10"));
    ASSERT_FALSE(ipv6_in_cidr_table(&tab, "aa:bbbb:10::11"));

    ipv6_cidr_table_clear(&tab);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}