#include <gtest/gtest.h>
#include <complex>

#include "Structs.h"

namespace jela::vec2f
{
	static constexpr inline float F_EPS = std::numeric_limits<float>::epsilon();
	static constexpr inline float F_LOW = std::numeric_limits<float>::lowest();
	static constexpr inline float F_MIN = std::numeric_limits<float>::min();
	static constexpr inline float F_MAX = std::numeric_limits<float>::max();

	static inline const std::map<std::string, Vector2f> INPUT_MAP
	{
		{"DefaultVector", Vector2f{}},
		{"OneOne", Vector2f{1,1}},
		{"Positives", Vector2f{34,67}},
		{"Negatives", Vector2f{-45,-78}},
		{"PositiveNegative", Vector2f{12, -75}},
		{"NegativePositive", Vector2f{-83, 92}},
		{"Max", Vector2f{F_MAX, F_MAX}},
		{"Min", Vector2f{F_LOW, F_LOW}},
	};

	static inline const std::string TEST_CONCATENATION{"_X_"};

	struct OperationInfo
	{
		std::string name{};
		Vector2f op1{};
		std::variant<Vector2f, float> op2{};
	};

	TEST(ConstructorTest2f, Constructors)
	{
		constexpr Vector2f vec{};
		EXPECT_FLOAT_EQ(vec.x, 0);
		EXPECT_FLOAT_EQ(vec.y, 0);
		constexpr Vector2f vec1{-34, 7};
		EXPECT_FLOAT_EQ(vec1.x, -34);
		EXPECT_FLOAT_EQ(vec1.y, 7);
		constexpr Vector2f vec2{1, -1};
		EXPECT_FLOAT_EQ(vec2.x, 1);
		EXPECT_FLOAT_EQ(vec2.y, -1);
		constexpr Vector2f vec3{vec1, vec2};
		EXPECT_FLOAT_EQ(vec3.x, 35);
		EXPECT_FLOAT_EQ(vec3.y, -8);
	}

	TEST(TestIndices2f, WrongIndices)
	{
		constexpr Vector2f vec{5,12};

		EXPECT_DEBUG_DEATH(vec[-1], ".*");
		EXPECT_DEBUG_DEATH(vec[2], ".*");
	}
	TEST(TestIndices2f, RightIndices)
	{
		constexpr Vector2f vec{5,12};

		EXPECT_FLOAT_EQ(vec[0], 5);
		EXPECT_FLOAT_EQ(vec[1], 12);
	}

	class TestUnaryOperators2f : public ::testing::TestWithParam<std::pair<const std::string, Vector2f>>
	{
	protected:
		void SetUp() override
		{
			const auto& [name, vector] = GetParam();
			vec = vector;
		}
		Vector2f vec{};
	};

	TEST_P(TestUnaryOperators2f, Plus)
	{
		const auto result = +vec; // tested operator

		EXPECT_FLOAT_EQ(result.x, +vec.x);
		EXPECT_FLOAT_EQ(result.y, +vec.y);
	}
	TEST_P(TestUnaryOperators2f, Minus)
	{
		const auto result = -vec; // tested operator

		EXPECT_FLOAT_EQ(result.x, -vec.x);
		EXPECT_FLOAT_EQ(result.y, -vec.y);
	}
	TEST_P(TestUnaryOperators2f, NormalizeVSNormalized)
	{
		const auto result = vec.Normalized(); // tested operator
		vec.Normalize();
		EXPECT_FLOAT_EQ(result.x, vec.x);
		EXPECT_FLOAT_EQ(result.y, vec.y);
	}
	TEST_P(TestUnaryOperators2f, LengthOfNormalizedVector)
	{
		const auto result = vec.Normalized(); // tested operator

		if ((std::abs(vec.x) < F_EPS && std::abs(vec.y) < F_EPS) ||
			vec.x == F_MAX || vec.y == F_MAX ||
			vec.x == F_LOW || vec.y == F_LOW)
			EXPECT_FLOAT_EQ(0, result.Length());
		else EXPECT_FLOAT_EQ(1, result.Length());
	}
	TEST_P(TestUnaryOperators2f, DerictionOfNormalizedVector)
	{
		const auto result = vec.Normalized(); // tested operator
		EXPECT_LT(std::abs(result.AngleBetween(vec,result)), std::numeric_limits<float>::round_error());
	}

	TEST_P(TestUnaryOperators2f, Orthogonal)
	{
		const auto result = vec.Orthogonal(); // tested operator
		if (vec.x == F_MAX || vec.y == F_MAX ||
			vec.x == F_LOW || vec.y == F_LOW)
			EXPECT_TRUE(std::isnan(result.Dot(vec,result) ));
		else EXPECT_FLOAT_EQ(0, result.Dot(vec,result));
	}

	class TestBinaryOperators2f : public ::testing::TestWithParam<OperationInfo>
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
		const inline static std::map<std::string, Vector2f> OPERANDS
		{
			{"DefaultVector", Vector2f{}},
			{"OneOne", Vector2f{1,1}},
			{"Positives", Vector2f{734,56}},
			{"Negatives", Vector2f{-412,-67}},
			{"PositiveNegative", Vector2f{21, -16}},
			{"NegativePositive", Vector2f{-79, 53}},
			{"Max", Vector2f{F_MAX, F_MAX}},
			{"Min", Vector2f{F_LOW, F_LOW}},
		};
	protected:
		void SetUp() override
		{
			const auto& p = GetParam();
			op1 = p.op1;
			op2 = std::get<Vector2f>(p.op2);
		}
		Vector2f op1{};
		Vector2f op2{};
	};
	TEST_P(TestBinaryOperators2f, Addition)
	{
		const auto result = op1 + op2; // tested operator

		EXPECT_FLOAT_EQ(result.x, op1.x + op2.x);
		EXPECT_FLOAT_EQ(result.y, op1.y + op2.y);
	}
	TEST_P(TestBinaryOperators2f, Subtraction)
	{
		const auto result = op1 - op2; // tested operator

		EXPECT_FLOAT_EQ(result.x, op1.x - op2.x);
		EXPECT_FLOAT_EQ(result.y, op1.y - op2.y);
	}
	TEST_P(TestBinaryOperators2f, AdditionAssignment)
	{
		auto result = op1;
		result += op2; // tested operator

		EXPECT_FLOAT_EQ(result.x, op1.x + op2.x);
		EXPECT_FLOAT_EQ(result.y, op1.y + op2.y);
	}
	TEST_P(TestBinaryOperators2f, SubtractionAssignment)
	{
		auto result = op1;
		result -= op2; // tested operator

		EXPECT_FLOAT_EQ(result.x, op1.x - op2.x);
		EXPECT_FLOAT_EQ(result.y, op1.y - op2.y);
	}
	TEST_P(TestBinaryOperators2f, Scale)
	{
		auto result1 = op1; result1.Scale(op2);

		EXPECT_FLOAT_EQ(result1.x, op1.x * op2.x);
		EXPECT_FLOAT_EQ(result1.y, op1.y * op2.y);

		const auto result2 = Vector2f::Scale(op1, op2);

		EXPECT_FLOAT_EQ(result2.x, op1.x * op2.x);
		EXPECT_FLOAT_EQ(result2.y, op1.y * op2.y);

		auto result3 = op1; result3.Scale(op2.x, op2.y);

		EXPECT_FLOAT_EQ(result3.x, op1.x * op2.x);
		EXPECT_FLOAT_EQ(result3.y, op1.y * op2.y);

		const auto result4 = Vector2f::Scale(op1, op2.x, op2.y);

		EXPECT_FLOAT_EQ(result4.x, op1.x * op2.x);
		EXPECT_FLOAT_EQ(result4.y, op1.y * op2.y);
	}

	TEST_P(TestBinaryOperators2f, DotCommunitativity)
	{
		const float result1 = Vector2f::Dot(op1,op2);
		const float result2 = Vector2f::Dot(op2,op1);

		if (std::isnan(result1) || std::isnan(result2))
			EXPECT_EQ(std::isnan(result1), std::isnan(result2));
		else EXPECT_FLOAT_EQ(result1, result2);
	}
	TEST_P(TestBinaryOperators2f, CrossAnticommunitativity)
	{
		const float result1 = Vector2f::Cross(op1,op2);
		const float result2 = Vector2f::Cross(op2,op1);

		if (std::isnan(result1) || std::isnan(result2))
			EXPECT_EQ(std::isnan(result1), std::isnan(result2));
		else EXPECT_FLOAT_EQ(result1, -result2);
	}

	class TestComparison2f : public TestBinaryOperators2f
	{
	public:
		static std::vector<OperationInfo> GetOperations()
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
	TEST_P(TestComparison2f, Comparison)
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
	TEST(TestLowComparison2f, LowComparison)
	{
		constexpr Vector2f zero{};
		constexpr Vector2f min{F_MIN, F_MIN};

		EXPECT_TRUE(zero == min);
		EXPECT_FALSE(zero != min);

		constexpr Vector2f epsilon{F_EPS, F_LOW};

		EXPECT_FALSE(zero == epsilon);
		EXPECT_TRUE(zero != epsilon);

	}

	class TestScalarOperations2f : public ::testing::TestWithParam<OperationInfo>
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
		Vector2f op1{};
		float op2{};
	};

	TEST_P(TestScalarOperations2f, Multiplication)
	{
		const auto result1 = op1 * op2; // tested operator

		EXPECT_FLOAT_EQ(result1.x, op1.x * op2);
		EXPECT_FLOAT_EQ(result1.y, op1.y * op2);

		const auto result2 = op2 * op1; // tested operator

		EXPECT_FLOAT_EQ(result2.x, op1.x * op2);
		EXPECT_FLOAT_EQ(result2.y, op1.y * op2);
	}
	TEST_P(TestScalarOperations2f, Division)
	{
		if (std::abs(op2) <= F_EPS)
		{
			EXPECT_DEBUG_DEATH(op1 / op2, ".*"); // tested operator
			return;
		}
		const auto result = op1 / op2; // tested operator
		EXPECT_FLOAT_EQ(result.x, op1.x / op2);
		EXPECT_FLOAT_EQ(result.y, op1.y / op2);

	}
	TEST_P(TestScalarOperations2f, MultiplicationAssignment)
	{
		auto result1 = op1;
		result1 *= op2; // tested operator

		EXPECT_FLOAT_EQ(result1.x, op1.x * op2);
		EXPECT_FLOAT_EQ(result1.y, op1.y * op2);
	}
	TEST_P(TestScalarOperations2f, DivisionAssignment)
	{
		auto result = op1;

		if (std::abs(op2) <= F_EPS)
		{
			EXPECT_DEBUG_DEATH(result /= op2, ".*"); // tested operator
			return;
		}

		result /= op2; // tested operator
		EXPECT_FLOAT_EQ(result.x, op1.x / op2);
		EXPECT_FLOAT_EQ(result.y, op1.y / op2);
	}

	class TestDotProduct2f : public testing::Test{};

	TEST_F(TestDotProduct2f, Positives)
	{
		constexpr Vector2f val1{4,7};
		constexpr Vector2f val2{5,6};
		constexpr float expected{62};

		EXPECT_FLOAT_EQ(expected, Vector2f::Dot(val1,val2));
	}

	TEST_F(TestDotProduct2f, Negatives)
	{
		constexpr Vector2f val1{-12,-7};
		constexpr Vector2f val2{-5,-2};
		constexpr float expected{74};

		EXPECT_FLOAT_EQ(expected, Vector2f::Dot(val1,val2));
	}
	TEST_F(TestDotProduct2f, MixedSigns)
	{
		constexpr Vector2f val1{-6,7};
		constexpr Vector2f val2{-5,-2};
		constexpr float expected{16};

		EXPECT_FLOAT_EQ(expected, Vector2f::Dot(val1,val2));
	}
	TEST_F(TestDotProduct2f, Zero)
	{
		constexpr float expected{0};

		constexpr Vector2f val1{0,0};
		constexpr Vector2f val2{23,45};
		EXPECT_FLOAT_EQ(expected, Vector2f::Dot(val1,val2));
	}
	TEST_F(TestDotProduct2f, Orthogonality)
	{
		constexpr float expected{0};

		Vector2f val1{1,0};
		Vector2f val2{0,1};
		EXPECT_FLOAT_EQ(expected, Vector2f::Dot(val1,val2));

		val1 = {0,1};
		val2 = {-1,0};
		EXPECT_FLOAT_EQ(expected, Vector2f::Dot(val1,val2));

		val1 = {-1,0};
		val2 = {0,-1};
		EXPECT_FLOAT_EQ(expected, Vector2f::Dot(val1,val2));

		val1 = {0,-1};
		val2 = {1,0};
		EXPECT_FLOAT_EQ(expected, Vector2f::Dot(val1,val2));
	}

	class TestCrossProduct2f : public testing::Test{};

	TEST_F(TestCrossProduct2f, Positives)
	{
		constexpr Vector2f val1{4,7};
		constexpr Vector2f val2{5,6};
		constexpr float expected{-11};

		EXPECT_FLOAT_EQ(expected, Vector2f::Cross(val1,val2));
		EXPECT_FLOAT_EQ(1, Vector2f::Cross(Vector2f::UnitX,Vector2f::UnitY));
	}

	TEST_F(TestCrossProduct2f, Negatives)
	{
		constexpr Vector2f val1{-12,-7};
		constexpr Vector2f val2{-5,-3};
		constexpr float expected{1};

		EXPECT_FLOAT_EQ(expected, Vector2f::Cross(val1,val2));
	}
	TEST_F(TestCrossProduct2f, MixedSigns)
	{
		constexpr Vector2f val1{-6,7};
		constexpr Vector2f val2{-5,-2};
		constexpr float expected{47};

		EXPECT_FLOAT_EQ(expected, Vector2f::Cross(val1,val2));
	}
	TEST_F(TestCrossProduct2f, Zero)
	{
		constexpr float expected{0};

		constexpr Vector2f val1{0,0};
		constexpr Vector2f val2{23,45};
		EXPECT_FLOAT_EQ(expected, Vector2f::Cross(val1,val2));
	}
	TEST_F(TestCrossProduct2f, SameDirection)
	{
		constexpr float expected{0};

		Vector2f val1{1,1};
		Vector2f val2{1,1};
		EXPECT_FLOAT_EQ(expected, Vector2f::Cross(val1,val2));

		val1 = {1,1};
		val2 = {3,3};
		EXPECT_FLOAT_EQ(expected, Vector2f::Cross(val1,val2));

		val1 = {1,0};
		val2 = {1,0};
		EXPECT_FLOAT_EQ(expected, Vector2f::Cross(val1,val2));

	}
	TEST_F(TestCrossProduct2f, OpositeDirection)
	{
		constexpr float expected{0};

		Vector2f val1{1,1};
		Vector2f val2{-1,-1};
		EXPECT_FLOAT_EQ(expected, Vector2f::Cross(val1,val2));

		val1 = {1,1};
		val2 = {-3,-3};
		EXPECT_FLOAT_EQ(expected, Vector2f::Cross(val1,val2));

		val1 = {1,0};
		val2 = {-1,0};
		EXPECT_FLOAT_EQ(expected, Vector2f::Cross(val1,val2));

	}

	class TestAngleBetween2f : public testing::Test{};

	TEST_F(TestAngleBetween2f, PositiveRightAngle)
	{
		constexpr float expectedDegrees{90};
		constexpr float expectedRadians{std::numbers::pi_v<float> / 2};

		Vector2f val1{1,0};
		Vector2f val2{0,1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));

		val1 = {0,1};
		val2 = {-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));

		val1 = {-1,0};
		val2 = {0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));

		val1 = {0,-1};
		val2 = {1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));
	}

	TEST_F(TestAngleBetween2f, StraightAngle)
	{
		constexpr float expectedDegrees{180};
		constexpr float expectedRadians{std::numbers::pi_v<float>};

		Vector2f val1{1,0};
		Vector2f val2{-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector2f::AngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector2f::AngleBetween(val1,val2,false)));

		val1 = {0,1};
		val2 = {0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector2f::AngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector2f::AngleBetween(val1,val2,false)));

		val1 = {-1,0};
		val2 = {1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector2f::AngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector2f::AngleBetween(val1,val2,false)));

		val1 = {0,-1};
		val2 = {0,1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector2f::AngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector2f::AngleBetween(val1,val2,false)));
	}
	TEST_F(TestAngleBetween2f, NegativeRightAngle)
	{
		constexpr float expectedDegrees{-90};
		constexpr float expectedRadians{-std::numbers::pi_v<float> / 2};

		Vector2f val1{0,1};
		Vector2f val2{1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));

		val1 = {-1,0};
		val2 = {0,1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));

		val1 = {0,-1};
		val2 = {-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));

		val1 = {1,0};
		val2 = {0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));
	}
	TEST_F(TestAngleBetween2f, FortyFiveAngle)
	{
		constexpr float expectedDegrees{45};
		constexpr float expectedRadians{std::numbers::pi_v<float> / 4};

		Vector2f val1{1,0};
		Vector2f val2{1,1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));

		val1 = {1,1};
		val2 = {0,1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));

		val1 = {0,1};
		val2 = {-1,1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));

		val1 = {-1,1};
		val2 = {-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));
	}
	TEST_F(TestAngleBetween2f, Zero)
	{
		constexpr float expectedDegrees{0};
		constexpr float expectedRadians{0};

		constexpr Vector2f val1{0,0};
		constexpr Vector2f val2{1,1};

		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val2,val1));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val2,val1,false));

		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val2,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val2,val2,false));
	}
	TEST_F(TestAngleBetween2f, SmallAngle)
	{
		constexpr float expectedDegrees{0.057295760414501f};
		constexpr float expectedRadians{expectedDegrees * std::numbers::pi_v<float> / 180};

		constexpr Vector2f val1{1,0};
		constexpr Vector2f val2{1000,1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector2f::AngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector2f::AngleBetween(val1,val2,false));

		EXPECT_FLOAT_EQ(-expectedDegrees, Vector2f::AngleBetween(val2,val1));
		EXPECT_FLOAT_EQ(-expectedRadians, Vector2f::AngleBetween(val2,val1,false));
	}

	class TestReflecting2f : public testing::Test{};

	TEST_F(TestReflecting2f, Orthogonal)
	{
		Vector2f normal{0,1};
		Vector2f incoming{1,0};
		Vector2f outgoing{Vector2f::Reflect(incoming, normal)};
		EXPECT_EQ(incoming, outgoing);

		incoming = {-1, 0};
		outgoing = Vector2f::Reflect(incoming, normal);
		EXPECT_EQ(incoming, outgoing);

		normal = {1,1};
		incoming = {1, -1};
		outgoing = Vector2f::Reflect(incoming, normal);
		EXPECT_EQ(incoming, outgoing);

		incoming = {-1, 1};
		outgoing = Vector2f::Reflect(incoming, normal);
		EXPECT_EQ(incoming, outgoing);
	}

	TEST_F(TestReflecting2f, Parallel)
	{
		Vector2f normal{0,1};
		Vector2f incoming{0,1};
		Vector2f outgoing{Vector2f::Reflect(incoming, normal)};
		EXPECT_EQ(-incoming, outgoing);

		incoming = {0, -1};
		outgoing = Vector2f::Reflect(incoming, normal);
		EXPECT_EQ(-incoming, outgoing);

		normal = {1,1};
		incoming = {1,1};
		outgoing = Vector2f::Reflect(incoming, normal);
		EXPECT_EQ(-incoming, outgoing);

		incoming = {-1, -1};
		outgoing = Vector2f::Reflect(incoming, normal);
		EXPECT_EQ(-incoming, outgoing);
	}
	TEST_F(TestReflecting2f, Slanted)
	{
		Vector2f normal{0,1};
		Vector2f incoming{1,-1};
		Vector2f outgoing{Vector2f::Reflect(incoming, normal)};
		EXPECT_FLOAT_EQ(1, outgoing.x);
		EXPECT_FLOAT_EQ(1, outgoing.y);

		normal = {-1,0};
		incoming = {1,1};
		outgoing = Vector2f::Reflect(incoming, normal);
		EXPECT_FLOAT_EQ(-1, outgoing.x);
		EXPECT_FLOAT_EQ(1, outgoing.y);
		incoming = {3,1};
		outgoing = Vector2f::Reflect(incoming, normal);
		EXPECT_FLOAT_EQ(-3, outgoing.x);
		EXPECT_FLOAT_EQ(1, outgoing.y);
	}

	class TestLength2f : public testing::Test{};

	TEST_F(TestLength2f, NormalLength)
	{
		constexpr Vector2f unitX{Vector2f::UnitX};
		EXPECT_FLOAT_EQ(1, unitX.Length());
		constexpr Vector2f unitY{Vector2f::UnitY};
		EXPECT_FLOAT_EQ(1, unitY.Length());

		constexpr Vector2f scaledUnitX{3,0};
		EXPECT_FLOAT_EQ(3, scaledUnitX.Length());
		constexpr Vector2f scaledUnitY{0,3};
		EXPECT_FLOAT_EQ(3, scaledUnitY.Length());

		constexpr Vector2f val1{3,4};
		EXPECT_FLOAT_EQ(5, val1.Length());
		constexpr Vector2f val2{-3,-4};
		EXPECT_FLOAT_EQ(5, val2.Length());
		constexpr Vector2f val3{3,-4};
		EXPECT_FLOAT_EQ(5, val3.Length());
		constexpr Vector2f val4{-3,4};
		EXPECT_FLOAT_EQ(5, val4.Length());
	}
	TEST_F(TestLength2f, SquaredLength)
	{
		constexpr Vector2f unitX{Vector2f::UnitX};
		EXPECT_FLOAT_EQ(1, unitX.SquaredLength());
		constexpr Vector2f unitY{Vector2f::UnitY};
		EXPECT_FLOAT_EQ(1, unitY.SquaredLength());

		constexpr Vector2f scaledUnitX{3,0};
		EXPECT_FLOAT_EQ(9, scaledUnitX.SquaredLength());
		constexpr Vector2f scaledUnitY{0,3};
		EXPECT_FLOAT_EQ(9, scaledUnitY.SquaredLength());

		constexpr Vector2f val1{3,4};
		EXPECT_FLOAT_EQ(25, val1.SquaredLength());
		constexpr Vector2f val2{-3,-4};
		EXPECT_FLOAT_EQ(25, val2.SquaredLength());
		constexpr Vector2f val3{3,-4};
		EXPECT_FLOAT_EQ(25, val3.SquaredLength());
		constexpr Vector2f val4{-3,4};
		EXPECT_FLOAT_EQ(25, val4.SquaredLength());
	}

	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestUnaryOperators2f,
		testing::ValuesIn(INPUT_MAP),
		[](const testing::TestParamInfo<TestUnaryOperators2f::ParamType>& info){return info.param.first;}
	);
	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestBinaryOperators2f,
		testing::ValuesIn(TestBinaryOperators2f::GetOperations()),
		[](const testing::TestParamInfo<TestBinaryOperators2f::ParamType>& info){return info.param.name;}
	);
	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestComparison2f,
		testing::ValuesIn(TestComparison2f::GetOperations()),
		[](const testing::TestParamInfo<TestComparison2f::ParamType>& info){return info.param.name;}
	);

	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestScalarOperations2f,
		testing::ValuesIn(TestScalarOperations2f::GetOperations()),
		[](const testing::TestParamInfo<TestScalarOperations2f::ParamType>& info){return info.param.name;}
	);


}
