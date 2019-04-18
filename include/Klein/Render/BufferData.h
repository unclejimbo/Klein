#ifndef KLEIN_BUFFERDATA_H
#define KLEIN_BUFFERDATA_H

#include <algorithm>
#include <iterator>
#include <QByteArray>
#include <QColor>

namespace Klein
{

// implementation helper
template<typename RandomIt, typename V>
struct _CreateByteArray
{
    static QByteArray impl(RandomIt first, RandomIt last)
    {
        using T = typename std::iterator_traits<RandomIt>::value_type;
        QByteArray bytes;
        bytes.resize(static_cast<int>((last - first) * sizeof(T)));
        auto data = reinterpret_cast<T*>(bytes.data());
        std::copy(first, last, data);
        return bytes;
    }
};

// partial specialization on QColor
template<typename RandomIt>
struct _CreateByteArray<RandomIt, QColor>
{
    static QByteArray impl(RandomIt first, RandomIt last)
    {
        auto cnt = static_cast<int>(last - first);
        QByteArray bytes;
        bytes.resize(cnt * 3 * sizeof(float));
        auto data = reinterpret_cast<float*>(bytes.data());
        for (int i = 0; i < cnt; ++i) {
            data[3 * i + 0] = first->redF();
            data[3 * i + 1] = first->greenF();
            data[3 * i + 2] = first->blueF();
            ++first;
        }
        return bytes;
    }
};

template<typename RandomIt>
QByteArray createByteArray(RandomIt first, RandomIt last)
{
    return _CreateByteArray<
        RandomIt,
        typename std::iterator_traits<RandomIt>::value_type>::impl(first, last);
}

} // namespace Klein

#endif
