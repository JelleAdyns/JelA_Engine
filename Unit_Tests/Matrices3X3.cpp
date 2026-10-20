#include <gtest/gtest.h>

#include "Structs.h"

namespace jela::mat3x3
{
    static constexpr inline float F_EPS = std::numeric_limits<float>::epsilon();
    static constexpr inline float F_LOW = std::numeric_limits<float>::lowest();
    static constexpr inline float F_MIN = std::numeric_limits<float>::min();
    static constexpr inline float F_MAX = std::numeric_limits<float>::max();

    TEST(TestIndices, WrongIndices)
    {
        constexpr Matrix3X3 mat{};

        EXPECT_DEBUG_DEATH(mat[-1], ".*");
        EXPECT_DEBUG_DEATH(mat[3], ".*");
    }
    TEST(TestIndices, RightIndices)
    {
        constexpr Matrix3X3 mat{};

        EXPECT_EQ(mat[0], Vector3f::UnitX);
        EXPECT_EQ(mat[1], Vector3f::UnitY);
        EXPECT_EQ(mat[2], Vector3f::UnitZ);
    }
    TEST(ConstructorTest, Constructors)
    {
        constexpr Matrix3X3 mat{}; // Default
        EXPECT_EQ(mat.GetXAxis(), Vector2f::UnitX);
        EXPECT_EQ(mat.GetYAxis(), Vector2f::UnitY);
        EXPECT_EQ(mat.GetTranslation(), Vector2f::Zero);

        constexpr Vector2f vec1{45,67}, vec2{-54, 67}, vec3{-91, 4};
        constexpr Matrix3X3 mat1{vec1,vec2,vec3}; // Vector2f
        EXPECT_EQ(mat1.GetXAxis(), vec1);
        EXPECT_EQ(mat1.GetYAxis(), vec2);
        EXPECT_EQ(mat1.GetTranslation(), vec3);

        constexpr Vector3f vec4{4,867,3}, vec5{-23, 78, -45}, vec6{-9, 4, 72};
        constexpr Matrix3X3 mat2{vec4, vec5, vec6}; // Vector3f
        EXPECT_EQ(mat2.GetXAxis(), (Vector2f{vec4[0], vec4[1]}));
        EXPECT_EQ(mat2.GetYAxis(), (Vector2f{vec5[0], vec5[1]}));
        EXPECT_EQ(mat2.GetTranslation(), (Vector2f{vec6[0], vec6[1]}));
        EXPECT_EQ(mat2[0], vec4);
        EXPECT_EQ(mat2[1], vec5);
        EXPECT_EQ(mat2[2], vec6);
    }

    class TestTransformCreations : public testing::Test{};

    TEST_F(TestTransformCreations, Translation)
    {
        constexpr Vector2f translation{8,45};
        constexpr Matrix3X3 mat{Matrix3X3::Translation(translation)};
        EXPECT_EQ(mat.GetXAxis(), Vector2f::UnitX);
        EXPECT_EQ(mat.GetYAxis(), Vector2f::UnitY);
        EXPECT_EQ(mat.GetTranslation(), translation);
        EXPECT_EQ(mat[0], Vector3f::UnitX);
        EXPECT_EQ(mat[1], Vector3f::UnitY);
        EXPECT_EQ(mat[2], (Vector3f{translation, 1}));
    }
    TEST_F(TestTransformCreations, RotationPositiveRightAngle)
    {
        constexpr float toRadians{std::numbers::pi_v<float> / 180.f};
        constexpr float degrees{90};
        constexpr float radians{degrees * toRadians};

        const Matrix3X3 matDeg{Matrix3X3::Rotation(degrees)};
        EXPECT_EQ(matDeg.GetXAxis(), Vector2f::UnitY);
        EXPECT_EQ(matDeg.GetYAxis(), -Vector2f::UnitX);
        EXPECT_EQ(matDeg.GetTranslation(), Vector2f::Zero);
        EXPECT_EQ(matDeg[0], Vector3f::UnitY);
        EXPECT_EQ(matDeg[1], -Vector3f::UnitX);
        EXPECT_EQ(matDeg[2], (Vector3f{Vector2f::Zero, 1}));

        const Matrix3X3 matRad{Matrix3X3::Rotation(radians, false)};
        EXPECT_EQ(matDeg, matRad);
    }
    TEST_F(TestTransformCreations, RotationNegativeRightAngle)
    {
        constexpr float toRadians{std::numbers::pi_v<float> / 180};
        constexpr float degrees{-90};
        constexpr float radians{degrees * toRadians};

        const Matrix3X3 matDeg{Matrix3X3::Rotation(degrees)};

        EXPECT_EQ(matDeg.GetXAxis(), -Vector2f::UnitY);
        EXPECT_EQ(matDeg.GetYAxis(), Vector2f::UnitX);
        EXPECT_EQ(matDeg.GetTranslation(), Vector2f::Zero);
        EXPECT_EQ(matDeg[0], -Vector3f::UnitY);
        EXPECT_EQ(matDeg[1], Vector3f::UnitX);
        EXPECT_EQ(matDeg[2], (Vector3f{Vector2f::Zero, 1}));

        const Matrix3X3 matRad{Matrix3X3::Rotation(radians, false)};
        EXPECT_EQ(matDeg, matRad);
    }
    TEST_F(TestTransformCreations, RotationStraightAngle)
    {
        constexpr float degrees{180};
        constexpr float radians{std::numbers::pi_v<float>};

        const Matrix3X3 matDeg{Matrix3X3::Rotation(degrees)};

        EXPECT_EQ(matDeg.GetXAxis(), -Vector2f::UnitX);
        EXPECT_EQ(matDeg.GetYAxis(), -Vector2f::UnitY);
        EXPECT_EQ(matDeg.GetTranslation(), Vector2f::Zero);
        EXPECT_EQ(matDeg[0], -Vector3f::UnitX);
        EXPECT_EQ(matDeg[1], -Vector3f::UnitY);
        EXPECT_EQ(matDeg[2], (Vector3f{Vector2f::Zero, 1}));

        const Matrix3X3 matRad{Matrix3X3::Rotation(radians, false)};
        EXPECT_EQ(matDeg, matRad);
    }
    TEST_F(TestTransformCreations, Scale)
    {
        constexpr Vector2f scale{2,3};
        constexpr Matrix3X3 mat{Matrix3X3::Scale(scale)};
        EXPECT_EQ(mat.GetXAxis(), scale.x * Vector2f::UnitX);
        EXPECT_EQ(mat.GetYAxis(), scale.y * Vector2f::UnitY);
        EXPECT_EQ(mat.GetTranslation(), Vector2f::Zero);
        EXPECT_EQ(mat[0], scale.x * Vector3f::UnitX);
        EXPECT_EQ(mat[1], scale.y * Vector3f::UnitY);
        EXPECT_EQ(mat[2], (Vector3f{Vector2f::Zero, 1}));
    }

    const inline static std::unordered_map<std::string, Matrix3X3> INPUT_MAP
    {
        {"Identity", {}},
        {"Zeros", {Vector3f::Zero, Vector3f::Zero, Vector3f::Zero}},
        {"Ones", {Vector3f::One, Vector3f::One, Vector3f::One}},
        {"Positives", {Vector3f{34,6,23},Vector3f{85,76,3},Vector3f{9,43,92}}},
        {"Negatives", {Vector3f{-3,-53,-6},Vector3f{-43,-63,-234},Vector3f{-38,-62,-32}}},
        {"MixedSigns", {Vector3f{3,-5,-65},Vector3f{4,-63,24},Vector3f{-8,62,-3}}},
    };

    struct OperationInfo
    {
        std::string name{};
        Matrix3X3 op1{};
        std::variant<Matrix3X3, Vector2f, float> op2{};
    };

    static inline const std::string TEST_CONCATENATION{"_X_"};

    class TestBinaryOperationsMat3X3 : public testing::TestWithParam<OperationInfo>
    {
    public:
        static std::vector<OperationInfo> GetOperations()
        {
            std::vector<OperationInfo> result{};

            for (const auto & [op1Name, op1] : INPUT_MAP)
                for (const auto & [op2Name, op2] : OPERANDS)
                    result.emplace_back(std::format("{}{}{}", op1Name, TEST_CONCATENATION, op2Name), op1, op2);

            return result;
        }
        const inline static std::unordered_map<std::string, Matrix3X3> OPERANDS
        {
            {"Identity", {}},
            {"Zeros", {Vector3f::Zero, Vector3f::Zero, Vector3f::Zero}},
            {"Ones", {Vector3f::One, Vector3f::One, Vector3f::One}},
            {"PositivesA", INPUT_MAP.at("Positives")},
            {"PositivesB", {Vector3f{375,32,3},Vector3f{45,61,12},Vector3f{55,566,521}}},
            {"NegativesA", INPUT_MAP.at("Negatives")},
            {"NegativesB", {Vector3f{-83,-123,-211},Vector3f{-45,-631,-134},Vector3f{-341,-61,-31}}},
            {"MixedSignsA", INPUT_MAP.at("MixedSigns")},
            {"MixedSignsB", {Vector3f{-32,51,-6},Vector3f{-2,61,241},Vector3f{-4,-16,-41}}},
        };

    protected:
        void SetUp() override
        {
            const auto& p = GetParam();
            op1 = p.op1;
            op2 = std::get<Matrix3X3>(p.op2);
        }

        Matrix3X3 op1{};
        Matrix3X3 op2{};
    };
    class TestComparisonMat3X3 : public TestBinaryOperationsMat3X3
    {
    public:
        static std::vector<OperationInfo> GetCompareOperations()
        {
            std::vector<OperationInfo> result{};

            for (const auto & [op1Name, op1] : INPUT_MAP)
                for (const auto & [op2Name, op2] : INPUT_MAP)
                    result.emplace_back(std::format("{}{}{}", op1Name, TEST_CONCATENATION, op2Name), op1, op2);

            return result;
        }
    protected:
        static bool CompareSameMatrix()
        {
            const auto& name = GetParam().name;

            const std::regex r(std::format("(.*){}(.*)",TEST_CONCATENATION));
            std::smatch matches{};
            std::regex_search(name.cbegin(),name.cend(),matches,r);
            const auto firstName = matches.str(1);
            const auto secondName = matches.str(2);

            return firstName == secondName;
        }
    };
    TEST_P(TestComparisonMat3X3, Comparison)
    {
        if (CompareSameMatrix())
        {
            EXPECT_TRUE(op1 == op2);
            EXPECT_FALSE(op1 != op2);
        }
        else
        {
            EXPECT_FALSE(op1 == op2);
            EXPECT_TRUE(op1 != op2);
        }
    }
    TEST_P(TestBinaryOperationsMat3X3, Addition)
    {
        const auto result = op1 + op2;
        EXPECT_FLOAT_EQ(result[0][0], op1[0][0] + op2[0][0]);
        EXPECT_FLOAT_EQ(result[1][1], op1[1][1] + op2[1][1]);
        EXPECT_FLOAT_EQ(result[2][2], op1[2][2] + op2[2][2]);

        EXPECT_FLOAT_EQ(result[0][1], op1[0][1] + op2[0][1]);
        EXPECT_FLOAT_EQ(result[0][2], op1[0][2] + op2[0][2]);

        EXPECT_FLOAT_EQ(result[1][0], op1[1][0] + op2[1][0]);
        EXPECT_FLOAT_EQ(result[2][0], op1[2][0] + op2[2][0]);

        EXPECT_FLOAT_EQ(result[1][2], op1[1][2] + op2[1][2]);
        EXPECT_FLOAT_EQ(result[2][1], op1[2][1] + op2[2][1]);

        op1 += op2;
        EXPECT_EQ(result, op1);
    }
    TEST_P(TestBinaryOperationsMat3X3, Subtraction)
    {
        const auto result = op1 - op2;
        EXPECT_FLOAT_EQ(result[0][0], op1[0][0] - op2[0][0]);
        EXPECT_FLOAT_EQ(result[1][1], op1[1][1] - op2[1][1]);
        EXPECT_FLOAT_EQ(result[2][2], op1[2][2] - op2[2][2]);

        EXPECT_FLOAT_EQ(result[0][1], op1[0][1] - op2[0][1]);
        EXPECT_FLOAT_EQ(result[0][2], op1[0][2] - op2[0][2]);

        EXPECT_FLOAT_EQ(result[1][0], op1[1][0] - op2[1][0]);
        EXPECT_FLOAT_EQ(result[2][0], op1[2][0] - op2[2][0]);

        EXPECT_FLOAT_EQ(result[1][2], op1[1][2] - op2[1][2]);
        EXPECT_FLOAT_EQ(result[2][1], op1[2][1] - op2[2][1]);

        op1 -= op2;
        EXPECT_EQ(result, op1);
    }
    TEST_P(TestBinaryOperationsMat3X3, Multiplication)
    {
        // [0,0] [0,1] [0,2]
        //
        // [1,0] [1,1] [1,2]
        //
        // [2,0] [2,1] [2,2]

        const auto result = op1 * op2;
        EXPECT_FLOAT_EQ(result[0][0], op1[0][0] * op2[0][0] + op1[0][1] * op2[1][0] + op1[0][2] * op2[2][0]);
        EXPECT_FLOAT_EQ(result[0][1], op1[0][0] * op2[0][1] + op1[0][1] * op2[1][1] + op1[0][2] * op2[2][1]);
        EXPECT_FLOAT_EQ(result[0][2], op1[0][0] * op2[0][2] + op1[0][1] * op2[1][2] + op1[0][2] * op2[2][2]);

        EXPECT_FLOAT_EQ(result[1][0], op1[1][0] * op2[0][0] + op1[1][1] * op2[1][0] + op1[1][2] * op2[2][0]);
        EXPECT_FLOAT_EQ(result[1][1], op1[1][0] * op2[0][1] + op1[1][1] * op2[1][1] + op1[1][2] * op2[2][1]);
        EXPECT_FLOAT_EQ(result[1][2], op1[1][0] * op2[0][2] + op1[1][1] * op2[1][2] + op1[1][2] * op2[2][2]);

        EXPECT_FLOAT_EQ(result[2][0], op1[2][0] * op2[0][0] + op1[2][1] * op2[1][0] + op1[2][2] * op2[2][0]);
        EXPECT_FLOAT_EQ(result[2][1], op1[2][0] * op2[0][1] + op1[2][1] * op2[1][1] + op1[2][2] * op2[2][1]);
        EXPECT_FLOAT_EQ(result[2][2], op1[2][0] * op2[0][2] + op1[2][1] * op2[1][2] + op1[2][2] * op2[2][2]);

        op1 *= op2;
        EXPECT_EQ(result, op1);
    }
    TEST_P(TestBinaryOperationsMat3X3, TransposeDistributiveProperty)
    {
        const auto transposedAddition = (op1 + op2).Transpose();
        const auto transposedA = op1.Transpose();
        const auto transposedB = op2.Transpose();

        EXPECT_EQ(transposedAddition, transposedA + transposedB);
    }
    TEST_P(TestBinaryOperationsMat3X3, TransposeSocksAndBootsProperty)
    {
        const auto transposedDot = (op1 * op2).Transpose();
        const auto transposedA = op1.Transpose();
        const auto transposedB = op2.Transpose();

        EXPECT_EQ(transposedDot, transposedB * transposedA);
    }


    struct TernaryOperationInfo
    {
        std::string name{};
        Matrix3X3 op1{};
        Matrix3X3 op2{};
        Matrix3X3 op3{};
    };
    class TestTernaryOperationsMat3X3 : public testing::TestWithParam<TernaryOperationInfo>
    {
    public:
        static std::vector<TernaryOperationInfo> GetOperations()
        {
            std::vector<TernaryOperationInfo> result{};

            for (const auto & [op1Name, op1] : INPUT_MAP)
                for (const auto & [op2Name, op2] : OPERANDS)
                    for (const auto & [op3Name, op3] : OPERANDS)
                    result.emplace_back(std::format("{1}{0}{2}{0}{3}", TEST_CONCATENATION, op1Name, op2Name, op3Name), op1, op2, op3);

            return result;
        }
        const inline static std::unordered_map<std::string, Matrix3X3> OPERANDS
        {
            {"Identity", {}},
            {"Zeros", {Vector3f::Zero, Vector3f::Zero, Vector3f::Zero}},
            {"Ones", {Vector3f::One, Vector3f::One, Vector3f::One}},
            {"PositivesA", INPUT_MAP.at("Positives")},
            {"PositivesB", {Vector3f{37,32,3},Vector3f{45,61,12},Vector3f{55,56,21}}},
            {"NegativesA", INPUT_MAP.at("Negatives")},
            {"NegativesB", {Vector3f{-83,-13,-21},Vector3f{-45,-31,-14},Vector3f{-41,-61,-31}}},
            {"MixedSignsA", INPUT_MAP.at("MixedSigns")},
            {"MixedSignsB", {Vector3f{-32,51,-6},Vector3f{-2,61,21},Vector3f{-4,-16,-41}}},
        };

    protected:
        void SetUp() override
        {
            const auto& p = GetParam();
            op1 = p.op1;
            op2 = p.op2;
            op3 = p.op3;
        }

        Matrix3X3 op1{};
        Matrix3X3 op2{};
        Matrix3X3 op3{};
    };

    TEST_P(TestTernaryOperationsMat3X3, RightDistributiveLaw)
    {
        EXPECT_EQ
        (
            (op1 + op2) * op3,
            op1 * op3 + op2 * op3
        );
    }
    TEST_P(TestTernaryOperationsMat3X3, LeftDistributiveLaw)
    {
        EXPECT_EQ
        (
            op3 * (op1 + op2),
            op3 * op1 + op3 * op2
        );
    }
    TEST_P(TestTernaryOperationsMat3X3, AssociativeProperty)
    {
        EXPECT_EQ
        (
            (op1 * op2) * op3,
            op1 * (op2 * op3)
        );
    }

    class TestScalarOperationsMat3X3: public ::testing::TestWithParam<OperationInfo>
    {
    public:
        static std::vector<OperationInfo> GetOperations()
        {
            std::vector<OperationInfo> result{};

            for (const auto & [op1Name, op1] : INPUT_MAP)
                for (const auto & [op2Name, op2] : SCALAR_OPERANDS)
                    result.emplace_back(std::format("{}{}{}", op1Name, TEST_CONCATENATION, op2Name), op1, op2);

            return result;
        }
        const inline static std::map<std::string, float> SCALAR_OPERANDS
        {
			{"Zero", 0.f},
            {"One", 1.f},
            {"Positive", 734.f},
            {"Negative", -412.f},
            {"Max", F_MAX},
            {"Min", F_LOW},
        };
    protected:
        void SetUp() override
        {
            const auto& p = GetParam();
            op1 = p.op1;
            op2 = std::get<float>(p.op2);
        }
        Matrix3X3 op1{};
        float op2{};
    };

    TEST_P(TestScalarOperationsMat3X3, Multiplication)
    {
        const auto result = op1 * op2;

        EXPECT_FLOAT_EQ(result[0][0], op1[0][0] * op2);
        EXPECT_FLOAT_EQ(result[1][1], op1[1][1] * op2);
        EXPECT_FLOAT_EQ(result[2][2], op1[2][2] * op2);

        EXPECT_FLOAT_EQ(result[0][1], op1[0][1] * op2);
        EXPECT_FLOAT_EQ(result[0][2], op1[0][2] * op2);

        EXPECT_FLOAT_EQ(result[1][0], op1[1][0] * op2);
        EXPECT_FLOAT_EQ(result[2][0], op1[2][0] * op2);

        EXPECT_FLOAT_EQ(result[1][2], op1[1][2] * op2);
        EXPECT_FLOAT_EQ(result[2][1], op1[2][1] * op2);

        const auto result2 = op2 * op1;
        EXPECT_EQ(result, result2);

        op1 *= op2;
        EXPECT_EQ(result, op1);
    }

    class TestUnaryOperationsMat3X3 : public testing::TestWithParam<std::pair<const std::string, Matrix3X3>>
    {
    protected:
        void SetUp() override { op1 = GetParam().second; }
        Matrix3X3 op1{};
    };
    TEST_P(TestUnaryOperationsMat3X3, Plus)
    {
        const auto result = +op1;
        EXPECT_EQ(result, op1);
    }
    TEST_P(TestUnaryOperationsMat3X3, Minus)
    {
        const auto result = -op1;
        EXPECT_FLOAT_EQ(result[0][0],-op1[0][0]);
        EXPECT_FLOAT_EQ(result[1][1],-op1[1][1]);
        EXPECT_FLOAT_EQ(result[2][2],-op1[2][2]);

        EXPECT_FLOAT_EQ(result[0][1],-op1[0][1]);
        EXPECT_FLOAT_EQ(result[0][2],-op1[0][2]);

        EXPECT_FLOAT_EQ(result[1][0],-op1[1][0]);
        EXPECT_FLOAT_EQ(result[2][0],-op1[2][0]);

        EXPECT_FLOAT_EQ(result[1][2],-op1[1][2]);
        EXPECT_FLOAT_EQ(result[2][1],-op1[2][1]);
    }
    TEST_P(TestUnaryOperationsMat3X3, Transpose)
    {
        const auto result = Matrix3X3::Transpose(op1);

        EXPECT_FLOAT_EQ(result[0][0],op1[0][0]);
        EXPECT_FLOAT_EQ(result[1][1],op1[1][1]);
        EXPECT_FLOAT_EQ(result[2][2],op1[2][2]);

        EXPECT_FLOAT_EQ(result[0][1],op1[1][0]);
        EXPECT_FLOAT_EQ(result[0][2],op1[2][0]);

        EXPECT_FLOAT_EQ(result[1][0],op1[0][1]);
        EXPECT_FLOAT_EQ(result[2][0],op1[0][2]);

        EXPECT_FLOAT_EQ(result[1][2],op1[2][1]);
        EXPECT_FLOAT_EQ(result[2][1],op1[1][2]);

        op1.Transpose();
        EXPECT_EQ(result, op1);
    }
    class TestVectorTransformationMat3X3 : public testing::TestWithParam<OperationInfo>
    {
    public:
        static std::vector<OperationInfo> GetOperations()
        {
            std::vector<OperationInfo> result{};

            for (const auto & [op1Name, op1] : INPUT_MAP)
                for (const auto & [op2Name, op2] : VECTOR_OPERANDS)
                    result.emplace_back(std::format("{}{}{}", op1Name, TEST_CONCATENATION, op2Name), op1, op2);

            return result;
        }
        const inline static std::map<std::string, Vector2f> VECTOR_OPERANDS
        {
            {"Default", {} },
            {"One", Vector2f{1.f,1.f}},
            {"Positive", Vector2f{23.f,421.f}},
            {"Negative", Vector2f{-264.f,-41.f}},
            {"NegativePositive", Vector2f{-65.f,41.f}},
            {"PositiveNegative", Vector2f{65.f,-41.f}},
            {"Max", Vector2f{F_MAX, F_MAX}},
            {"Min", Vector2f{F_MIN, F_MIN}},
        };
    protected:
        void SetUp() override
        {
            const auto& p = GetParam();
            mat = p.op1;
            vec = std::get<Vector2f>(p.op2);
        }
        Matrix3X3 mat{};
        Vector2f vec{};
    };

    TEST_P(TestVectorTransformationMat3X3, TransformVector)
    {
        const auto result = mat.TransformVector(vec);
        const auto xAxis = mat.GetXAxis();
        const auto yAxis = mat.GetYAxis();
        EXPECT_FLOAT_EQ(result.x, xAxis.x * vec.x + yAxis.x * vec.y);
        EXPECT_FLOAT_EQ(result.y, xAxis.y * vec.x + yAxis.y * vec.y);

        EXPECT_EQ(result, mat.TransformVector(vec.x, vec.y));
    }
    TEST_P(TestVectorTransformationMat3X3, TransformPoint)
    {
        const auto result = mat.TransformPoint(vec);
        const auto xAxis = mat.GetXAxis();
        const auto yAxis = mat.GetYAxis();
        const auto translation = mat.GetTranslation();
        EXPECT_FLOAT_EQ(result.x, xAxis.x * vec.x + yAxis.x * vec.y + translation.x);
        EXPECT_FLOAT_EQ(result.y, xAxis.y * vec.x + yAxis.y * vec.y + translation.y);

        EXPECT_EQ(result, mat.TransformPoint(vec.x, vec.y));
    }
    INSTANTIATE_TEST_SUITE_P
    (
        Operators,
        TestUnaryOperationsMat3X3,
        testing::ValuesIn(INPUT_MAP),
        [](const testing::TestParamInfo<TestUnaryOperationsMat3X3::ParamType>& info){return info.param.first;}
    );
    INSTANTIATE_TEST_SUITE_P
    (
        Operators,
        TestBinaryOperationsMat3X3,
        testing::ValuesIn(TestBinaryOperationsMat3X3::GetOperations()),
        [](const testing::TestParamInfo<TestBinaryOperationsMat3X3::ParamType>& info){return info.param.name;}
    );
    INSTANTIATE_TEST_SUITE_P
    (
        Operators,
        TestComparisonMat3X3,
        testing::ValuesIn(TestComparisonMat3X3::GetCompareOperations()),
        [](const testing::TestParamInfo<TestComparisonMat3X3::ParamType>& info){return info.param.name;}
    );
    INSTANTIATE_TEST_SUITE_P
    (
        Operators,
        TestTernaryOperationsMat3X3,
        testing::ValuesIn(TestTernaryOperationsMat3X3::GetOperations()),
        [](const testing::TestParamInfo<TestTernaryOperationsMat3X3::ParamType>& info){return info.param.name;}
    );
    INSTANTIATE_TEST_SUITE_P
    (
        Operators,
        TestScalarOperationsMat3X3,
        testing::ValuesIn(TestScalarOperationsMat3X3::GetOperations()),
        [](const testing::TestParamInfo<TestScalarOperationsMat3X3::ParamType>& info){return info.param.name;}
    );
    INSTANTIATE_TEST_SUITE_P
    (
        Operators,
        TestVectorTransformationMat3X3,
        testing::ValuesIn(TestVectorTransformationMat3X3::GetOperations()),
        [](const testing::TestParamInfo<TestVectorTransformationMat3X3::ParamType>& info){return info.param.name;}
    );


}
