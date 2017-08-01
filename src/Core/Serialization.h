#pragma once

#include <cereal/details/traits.hpp>
#include <QString>

namespace cereal
{

template<typename Archive, typename Scalar, int Rows,
	int Cols, int Opts, int MaxRows, int MaxCols,
	cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value> =
	cereal::traits::sfinae>
inline void save(Archive& arch,
	const Eigen::Matrix<Scalar, Rows, Cols, Opts, MaxRows, MaxCols>& matrix)
{
	int rows = matrix.rows();
	arch(cereal::make_nvp("num_rows", rows));
	int cols = matrix.cols();
	arch(cereal::make_nvp("num_cols", cols));
	for (auto c = 0; c < cols; ++c) {
		for (auto r = 0; r < rows; ++r) {
			auto name = QString("value(%1, %2)").arg(r).arg(c);
			Scalar value = matrix(r, c);
			arch(cereal::make_nvp(name.toStdString().c_str(), value));
		}
	}
}

template<typename Archive, typename Scalar, int Rows,
	int Cols, int Opts, int MaxRows, int MaxCols,
	cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value> =
	cereal::traits::sfinae>
inline void load(Archive& arch,
	Eigen::Matrix<Scalar, Rows, Cols, Opts, MaxRows, MaxCols>& matrix)
{
	int rows, cols;
	arch(rows);
	arch(cols);
	matrix.resize(rows, cols);
	for (auto c = 0; c < cols; ++c) {
		for (auto r = 0; r < rows; ++r) {
			arch(matrix(r, c));
		}
	}
}

template<typename Archive, typename Scalar, int Rows,
	int Cols, int Opts, int MaxRows, int MaxCols,
	cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value> =
	cereal::traits::sfinae>
inline void save(Archive& arch,
	const Eigen::Matrix<Scalar, Rows, Cols, Opts, MaxRows, MaxCols>& matrix)
{
	int rows = matrix.rows();
	arch(rows);
	int cols = matrix.cols();
	arch(cols);
	arch(binary_data(matrix.data(), static_cast<size_t>(rows * cols * sizeof(Scalar))));
}

template<typename Archive, typename Scalar, int Rows,
	int Cols, int Opts, int MaxRows, int MaxCols,
	cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value> =
	cereal::traits::sfinae>
inline void load(Archive& arch,
	Eigen::Matrix<Scalar, Rows, Cols, Opts, MaxRows, MaxCols>& matrix)
{
	int rows, cols;
	arch(rows);
	arch(cols);
	matrix.resize(rows, cols);
	arch(binary_data(matrix.data(), static_cast<size_t>(rows * cols * sizeof(Scalar))));
}

} // namespace cereal
