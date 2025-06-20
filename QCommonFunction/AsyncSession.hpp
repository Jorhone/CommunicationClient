#ifndef ASYNCSESSION_HPP
#define ASYNCSESSION_HPP

#include <QElapsedTimer>
#include <QCoreApplication>
#include <QReadWriteLock>
#include <QException>
#include <QDebug>

#define CLASS_NOCOPY(classname) \
    private : \
        classname( const classname &dest ); \
        classname &operator= (const  classname &dest);

//跨线程使用
template<class T>
class AsyncSession
{
public:
    AsyncSession(void);
    ~AsyncSession(void);

    inline int wait(void);
    inline int wait(int millisecond);
    inline void awaken();

    inline void set(const T& res);
    inline T& get();

private:
    T _res;
    QReadWriteLock _lock;
    bool _waiting;

    CLASS_NOCOPY(AsyncSession)
};
template<class T>
AsyncSession<T>::AsyncSession()
{

}

template<class T>
AsyncSession<T>::~AsyncSession()
{

}

template<class T>
inline int AsyncSession<T>::wait(void)
{
    return this->wait(-1);
}

template<class T>
inline int AsyncSession<T>::wait(int millisecond)
{
    //无等待时间，立即超时返回
    if(millisecond == 0)
        return -1;

    try
    {
        {
            QWriteLocker x(&_lock);
            if(!_waiting)
                _waiting = true;
            else
                return -1;
        }

        QElapsedTimer tElapsedTimer;
        tElapsedTimer.start();

        do
        {
            {
                QReadLocker x(&_lock);
                if(!_waiting)
                    break;
            }

            if(millisecond > 0)
            {
                if((uint64_t)tElapsedTimer.elapsed() >= (uint64_t)millisecond)
                {
                    //超时
                    QWriteLocker x(&_lock);
                    if(_waiting)
                        _waiting = false;

                    return -1;
                }
            }

            quint64 timeout = 10; //ms
            QCoreApplication::processEvents(QEventLoop::AllEvents, timeout);

        }while(1);
    }
    catch (QException& exce)
    {
        qDebug() << QString("%1 %2").arg(__FUNCTION__, exce.what());
        return -1;
    }

    return 0;
}

template<class T>
inline void AsyncSession<T>::awaken()
{
    try
    {
        QWriteLocker x(&_lock);
        if(_waiting)
            _waiting = false;
    }
    catch (QException& exce)
    {
        qDebug() << QString("%1 %2").arg(__FUNCTION__, exce.what());
    }
}

template<class T>
inline void AsyncSession<T>::set(const T& res)
{
    _res = res;
}

template<class T>
inline T& AsyncSession<T>::get()
{
    return _res;
}

#endif // ASYNCSESSION_HPP
