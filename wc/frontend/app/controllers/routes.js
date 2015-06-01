'use strict';

angular.module('myApp.routes', ['ngRoute'])

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
.controller('RouteFormCtrl', function($scope, $http,$routeParams, $modal) {
  $scope.points = [];
  $http.get('/api/points').success(function(data) {
    $scope.points = data;
  });

	FormController('/api/routes', 'route_id', $scope, $http, $routeParams, $modal);
})
.controller('RouteCtrl', function($scope, $http) {
  BaseController('/api/routes', 'route_id', $scope, $http);
});