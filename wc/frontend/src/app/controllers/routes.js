'use strict';

angular
.module('ac.controllers', ['ngRoute'])
.config(['$routeProvider', function($routeProvider) {
  $routeProvider.when('/routes', {
    templateUrl: '/views/routes/routes.html',
    controller: 'RouteCtrl'
  }).when('/routes/new', {
    templateUrl: '/views/routes/route.form.html',
    controller: 'RouteFormCtrl'
  })
.when('/routes/:route_id', {
    templateUrl: '/views/routes/route.form.html',
    controller: 'RouteFormCtrl'
  })
}])
.controller('RouteFormCtrl', function($scope, $http,$routeParams, $uibModal) {
  $scope.points = [];
  $http.get('/api/points').success(function(data) {
    $scope.points = {};
    for (var i in data) {
      $scope.points[data[i].point_id] = data[i];
    }
  });

  $scope.$on('points:states', function(points) {
    angular.forEach(points, function(point) {
        $scope.points[point.point_id].status = point.status;
    });
  });

  var source_mask_id = 0, mask_index = 0, dest_index = 0;

  $scope.$on('after_find', function() {
    for(var i in $scope.current.source_number.from) {
      source_mask_id = Math.max($scope.current.source_number.from[i].id || 0, source_mask_id);
    }
    for(var i in $scope.current.source_number.to) {
      source_mask_id = Math.max($scope.current.source_number.to[i].id || 0, source_mask_id);
    }
  });

  $scope.removeSourceMask = function(mask)
  {
    if (mask.id) {
       angular.forEach($scope.current.destination_number, function(value, key) {
          if (angular.isObject(value)) {
              angular.forEach(value, function(v, k) {
                if (v.id && v.id == mask.id) {
                  v.error = true;
                }
              });
          }
       });
    }
  }

  $scope.addSourceMask = function(value)
  {
    if (value[0] == '~') {
      if (value[1] == '~') {
        return {value: value.substr(1), index: ++mask_index};
      }
      return {value: value.substr(1), id: ++source_mask_id, index: ++mask_index};
    } else {
      return {value: value, index: ++mask_index};
    }
  }

  $scope.transformChip = function(chip)
  {
      if (angular.isObject(chip)) {
        return {value:chip.id, id: chip.id, index: ++dest_index};
      } else {
        return {value:chip, index: ++dest_index};
      }
  }

  $scope.querySearch = function(text)
  {
     var ids = [];
     angular.forEach($scope.current.source_number, function(tp) {
      angular.forEach(tp, function(chunk) {
        if (chunk.id) {
          if (!text || chunk.id.toString().indexOf(text) != -1) {
            ids.push({id: chunk.id});
          }
        }
      });
     });
     console.log(ids);
     return ids;
  }

	FormController('/api/routes', 'route_id', $scope, $http, $routeParams, $uibModal);
})
.controller('RouteCtrl', function($scope, $http) {
  BaseController('/api/routes', 'route_id', $scope, $http);
});