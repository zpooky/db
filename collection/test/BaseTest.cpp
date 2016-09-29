//
// Created by spooky on 2016-09-25.
//

#include "gtest/gtest.h"
#include "../Base.h"
#include<iostream>

using std::cout;
using std::string;

TEST(Base, hex) {
    constexpr sp::Hex hex = sp::hex();
    std::string x{"1234567890qwertyuiopasdfghjklzxcvbnm"};
    auto encoded = hex.encode(x);
    ASSERT_EQ("3132333435363738393071776572747975696F706173646667686A6B6C7A786376626E6D", encoded);
    ASSERT_EQ(x, hex.decode<string>(encoded));
}

TEST(Base, base32) {
    constexpr sp::Base32 base = sp::base32();
    {
        std::string x{"1234567890qwertyuiopasdfghjklzxcvbnm"};
        auto encoded = base.encode(x);
        ASSERT_EQ("GEZDGNBVGY3TQOJQOF3WK4TUPF2WS33QMFZWIZTHNBVGW3D2PBRXMYTONU======", encoded);
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
    {
        std::string x{""};
        auto encoded = base.encode(x);
        ASSERT_EQ("", encoded);
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
    {
        std::string x{"f"};
        auto encoded = base.encode(x);
        ASSERT_EQ("MY======", encoded);
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
    {
        std::string x{"fo"};
        auto encoded = base.encode(x);
        ASSERT_EQ("MZXQ====", encoded);
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
    {
        std::string x{"foo"};
        auto encoded = base.encode(x);
        ASSERT_EQ("MZXW6===", encoded);
        ASSERT_EQ(x, base.decode<string>(encoded));
    }

    {
        std::string x{"foob"};
        auto encoded = base.encode(x);
        ASSERT_EQ("MZXW6YQ=", encoded);
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
    {
        std::string x{"fooba"};
        auto encoded = base.encode(x);
        ASSERT_EQ("MZXW6YTB", encoded);
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
    {
        std::string x{"foobar"};
        auto encoded = base.encode(x);
        ASSERT_EQ("MZXW6YTBOI======", encoded);
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
}

TEST(Base, base64) {
    constexpr sp::Base64 base = sp::base64();
    std::string x{"1234567890qwertyuiopasdfghjklzxcvbnm"};
    auto encoded = base.encode(x);
    ASSERT_EQ("MTIzNDU2Nzg5MHF3ZXJ0eXVpb3Bhc2RmZ2hqa2x6eGN2Ym5t", encoded);
    ASSERT_EQ(x, base.decode<string>(encoded));
}

TEST(Base, base64_2) {
    constexpr sp::Base64 base = sp::base64();
    {
        std::string x("any carnal pleasure.");
        auto encoded = base.encode(x);
        ASSERT_EQ("YW55IGNhcm5hbCBwbGVhc3VyZS4=", encoded);
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
    {
        std::string x("any carnal pleasure");
        auto encoded = base.encode(x);
        ASSERT_EQ("YW55IGNhcm5hbCBwbGVhc3VyZQ==", base.encode(x));
        ASSERT_EQ(x, base.decode<string>(encoded));
    }

    {
        std::string x("any carnal pleasur");
        auto encoded = base.encode(x);
        ASSERT_EQ("YW55IGNhcm5hbCBwbGVhc3Vy", base.encode(x));
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
    {
        std::string x("any carnal pleasu");
        auto encoded = base.encode(x);
        ASSERT_EQ("YW55IGNhcm5hbCBwbGVhc3U=", base.encode(x));
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
    {
        std::string x("any carnal pleas");
        auto encoded = base.encode(x);
        ASSERT_EQ("YW55IGNhcm5hbCBwbGVhcw==", base.encode(x));
        ASSERT_EQ(x, base.decode<string>(encoded));
    }
}

//TEST(Base, tes) {
//    //16
//    uint64_t c(~uint64_t(0));
//    cout << size_t(101 & 101) << "\n";
//}