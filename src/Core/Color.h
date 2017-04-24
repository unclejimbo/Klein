#pragma once

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QVector3D>
#include <QVector4D>
#include <vector>

#ifdef _WIN32
#pragma warning(pop)
#endif

class Color
{
public:
	static QVector3D nextColor();
	static QVector3D get(size_t i);

private:
	static size_t _current;
	static std::vector<QVector3D> _colors;
};


template<typename T>
inline std::vector<QVector3D>
temperature(const std::vector<T>& values, const QVector3D& hot, const QVector3D& cold)
{
	auto mm = std::minmax_element(values.begin(), values.end());
	auto maxDiff = *mm.second - *mm.first;
	std::vector<QVector3D> colors(values.size());
	if (maxDiff != 0) {
		std::transform(values.begin(), values.end(), colors.begin(),
			[hot, cold, mm, maxDiff](T value) {
				auto alpha = (value - *mm.first) / (maxDiff + 0.0f);
				return alpha * hot + (1.0f - alpha) * cold;
			}
		);
	}
	else {
		for (auto&& c : colors) {
			c = hot;
		}
	}
	return colors;
}

template<typename T>
inline std::vector<QVector4D>
temperature(const std::vector<T>& values, const QVector4D& hot, const QVector4D& cold)
{
	auto mm = std::minmax_element(values.begin(), values.end());
	auto maxDiff = *mm.second - *mm.first;
	std::vector<QVector4D> colors(values.size());
	if (maxDiff != 0) {
		std::transform(values.begin(), values.end(), colors.begin(),
			[hot, cold, mm, maxDiff](T value) {
				auto alpha = (value - *mm.first) / (maxDiff + 0.0f);
				return alpha * hot + (1.0f - alpha) * cold;
			}
		);
	}
	else {
		for (auto&& c : colors) {
			c = hot;
		}
	}
	return colors;
}
