#pragma once
#include "Storage.h"

namespace Commutator {

	class PointHandler {
		public:
			virtual void OnCallReceived(Point& point, const std::string& number) = 0;
			virtual void OnCallEnded(Point& point) = 0;
	};

	class Point {
		private:
			const Storage::Point& point_;
			PointHandler& handler_;

		public:
			Point(const Storage::Point& point, PointHandler& handler)
				: point_(point)
				, handler_(handler)
			{}

			const Storage::Point& getConfiguration() const {
				return point_;
			}

			PointHandler& Handler()
			{
				return handler_;
			}

			virtual void Run() = 0;
			virtual void Initiate(const std::string& number) = 0;
			virtual void Hangup() = 0;
	};

	class PointFactory {
		public:
			virtual Point Create(const Storage::Point& point, PointHandler& handler) {
				return std::move(Point(point, handler));
			}
	};
}