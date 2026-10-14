#include <gtest/gtest.h>

#include <complex>

#include "Structs.h"

namespace jela::vec3f
{
	static constexpr inline float F_EPS = std::numeric_limits<float>::epsilon();
	static constexpr inline float F_LOW = std::numeric_limits<float>::lowest();
	static constexpr inline float F_MIN = std::numeric_limits<float>::min();
	static constexpr inline float F_MAX = std::numeric_limits<float>::max();

	static inline const std::map<std::string, Vector3f> INPUT_MAP
	{
		{"DefaultVector", Vector3f{}},
		{"OneOne", Vector3f{1,1,1}},
		{"Positives", Vector3f{34,67,546}},
		{"Negatives", Vector3f{-45,-78,-234}},
		{"OneNegative", Vector3f{12, -75, 65}},
		{"OnePositive", Vector3f{-83, 92, -262}},
		{"Max", Vector3f{F_MAX, F_MAX, F_MAX}},
		{"Min", Vector3f{F_LOW, F_LOW, F_LOW}},
	};

	static inline const std::string TEST_CONCATENATION{"_X_"};

	struct OperationInfo
	{
		std::string name{};
		Vector3f op1{};
		std::variant<Vector3f, float> op2{};
	};

	TEST(ConstructorTest3f, Constructors)
	{
		constexpr Vector3f vec{};
		EXPECT_FLOAT_EQ(vec.x, 0);
		EXPECT_FLOAT_EQ(vec.y, 0);
		EXPECT_FLOAT_EQ(vec.z, 0);
		constexpr Vector3f vec1{-34, 7, -54};
		EXPECT_FLOAT_EQ(vec1.x, -34);
		EXPECT_FLOAT_EQ(vec1.y, 7);
		EXPECT_FLOAT_EQ(vec1.z, -54);
		constexpr Vector3f vec2{1, -1, 3};
		EXPECT_FLOAT_EQ(vec2.x, 1);
		EXPECT_FLOAT_EQ(vec2.y, -1);
		EXPECT_FLOAT_EQ(vec2.z, 3);
		constexpr Vector3f vec3{Vector2f{3,-37}, -72};
		EXPECT_FLOAT_EQ(vec3.x, 3);
		EXPECT_FLOAT_EQ(vec3.y, -37);
		EXPECT_FLOAT_EQ(vec3.z, -72);
		constexpr Vector3f vec4{vec1, vec2};
		EXPECT_FLOAT_EQ(vec4.x, 35);
		EXPECT_FLOAT_EQ(vec4.y, -8);
		EXPECT_FLOAT_EQ(vec4.z, 57);
	}

	TEST(TestIndices3f, WrongIndices)
	{
		constexpr Vector3f vec{5,12, -34};

		EXPECT_DEBUG_DEATH(vec[-1], ".*");
		EXPECT_DEBUG_DEATH(vec[3], ".*");
	}
	TEST(TestIndices3f, RightIndices)
	{
		constexpr Vector3f vec{5,12, -34};

		EXPECT_FLOAT_EQ(vec[0], 5);
		EXPECT_FLOAT_EQ(vec[1], 12);
		EXPECT_FLOAT_EQ(vec[2], -34);
	}

	class TestUnaryOperators3f : public ::testing::TestWithParam<std::pair<const std::string, Vector3f>>
	{
	protected:
		void SetUp() override
		{
			const auto& [name, vector] = GetParam();
			vec = vector;
		}
		Vector3f vec{};
	};

	TEST_P(TestUnaryOperators3f, Plus)
	{
		const auto result = +vec; // tested operator

		EXPECT_FLOAT_EQ(result.x, +vec.x);
		EXPECT_FLOAT_EQ(result.y, +vec.y);
		EXPECT_FLOAT_EQ(result.z, +vec.z);
	}
	TEST_P(TestUnaryOperators3f, Minus)
	{
		const auto result = -vec; // tested operator

		EXPECT_FLOAT_EQ(result.x, -vec.x);
		EXPECT_FLOAT_EQ(result.y, -vec.y);
		EXPECT_FLOAT_EQ(result.z, -vec.z);
	}
	TEST_P(TestUnaryOperators3f, NormalizeVSNormalized)
	{
		const auto result = vec.Normalized(); // tested operator
		vec.Normalize();
		EXPECT_FLOAT_EQ(result.x, vec.x);
		EXPECT_FLOAT_EQ(result.y, vec.y);
		EXPECT_FLOAT_EQ(result.z, vec.z);
	}
	TEST_P(TestUnaryOperators3f, LengthOfNormalizedVector)
	{
		const auto result = vec.Normalized(); // tested operator

		if ((std::abs(vec.x) <= F_EPS && std::abs(vec.y) <= F_EPS && std::abs(vec.z) <= F_EPS) ||
			vec.x == F_MAX || vec.y == F_MAX || vec.z == F_MAX ||
			vec.x == F_LOW || vec.y == F_LOW || vec.z == F_LOW)
			EXPECT_FLOAT_EQ(0, result.Length());
		else EXPECT_FLOAT_EQ(1, result.Length());
	}
	TEST_P(TestUnaryOperators3f, DerictionOfNormalizedVector)
	{
		const auto result = vec.Normalized(); // tested operator
		EXPECT_LT(std::abs(result.AbsoluteAngleBetween(vec,result)), std::numeric_limits<float>::round_error());
	}


	class TestBinaryOperators3f : public ::testing::TestWithParam<OperationInfo>
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
		const inline static std::map<std::string, Vector3f> OPERANDS
		{
			{"DefaultVector", Vector3f{}},
			{"OneOne", Vector3f{1,1,1}},
			{"Positives", Vector3f{734,56,123}},
			{"Negatives", Vector3f{-412,-67,-67}},
			{"OneNegative", Vector3f{21, -16, 89}},
			{"OnePositive", Vector3f{-79, 53,-38}},
			{"Max", Vector3f{F_MAX, F_MAX, F_MAX}},
			{"Min", Vector3f{F_LOW, F_LOW, F_LOW}},
		};
	protected:
		void SetUp() override
		{
			const auto& p = GetParam();
			op1 = p.op1;
			op2 = std::get<Vector3f>(p.op2);
		}
		Vector3f op1{};
		Vector3f op2{};
	};
	TEST_P(TestBinaryOperators3f, Addition)
	{
		const auto result = op1 + op2; // tested operator

		EXPECT_FLOAT_EQ(result.x, op1.x + op2.x);
		EXPECT_FLOAT_EQ(result.y, op1.y + op2.y);
		EXPECT_FLOAT_EQ(result.z, op1.z + op2.z);
	}
	TEST_P(TestBinaryOperators3f, Subtraction)
	{
		const auto result = op1 - op2; // tested operator

		EXPECT_FLOAT_EQ(result.x, op1.x - op2.x);
		EXPECT_FLOAT_EQ(result.y, op1.y - op2.y);
		EXPECT_FLOAT_EQ(result.z, op1.z - op2.z);
	}
	TEST_P(TestBinaryOperators3f, AdditionAssignment)
	{
		auto result = op1;
		result += op2; // tested operator

		EXPECT_FLOAT_EQ(result.x, op1.x + op2.x);
		EXPECT_FLOAT_EQ(result.y, op1.y + op2.y);
		EXPECT_FLOAT_EQ(result.z, op1.z + op2.z);
	}
	TEST_P(TestBinaryOperators3f, SubtractionAssignment)
	{
		auto result = op1;
		result -= op2; // tested operator

		EXPECT_FLOAT_EQ(result.x, op1.x - op2.x);
		EXPECT_FLOAT_EQ(result.y, op1.y - op2.y);
		EXPECT_FLOAT_EQ(result.z, op1.z - op2.z);
	}
	TEST_P(TestBinaryOperators3f, Scale)
	{
		auto result1 = op1; result1.Scale(op2);

		EXPECT_FLOAT_EQ(result1.x, op1.x * op2.x);
		EXPECT_FLOAT_EQ(result1.y, op1.y * op2.y);
		EXPECT_FLOAT_EQ(result1.z, op1.z * op2.z);

		const auto result2 = Vector3f::Scale(op1, op2);

		EXPECT_FLOAT_EQ(result2.x, op1.x * op2.x);
		EXPECT_FLOAT_EQ(result2.y, op1.y * op2.y);
		EXPECT_FLOAT_EQ(result2.z, op1.z * op2.z);

		auto result3 = op1; result3.Scale(op2.x, op2.y, op2.z);

		EXPECT_FLOAT_EQ(result3.x, op1.x * op2.x);
		EXPECT_FLOAT_EQ(result3.y, op1.y * op2.y);
		EXPECT_FLOAT_EQ(result3.z, op1.z * op2.z);

		const auto result4 = Vector3f::Scale(op1, op2.x, op2.y, op2.z);

		EXPECT_FLOAT_EQ(result4.x, op1.x * op2.x);
		EXPECT_FLOAT_EQ(result4.y, op1.y * op2.y);
		EXPECT_FLOAT_EQ(result4.z, op1.z * op2.z);
	}

	TEST_P(TestBinaryOperators3f, DotCommunitativity)
	{
		const float result1 = Vector3f::Dot(op1,op2);
		const float result2 = Vector3f::Dot(op2,op1);

		if (std::isnan(result1) || std::isnan(result2))
			EXPECT_EQ(std::isnan(result1), std::isnan(result2));
		else EXPECT_FLOAT_EQ(result1, result2);
	}
	TEST_P(TestBinaryOperators3f, CrossAnticommunitativity)
	{
		const Vector3f result1 = Vector3f::Cross(op1,op2);
		const Vector3f result2 = Vector3f::Cross(op2,op1);

		if (std::isnan(result1.x) || std::isnan(result1.y) || std::isnan(result1.z)
			|| std::isnan(result2.x) || std::isnan(result2.y) || std::isnan(result2.z))
			return;

		EXPECT_EQ(result1, -result2);
	}

	class TestComparison3f : public TestBinaryOperators3f
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
	TEST_P(TestComparison3f, Comparison)
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
	TEST(TestLowComparison3f, LowComparison)
	{
		constexpr Vector3f zero{};
		constexpr Vector3f min{F_MIN, F_MIN, F_MIN};

		EXPECT_TRUE(zero == min);
		EXPECT_FALSE(zero != min);

		constexpr Vector3f epsilon{F_EPS, F_LOW, F_LOW};

		EXPECT_FALSE(zero == epsilon);
		EXPECT_TRUE(zero != epsilon);

	}

	class TestScalarOperations3f : public ::testing::TestWithParam<OperationInfo>
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
		Vector3f op1{};
		float op2{};
	};

	TEST_P(TestScalarOperations3f, Multiplication)
	{
		const auto result1 = op1 * op2; // tested operator

		EXPECT_FLOAT_EQ(result1.x, op1.x * op2);
		EXPECT_FLOAT_EQ(result1.y, op1.y * op2);
		EXPECT_FLOAT_EQ(result1.z, op1.z * op2);

		const auto result2 = op2 * op1; // tested operator

		EXPECT_FLOAT_EQ(result2.x, op1.x * op2);
		EXPECT_FLOAT_EQ(result2.y, op1.y * op2);
		EXPECT_FLOAT_EQ(result2.z, op1.z * op2);
	}
	TEST_P(TestScalarOperations3f, Division)
	{
		if (std::abs(op2) <= F_EPS)
		{
			EXPECT_DEBUG_DEATH(op1 / op2, ".*"); // tested operator
			return;
		}
		const auto result = op1 / op2; // tested operator
		EXPECT_FLOAT_EQ(result.x, op1.x / op2);
		EXPECT_FLOAT_EQ(result.y, op1.y / op2);
		EXPECT_FLOAT_EQ(result.z, op1.z / op2);

	}
	TEST_P(TestScalarOperations3f, MultiplicationAssignment)
	{
		auto result1 = op1;
		result1 *= op2; // tested operator

		EXPECT_FLOAT_EQ(result1.x, op1.x * op2);
		EXPECT_FLOAT_EQ(result1.y, op1.y * op2);
		EXPECT_FLOAT_EQ(result1.z, op1.z * op2);
	}
	TEST_P(TestScalarOperations3f, DivisionAssignment)
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
		EXPECT_FLOAT_EQ(result.z, op1.z / op2);
	}

	class TestDotProduct3f : public testing::Test{};

	TEST_F(TestDotProduct3f, Positives)
	{
		constexpr Vector3f val1{4,7,2};
		constexpr Vector3f val2{5,6,3};
		constexpr float expected{68};

		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));
	}

	TEST_F(TestDotProduct3f, Negatives)
	{
		constexpr Vector3f val1{-12,-7,-3};
		constexpr Vector3f val2{-5,-2,-6};
		constexpr float expected{92};

		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));
	}
	TEST_F(TestDotProduct3f, MixedSigns)
	{
		constexpr Vector3f val1{-6,7,-6};
		constexpr Vector3f val2{-5,-2,3};
		constexpr float expected{-2};

		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));
	}
	TEST_F(TestDotProduct3f, Zero)
	{
		constexpr float expected{0};

		constexpr Vector3f val1{0,0,0};
		constexpr Vector3f val2{23,45,3};
		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));
	}
	TEST_F(TestDotProduct3f, Orthogonality)
	{
		constexpr float expected{0};

		Vector3f
		val1 = Vector3f::UnitX, val2 = Vector3f::UnitY;
		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));

		val1 = Vector3f::UnitY; val2 = -Vector3f::UnitX;
		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));

		val1 = -Vector3f::UnitX; val2 = -Vector3f::UnitY;
		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));

		val1 = -Vector3f::UnitY; val2 = Vector3f::UnitX;
		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));


		val1 = Vector3f::UnitX; val2 = Vector3f::UnitZ;
		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));

		val1 = Vector3f::UnitZ; val2 = -Vector3f::UnitX;
		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));

		val1 = -Vector3f::UnitX; val2 = -Vector3f::UnitZ;
		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));

		val1 = -Vector3f::UnitZ; val2 = Vector3f::UnitX;
		EXPECT_FLOAT_EQ(expected, Vector3f::Dot(val1,val2));
	}

	class TestCrossProduct3f : public testing::Test{};

	TEST_F(TestCrossProduct3f, Positives)
	{
		constexpr Vector3f val1{4,7,3};
		constexpr Vector3f val2{5,6,5};
		constexpr Vector3f expected{17, -5, -11};
		EXPECT_EQ(expected, Vector3f::Cross(val1,val2));
		EXPECT_EQ(Vector3f::UnitZ, Vector3f::Cross(Vector3f::UnitX,Vector3f::UnitY));
	}

	TEST_F(TestCrossProduct3f, Negatives)
	{
		constexpr Vector3f val1{-12,-7,-3};
		constexpr Vector3f val2{-5,-3,-20};
		constexpr Vector3f expected{131,-225,1};

		EXPECT_EQ(expected, Vector3f::Cross(val1,val2));
	}
	TEST_F(TestCrossProduct3f, MixedSigns)
	{
		constexpr Vector3f val1{-6,7, -1};
		constexpr Vector3f val2{-5,-2, 23};
		constexpr Vector3f expected1{159,143,47};

		EXPECT_EQ(expected1, Vector3f::Cross(val1,val2));

		constexpr Vector3f val3{6,7, -1};
		constexpr Vector3f val4{5,-2, 23};
		constexpr Vector3f expected2{159,-143,-47};

		EXPECT_EQ(expected2, Vector3f::Cross(val3,val4));

		constexpr Vector3f val5{-6,7, -1};
		constexpr Vector3f val6{5,-2, -23};
		constexpr Vector3f expected3{-163,-143,-23};

		EXPECT_EQ(expected3, Vector3f::Cross(val5,val6));
	}
	TEST_F(TestCrossProduct3f, Zero)
	{
		constexpr Vector3f expected{0,0,0};

		constexpr Vector3f val1{0,0,0};
		constexpr Vector3f val2{23,45,6};
		EXPECT_EQ(expected, Vector3f::Cross(val1,val2));
	}
	TEST_F(TestCrossProduct3f, SameDirection)
	{
		constexpr Vector3f expected{0,0,0};

		Vector3f
		val1 = {1,1,1}, val2 = {1,1,1};
		EXPECT_EQ(expected, Vector3f::Cross(val1,val2));

		val1 = {1,1,1}; val2 = {3,3,3};
		EXPECT_EQ(expected, Vector3f::Cross(val1,val2));

		val1 = {1,0,1}; val2 = {1,0,1};
		EXPECT_EQ(expected, Vector3f::Cross(val1,val2));

	}
	TEST_F(TestCrossProduct3f, OpositeDirection)
	{
		constexpr Vector3f expected{0,0,0};

		Vector3f
		val1 = {1,1,1}, val2 = {-1,-1,-1};
		EXPECT_EQ(expected, Vector3f::Cross(val1,val2));

		val1 = {1,1,1}; val2 = {-3,-3,-3};
		EXPECT_EQ(expected, Vector3f::Cross(val1,val2));

		val1 = {1,0,1}; val2 = {-1,0,-1};
		EXPECT_EQ(expected, Vector3f::Cross(val1,val2));
	}

	class TestAbsoluteAngleBetween3f : public testing::Test{};

	TEST_F(TestAbsoluteAngleBetween3f, RightAngle)
	{
		constexpr float expectedDegrees{90};
		constexpr float expectedRadians{std::numbers::pi_v<float> / 2};

		Vector3f
		val1 = {0,0,1}, val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {0,1,0}; val2 = {0,0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {0,0,-1}; val2 = {0,-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {0,-1,0}; val2 = {0,0,1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));
	}

	TEST_F(TestAbsoluteAngleBetween3f, StraightAngle)
	{
		constexpr float expectedDegrees{180};
		constexpr float expectedRadians{std::numbers::pi_v<float>};

		Vector3f
		val1 = {1,0,0}, val2 = {-1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {0,1,0}; val2 = {0,-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {-1,0,0}; val2 = {1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {0,-1,0}; val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {0,-1,-1}; val2 = {0,1,1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {0,0,1}; val2 = {0,0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {1,-1,0}; val2 = {-1,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));
	}
	TEST_F(TestAbsoluteAngleBetween3f, FortyFiveAngle)
	{
		constexpr float expectedDegrees{45};
		constexpr float expectedRadians{std::numbers::pi_v<float> / 4};

		Vector3f
		val1 ={0,0,1}, val2 = {0,1,1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {0,1,1}; val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {0,1,-1}; val2 = {0,0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));

		val1 = {-1,0,-1}; val2 = {-1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false)));
	}
	TEST_F(TestAbsoluteAngleBetween3f, ZeroAngle)
	{
		constexpr float expectedDegrees{0};
		constexpr float expectedRadians{0};

		constexpr Vector3f val1{0,0,0};
		constexpr Vector3f val2{1,1,1};

		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AbsoluteAngleBetween(val1,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AbsoluteAngleBetween(val1,val2,false));
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AbsoluteAngleBetween(val2,val1));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AbsoluteAngleBetween(val2,val1,false));

		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AbsoluteAngleBetween(val2,val2));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AbsoluteAngleBetween(val2,val2,false));
	}
	TEST_F(TestAbsoluteAngleBetween3f, SmallAngle)
	{
		constexpr float expectedDegrees{0.057295760414501f};
		constexpr float expectedRadians{expectedDegrees * std::numbers::pi_v<float> / 180};

		constexpr Vector3f val1{0,0,1};
		constexpr Vector3f val2{0,1,1000};
		EXPECT_LT(std::abs(expectedDegrees - std::abs(Vector3f::AbsoluteAngleBetween(val1,val2))), 0.01f);
		EXPECT_LT(std::abs(expectedRadians - std::abs(Vector3f::AbsoluteAngleBetween(val1,val2,false))), 0.01f);
	}

	class TestAngleBetween3f : public ::testing::Test {};

	TEST_F(TestAngleBetween3f, PositiveRightAngleScreenPlane)
	{
		constexpr Vector3f planeNormal{0,0,1};
		constexpr float expectedDegrees{90};
		constexpr float expectedRadians{std::numbers::pi_v<float> / 2};

		Vector3f
		val1 = {1,0,0}, val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,1,0}; val2 = {-1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {-1,0,0}; val2 = {0,-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,-1,0}; val2 = {1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = planeNormal; val2 = {1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));
	}

	TEST_F(TestAngleBetween3f, StraightAngleScreenPlane)
	{
		constexpr Vector3f planeNormal{0,0,1};
		constexpr float expectedDegrees{180};
		constexpr float expectedRadians{std::numbers::pi_v<float>};

		Vector3f
		val1 = {1,0,0}, val2 = {-1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = {0,1,0}; val2 = {0,-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = {-1,0,0}; val2 = {1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = {0,-1,0}; val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));


		val1 = {0,-1,-1}; val2 = {0,1,1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = {0,0,1}; val2 = {0,0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = {1,-1,0}; val2 = {-1,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = planeNormal; val2 = -planeNormal;
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));
	}
	TEST_F(TestAngleBetween3f, NegativeRightAngleScreenPlane)
	{
		constexpr Vector3f planeNormal{0,0,1};
		constexpr float expectedDegrees{-90};
		constexpr float expectedRadians{-std::numbers::pi_v<float> / 2};

		Vector3f
		val1 = {0,1,0}, val2 = {1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {-1,0,0}; val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,-1,0}; val2 = {-1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {1,0,0}; val2 = {0,-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

	}
	TEST_F(TestAngleBetween3f, FortyFiveAngleScreenPlane)
	{
		constexpr Vector3f planeNormal{0,0,1};
		constexpr float expectedDegrees{45};
		constexpr float expectedRadians{std::numbers::pi_v<float> / 4};

		Vector3f
		val1 = {0,0,1}, val2 = {0,1,1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,1,1}; val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,1,-1}; val2 = {0,0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {-1,0,-1}; val2 = {-1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));
	}
	TEST_F(TestAngleBetween3f, PositiveRightAngle)
	{
		constexpr Vector3f planeNormal{1,0,0};
		constexpr float expectedDegrees{90};
		constexpr float expectedRadians{std::numbers::pi_v<float> / 2};

		Vector3f
		val1 = {0,0,-1}, val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,1,0}; val2 = {0,0,1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,0,1}; val2 = {0,-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,-1,0}; val2 = {0,0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = planeNormal; val2 = {0,0,1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));
	}

	TEST_F(TestAngleBetween3f, StraightAngle)
	{
		constexpr Vector3f planeNormal{1,0,0};
		constexpr float expectedDegrees{180};
		constexpr float expectedRadians{std::numbers::pi_v<float>};

		Vector3f
		val1 = {0,0,1}, val2 = {0,0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = {0,1,0}; val2 = {0,-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = {0,0,-1}; val2 = {0,0,1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = {0,-1,0}; val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));


		val1 = {0,-1,-1}; val2 = {0,1,1};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = {1,0,0}; val2 = {-1,0,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = {1,-1,0}; val2 = {-1,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));

		val1 = planeNormal; val2 = -planeNormal;
		EXPECT_FLOAT_EQ(expectedDegrees, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal)));
		EXPECT_FLOAT_EQ(expectedRadians, std::abs(Vector3f::AngleBetween(val1,val2, planeNormal,false)));
	}
	TEST_F(TestAngleBetween3f, NegativeRightAngle)
	{
		constexpr Vector3f planeNormal{1,0,0};
		constexpr float expectedDegrees{-90};
		constexpr float expectedRadians{-std::numbers::pi_v<float> / 2};

		Vector3f
		val1 = {0,1,0}, val2 = {0,0,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,0,1}; val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,-1,0}; val2 = {0,0,1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,0,-1}; val2 = {0,-1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

	}
	TEST_F(TestAngleBetween3f, FortyFiveAngle)
	{
		constexpr Vector3f planeNormal{1,0,0};
		constexpr float expectedDegrees{45};
		constexpr float expectedRadians{std::numbers::pi_v<float> / 4};

		Vector3f
		val1 = {0,0,-1}, val2 = {0,1,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,1,-1}; val2 = {0,1,0};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,0,-1}; val2 = {0,1,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));

		val1 = {0,-1,0}; val2 = {0,-1,-1};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));
	}
	TEST_F(TestAngleBetween3f, ZeroAngle)
	{
		constexpr float expectedDegrees{0};
		constexpr float expectedRadians{0};

		constexpr Vector3f planeNormal{0,0,1};
		constexpr Vector3f val1{0,0,0};
		constexpr Vector3f val2{1,1,1};

		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal,false));
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val2,val1, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val2,val1, planeNormal,false));

		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val2,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val2,val2, planeNormal,false));
	}
	TEST_F(TestAngleBetween3f, SmallAngle)
	{
		constexpr float expectedDegrees{0.057295760414501f};
		constexpr float expectedRadians{expectedDegrees * std::numbers::pi_v<float> / 180};

		constexpr Vector3f planeNormal{0,0,1};
		constexpr Vector3f val1{0,0,1};
		constexpr Vector3f val2{0,1,1000};
		EXPECT_FLOAT_EQ(expectedDegrees, Vector3f::AngleBetween(val1,val2, planeNormal));
		EXPECT_FLOAT_EQ(expectedRadians, Vector3f::AngleBetween(val1,val2, planeNormal, false));
	}
	class TestReflecting3f : public testing::Test{};

	TEST_F(TestReflecting3f, Orthogonal)
	{
		Vector3f normal{0,1,0};
		Vector3f incoming{1,0,0};
		Vector3f outgoing{Vector3f::Reflect(incoming, normal)};
		EXPECT_EQ(incoming, outgoing);

		incoming = {-1, 0, 0};
		outgoing = Vector3f::Reflect(incoming, normal);
		EXPECT_EQ(incoming, outgoing);

		normal = {0,1,1};
		incoming = {0, 1, -1};
		outgoing = Vector3f::Reflect(incoming, normal);
		EXPECT_EQ(incoming, outgoing);

		incoming = {0, 1,-1};
		outgoing = Vector3f::Reflect(incoming, normal);
		EXPECT_EQ(incoming, outgoing);
	}

	TEST_F(TestReflecting3f, Parallel)
	{
		Vector3f normal{0,1,0};
		Vector3f incoming{0,1,0};
		Vector3f outgoing{Vector3f::Reflect(incoming, normal)};
		EXPECT_EQ(-incoming, outgoing);
		incoming = {0, -1,0};
		outgoing = Vector3f::Reflect(incoming, normal);
		EXPECT_EQ(-incoming, outgoing);

		normal = {1,1,1};
		incoming = {1,1,1};
		outgoing = Vector3f::Reflect(incoming, normal);
		EXPECT_EQ(-incoming, outgoing);

		incoming = {-1, -1, -1};
		outgoing = Vector3f::Reflect(incoming, normal);
		EXPECT_EQ(-incoming, outgoing);
	}
	TEST_F(TestReflecting3f, Slanted)
	{
		Vector3f normal{0,1,0};
		Vector3f incoming{0,-1,1};
		Vector3f outgoing{Vector3f::Reflect(incoming, normal)};
		EXPECT_FLOAT_EQ(0, outgoing.x);
		EXPECT_FLOAT_EQ(1, outgoing.y);
		EXPECT_FLOAT_EQ(1, outgoing.z);

		normal = {0,0,-1};
		incoming = {0,1,1};
		outgoing = Vector3f::Reflect(incoming, normal);
		EXPECT_FLOAT_EQ(0, outgoing.x);
		EXPECT_FLOAT_EQ(1, outgoing.y);
		EXPECT_FLOAT_EQ(-1, outgoing.z);

		incoming = {0,1,3};
		outgoing = Vector3f::Reflect(incoming, normal);
		EXPECT_FLOAT_EQ(0, outgoing.x);
		EXPECT_FLOAT_EQ(1, outgoing.y);
		EXPECT_FLOAT_EQ(-3, outgoing.z);
	}

	class TestLength3f : public testing::Test{};

	TEST_F(TestLength3f, NormalLength)
	{
		constexpr Vector3f unitX{Vector3f::UnitX};
		EXPECT_FLOAT_EQ(1, unitX.Length());
		constexpr Vector3f unitY{Vector3f::UnitY};
		EXPECT_FLOAT_EQ(1, unitY.Length());
		constexpr Vector3f unitZ{Vector3f::UnitZ};
		EXPECT_FLOAT_EQ(1, unitZ.Length());

		constexpr Vector3f scaledUnitX{3,0,0};
		EXPECT_FLOAT_EQ(3, scaledUnitX.Length());
		constexpr Vector3f scaledUnitY{0,3,0};
		EXPECT_FLOAT_EQ(3, scaledUnitY.Length());
		constexpr Vector3f scaledUnitZ{0,0,3};
		EXPECT_FLOAT_EQ(3, scaledUnitZ.Length());

		constexpr Vector3f val1{3,4,0};
		EXPECT_FLOAT_EQ(5, val1.Length());
		constexpr Vector3f val2{-3,-4,0};
		EXPECT_FLOAT_EQ(5, val2.Length());
		constexpr Vector3f val3{3,-4,0};
		EXPECT_FLOAT_EQ(5, val3.Length());
		constexpr Vector3f val4{-3,4,0};
		EXPECT_FLOAT_EQ(5, val4.Length());

		constexpr Vector3f val5{0,3,4};
		EXPECT_FLOAT_EQ(5, val5.Length());
		constexpr Vector3f val6{0,-3,-4};
		EXPECT_FLOAT_EQ(5, val6.Length());
		constexpr Vector3f val7{0,3,-4};
		EXPECT_FLOAT_EQ(5, val7.Length());
		constexpr Vector3f val8{0,-3,4};
		EXPECT_FLOAT_EQ(5, val8.Length());
	}
	TEST_F(TestLength3f, SquaredLength)
	{
		constexpr Vector3f unitX{Vector3f::UnitX};
		EXPECT_FLOAT_EQ(1, unitX.SquaredLength());
		constexpr Vector3f unitY{Vector3f::UnitY};
		EXPECT_FLOAT_EQ(1, unitY.SquaredLength());
		constexpr Vector3f unitZ{Vector3f::UnitZ};
		EXPECT_FLOAT_EQ(1, unitZ.SquaredLength());

		constexpr Vector3f scaledUnitX{3,0,0};
		EXPECT_FLOAT_EQ(9, scaledUnitX.SquaredLength());
		constexpr Vector3f scaledUnitY{0,3,0};
		EXPECT_FLOAT_EQ(9, scaledUnitY.SquaredLength());
		constexpr Vector3f scaledUnitZ{0,0,3};
		EXPECT_FLOAT_EQ(9, scaledUnitZ.SquaredLength());

		constexpr Vector3f val1{3,4,0};
		EXPECT_FLOAT_EQ(25, val1.SquaredLength());
		constexpr Vector3f val2{-3,-4,0};
		EXPECT_FLOAT_EQ(25, val2.SquaredLength());
		constexpr Vector3f val3{3,-4,0};
		EXPECT_FLOAT_EQ(25, val3.SquaredLength());
		constexpr Vector3f val4{-3,4,0};
		EXPECT_FLOAT_EQ(25, val4.SquaredLength());

		constexpr Vector3f val5{0,3,4};
		EXPECT_FLOAT_EQ(25, val5.SquaredLength());
		constexpr Vector3f val6{0,-3,-4};
		EXPECT_FLOAT_EQ(25, val6.SquaredLength());
		constexpr Vector3f val7{0,3,-4};
		EXPECT_FLOAT_EQ(25, val7.SquaredLength());
		constexpr Vector3f val8{0,-3,4};
		EXPECT_FLOAT_EQ(25, val8.SquaredLength());
	}

	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestUnaryOperators3f,
		testing::ValuesIn(INPUT_MAP),
		[](const testing::TestParamInfo<TestUnaryOperators3f::ParamType>& info){return info.param.first;}
	);
	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestBinaryOperators3f,
		testing::ValuesIn(TestBinaryOperators3f::GetOperations()),
		[](const testing::TestParamInfo<TestBinaryOperators3f::ParamType>& info){return info.param.name;}
	);
	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestComparison3f,
		testing::ValuesIn(TestComparison3f::GetOperations()),
		[](const testing::TestParamInfo<TestComparison3f::ParamType>& info){return info.param.name;}
	);

	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestScalarOperations3f,
		testing::ValuesIn(TestScalarOperations3f::GetOperations()),
		[](const testing::TestParamInfo<TestScalarOperations3f::ParamType>& info){return info.param.name;}
	);


}
