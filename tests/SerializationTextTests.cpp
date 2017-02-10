//
// Created by fraillt on 17.2.7.
//

#include <gmock/gmock.h>
#include "SerializationTestUtils.h"
using namespace testing;

TEST(SerializerText, BasicString) {
    SerializationContext ctx;
    std::string t1 = "some random text";
    std::string res;

    ctx.createSerializer().text(t1);
    ctx.createDeserializer().text(res);

    EXPECT_THAT(res, StrEq(t1));
    EXPECT_THAT(res, ContainerEq(t1));

}

TEST(SerializerText, WhenSizeOfTypeNotEqualsOneThenSetSizeExplicitly) {
    SerializationContext ctx;
    constexpr auto VSIZE = sizeof(char32_t);
    std::basic_string<char32_t> t1 = U"some random text";
    std::basic_string<char32_t> res;
    static_assert(VSIZE > 1, "on this system, all character types has sizeof == 1, cannot run this tests");

    ctx.createSerializer().text<VSIZE>(t1);
    ctx.createDeserializer().text<VSIZE>(res);

    EXPECT_THAT(res, ContainerEq(t1));
}

TEST(SerializerText, BasicStringUseSizeMethodNotNullterminatedLength) {
    SerializationContext ctx;
    std::wstring t1(L"some random text\0xxxxxx", 20);
    std::wstring wres;
    constexpr auto VSIZE = sizeof(std::wstring::value_type);

    ctx.createSerializer().text<VSIZE>(t1);
    ctx.createDeserializer().text<VSIZE>(wres);

    EXPECT_THAT(wres, StrEq(t1));
    EXPECT_THAT(wres.size(), Eq(t1.size()));
    EXPECT_THAT(wres.size(), Gt(std::char_traits<std::wstring::value_type>::length(t1.data())));

    SerializationContext ctx2;
    std::string t2("\0no one cares what is there", 10);
    std::string res;
    ctx2.createSerializer().text(t2);
    ctx2.createDeserializer().text(res);

    EXPECT_THAT(res, StrEq(t2));
    EXPECT_THAT(res.size(), Eq(t2.size()));

    SerializationContext ctx3;
    std::string t3("never ending buffer that doesnt fit in this string", 10);
    ctx3.createSerializer().text(t3);
    ctx3.createDeserializer().text(res);
    EXPECT_THAT(res, StrEq(t3));
    EXPECT_THAT(res.size(), Eq(10));
}

const int CARR_LENGTH = 10;

TEST(SerializerText, CArraySerializesTextLength) {
    SerializationContext ctx;
    char t1[CARR_LENGTH]{"some text"};
    char r1[CARR_LENGTH]{};

    ctx.createSerializer().text(t1);
    ctx.createDeserializer().text(r1);

    EXPECT_THAT(ctx.getBufferSize(), Eq(ctx.containerSizeSerializedBytesCount(CARR_LENGTH) +
                                                std::char_traits<char>::length(t1)));

    EXPECT_THAT(r1, StrEq(t1));
    EXPECT_THAT(r1, ContainerEq(t1));

    //zero length string
    t1[0] = 0;
    SerializationContext ctx2;
    ctx2.createSerializer().text(t1);
    ctx2.createDeserializer().text(r1);

    EXPECT_THAT(ctx2.getBufferSize(), Eq(ctx2.containerSizeSerializedBytesCount(CARR_LENGTH)));
    EXPECT_THAT(r1, StrEq(t1));
    EXPECT_THAT(r1, ContainerEq(t1));
}

TEST(SerializerText, WhenCArrayWithLargerTypeThenSetSizeExplicitly) {
    SerializationContext ctx;
    char32_t t1[10]{U"some text"};
    char32_t r1[10]{};
    constexpr auto SIZE = sizeof(char32_t);
    ctx.createSerializer().text<SIZE>(t1);
    ctx.createDeserializer().text<SIZE>(r1);

    EXPECT_THAT(ctx.getBufferSize(), Eq(ctx.containerSizeSerializedBytesCount(CARR_LENGTH) +
                                                std::char_traits<char32_t>::length(t1) * SIZE));
    EXPECT_THAT(r1, ContainerEq(t1));
}


TEST(SerializerText, WhenCArrayNotNullterminatedThenMakeItNullterminated) {
    SerializationContext ctx;
    char t1[CARR_LENGTH]{"some text"};
    //make last character not nullterminated
    t1[CARR_LENGTH-1] = 'x';
    char r1[CARR_LENGTH]{};

    ctx.createSerializer().text(t1);
    ctx.createDeserializer().text(r1);

    EXPECT_THAT(ctx.getBufferSize(), Eq(ctx.containerSizeSerializedBytesCount(CARR_LENGTH) +
                                                CARR_LENGTH - 1));
    EXPECT_THAT(r1[CARR_LENGTH-1], Eq(0));
}