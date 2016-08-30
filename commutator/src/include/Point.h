#pragma once
#include <memory>

#include "Storage.h"

namespace Commutator {

    class Point;

    typedef std::shared_ptr<Point> PointPtr;

    class PointHandler {
        public:
            virtual void OnReady(Point* const point) { };
            virtual bool OnCallReceived(Point* const point, const std::string& number) {};
            virtual void OnCallEnded(Point* const point) {};
    };

    class Point {
        private:
            Storage::Point point_;
            PointHandler* const handler_;

        public:
            Point(Storage::Point point, PointHandler* const handler)
                : point_(point)
                , handler_(handler)
            {}

            const Storage::Point& getConfiguration() const {
                return point_;
            }

            PointHandler* const Handler()
            {
                return handler_;
            }

            virtual bool Link(PointPtr& point) { return true; }
            virtual void UnLink(PointPtr& point) { }

            virtual void SendCode(uint8_t code) { }
            virtual bool PhoneModeMaster() { return false; }

            virtual void Run() = 0;
            virtual void Stop() = 0;
            virtual void Initiate(const std::string& number) = 0;
            virtual void Callback() {}
            virtual void Hangup() = 0;
    };

    class PointFactory {
        public:
            virtual PointPtr Create(const Storage::Point& point, PointHandler* const handler) = 0;
    };
}