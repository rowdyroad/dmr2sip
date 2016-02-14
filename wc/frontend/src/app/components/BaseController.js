'use strict';

function BaseController(prefix, pkAttribute, $scope, $http, init)
{
    $scope.page = 0;
    $scope.pkAttribute = pkAttribute;
    $scope.items = [];

    $scope.pageCount = 0;

    $scope.localDate = function(value)
    {
      return Date.fromUTCString(value).toString();
    }

    $scope.getParams = function() { return  {}; }

    $scope.get = function(clear)
    {
        if (clear) {
            $scope.page = 0;
            $scope.pageCount = 0;
            $scope.items = [];
        }

        var options = { params: angular.extend({}, $scope.getParams(), {page: ++$scope.page}) };
        $http.get(prefix, options).success(function(data) {
            if (data.data && data._meta) {
                $scope.pageCount = data._meta.pageCount;
                if ($scope.page <= data._meta.pageCount) {
                    angular.forEach(data.data, function(v) {
                        $scope.items.push(v);
                    });
                }
            } else {
                $scope.items = data;
            }
        });
    }

    if (angular.isFunction(init)) {
        init();
    }
    $scope.get();
}