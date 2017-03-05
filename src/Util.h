#pragma once

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QVector3D>
#include <QImage>
#include <Eigen/Dense>
#include <algorithm>
#include <vector>

#ifdef _WIN32
#pragma warning(pop)
#endif

/*****************************
 Vector and Matrix Conversion
*****************************/
template<typename FT>
inline Eigen::Matrix<FT, 3, 1> 
qtToEigen(const QVector3D& vec)
{
	return Eigen::Matrix<FT, 3, 1>(vec.x(), vec.y(), vec.z());
}

template<typename Kernel>
inline typename Kernel::Point_3
qtToCgal(const QVector3D& vec)
{
	return typename Kernel::Point_3(vec.x(), vec.y(), vec.z());
}

template<typename FT>
inline QVector3D 
eigenToQt(const Eigen::Matrix<FT, 3, 1>& vec)
{
	return QVector3D(vec(0), vec(1), vec(2));
}

template<typename Kernel>
inline typename Kernel::Point_3
eigenToCgal(const Eigen::Matrix<typename Kernel::FT, 3, 1>& vec)
{
	return typename Kernel::Point_3(vec(0), vec(1), vec(2));
}

template<typename Kernel>
inline Eigen::Matrix<typename Kernel::FT, 3, 1>
cgalToEigen(const typename Kernel::Point_3& p)
{
	return Eigen::Matrix<typename Kernel::FT, 3, 1>(p.x(), p.y(), p.z());
}

template<typename Kernel>
inline QVector3D
cgalToQt(const typename Kernel::Point_3& p)
{
	return QVector3D(p.x(), p.y(), p.z());
}
