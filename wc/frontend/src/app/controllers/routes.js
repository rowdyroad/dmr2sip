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

	FormController('/api/routes', 'route_id', $scope, $http, $routeParams, $uibModal);
})
.controller('RouteCtrl', function($scope, $http) {
  BaseController('/api/routes', 'route_id', $scope, $http);
});