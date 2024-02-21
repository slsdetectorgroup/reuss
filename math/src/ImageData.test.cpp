#include <catch2/catch_test_macros.hpp>
#include "reuss/ImageData.h"
#include <array>

using reuss::ImageData;
using reuss::DataSpan;
using reuss::Shape;

TEST_CASE("Initial size is zero if no size is specified")
{
    ImageData<double> a;
    REQUIRE(a.size() == 0);
}


TEST_CASE("Construct from a DataSpan"){
    std::vector<int> some_data(9,42);
    DataSpan<int, 2> span(some_data.data(), Shape<2>{3,3});

    ImageData<int,2> image(span);

    REQUIRE(image.shape() == span.shape());
    REQUIRE(image.size() == span.size());
    REQUIRE(image.data() != span.data());

    for (int i = 0; i<image.size(); ++i){
        REQUIRE(image(i) == span(i));
    }

    image = 43;
    for (int i = 0; i<image.size(); ++i){
        REQUIRE(image(i) != span(i));
    }


}

TEST_CASE("1D image"){
    std::array<ssize_t, 1>shape{{20}};
    ImageData<short,1>img(shape,3);
    REQUIRE(img.size()==20);
    REQUIRE(img(5)==3);

}

TEST_CASE("Accessing a const object"){
    const ImageData<double, 3> img({3,4,5}, 0);
    REQUIRE(img(1,1,1)==0);
    REQUIRE(img.size() == 3*4*5);
    REQUIRE(img.shape() == Shape<3>{3,4,5});
    REQUIRE(img.shape(0) == 3);
    REQUIRE(img.shape(1) == 4);
    REQUIRE(img.shape(2) == 5);

}

TEST_CASE("Indexing of a 2D image")
{
    std::array<ssize_t, 2>shape{{ 3, 7 }};
    ImageData<long> img( shape, 5 );
    for (int i = 0; i != img.size(); ++i) {
        REQUIRE(img(i) == 5);
    }

    for (int i = 0; i != img.size(); ++i) {
        img(i) = i;
    }
    REQUIRE(img(0, 0) == 0);
    REQUIRE(img(0, 1) == 1);
    REQUIRE(img(1, 0) == 7);
}

TEST_CASE("Indexing of a 3D image")
{
    ImageData<float, 3> img{ {{ 3, 4, 2 }}, 5.0f };
    for (int i = 0; i != img.size(); ++i) {
        REQUIRE(img(i) == 5.0f);
    }

    //Double check general properties
    REQUIRE(img.size() == 3 * 4 * 2);

    for (int i = 0; i != img.size(); ++i) {
        img(i) = float(i);
    }
    REQUIRE(img(0, 0, 0) == 0);
    REQUIRE(img(0, 0, 1) == 1);
    REQUIRE(img(0, 1, 1) == 3);
    REQUIRE(img(1, 2, 0) == 12);
    REQUIRE(img(2, 3, 1) == 23);
}

TEST_CASE("Divide double by int"){
    ImageData<double,1> a{{5}, 5};
    ImageData<int,1> b{{5},5};
    a /=b;
    for (auto it : a){
        REQUIRE(it == 1.0);
    }

}

TEST_CASE("Elementwise multiplication of 3D image")
{
    std::array<ssize_t, 3>shape{3, 4, 2};
    ImageData<double, 3> a{ shape };
    ImageData<double, 3> b{ shape };
    for (int i = 0; i != a.size(); ++i) {
        a(i) = i;
        b(i) = i;
    }
    auto c = a * b;
    REQUIRE(c(0, 0, 0) == 0 * 0);
    REQUIRE(c(0, 0, 1) == 1 * 1);
    REQUIRE(c(0, 1, 1) == 3 * 3);
    REQUIRE(c(1, 2, 0) == 12 * 12);
    REQUIRE(c(2, 3, 1) == 23 * 23);
}

TEST_CASE("Compare two images")
{
    ImageData<int> a;
    ImageData<int> b;
    CHECK(a == b);

    a = ImageData<int>{ { 5, 10 }, 0 };
    CHECK(a != b);

    b = ImageData<int>{ { 5, 10 }, 0 };
    CHECK(a == b);

    b(3, 3) = 7;
    CHECK(a != b);
}

TEST_CASE("Size and shape matches")
{
    ssize_t w = 15;
    ssize_t h = 75;
    std::array<ssize_t, 2> shape{ w, h };
    ImageData<double> a{ shape };
    REQUIRE(a.size() == w * h);
    REQUIRE(a.shape() == shape);
}

TEST_CASE("Initial value matches for all elements")
{
    double v = 4.35;
    ImageData<double> a{ { 5, 5 }, v };
    for (int i = 0; i < a.size(); ++i) {
        REQUIRE(a(i) == v);
    }
}

TEST_CASE("Data layout of 3D image, fast index last"){
    ImageData<int,3> a{{3,3,3}, 0};
    REQUIRE(a.size()== 27);
    int* ptr = a.data();

    for (int i = 0; i<9; ++i){
        *ptr++ = 10+i;
        REQUIRE(a(0,0,i) == 10+i);
        REQUIRE(a(i) == 10+i);

    }
}

TEST_CASE("Bitwise and on data"){

    ImageData<uint16_t, 1> a({3}, 0);
    uint16_t mask = 0x3FF;
    a(0) = 16684;
    a(1) = 33068;
    a(2) = 52608;

    a &= mask;

    REQUIRE(a(0) == 300);
    REQUIRE(a(1) == 300);
    REQUIRE(a(2) == 384);

}

// TEST_CASE("Benchmarks")
// {
//     ImageData<double> img;
//     std::array<ssize_t, 2> shape{ 512, 1024 };
//     BENCHMARK("Allocate 500k double image")
//     {
//         ImageData<double>im{ shape };
//     }
//     BENCHMARK("Allocate 500k double image with initial value")
//     {
//         ImageData<double>im{ shape, 3.14 };
//     }

//     ImageData<double> a{ shape, 1.2 };
//     ImageData<double> b{ shape, 53. };
//     auto c = a + b;
//     c      = a * b;
//     BENCHMARK("Multiply two images")
//     {
//         c = a * b;
//     }
//     BENCHMARK("Divide two images")
//     {
//         c = a / b;
//     }
//     BENCHMARK("Add two images")
//     {
//         c = a + b;
//     }
//     BENCHMARK("Subtract two images")
//     {
//         c = a - b;
//     }
// }

TEST_CASE("Elementwise operatios on images")
{
    std::array<ssize_t, 2> shape{ 5, 5 };
    double a_val = 3.0;
    double b_val = 8.0;

    SECTION("Add two images")
    {
        ImageData<double> A(shape, a_val);
        ImageData<double> B(shape, b_val);

        auto C = A + B;

        //Value of C matches
        for (int i = 0; i < C.size(); ++i) {
            REQUIRE(C(i) == a_val + b_val);
        }

        //Value of A is not changed
        for (int i = 0; i < A.size(); ++i) {
            REQUIRE(A(i) == a_val);
        }

        //Value of B is not changed
        for (int i = 0; i < B.size(); ++i) {
            REQUIRE(B(i) == b_val);
        }

        //A, B and C referes to different data
        REQUIRE(A.data() != B.data());
        REQUIRE(B.data() != C.data());
    }
    SECTION("Subtract two images")
    {
        ImageData<double> A(shape, a_val);
        ImageData<double> B(shape, b_val);
        auto C = A - B;

        //Value of C matches
        for (int i = 0; i < C.size(); ++i) {
            REQUIRE(C(i) == a_val - b_val);
        }

        //Value of A is not changed
        for (int i = 0; i < A.size(); ++i) {
            REQUIRE(A(i) == a_val);
        }

        //Value of B is not changed
        for (int i = 0; i < B.size(); ++i) {
            REQUIRE(B(i) == b_val);
        }

        //A, B and C referes to different data
        REQUIRE(A.data() != B.data());
        REQUIRE(B.data() != C.data());
    }
    SECTION("Multiply two images")
    {
        ImageData<double> A(shape, a_val);
        ImageData<double> B(shape, b_val);
        auto C = A * B;

        //Value of C matches
        for (int i = 0; i < C.size(); ++i) {
            REQUIRE(C(i) == a_val * b_val);
        }

        //Value of A is not changed
        for (int i = 0; i < A.size(); ++i) {
            REQUIRE(A(i) == a_val);
        }

        //Value of B is not changed
        for (int i = 0; i < B.size(); ++i) {
            REQUIRE(B(i) == b_val);
        }

        //A, B and C referes to different data
        REQUIRE(A.data() != B.data());
        REQUIRE(B.data() != C.data());
    }
    SECTION("Divide two images")
    {
        ImageData<double> A(shape, a_val);
        ImageData<double> B(shape, b_val);
        auto C = A / B;

        //Value of C matches
        for (int i = 0; i < C.size(); ++i) {
            REQUIRE(C(i) == a_val / b_val);
        }

        //Value of A is not changed
        for (int i = 0; i < A.size(); ++i) {
            REQUIRE(A(i) == a_val);
        }

        //Value of B is not changed
        for (int i = 0; i < B.size(); ++i) {
            REQUIRE(B(i) == b_val);
        }

        //A, B and C referes to different data
        REQUIRE(A.data() != B.data());
        REQUIRE(B.data() != C.data());
    }

    SECTION("subtract scalar")
    {
        ImageData<double> A(shape, a_val);
        ImageData<double> B(shape, b_val);
        double v = 1.0;
        auto C   = A - v;
        REQUIRE(C.data() != A.data());

        //Value of C matches
        for (int i = 0; i < C.size(); ++i) {
            REQUIRE(C(i) == a_val - v);
        }

        //Value of A is not changed
        for (int i = 0; i < A.size(); ++i) {
            REQUIRE(A(i) == a_val);
        }
    }
    SECTION("add scalar")
    {
        ImageData<double> A(shape, a_val);
        ImageData<double> B(shape, b_val);
        double v = 1.0;
        auto C   = A + v;
        REQUIRE(C.data() != A.data());

        //Value of C matches
        for (int i = 0; i < C.size(); ++i) {
            REQUIRE(C(i) == a_val + v);
        }

        //Value of A is not changed
        for (int i = 0; i < A.size(); ++i) {
            REQUIRE(A(i) == a_val);
        }
    }
    SECTION("divide with scalar")
    {
        ImageData<double> A(shape, a_val);
        ImageData<double> B(shape, b_val);
        double v = 3.7;
        auto C   = A / v;
        REQUIRE(C.data() != A.data());

        //Value of C matches
        for (int i = 0; i < C.size(); ++i) {
            REQUIRE(C(i) == a_val / v);
        }

        //Value of A is not changed
        for (int i = 0; i < A.size(); ++i) {
            REQUIRE(A(i) == a_val);
        }
    }
    SECTION("multiply with scalar")
    {
        ImageData<double> A(shape, a_val);
        ImageData<double> B(shape, b_val);
        double v = 3.7;
        auto C   = A / v;
        REQUIRE(C.data() != A.data());

        //Value of C matches
        for (int i = 0; i < C.size(); ++i) {
            REQUIRE(C(i) == a_val / v);
        }

        //Value of A is not changed
        for (int i = 0; i < A.size(); ++i) {
            REQUIRE(A(i) == a_val);
        }
    }
}