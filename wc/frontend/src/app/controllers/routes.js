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
  var source_mask_id = 0, mask_index = 0;

  $scope.$on('after_find', function() {
    for(var i in $scope.current.source_number.from) {
      source_mask_id = Math.max($scope.current.source_number.from[i].id || 0, source_mask_id); 
    } 
    for(var i in $scope.current.source_number.to) {
      source_mask_id = Math.max($scope.current.source_number.to[i].id || 0, source_mask_id); 
    } 
  });
  $scope.addSourceMask = function(value)
  {
    if (value[0] == '~') {
      return {value: value.substr(1), id: ++source_mask_id, index: ++mask_index};
    } else {
      return {value: value, index: ++mask_index};
    }
  }

	FormController('/api/routes', 'route_id', $scope, $http, $routeParams, $uibModal);
})
.controller('RouteCtrl', function($scope, $http) {
  BaseController('/api/routes', 'route_id', $scope, $http);
});