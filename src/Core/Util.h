#pragma once

#include <QVector3D>
#include <Eigen/Dense>

/*****************************
 Vector and Matrix Conversion
*****************************/
template<typename FT>
inline Eigen::Matrix<FT, 3, 1> 
qtToEigen(const QVector3D& vec)
{
	return Eigen::Matrix<FT, 3, 1>(vec.x(), vec.y(), vec.z());
}

template<typename CGALVec>
inline CGALVec
qtToCgal(const QVector3D& vec)
{
	return CGALVec(vec.x(), vec.y(), vec.z());
}

template<typename FT>
inline QVector3D 
eigenToQt(const Eigen::Matrix<FT, 3, 1>& vec)
{
	return QVector3D(vec(0), vec(1), vec(2));
}

template<typename CGALVec>
inline CGALVec
eigenToCgal(const Eigen::Matrix<typename Kernel::FT, 3, 1>& vec)
{
	return CGALVec(vec(0), vec(1), vec(2));
}

template<typename CGALVec>
inline Eigen::Matrix<typename CGAL::Kernel_traits<CGALVec>::Kernel::FT, 3, 1>
cgalToEigen(const CGALVec& p)
{
	return Eigen::Matrix<typename CGAL::Kernel_traits<CGALVec>::Kernel::FT, 3, 1>(p.x(), p.y(), p.z());
}

template<typename CGALVec>
inline QVector3D
cgalToQt(const CGALVec& p)
{
	return QVector3D(p.x(), p.y(), p.z());
}
