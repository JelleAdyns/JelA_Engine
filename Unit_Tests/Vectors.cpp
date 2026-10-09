#include <gtest/gtest.h>

#include "Structs.h"

namespace jela
{

	static inline const std::map<std::string, Vector2f> INPUT_MAP
	{
		{"DefaultVector", Vector2f{}},
		{"OneOne", Vector2f{1,1}},
		{"Positives", Vector2f{34,67}},
		{"Negatives", Vector2f{-45,-78}},
		{"PositiveNegative", Vector2f{12, -75}},
		{"NegativePositive", Vector2f{-83, 92}},
		{"Max", Vector2f{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()}},
		{"Min", Vector2f{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()}},
	};

	static inline const std::string TEST_CONCATENATION{"_X_"};

	struct OperationInfo
	{
		std::string name{};
		Vector2f op1{};
		std::variant<Vector2f, float> op2{};
	};


	class TestBinaryOperators : public ::testing::TestWithParam<OperationInfo>
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
			{"Max", Vector2f{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()}},
			{"Min", Vector2f{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()}},
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
	class TestComparison : public TestBinaryOperators
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
	};
	class TestUnaryOperators : public ::testing::TestWithParam<std::pair<const std::string, Vector2f>>
	{
	protected:
		void SetUp() override
		{
			const auto& [name, vector] = GetParam();
			vec = vector;
		}
		Vector2f vec{};
	};
	class TestScalarOperations : public ::testing::TestWithParam<OperationInfo>
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
			{"Max", std::numeric_limits<float>::max()},
			{"Min", std::numeric_limits<float>::lowest()},
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

	TEST(ConstructorTest, Constructors)
	{
		constexpr Vector2f vec{};
		EXPECT_EQ(vec.x, 0);
		EXPECT_EQ(vec.y, 0);
		constexpr Vector2f vec1{-34, 7};
		EXPECT_EQ(vec1.x, -34);
		EXPECT_EQ(vec1.y, 7);
		constexpr Vector2f vec2{1, -1};
		EXPECT_EQ(vec2.x, 1);
		EXPECT_EQ(vec2.y, -1);
		constexpr Vector2f vec3{vec1, vec2};
		EXPECT_EQ(vec3.x, 35);
		EXPECT_EQ(vec3.y, -8);
	}

	TEST(TestIndices, WrongIndices)
	{
		constexpr Vector2f vec{5,12};
#ifndef NDEBUG
		EXPECT_DEBUG_DEATH(vec[-1], ".*");
		EXPECT_DEBUG_DEATH(vec[2], ".*");
#endif
	}
	TEST(TestIndices, RightIndices)
	{
		constexpr Vector2f vec{5,12};

		EXPECT_EQ(vec[0], 5);
		EXPECT_EQ(vec[1], 12);
	}
	TEST_P(TestUnaryOperators, Plus)
	{
		const auto result = +vec; // tested operator

		EXPECT_EQ(result.x, +vec.x);
		EXPECT_EQ(result.y, +vec.y);
	}
	TEST_P(TestUnaryOperators, Minus)
	{
		const auto result = -vec; // tested operator

		EXPECT_EQ(result.x, -vec.x);
		EXPECT_EQ(result.y, -vec.y);
	}

	TEST_P(TestBinaryOperators, Addition)
	{
		const auto result = op1 + op2; // tested operator

		EXPECT_EQ(result.x, op1.x + op2.x);
		EXPECT_EQ(result.y, op1.y + op2.y);
	}
	TEST_P(TestBinaryOperators, Subtraction)
	{
		const auto result = op1 - op2; // tested operator

		EXPECT_EQ(result.x, op1.x - op2.x);
		EXPECT_EQ(result.y, op1.y - op2.y);
	}
	TEST_P(TestBinaryOperators, AdditionAssignment)
	{
		auto result = op1;
		result += op2; // tested operator

		EXPECT_EQ(result.x, op1.x + op2.x);
		EXPECT_EQ(result.y, op1.y + op2.y);
	}
	TEST_P(TestBinaryOperators, SubtractionAssignment)
	{
		auto result = op1;
		result -= op2; // tested operator

		EXPECT_EQ(result.x, op1.x - op2.x);
		EXPECT_EQ(result.y, op1.y - op2.y);
	}
	TEST_P(TestBinaryOperators, Scale)
	{
		auto result1 = op1; result1.Scale(op2);

		EXPECT_EQ(result1.x, op1.x * op2.x);
		EXPECT_EQ(result1.y, op1.y * op2.y);

		const auto result2 = Vector2f::Scale(op1, op2);

		EXPECT_EQ(result2.x, op1.x * op2.x);
		EXPECT_EQ(result2.y, op1.y * op2.y);

		auto result3 = op1; result3.Scale(op2.x, op2.y);

		EXPECT_EQ(result3.x, op1.x * op2.x);
		EXPECT_EQ(result3.y, op1.y * op2.y);

		const auto result4 = Vector2f::Scale(op1, op2.x, op2.y);

		EXPECT_EQ(result4.x, op1.x * op2.x);
		EXPECT_EQ(result4.y, op1.y * op2.y);
	}
	TEST_P(TestComparison, Comparison)
	{
		const auto& name = GetParam().name;

		const std::regex r(std::format("(.*){}(.*)",TEST_CONCATENATION));
		std::smatch matches{};
		std::regex_search(name.cbegin(),name.cend(),matches,r);
		const auto firstName = matches.str(1);
		const auto secondName = matches.str(2);

		if (firstName == secondName)
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
	TEST(TestLowComparison, LowComparison)
	{
		constexpr Vector2f zero{};
		constexpr Vector2f min{std::numeric_limits<float>::min(), std::numeric_limits<float>::min()};

		EXPECT_TRUE(zero == min);
		EXPECT_FALSE(zero != min);

		constexpr Vector2f epsilon{std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::lowest()};

		EXPECT_FALSE(zero == epsilon);
		EXPECT_TRUE(zero != epsilon);

	}
	TEST_P(TestScalarOperations, Multiplication)
	{
		const auto result1 = op1 * op2; // tested operator

		EXPECT_EQ(result1.x, op1.x * op2);
		EXPECT_EQ(result1.y, op1.y * op2);

		const auto result2 = op2 * op1; // tested operator

		EXPECT_EQ(result2.x, op1.x * op2);
		EXPECT_EQ(result2.y, op1.y * op2);
	}
	TEST_P(TestScalarOperations, Division)
	{
		if (std::abs(op2) <= std::numeric_limits<float>::epsilon())
		{
#ifndef NDEBUG
			EXPECT_DEBUG_DEATH(op1 / op2, ".*"); // tested operator
#endif
			return;
		}
		const auto result = op1 / op2; // tested operator
		EXPECT_EQ(result.x, op1.x / op2);
		EXPECT_EQ(result.y, op1.y / op2);

	}
	TEST_P(TestScalarOperations, MultiplicationAssignment)
	{
		auto result1 = op1;
		result1 *= op2; // tested operator

		EXPECT_EQ(result1.x, op1.x * op2);
		EXPECT_EQ(result1.y, op1.y * op2);
	}
	TEST_P(TestScalarOperations, DivisionAssignment)
	{
		auto result = op1;

		if (std::abs(op2) <= std::numeric_limits<float>::epsilon())
		{
#ifndef NDEBUG
			EXPECT_DEBUG_DEATH(result /= op2, ".*"); // tested operator
#endif
			return;
		}

		result /= op2; // tested operator
		EXPECT_EQ(result.x, op1.x / op2);
		EXPECT_EQ(result.y, op1.y / op2);
	}

	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestUnaryOperators,
		testing::ValuesIn(INPUT_MAP),
		[](const testing::TestParamInfo<TestUnaryOperators::ParamType>& info){return info.param.first;}
	);
	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestBinaryOperators,
		testing::ValuesIn(TestBinaryOperators::GetOperations()),
		[](const testing::TestParamInfo<TestBinaryOperators::ParamType>& info){return info.param.name;}
	);
	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestComparison,
		testing::ValuesIn(TestComparison::GetOperations()),
		[](const testing::TestParamInfo<TestComparison::ParamType>& info){return info.param.name;}
	);

	INSTANTIATE_TEST_SUITE_P
	(
		Operators,
		TestScalarOperations,
		testing::ValuesIn(TestScalarOperations::GetOperations()),
		[](const testing::TestParamInfo<TestScalarOperations::ParamType>& info){return info.param.name;}
	);


}
