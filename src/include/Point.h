#pragma once
#include <memory>

#include "Storage.h"

namespace Commutator {

	class Point;

	typedef std::shared_ptr<Point> PointPtr;

	class PointHandler {
		public:
			virtual void OnCallReceived(Point* const point, const std::string& number) = 0;
			virtual void OnCallEnded(Point* const point) = 0;
	};

	class Point {
		private:
			const Storage::Point& point_;
			PointHandler* const handler_;

		public:
			Point(const Storage::Point& point, PointHandler* const handler)
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

			virtual void Run() = 0;
			virtual void Initiate(const std::string& number) = 0;
			virtual void Hangup() = 0;
	};

	class PointFactory {
		public:
			virtual PointPtr Create(const Storage::Point& point, PointHandler* const handler) = 0;
	};
}