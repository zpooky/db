//
// Created by spooky on 2016-09-25.
//

#include "gtest/gtest.h"
#include "../Base.h"

TEST(Base, hex) {
    constexpr sp::Hex hex = sp::hex();
    std::string x{"1234567890qwertyuiopasdfghjklzxcvbnm"};
    ASSERT_EQ("3132333435363738393071776572747975696F706173646667686A6B6C7A786376626E6D", hex.encode(x));
}

TEST(Base, base32) {
    constexpr sp::Base32 base = sp::base32();
    std::string x{"1234567890qwertyuiopasdfghjklzxcvbnm"};
    ASSERT_EQ("GEZDGNBVGY3TQOJQOF3WK4TUPF2WS33QMFZWIZTHNBVGW3D2PBRXMYTONU======", base.encode(x));
}

TEST(Base, base64) {
    constexpr sp::Base64 base = sp::base64();
    std::string x{"1234567890qwertyuiopasdfghjklzxcvbnm"};
    ASSERT_EQ("MTIzNDU2Nzg5MHF3ZXJ0eXVpb3Bhc2RmZ2hqa2x6eGN2Ym5t", base.encode(x));
}

TEST(Base, base64_2) {
    constexpr sp::Base64 base = sp::base64();
    ASSERT_EQ("YW55IGNhcm5hbCBwbGVhc3VyZS4=", base.encode("any carnal pleasure."));
    ASSERT_EQ("YW55IGNhcm5hbCBwbGVhc3VyZQ==", base.encode("any carnal pleasure"));
    ASSERT_EQ("YW55IGNhcm5hbCBwbGVhc3Vy", base.encode("any carnal pleasur"));
    ASSERT_EQ("YW55IGNhcm5hbCBwbGVhc3U=", base.encode("any carnal pleasu"));
    ASSERT_EQ("YW55IGNhcm5hbCBwbGVhcw==", base.encode("any carnal pleas"));
}

TEST(Base, tes){
    //16
    uint64_t c(~uint64_t(0));
}