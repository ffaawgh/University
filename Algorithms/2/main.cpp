#include <iostream>
#include <iomanip>

#include "math_functions.hpp"
#include "Interpolator.h"
#include "Utils.hpp"

class InputValues {
	public:
		InputValues()
			: InputValues(std::cin, std::cout) {}

		InputValues(std::istream& in, std::ostream& out) {
			out << "Enter left  border: ";
			in >> x_begin;
			out << "Enter right border: ";
			in >> x_end;
			out << "Enter step: ";
			in >> x_step;
			out << "Enter x: ";
			in >> x;
			out << "Enter polynomial degree: ";
			in >> degree;

			out << std::endl;
		}

		InputValues(int argc, const char* argv[]) {
			// todo:
		}

		double get_x_begin() const {
			return x_begin;
		}

		double get_x_end() const {
			return x_end;
		}

		double get_x_step() const {
			return x_step;
		}

		double get_x() const {
			return x;
		}

		unsigned int get_degree() const {
			return degree;
		}

	private:
		double x_begin;
		double x_end;
		double x_step;
		double x;
		unsigned int degree;
};

void PrintTable(std::ostream& out, const Utils::PointsTable& table) {
	for (auto& point: table) {
		out << std::setw(5) << point.x << ": " << point.y << std::endl;
	}
}

void RunTest(const InputValues& input_values, const std::function<double(double)>& func, std::ostream& out) {
	auto table = Utils::GenerateTable(Functions::F1,
	                                  input_values.get_x_begin(),
	                                  input_values.get_x_end(),
	                                  input_values.get_x_step());
	out << "Points table:" << std::endl;
	PrintTable(std::cout, table);
	out << std::endl;

	Interpolator interpolator(table.begin(), table.end());

	auto interpolated_value = interpolator.Calc(input_values.get_x(), input_values.get_degree());
	auto real_value = func(input_values.get_x());

	out << "Interpolated value: " << interpolated_value << std::endl;
	out << "Real value        : " << real_value << std::endl;
	out << "Difference        : " << std::abs(interpolated_value - real_value) << std::endl;
}

int main() {
	InputValues input_values;
	RunTest(input_values, Functions::F1, std::cout);

	return 0;
}
