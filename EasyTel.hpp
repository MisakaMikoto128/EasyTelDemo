
#ifndef _EASYTEL_H_
#define _EASYTEL_H_
#include "BytesUtil.hpp"
#include "SimpleDPP.hpp"
#include "LTimer.hpp"
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#define DEBUG

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#define UNUSED(x) (void)(x)
typedef std::function<void(const bu_byte *data, bu_uint32 len)> EasyTelCmdCallback;

/*
命令名命规则：
Q_开头，表示询问命令
R_开头，表示回复命令
*/

class EasyTelPoint : public QObject
{
    Q_OBJECT

public:
    static constexpr bu_uint8 Q_EXIST_POINT = 0x00;
    static constexpr bu_uint8 R_EXIST_POINT = 0x01;
    static constexpr bu_uint8 Q_ENDIAN = 0x02;
    static constexpr bu_uint8 R_ENDIAN = 0x03;

    static constexpr bu_uint8 CALLBACK_LIST_LENGTH = 0xFF;
    static constexpr bu_uint8 CMD_MAX = CALLBACK_LIST_LENGTH;

private:
    Endian endian;
    std::atomic<bool> need_to_change_endian{false};
    std::atomic<bool> found_point{false};

    std::function<void(const bu_byte *data, bu_uint32 len)> callback_list[CALLBACK_LIST_LENGTH];
    SimpleDPP sdp;
    std::thread *find_peer_thread = nullptr;
    int thread_delay_ms = 500;
    int find_peer_repeat = 10;
    // thread control

    std::atomic<bool> close_find_peer_thread{false};
signals:
    //before find peer,invoke this method to init resources.
    bool find_peer_iteration_callback(void);
public:
    EasyTelPoint()
    {
        endian = BytesUtil::getSelfEndian();
        need_to_change_endian = false;
        found_point = false;

        for (int i = 0; i < CALLBACK_LIST_LENGTH; i++)
        {
            callback_list[i] = nullptr;
        }
        connect(&sdp,&SimpleDPP::RecvCallback,this,&EasyTelPoint::SimpleDPPRecvCallback);
        connect(&sdp,&SimpleDPP::RevErrorCallback,this,&EasyTelPoint::SimpleDPPRevErrorCallback);
    }
    ~EasyTelPoint()
    {
        stop();
    }
    /**
     * @brief
     *
     * @tparam T
     * @param cmd
     * @param obj
     * @param callback : callback function to handle the data.
     * @return true : if register success.
     * @return false
     */
    template <typename T>
    bool registerCmdCallback(const bu_byte cmd, T *obj, void (T::*callback)(const bu_byte *data, bu_uint32 len))
    {
        if (callback == nullptr || cmd <= R_ENDIAN || cmd > CMD_MAX)
        {
            return false;
        }
        else
        {
            auto lambda = [obj, callback](const bu_byte *data, bu_uint32 len)
            { (obj->*callback)(data, len); };
            callback_list[cmd] = lambda;
            return true;
        }
    }

    /**
     * @brief
     *
     * @tparam T
     * @param obj T * pointer
     * @param func T::func
     */
    template <class T>
    void bindSendBuffer(T *obj, void (T::*func)(const QByteArray &senddata))
    {
        connect(&sdp,&SimpleDPP::SendBuffer,obj,func);
    }

    void bindSendBuffer(std::function<void(const QByteArray &senddata)> SendBuffer)
    {
        connect(&sdp,&SimpleDPP::SendBuffer,SendBuffer);
    }

    bool send(bu_byte cmd, const char *data = nullptr, bu_uint32 len = 0)
    {
        bu_uint32 send_len = sdp.send_datas(&cmd, sizeof(cmd), data, len);
        return send_len > 0;
    }

    void parse(const QByteArray & data){
        sdp.parse(data);
    }

    void parse(const sdp_byte *data, int len){
        sdp.parse(data,len);
    }

    void start()
    {

        close_find_peer_thread = false;
        find_peer_thread = new std::thread([&]()
        {
            while (!close_find_peer_thread.load())
            {
                if(found_point.load()){
                    stop();
                }else{

                        bool have_next = emit find_peer_iteration_callback();
                        if(have_next == false){
                            stop();
                            continue;

                    }
                }

                LTimer tim_find_peer;
                tim_find_peer.setReapet([&]{
                    if(found_point.load()){
                        tim_find_peer.stop();
                    }
                    send(Q_EXIST_POINT);
                },thread_delay_ms,find_peer_repeat);
            }


            delete find_peer_thread;

        });

        find_peer_thread->detach();

    }

    void stop()
    {
        if (find_peer_thread != nullptr)
        {
            close_find_peer_thread.store(true);
        }
    }

    bool isRunning()
    {
        return !close_find_peer_thread;
    }

    bool foundPoint()
    {
        return found_point;
    }

    SimpleDPP & getSimpleDPP()
    {
        return sdp;
    }
    inline void setFind_peer_iteration_callback(const std::function<bool(void)> &find_peer_iteration_callback)
    {
        connect(this,&EasyTelPoint::find_peer_iteration_callback,find_peer_iteration_callback);
    }
    bool isFoundPoint() const
    {
        return found_point.load();
    }

public slots:
    void SimpleDPPRecvCallback(const QByteArray &revdata)
    {
        bu_uint8 cmd = revdata[0];

        switch (cmd)
        {
        case Q_EXIST_POINT:
        {
            char endian_ = static_cast<char>(endian);
            send(R_EXIST_POINT, &endian_, sizeof(endian_));
        }

            break;
        case R_EXIST_POINT:
        {
            Endian peer_endian = (Endian)revdata[1];
            need_to_change_endian = (peer_endian != endian);
            found_point = true;
        }

            break;
        case Q_ENDIAN:
            send(R_ENDIAN);
            break;
        case R_ENDIAN:
        {
            Endian peer_endian = (Endian)revdata[1];
            need_to_change_endian = (peer_endian != endian);
        }

            break;
        default:
            if (callback_list[cmd] != nullptr && cmd < CALLBACK_LIST_LENGTH)
            {
                callback_list[cmd]((bu_byte *)revdata.data() + 1, revdata.size() - 1);
            }
            break;
            break;
        }
    }

    void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code)
    {
        UNUSED(error_code);
    }
};





#endif // _EASYTEL_H_
